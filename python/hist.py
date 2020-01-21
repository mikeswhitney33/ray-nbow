import os
import argparse
import numpy
from PIL import Image


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("emotion")
    args = parser.parse_args()
    emotion = args.emotion


    emodir = os.path.join("data", emotion)
    if not os.path.exists(emodir):
        return

    files = os.listdir(emodir)
    print(len(files))

if __name__ == '__main__':
    main()
