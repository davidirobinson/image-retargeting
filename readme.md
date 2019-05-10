# Image Retargeting

A C++ implementation of the [seam carving algorithm](https://en.wikipedia.org/wiki/Seam_carving) to perform content-aware image resizing. THe algorithm works by searching for the lowest energy 'seam' in the image using dynamic programming techniques, and then removing that seam to reduce the size of the image, as below.

![seam-carving-demo](https://drive.google.com/uc?export=download&id=1m9_rYT0iicvadZGN3bbqUrUBroS_KZlR)

## Usage
### Dependencies
OpenCV >= 3.0

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