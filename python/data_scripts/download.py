"""
used for downloading the art dataset
"""

import json
import multiprocessing as mp
import os
from queue import Queue
import warnings

import requests
from tqdm import tqdm

warnings.simplefilter("ignore")

DATA_DIR = "data"
MAX_PROCESSES = 120


def download(image_data):
    """ downloads a single image"""
    path = os.path.join(DATA_DIR, image_data["emotion"])
    impath = os.path.join(path, image_data["imname"])
    if not os.path.exists(path):
        os.mkdir(path)
    if os.path.exists(impath):
        return
    req = requests.get(image_data['url'], allow_redirects=True, verify=False)
    with open(impath, 'wb') as file:
        file.write(req.content)


def main():
    """runs the main program"""
    with open("data-anno.json", "r") as f:
        data = json.load(f)

    if not os.path.exists(DATA_DIR):
        os.mkdir(DATA_DIR)

    process_queue = Queue(MAX_PROCESSES)
    for image_data in tqdm(data):
        while process_queue.full():
            process_queue.get().join()

        proc = mp.Process(target=download, args=(image_data,))
        proc.start()
        process_queue.put(proc)
    while not process_queue.empty():
        process_queue.get().join()

if __name__ == "__main__":
    main()
