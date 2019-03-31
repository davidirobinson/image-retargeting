
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

def minSeam(img, energy_map):
    h, w, _ = img.shape
    M = energy_map.copy()
    backtrack = np.zeros_like(M, dtype=np.int)

    # Start on 2nd row
    for row in range(1, h):
        for col in range(0, w):

            # Deal with first column
            if col == 0: prev_col_range = 0 
            else: prev_col_range = 1

            # Get idx of min energy from previous row, on 8-connected cols  
            idx = np.argmin( M[row - 1, col - prev_col_range : col + 2] )
            
            # Remember the idx so we can backtrack, and record the min energy value 
            backtrack[row, col] = idx + col - prev_col_range
            min_energy = M[row - 1, idx + col - prev_col_range]

            # Add to running tally of energy
            M[row, col] += min_energy
    return M, backtrack
    
def carveSeam(img, energy_cumulative, backtrack):
    h, w, _ = img.shape
    removal_mask = np.ones((h, w), dtype=bool)
    
    col = np.argmin(energy_cumulative[-1, :])

    for row in reversed(range(h)):
        removal_mask[row, col] = False
        col = backtrack[row, col]

    red = np.zeros(img.shape, np.uint8)
    red[:] = (0, 0, 255)
    
    fg = cv.bitwise_or(red, red, mask=np.invert(removal_mask).astype('uint8'))
    bg = cv.bitwise_or(img, img, mask=removal_mask.astype('uint8'))
    final = cv.bitwise_or(fg, bg)
    
    return final
    
def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("-i", "--image", required=True,
        help="Path to image")
    args = vars(ap.parse_args())
    
    input_img = cv.imread(args['image'], cv.IMREAD_COLOR)

    # 1) Create Energy Map
    energy_map = computeEnergy(input_img)

    # 2) Find 8-connected path of pixels with least energy
    energy_cumulative, backtrack = minSeam(input_img, energy_map)

    # 3) Delete all pixels in path
    output_img = carveSeam(input_img, energy_cumulative, backtrack)

    # 4) Loop until desired size reached
    

    # Display result
    cv.imshow('image', input_img)
    cv.imshow('energy', energy_map / energy_map.max())
    cv.imshow('output', output_img)
    cv.waitKey(0)
    cv.destroyAllWindows()

if __name__ == '__main__':
    main()