#!/usr/bin/env python

from __future__ import print_function
import sys
import glob
import cv2
import numpy
import math

def get_fns():
    """
    return list of dirty/clean image pairs
    """
    fns1 = sorted(glob.glob('train_denoised/*.png'))
    fns2 = sorted(glob.glob('train_cleaned/*.png'))
    assert len(fns1) == len(fns2)
    return zip(fns1, fns2)

def main():
    fns = get_fns()
    sse = 0.0
    total = 0
    for fn in fns:
        print(fn[0], fn[1])
        sys.stdout.flush()
        img1 = cv2.imread(fn[0])
        img2 = cv2.imread(fn[1])
        img1 = cv2.cvtColor(img1, cv2.COLOR_BGR2GRAY);
        img2 = cv2.cvtColor(img2, cv2.COLOR_BGR2GRAY);
        assert img1.shape == img2.shape
        err = cv2.absdiff (img1, img2)
        sse += numpy.sum((err.astype("float") / 255) ** 2)
        total += img1.shape[0] * img1.shape[1]

    print(math.sqrt(sse / total))


if __name__ == '__main__':
    main()
