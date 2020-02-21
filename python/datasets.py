"""
datasets.py
an implementation of dataset used for training in PyTorch
"""

import os

from PIL import Image
import torch


class DirectoryDataset(torch.utils.data.Dataset):
    """
    A base class for getting each image from a directory containing only images
    """
    def __init__(self, root, transform=None):
        self.root = root
        self.transform = transform
        self.filenames = os.listdir(self.root)

    def __getitem__(self, idx):
        return NotImplemented

    def __len__(self):
        return len(self.filenames)


class GrayColorDataset(DirectoryDataset):
    """
    A dataset that takes in a folder and returns a gray image for input
         and a color image for ground truth
    """
    def __getitem__(self, idx):
        filename = os.path.join(self.root, self.filenames[idx])
        image = Image.open(filename)
        gray = image.convert("L")
        color = image.convert("RGB")
        if self.transform:
            gray, color = self.transform(gray, color)
        return gray, color


class GrayDataset(DirectoryDataset):
    """
    A dataset that takes in a folder and returns a gray image for inference
    """
    def __getitem__(self, idx):
        filename = os.path.join(self.root, self.filenames[idx])
        image = Image.open(filename).convert("L")
        if self.transform:
            image = self.transform(image)
        return image


class ColorDataset(DirectoryDataset):
    """
    A dataset that takes in a folder and returns a color image
    """
    def __getitem__(self, idx):
        filename = os.path.join(self.root, self.filenames[idx])
        image = Image.open(filename).convert("RGB")
        if self.transform:
            image = self.transform(image)
        return image
