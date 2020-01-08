# Image Retargeting

A C++ implementation of the [seam carving algorithm](https://en.wikipedia.org/wiki/Seam_carving) to perform content-aware image resizing. The algorithm works by searching for the lowest energy 'seam' in the image using dynamic programming techniques, and then removing that seam to reduce the size of the image, as below.

![seam-carving-demo](https://drive.google.com/uc?export=download&id=1m9_rYT0iicvadZGN3bbqUrUBroS_KZlR)

## Usage

### Dependencies

- [OpenCV (3.x)](https://opencv.org/)

### Build

    mkdir -p build && cd build/
    cmake .. && make -j<#-cores>

### Run

    cd build/
    ./src/seam_carving_demo -i <path-to-image> -w <%-width> -h <%-height>
