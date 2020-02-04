import torch 
import torchvision as tv
from tqdm import tqdm
import matplotlib.pyplot as plt


device = torch.device("cuda:1" if torch.cuda.is_available() else "cpu")


class MyDataset(torch.utils.data.Dataset):
    def __init__(self, subset, transform=None):
        super(MyDataset, self).__init__()
        self.subset = subset 
        self.transform = transform 

    def __getitem__(self, idx):
        x, y = self.subset[idx]
        if self.transform:
            x = self.transform(x)
        return x, y

    def __len__(self):
        return len(self.subset)


def resnet18(*args, **kwargs):
    model = tv.models.resnet18(*args, **kwargs)
    for param in model.parameters():
        param.requires_grad = False
    model.fc = torch.nn.Linear(512, 2)
    return model


train_transform = tv.transforms.Compose([
    tv.transforms.RandomResizedCrop(224),
    tv.transforms.Resize((224, 224)),
    tv.transforms.ToTensor(),
    tv.transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])
])
val_transform = tv.transforms.Compose([
    tv.transforms.Resize((224, 224)),
    tv.transforms.ToTensor(),
    tv.transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])
])
dataset = tv.datasets.ImageFolder("happiness_data")
trainset, valset = torch.utils.data.random_split(dataset, (len(dataset)-160,160))
trainset = MyDataset(trainset, train_transform)
valset = MyDataset(valset, val_transform)


train_loader = torch.utils.data.DataLoader(trainset, batch_size=32, shuffle=True, num_workers=16)
val_loader = torch.utils.data.DataLoader(valset, batch_size=32,shuffle=False,num_workers=16)


model = resnet18(pretrained=True).to(device)
criteria = torch.nn.CrossEntropyLoss()
optimizer = torch.optim.Adam(model.parameters())

i = 0
bssf = float('inf')
num_worse = 0
stop = False
losses = []
val_losses = []
while not stop:
    for x, y in train_loader:
        x, y = x.to(device), y.to(device)
        optimizer.zero_grad()
        outs = model(x)
        loss = criteria(outs, y)
        loss.backward()
        optimizer.step()
        i+=1
        if i % 5 == 0:
            losses.append(loss.item())
            with torch.no_grad():
                avg_val = 0
                for x, y in val_loader:
                    x, y = x.to(device), y.to(device)
                    outs = model(x)
                    loss = criteria(outs, y)
                    avg_val += loss.item() / len(val_loader)
                val_losses.append(avg_val)
            if avg_val < bssf:
                bssf = avg_val 
                num_worse = 0
            else:
                num_worse += 1
            print("{}: ({})Val Loss: {}".format(i, num_worse, avg_val))
            if num_worse >= 20:
                stop = True
                break


plt.plot(losses)
plt.plot(val_losses)
plt.show()
