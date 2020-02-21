import json
import requests
import tqdm 
import shutil
import multiprocessing as mp
import queue
import os 

r = requests.get("https://collectionapi.metmuseum.org/public/collection/v1/objects")
objs = r.json()
print(objs["total"])

existing = os.listdir("data/met")
existing = [e.split(".")[0] for e in existing]
print(len(existing))

def single(objectid):
    if os.path.exists(f"data/met/{objectid}.jpg"):
        return
    try:
        r2 = requests.get(f"https://collectionapi.metmuseum.org/public/collection/v1/objects/{objectid}")
        obj = r2.json()
        if "isPublicDomain" not in obj.keys() or not obj["isPublicDomain"]:
            return
        r3 = requests.get(obj["primaryImageSmall"], stream=True)
        with open(f"data/met/{objectid}.jpg", "wb") as f:
            r3.raw.decode_content = True
            shutil.copyfileobj(r3.raw, f)
    except requests.exceptions.MissingSchema:
        return


q = queue.Queue(maxsize=512)
for objectid in tqdm.tqdm(objs["objectIDs"]):
    while q.full():
        q.get().join()
    p = mp.Process(target=single, args=(objectid,))
    p.start()
    q.put(p)
while not q.empty():
    q.get().join()