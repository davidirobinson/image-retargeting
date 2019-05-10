# Image Retargeting

## Overview
Implementation of the [seam carving algorithm](https://en.wikipedia.org/wiki/Seam_carving), which uses dynamic programming concepts to resize images in a manner which minimizes artifacts due to resizing.

![seam-carving-demo](https://drive.google.com/uc?export=download&id=1m9_rYT0iicvadZGN3bbqUrUBroS_KZlR)

## Usage
### Dependencies
OpenCV > 3.0

### Build
```
mkdir build && cd build/
cmake ..
make -j<#-cores>
```

### Demo
```
cd build/src/
./seam_carving_demo -i <path-to-image> -w <%-width> -h <%-height>
```