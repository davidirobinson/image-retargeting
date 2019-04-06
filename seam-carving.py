
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
    
    # Calulate seam
    col = np.argmin(energy_cumulative[-1, :])
    for row in reversed(range(h)):
        removal_mask[row, col] = False
        col = backtrack[row, col]

    # Draw seam
    red = np.zeros(img.shape, np.uint8)
    red[:] = (0, 0, 255)
    fg = cv.bitwise_or(red, red, mask=np.invert(removal_mask).astype('uint8'))
    bg = cv.bitwise_or(img, img, mask=removal_mask.astype('uint8'))
    seam_img = cv.bitwise_or(fg, bg)

    # Apply seam
    stacked_mask = np.stack([removal_mask] * 3, axis=2)
    trimmed_img = img[stacked_mask].reshape((h, w-1, 3))
    
    return seam_img, trimmed_img  
    
def seamCarve(input_img, axis=1, visualize=False):
    energy_map = computeEnergy(input_img)
    if axis == 0: # Trim rows
        raise NotImplementedError
    elif axis == 1: # Trim cols
        energy_cumulative, backtrack = minSeam(input_img, energy_map)
        _, trimmed_img = carveSeam(input_img, energy_cumulative, backtrack)
    else:
        raise ValueError('Invalid axis value')
    return trimmed_img

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--image", required=True,
        help="Path to image")
    ap.add_argument("--width", required=False, default=1.0,
        help="Target width [0-1]")
    ap.add_argument("--height", required=False, default=1.0,
        help="Target height [0-1]")
    args = vars(ap.parse_args())
    
    img = cv.imread(args['image'], cv.IMREAD_COLOR)
    cv.imshow('input', img)

    trim_width = img.shape[1] - int(img.shape[1] * float(args['width']))
    for i in tqdm(range(trim_width)):
        img = seamCarve(img, axis=1, visualize=False)

    # Display result
    cv.imshow('output', img)
    cv.waitKey(0)
    cv.destroyAllWindows()

if __name__ == '__main__':
    main()