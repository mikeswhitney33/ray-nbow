import json 
import requests
import os
from tqdm import tqdm
import multiprocessing as mp
import warnings
warnings.simplefilter("ignore")

DATA_DIR = "data"
MAX_PROCESSES = 120

with open("data-anno.json", "r") as f:
    data = json.load(f)



if not os.path.exists(DATA_DIR):
    os.mkdir(DATA_DIR)

def download(im):
    path = os.path.join(DATA_DIR, im["emotion"])
    impath = os.path.join(path, im["imname"])
    if not os.path.exists(path):
        os.mkdir(path)
    if os.path.exists(impath):
        return
    r = requests.get(im['url'], allow_redirects=True, verify=False)
    with open(impath, 'wb') as f:
        f.write(r.content)

processes = []

for im in tqdm(data):
    while len(processes) >= MAX_PROCESSES:
        p = processes[0]
        p.join()
        del processes[0]
    proc = mp.Process(target=download, args=(im,))
    proc.start()
    processes.append(proc)
while len(processes) > 0:
    p = processes[0]
    p.join()
    del processes[0]
