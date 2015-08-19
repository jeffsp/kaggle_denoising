#!/usr/bin/env python

from __future__ import print_function
import cv2
import utils


def main():

    fns1 = utils.ground_truth_fns()
    fns2 = utils.noisy_fns()

    index = 0
    for (gt, noisy) in utils.gen_images(fns1, fns2):
        pass


if __name__ == '__main__':
    main()
