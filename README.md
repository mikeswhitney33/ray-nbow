# ray-nbow
An automatic ray tracing shader using AI and algorithms.

## Getting Started:
So far this program only has the dependency of `cmake`, which can be install using `brew` on a Mac:
```
brew install cmake
```
Once dependencies are installed, Use the following commands to build the program:
```
mkdir build && cd build 
cmake .. 
make 
```
From the `build` directory, run the following to see a simple sphere image made of the distances:
```
make test
```