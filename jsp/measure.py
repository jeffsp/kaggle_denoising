#!/usr/bin/env python

from __future__ import print_function
import sys
import glob
import cv2
import numpy
import math
import utils


def main():

    fns1 = utils.ground_truth_fns()
    fns2 = utils.denoised_fns()

    sse = 0.0
    total = 0
    for (gt, pred) in utils.gen_images(fns1, fns2):
        err = cv2.absdiff(gt, pred)
        sse += numpy.sum((err.astype("float") / 255) ** 2)
        total += pred.size

    print(math.sqrt(sse / total))


if __name__ == '__main__':
    main()
