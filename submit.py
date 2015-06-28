#!/usr/bin/env python

from __future__ import print_function
import cv2
import utils
import os


def main():

    fns1 = utils.test_denoised_fns()
    fns2 = [fn.replace('png', 'csv') for fn in fns1]

    for fn1, fn2 in zip(fns1, fns2):

        print(fn1, '->', fn2)

        img1 = cv2.imread(fn1)

        # Convert
        base_name = os.path.basename(fn2)
        filename = os.path.splitext(base_name)[0]
        img2 = utils.grayscale_to_csv(filename, img1)

        # Write it out
        with open(fn2, 'w') as fobj:
            fobj.write(img2)


if __name__ == '__main__':
    main()
