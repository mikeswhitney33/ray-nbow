"""
This attempts to model the problem as a Generative Adversarial Network
"""
import argparse
from multiprocessing import cpu_count
import os

import torch
import torch.autograd as autograd
import torch.nn as nn
import torchvision as tv
import tqdm

from datasets import ColorDataset, GrayDataset
from models import UNet


def discriminator_model(in_c, out_c):
    """ makes the discriminator network"""
    model = tv.models.resnet18(pretrained=False)
    model.conv1 = nn.Conv2d(in_c, 64, 7, 2, 3, bias=False)
    channels = model.fc.in_features
    model.fc = nn.Linear(channels, out_c)
    return model


def try_iter(iterator, loader):
    """
        tries to get the next out of the iterator and if
        stop iteration gets raised, then we'll start the
        iterator over again
    """
    try:
        data = next(iterator)
    except StopIteration:
        iterator = iter(loader)
        data = next(iterator)
    return data, iterator


def calc_gp(device, disc_net, real_data, fake_data, lam):
    """ calculates gradient penalty """
    alpha = torch.rand(real_data.shape[2], 1)
    alpha = alpha.expand(real_data.size())
    alpha = alpha.to(device)

    interpolates = alpha * real_data + ((1 - alpha) * fake_data)
    interpolates = interpolates.to(device)
    interpolates = autograd.Variable(interpolates, requires_grad=True)

    disc_interpolates = disc_net(interpolates)

    gradients = autograd.grad(
        outputs=disc_interpolates,
        inputs=interpolates,
        grad_outputs=torch.ones(disc_interpolates.size()).to(device),
        create_graph=True,
        retain_graph=True,
        only_inputs=True)[0]
    gradient_penalty = ((gradients.norm(2, dim=1) - 1) ** 2).mean() * lam
    return gradient_penalty


def train(device, gen_model, disc_model, real_dataset_path, fake_dataset_path, epochs):
    """trains a gan"""
    train_transform = tv.transforms.Compose([
        tv.transforms.Resize((224, 224)),
        tv.transforms.RandomHorizontalFlip(0.5),
        tv.transforms.RandomVerticalFlip(0.5),
        tv.transforms.ToTensor(),
        tv.transforms.Normalize((0.5,), (0.5,))
    ])

    fakedataset = GrayDataset(fake_dataset_path, transform=train_transform)
    fakeloader = torch.utils.data.DataLoader(
        fakedataset,
        batch_size=min(16, len(fakedataset)),
        shuffle=True,
        num_workers=cpu_count(),
        drop_last=True)
    fakeiter = iter(fakeloader)

    realdataset = ColorDataset(real_dataset_path, transform=train_transform)
    realloader = torch.utils.data.DataLoader(
        realdataset,
        batch_size=min(16, len(fakedataset)),
        shuffle=True,
        num_workers=cpu_count(),
        drop_last=True)
    realiter = iter(realloader)

    discriminator = discriminator_model(3, 1024).to(device)
    disc_optimizer = torch.optim.Adam(discriminator.parameters(), lr=0.0001, betas=(0, 0.9))
    if os.path.exists(disc_model):
        discriminator.load_state_dict(torch.load(disc_model))

    generator = UNet(1, 3).to(device)
    gen_optimizer = torch.optim.Adam(generator.parameters(), lr=0.0001, betas=(0, 0.9))
    if os.path.exists(gen_model):
        generator.load_state_dict(torch.load(gen_model))

    one = torch.FloatTensor([1])
    mone = one * -1
    one = one.to(device).squeeze()
    mone = mone.to(device).squeeze()

    n_critic = 5
    lam = 10
    for _ in tqdm.trange(epochs, desc="Epochs"):
        for param in discriminator.parameters():
            param.requires_grad = True

        for _ in range(n_critic):
            real_data, realiter = try_iter(realiter, realloader)
            real_data = real_data.to(device)

            disc_optimizer.zero_grad()

            disc_real = discriminator(real_data)
            real_cost = torch.mean(disc_real)
            real_cost.backward(mone)

            fake_data, fakeiter = try_iter(fakeiter, fakeloader)
            fake_data = fake_data.to(device)
            disc_fake = discriminator(generator(fake_data))
            fake_cost = torch.mean(disc_fake)
            fake_cost.backward(one)

            gradient_penalty = calc_gp(device, discriminator, real_data, fake_data, lam)
            gradient_penalty.backward()

            disc_optimizer.step()
        for param in discriminator.parameters():
            param.requires_grad = False
        gen_optimizer.zero_grad()

        fake_data, fakeiter = try_iter(fakeiter, fakeloader)
        fake_data = fake_data.to(device)
        disc_g = discriminator(generator(fake_data)).mean()
        disc_g.backward(mone)
        gen_optimizer.step()

        torch.save(generator.state_dict(), gen_model)
        torch.save(discriminator.state_dict(), disc_model)


def test(device, gen_model, fake_dataset_path, out_dir):
    """tests a gan"""
    print("Test a gan")
    val_transform = tv.transforms.Compose([
        tv.transforms.Resize((224, 224)),
        tv.transforms.ToTensor(),
        tv.transforms.Normalize((0.5,), (0.5,))
    ])
    fakedataset = GrayDataset(fake_dataset_path, transform=val_transform)
    fakeloader = torch.utils.data.DataLoader(
        fakedataset,
        batch_size=1,
        shuffle=False,
        num_workers=cpu_count())
    generator = UNet(1, 3).to(device)
    if os.path.exists(gen_model):
        generator.load_state_dict(torch.load(gen_model))
    with torch.no_grad():
        generator.eval()
        for i, fake_data in enumerate(tqdm.tqdm(fakeloader)):
            fake_data = fake_data.to(device)
            fake = generator(fake_data)
            fake_im = fake.squeeze().cpu() * 0.5 + 0.5
            fake_im = tv.transforms.functional.to_pil_image(fake_im)
            fake_im.save(os.path.join(out_dir, "{:06d}.png".format(i)))


def main():
    """
    runs the main program
    """
    parser = argparse.ArgumentParser()
    parser.add_argument("--disc_model", default="networks/disc.pth")
    parser.add_argument("--gen_model", default="networks/gen.pth")
    parser.add_argument("--real_dataset_path", default="data/met")
    parser.add_argument("--fake_dataset_path", default="../build/resources/depth")
    parser.add_argument("--out_dir", default="out/default")
    parser.add_argument("--test", action="store_true")
    parser.add_argument("--epochs", type=int, default=10)
    args = parser.parse_args()

    device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")

    if args.test:
        test(device, args.gen_model, args.fake_dataset_path, args.out_dir)
    else:
        train(
            device,
            args.gen_model,
            args.disc_model,
            args.real_dataset_path,
            args.fake_dataset_path,
            args.epochs)


if __name__ == "__main__":
    main()
