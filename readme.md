# Seam Carving

## Overview
Implementation of the [seam carving algorithm](https://en.wikipedia.org/wiki/Seam_carving), which uses dynamic programming concepts to resize images in a manner which minimizes artifacts due to resizing.

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

## TODO
- Add timing info and report
- Support increasing the size of images
- Make agnostic to image channel size
- Show example image in readme
- Add docs to header