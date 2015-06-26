#!/usr/bin/env python

from __future__ import print_function
import cv2
import utils


def main():

    fns1 = utils.noisy_fns()
    fns2 = utils.denoised_fns()

    for fn1, fn2 in zip(fns1, fns2):
        print(fn1, '->', fn2)
        img1 = cv2.imread(fn1)
        img2 = img1
        cv2.imwrite(fn2, img2)


if __name__ == '__main__':
    main()
