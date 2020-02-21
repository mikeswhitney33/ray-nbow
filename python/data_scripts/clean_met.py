"""
this is a simple script to clean the met dataset from all the already grayscale images
"""
import argparse
import os
import multiprocessing as mp
import queue

from PIL import Image, UnidentifiedImageError
import numpy as np
import tqdm


def single(root, filename):
    """ performs on a single file """
    try:
        filepath = os.path.join(root, filename)
        image = Image.open(filepath)
        arr = np.array(image)
        if len(arr.shape) != 3:
            os.remove(filepath)
    except UnidentifiedImageError:
        os.remove(filepath)


def clean_folder(root):
    """ cleans an image folder removing any corrupt or non RGB images """
    filenames = os.listdir(root)

    proc_queue = queue.Queue(512)
    for filename in tqdm.tqdm(filenames):
        while proc_queue.full():
            proc_queue.get().join()
        proc = mp.Process(target=single, args=(root, filename))
        proc.start()
        proc_queue.put(proc)
    while not proc_queue.empty():
        proc_queue.get().join()


def main():
    """ runs the main program """
    parser = argparse.ArgumentParser()
    parser.add_argument("root")
    args = parser.parse_args()
    clean_folder(args.root)


if __name__ == "__main__":
    main()
