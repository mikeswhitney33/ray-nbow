import torch 
import torchvision as tv
from tqdm import tqdm
import matplotlib.pyplot as plt


def resnet18(*args, **kwargs):
    model = tv.models.resnet18(*args, **kwargs)
    for param in model.parameters():
        param.requires_grad = False
    model.fc = torch.nn.Linear(512, 2)
    return model

model = resnet18(pretrained=True)

transform = tv.transforms.Compose([
    tv.transforms.RandomResizedCrop(224),
    tv.transforms.Resize((224, 224)),
    tv.transforms.ToTensor(),
    tv.transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])
])
dataset = tv.datasets.ImageFolder("happiness_data", transform=transform)
loader = torch.utils.data.DataLoader(dataset, batch_size=32, shuffle=True, num_workers=4)
criteria = torch.nn.CrossEntropyLoss()
optimizer = torch.optim.Adam(model.parameters())

losses = []
for x, y in tqdm(loader):
    optimizer.zero_grad()
    outs = model(x)
    loss = criteria(outs, y)
    losses.append(loss.item())
    loss.backward()
    optimizer.step()
plt.plot(losses)
plt.show()
