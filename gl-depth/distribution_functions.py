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


class EpsilonGreedy:
    def __init__(
            self,
            kd_file="tmp/tmp_kd.npy",
            ks_file="tmp/tmp_ks.npy",
            ka_file="tmp/tmp_ka.npy",
            alpha_file="tmp/tmp_alpha.npy",
            starting_epsilon=1.0,
            decay_factor=0.999,
            eval_threshold=0.5):

        self.kd_seeds = np.zeros((0, 3))
        self.ks_seeds = np.zeros((0, 3))
        self.ka_seeds = np.zeros((0, 3))
        self.alpha_seeds = np.zeros((0,1))

        self.kd_file = kd_file
        self.ks_file = ks_file
        self.ka_file = ka_file
        self.alpha_file = alpha_file

        if os.path.exists(kd_file):
            self.kd_seeds = np.load(kd_file)
        if os.path.exists(ks_file):
            self.ks_seeds = np.load(ks_file)
        if os.path.exists(ka_file):
            self.ka_seeds = np.load(ka_file)
        if os.path.exists(alpha_file):
            self.alpha_seeds = np.load(alpha_file)

        self.epsilon = starting_epsilon
        self.decay_factor = decay_factor
        self.eval_threshold = eval_threshold

        self.last_kd = []
        self.last_ks = []
        self.last_ka = []
        self.last_alpha = []

    def __check_seed_length(self, seeds):
        return len(seeds) > 0

    def __check_seed_lengths(self):
        return np.all([self.__check_seed_length(seed) for seed in \
             [self.kd_seeds, self.ks_seeds, self.ka_seeds, self.alpha_seeds]])

    def __sample_seeds(self, seeds):
        return seeds[np.random.randint(0, len(seeds))]

    def __call__(self):
        if np.random.uniform() < self.epsilon or not self.__check_seed_lengths():
            mat = uniform_random_material()
        else:
            mat = self.sample_seeds()
        self.last_kd.append(mat['kd'].reshape(-1, 3))
        self.last_ks.append(mat['ks'].reshape(-1, 3))
        self.last_ka.append(mat['ka'].reshape(-1, 3))
        self.last_alpha.append(np.array(mat['alpha']).reshape(-1, 1))
        self.epsilon *= self.decay_factor
        return mat

    def sample_seeds(self):
        return {
            "type":np.random.choice(MAT_TYPES),
            "kd":self.kd_seeds[np.random.randint(0, len(self.kd_seeds))],
            "ks":self.ks_seeds[np.random.randint(0, len(self.ks_seeds))],
            "ka":self.ka_seeds[np.random.randint(0, len(self.ka_seeds))],
            "alpha":self.alpha_seeds[np.random.randint(0, len(self.alpha_seeds))]
        }

    def update(self, eval_score):
        if eval_score > self.eval_threshold:
            for i in range(len(self.last_ks)):
                self.kd_seeds = np.concatenate([self.kd_seeds, self.last_kd[i]])
                self.ks_seeds = np.concatenate([self.ks_seeds, self.last_ks[i]])
                self.ka_seeds = np.concatenate([self.ka_seeds, self.last_ka[i]])
                self.alpha_seeds = np.concatenate([self.alpha_seeds, self.last_alpha[i]])
        self.last_kd = []
        self.last_ks = []
        self.last_ka = []
        self.last_alpha = []

    def print_seed_lengths(self):
        print("Seed Lengths:")
        print("  KS: ", len(self.ks_seeds))
        print("  KD: ", len(self.kd_seeds))
        print("  KA: ", len(self.ka_seeds))
        print("  ALPHA: ", len(self.alpha_seeds))



