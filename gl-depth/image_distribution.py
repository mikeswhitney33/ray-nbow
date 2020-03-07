"""
image_distribution.py
this runs the main program using an image or
image directory to make a seed for the materials
"""
import argparse
from distribution_functions import image_dist, uniform_random_material
from rendering import render_scene


def main():
    """The entry point to the program"""
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--image", default=None, type=str)
    parser.add_argument("-d", "--depth", default=2, type=int)
    parser.add_argument("--width", default=224, type=int)
    parser.add_argument("--height", default=224, type=int)
    parser.add_argument("-s", "--scene", default="scenes/basic.json", type=str)
    args = parser.parse_args()
    if args.image is None:
        func = uniform_random_material
    else:
        func = image_dist(args.image)
    image = render_scene(args.scene, func, args.depth, (args.width, args.height))
    image.show()


if __name__ == "__main__":
    main()
