"""
models.py
defining neural network models used for training
"""
import torch
import torch.nn as nn


def double_conv(in_c, out_c):
    """
    double_conv(in_c=int, out_c=int)
    --------------------------------
    performs two convolutions in a row

    in_c=int: the number of channels in
    out_c=int: the number of channels out
    returns: a layer that performs two 3x3 convolutions
    """
    return torch.nn.Sequential(
        nn.Conv2d(in_c, out_c, 3, 1, 1),
        nn.BatchNorm2d(out_c),
        nn.ReLU(),
        nn.Conv2d(out_c, out_c, 3, 1, 1),
        nn.BatchNorm2d(out_c),
        nn.ReLU())

def down_block(in_c, out_c):
    """
    down_block(in_c=int, out_c=int)
    -------------------------------
    performs a downsample on a feature map

    in_c=int: the number of channels in
    out_c=int: the number of channels out
    returns: a layer that performs a downsample followed by
        two 3x3 convolutions
    """
    return nn.Sequential(
        nn.MaxPool2d(2, 2),
        double_conv(in_c, out_c))


class UpBlock(nn.Module):
    """
    up_block(nn.Module)
    -------------------
    an implementation of a torch module that upsamples and concatenates two feature maps together
    """
    def __init__(self, in_c, out_c):
        super(UpBlock, self).__init__()
        self.up_sample = nn.ConvTranspose2d(in_c, out_c, 2, 2)
        self.dconv = double_conv(in_c, out_c)

    def forward(self, *args):
        input_1, input_2 = args
        return self.dconv(torch.cat([input_2, self.up_sample(input_1)], dim=1))


def up_block(in_c, out_c):
    """
    a convenience function for making an UpBlock layer
    """
    return UpBlock(in_c, out_c)


class UNet(torch.nn.Module):
    """
    UNet(nn.Module)
    ---------------
    an implementation of the UNet Architecture
    original paper found here: https://arxiv.org/pdf/1505.04597.pdf
    """
    def __init__(self, in_c, out_c):
        super(UNet, self).__init__()
        self.inconv = double_conv(in_c, 64) # 224
        self.down1 = down_block(64, 128) # 112
        self.down2 = down_block(128, 256) # 56
        self.down3 = down_block(256, 512) # 28
        self.down4 = down_block(512, 1024) # 14
        self.up1 = up_block(1024, 512)
        self.up2 = up_block(512, 256)
        self.up3 = up_block(256, 128)
        self.up4 = up_block(128, 64)
        self.outc = nn.Conv2d(64, out_c, 1)

    def forward(self, *args):
        net_input = args[0]
        layer1 = self.inconv(net_input)
        layer2 = self.down1(layer1)
        layer3 = self.down2(layer2)
        layer4 = self.down3(layer3)
        layer5 = self.down4(layer4)
        out = self.up1(layer5, layer4)
        out = self.up2(out, layer3)
        out = self.up3(out, layer2)
        out = self.up4(out, layer1)
        logits = self.outc(out)
        return logits