from cv2 import cv2 as cv
import numpy as np
from matplotlib import pyplot as plt
from matplotlib import image as matImg
import PIL
import matplotlib
import scipy
import pandas as pd
from scipy.cluster.vq import whiten
from scipy.cluster.vq import kmeans

from colormath.color_objects import sRGBColor, LabColor
from colormath.color_conversions import convert_color
from colormath.color_diff import delta_e_cie2000
import sys


# import imutils




angleTolerance = 12
sizeTolerance = 8
complimentarynessThreshold = 20.0
maxContrast = 128

def anglesClose(one, two):

    diff = one - two
    diff = (diff + 180) % 360 - 180
    return diff > angleTolerance

def sizeClose(one, two):
    return abs(one - two) < sizeTolerance



def getSymmetry(img):

    # Initiate ORB detector
    orb = cv.ORB_create()
    # find the keypoints with ORB
    kp = orb.detect(img,None)
    # compute the descriptors with ORB
    kp, des = orb.compute(img, kp)
    # draw only keypoints location,not size and orientation
    img2 = cv.drawKeypoints(img, kp, None, color=(0,255,0), flags=(cv.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS))
    plt.imshow(img2), plt.show()


    # for each point compare to each other point and call it symmetry if there is a corresponding point?
    # cases of symmetry
    # opposite direction parrallel
    # 
    numKeyPoints = len(kp)
    matched = []
    index = 0

    while index < len(kp):
        for i in range(1, len(kp)):
            # simple check for whether size is close enough and angle reflected along Y is close enough
            if anglesClose(kp[0].angle,360-kp[i].angle) and sizeClose(kp[0].size,kp[i].size):
                matched.append(kp[0])
                matched.append(kp[i])
                del kp[i]
                del kp[0]
                break
        index += 1

    return len(matched) / numKeyPoints


# Sum of the min & max of (a, b, c)
def hilo(a, b, c):
    if c < b: b, c = c, b
    if b < a: a, b = b, a
    if c < b: b, c = c, b
    return a + c

def complement(r, g, b):
    k = hilo(r, g, b)
    return tuple(k - u for u in (r, g, b))



def getComplimentaryColoredness(fileName):
    image = matImg.imread('./' + fileName)
    r = []
    g = []
    b = []
    for line in image:
        for pixel in line:
            temp_r, temp_g, temp_b = pixel
            r.append(temp_r)
            g.append(temp_g)
            b.append(temp_b)
    df = pd.DataFrame({'red': r, 'blue': b, 'green': g})
    df['scaled_red'] = whiten(df['red'])
    df['scaled_blue'] = whiten(df['blue'])
    df['scaled_green'] = whiten(df['green'])
    df.sample(n = 10)
    cluster_centers, distortion = kmeans(df[['scaled_red', 'scaled_green', 'scaled_blue']], 4)


    colors = []
    r_std, g_std, b_std = df[['red', 'green', 'blue']].std()
    for cluster_center in cluster_centers:
        scaled_r, scaled_g, scaled_b = cluster_center
        colors.append((
            scaled_r * r_std / 255,
            scaled_g * g_std / 255,
            scaled_b * b_std / 255
            ))
    # plt.imshow([colors])
    # plt.show()
    compl = complement(colors[1][0], colors[1][1], colors[1][2])
    color1_rgb = sRGBColor(colors[0][0], colors[0][1], colors[0][2])
    color2_rgb = sRGBColor(compl[0], compl[1], compl[2])

    # Convert from RGB to Lab Color Space
    color1_lab = convert_color(color1_rgb, LabColor)
    color2_lab = convert_color(color2_rgb, LabColor)
    # Find the color difference
    delta_e = delta_e_cie2000(color1_lab, color2_lab)

    # print(delta_e)
    ret = 1.0 - (delta_e / 80.0)
    return ret


def getRuleOfThirds(img):

    return 1.0


# aka "Colorfulness"
def getSaturation(img):
	# split the image into its respective RGB components
	(B, G, R) = cv.split(img.astype("float"))
	# compute rg = R - G
	rg = np.absolute(R - G)
	# compute yb = 0.5 * (R + G) - B
	yb = np.absolute(0.5 * (R + G) - B)
	# compute the mean and standard deviation of both `rg` and `yb`
	(rbMean, rbStd) = (np.mean(rg), np.std(rg))
	(ybMean, ybStd) = (np.mean(yb), np.std(yb))
	# combine the mean and standard deviations
	stdRoot = np.sqrt((rbStd ** 2) + (ybStd ** 2))
	meanRoot = np.sqrt((rbMean ** 2) + (ybMean ** 2))
	# derive the "colorfulness" metric and return it
	return stdRoot + (0.3 * meanRoot)

def getContrast(img):
    img_grey = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
    contrast = img_grey.std()
    return 1.0 - (contrast / maxContrast)

# measures how tightly grouped the keypoints of the image are
def getFocalPointedness(img):

    return 1.0


def main():
    fileName = "contrast.png"
    # fileName = sys.argv[1]
    img = cv.imread( fileName, cv.IMREAD_COLOR)
    # img = imutils.resize(image, width=250)

    symmetry = getSymmetry(img)
    complimentary = getComplimentaryColoredness(fileName)
    saturation = getSaturation(img)
    contrast = getContrast(img)

    print(symmetry, complimentary, contrast, saturation)

    return 0

main()