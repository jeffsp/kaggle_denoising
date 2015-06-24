#!/usr/bin/env python

from __future__ import print_function
import sys
import glob
import pylab

def get_fns():
    """
    return list of dirty/clean image pairs
    """
    fns1 = sorted(glob.glob('train/*.png'))
    fns2 = sorted(glob.glob('train_cleaned/*.png'))
    assert len(fns1) == len(fns2)
    return zip(fns1, fns2)

def main():
    fns = get_fns()
    for fn in fns:
        print(fn[0], fn[1])
        sys.stdout.flush()
        img1 = pylab.imread(fn[0])
        img2 = pylab.imread(fn[1])

if __name__ == '__main__':
    main()
