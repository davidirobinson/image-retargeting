
import argparse
import cv2 as cv
import numpy as np
from scipy.ndimage.filters import convolve

from tqdm import tqdm
import pdb

def computeEnergy(img):
    filtered_img = cv.GaussianBlur(img, (5, 5), 0)
    gray_img = cv.cvtColor(filtered_img, cv.COLOR_BGR2GRAY)
    dx = cv.Sobel(gray_img, cv.CV_64F, 1, 0, ksize=3,
        scale=1, delta=0, borderType=cv.BORDER_DEFAULT)
    dy = cv.Sobel(gray_img, cv.CV_64F, 0, 1, ksize=3,
        scale=1, delta=0, borderType=cv.BORDER_DEFAULT)
    return np.abs(dx) + np.abs(dy)

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("-i", "--image", required=True,
        help="Path to image")
    args = vars(ap.parse_args())
    
    input_img = cv.imread(args['image'], cv.IMREAD_COLOR)

    # 1) Create Energy Map
    energy_map = computeEnergy(input_img)

    # 2) Find 8-connected path of pixels with least energy

    # 3) Delete all pixels in path

    # 4) Loop until desired size reached

    # Display result
    cv.imshow('image', input_img)
    cv.imshow('energy', energy_map / energy_map.max())
    cv.waitKey(0)
    cv.destroyAllWindows()

if __name__ == '__main__':
    main()