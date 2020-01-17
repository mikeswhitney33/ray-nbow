# ray-nbow
An automatic ray tracing shader using AI and algorithms.

## Getting Started:
### Dependencies:
* cmake
* opencv
Both of these dependencies can be installed with `brew` on a mac:
```
brew install cmake opencv
```

### Building
Once dependencies are installed, Use the following commands to build the program:
```
mkdir build && cd build
cmake ..
make
```
From the `build` directory, run the following to show the distances as an image:
```
./application/trace-scene path/to/scene
```
where path/to/scene is the path to the .scene file (a basic one is found in resources/scenes/basic.scene)