"""
Simulates falling paint on a depth image
"""

import argparse
import queue

import cv2 as cv
import numpy as np


def check_neighbor(y_coord, x_coord, visited, lowest, dist):
    """
    Checks if a given neighbor is valid
    """
    height, width = visited.shape
    return \
    y_coord < height and \
    y_coord >= 0 and \
    x_coord < width and \
    x_coord >= 0 and \
    visited[y_coord, x_coord] == 0 and \
    dist[y_coord, x_coord] < lowest and \
    dist[y_coord, x_coord] != 0


def falling_paint(depth_image):
    """
    simulates falling paint on a depth image
    """
    dist = cv.cvtColor(depth_image, cv.COLOR_BGR2GRAY)
    fdist = dist.astype(np.float32) / 255

    fim = depth_image.astype(np.float32)/255
    visited = np.zeros_like(fim[:, :, 0])
    color = np.random.uniform(0, 1, (3))
    for _ in range(1000):

        while True:
            y_coord = np.random.randint(0, fim.shape[1]-1)
            x_coord = np.random.randint(0, fim.shape[0]-1)
            if np.sum(fim[x_coord, y_coord]) != 0:
                break
        neighbor_queue = queue.Queue()
        neighbor_queue.put((y_coord, x_coord))
        while not neighbor_queue.empty():
            color = np.clip(color + np.random.uniform(-0.05, 0.05, size=(3,)), 0, 1)
            coord = neighbor_queue.get()

            y_coord, x_coord = coord
            if visited[y_coord, x_coord] == 0:
                cv.circle(fim, (x_coord, y_coord), 1, color * fdist[y_coord, x_coord], -1)
            visited[y_coord, x_coord] = 1
            lowest = 255*3
            bssf = None
            if check_neighbor(y_coord + 1, x_coord, visited, lowest, dist):
                bssf = (y_coord + 1, x_coord)
                lowest = dist[y_coord, x_coord]
            if check_neighbor(y_coord, x_coord + 1, visited, lowest, dist):
                bssf = (y_coord, x_coord + 1)
                lowest = dist[y_coord, x_coord]
            if check_neighbor(y_coord - 1, x_coord, visited, lowest, dist):
                bssf = (y_coord - 1, x_coord)
                lowest = dist[y_coord, x_coord]
            if check_neighbor(y_coord, x_coord - 1, visited, lowest, dist):
                bssf = (y_coord, x_coord - 1)
                lowest = dist[y_coord, x_coord]
            if bssf:
                neighbor_queue.put(bssf)
    return fim

def main():
    """
    runs the main program
    """
    parser = argparse.ArgumentParser()
    parser.add_argument("depth_image")
    args = parser.parse_args()
    depth_image = cv.imread(args.depth_image, cv.IMREAD_COLOR)
    if depth_image is None:
        print(args.depth_image, "not found!")
        return
    color_image = falling_paint(depth_image)

    cv.imshow(args.depth_image, color_image)
    cv.waitKey()
    cv.destroyAllWindows()


if __name__ == '__main__':
    main()
