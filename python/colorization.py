"""
colorization.py
this attempts to model the problem as colorization
"""
import argparse
from multiprocessing import cpu_count
import os

import numpy as np
from PIL import Image
import torch
import torchvision.transforms.functional as F
import tqdm

from datasets import GrayColorDataset, GrayDataset
from models import UNet


def default_transform(gray, color):
    """
    defines a transform to use for all instances
    """
    color = F.to_tensor(color)
    gray = F.to_tensor(gray)
    color = F.normalize(color, (0.5, 0.5, 0.5), (0.5, 0.5, 0.5))
    gray = F.normalize(gray, (0.5,), (0.5,))
    return gray, color


def train_transform(gray, color):
    """
    defines a transform to use for training images
    """
    assert(gray.width == color.width and gray.height == color.height)
    size = np.random.randint(256, 1024)
    gray = F.resize(gray, size)
    color = F.resize(color, size)
    width, height = gray.width, gray.height
    x_coord = np.random.randint(0, width-224)
    y_coord = np.random.randint(0, height-224)
    gray = gray.crop((x_coord, y_coord, x_coord+224, y_coord+224))
    color = color.crop((x_coord, y_coord, x_coord+224, y_coord+224))

    transposes = [None,
                  Image.FLIP_LEFT_RIGHT,
                  Image.FLIP_TOP_BOTTOM,
                  Image.ROTATE_90,
                  Image.ROTATE_180,
                  Image.ROTATE_270,
                  Image.TRANSPOSE,
                  Image.TRANSVERSE]
    transpose = np.random.choice(transposes)
    if transpose is not None:
        color = color.transpose(transpose)
        gray = gray.transpose(transpose)
    return default_transform(gray, color)


def val_transform(gray):
    """
    defines a transform for validation instances
    """
    gray = gray.resize((224, 224))
    gray = F.to_tensor(gray)
    gray = F.normalize(gray, (0.5,), (0.5,))
    return gray


def train(device, model_path, dataset_path):
    """
    Trains the network according on the dataset_path
    """
    network = UNet(1, 3).to(device)
    optimizer = torch.optim.Adam(network.parameters())
    criteria = torch.nn.MSELoss()

    dataset = GrayColorDataset(dataset_path, transform=train_transform)
    loader = torch.utils.data.DataLoader(
        dataset, batch_size=16, shuffle=True, num_workers=cpu_count())

    if os.path.exists(model_path):
        network.load_state_dict(torch.load(model_path))
    for _ in tqdm.trange(10, desc="Epoch"):
        network.train()
        for gray, color in tqdm.tqdm(loader, desc="Training", leave=False):
            gray, color = gray.to(device), color.to(device)
            optimizer.zero_grad()
            pred_color = network(gray)
            loss = criteria(pred_color, color)
            loss.backward()
            optimizer.step()
        torch.save(network.state_dict(), model_path)


def test(device, model_path, dataset_path, out_path):
    """
    Tests the network on the dataset_path
    """
    network = UNet(1, 3).to(device)
    if os.path.exists(model_path):
        network.load_state_dict(torch.load(model_path))
    dataset = GrayDataset(dataset_path, transform=val_transform)
    loader = torch.utils.data.DataLoader(
        dataset, batch_size=1, shuffle=False, num_workers=cpu_count())
    with torch.no_grad():
        network.eval()
        for i, gray in enumerate(tqdm.tqdm(loader, desc="Testing", leave=False)):
            gray = gray.to(device)
            pred_color = network(gray)
            result = F.to_pil_image((pred_color.cpu().squeeze() * 0.5) + 0.5)
            result.save(os.path.join(out_path, "{:06d}.png".format(i)))


def main():
    """
    runs the actual program
    """
    parser = argparse.ArgumentParser()
    parser.add_argument("model_path")
    parser.add_argument("dataset_path")
    parser.add_argument("--test", action="store_true")
    parser.add_argument("--out_path", type=str, default="out/default")
    args = parser.parse_args()

    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    if args.test:
        test(device, args.model_path, args.dataset_path, args.out_path)
    else:
        train(device, args.model_path, args.dataset_path)



if __name__ == "__main__":
    main()
