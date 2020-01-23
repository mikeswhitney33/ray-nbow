import os
import argparse
import numpy as np
from PIL import Image
from tqdm import tqdm
import matplotlib.pyplot as plt 


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("emotion")
    args = parser.parse_args()
    emotion = args.emotion


    emodir = os.path.join("data", emotion)
    if not os.path.exists(emodir):
        return

    files = os.listdir(emodir)
    avg_h_hist = np.zeros(256, dtype=np.float32)
    avg_s_hist = np.zeros(256, dtype=np.float32)
    avg_v_hist = np.zeros(256, dtype=np.float32)
    for file in tqdm(files):
        impath = os.path.join(emodir, file)
        im = np.array(Image.open(impath).resize((800,600)).convert("HSV"), dtype=np.float32)
        h_hist, _ = np.histogram(im[:,:,0], 256, density=True)
        s_hist, _ = np.histogram(im[:,:,1], 256, density=True)
        v_hist, _ = np.histogram(im[:,:,2], 256, density=True)
        avg_h_hist += h_hist / len(files)
        avg_s_hist += s_hist / len(files)
        avg_v_hist += v_hist / len(files)
    plt.plot(avg_h_hist, label="h")
    plt.title("Hue")
    plt.show()
    plt.plot(avg_s_hist, label="s")
    plt.title("Saturation")
    plt.show()
    plt.plot(avg_v_hist, label="v")
    plt.title("Value")
    plt.show()


    # print(len(files))

if __name__ == '__main__':
    main()
