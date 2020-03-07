""" this handles all the distribution functions """
import os
import numpy as np
from PIL import Image


MAT_TYPES = ["MAT_TYPE_DIFF", "MAT_TYPE_SPEC", "MAT_TYPE_LIGHT"]


def uniform_random_material():
    """
    generates a material sampled from a uniform random distribution
    """
    return {
        "type":np.random.choice(MAT_TYPES),
        "kd":np.random.uniform(0, 1, 3),
        "ks":np.random.uniform(0, 1, 3),
        "ka":np.random.uniform(0, 1, 3),
        "alpha":np.random.uniform(0, 128)
    }


def image_dist(image_filename):
    """
    generates the material from pixels of an image
    """
    if not os.path.exists(image_filename):
        return uniform_random_material
    if os.path.isdir(image_filename):
        flat = get_dir_flat_image(image_filename)
    else:
        flat = get_flat_image(image_filename)
    if len(flat) == 0:
        return uniform_random_material
    def func():
        return {
            "type":np.random.choice(MAT_TYPES),
            "kd":flat[np.random.randint(0, len(flat))],
            "ks":flat[np.random.randint(0, len(flat))],
            "ka":flat[np.random.randint(0, len(flat))],
            "alpha":np.random.uniform(0, 128)
        }
    return func


def get_dir_flat_image(dirname):
    """ gets a flat image from a directory"""
    image_files = os.listdir(dirname)
    images = np.concatenate(
        [
            (np.array(
                Image.open(os.path.join(dirname, filename)).convert("RGB").resize((224, 224)),
                dtype=np.float32) / 255).reshape(-1, 3) \
                for filename in image_files])
    return images


def get_flat_image(filename):
    """ gets a flat image from a file """
    image = np.array(Image.open(filename).resize((224, 224)), dtype=np.float32) / 255
    flat = image.reshape(-1, 3)
    return flat
