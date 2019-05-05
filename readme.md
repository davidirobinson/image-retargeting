# Image Retargeting

## Overview
Implementation of the [seam carving algorithm](https://en.wikipedia.org/wiki/Seam_carving), which uses dynamic programming concepts to resize images in a manner which minimizes artifacts due to resizing.

TODO: Show example image

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
./image_retargeter_demo -i <path-to-image> -w <%-width> -h <%-height>
```

## TODO
- Wrap implementation in class
- Implement Callback