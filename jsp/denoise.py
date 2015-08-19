#!/usr/bin/env python

from __future__ import print_function
import cv2
import utils


def copy_it(img):
    """
    Stupid denoise function
    """
    return img


def all_white(img):
    """
    Stupider denoise function
    """
    img[0:] = 255
    return img


def nlmeans(img):
    """
    Non-local means denoising
    """
    return cv2.fastNlMeansDenoising(img)


def main():

    fns1 = utils.noisy_fns()
    fns2 = utils.denoised_fns()

    for fn1, fn2 in zip(fns1, fns2):
        print(fn1, '->', fn2)

        # Get noisy image
        img1 = cv2.imread(fn1)

        # Denoise
        # img2 = all_white(img1)
        img2 = copy_it(img1)
        # img2 = nlmeans(img1)

        # Write it out
        cv2.imwrite(fn2, img2)


if __name__ == '__main__':
    main()
