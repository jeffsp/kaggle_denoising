from __future__ import print_function
import cv2
import glob
import sys
import itertools
import numpy as np


def ground_truth_fns():
    """
    Return list of ground truth filenames
    """
    return sorted(glob.glob('input_cleaned/*.png'))


def noisy_fns():
    """
    Return list of noisy filenames
    """
    return sorted(glob.glob('input/*.png'))


def test_fns():
    """
    Return list of the test filenames -- the ones for submission
    """
    return sorted(glob.glob('test/*.png'))


def test_denoised_fns():
    """
    Return list of the densoied test filenames.
    """
    return sorted(glob.glob('test_denoised/*.png'))


def denoised_fns():
    """
    Return list of denoised filenames

    Don't glob the filenames, instead generate them from
    the ground truth filenames.
    """
    fns = ground_truth_fns()
    fns = [fn.replace('input_cleaned', 'input_denoised') for fn in fns]
    return sorted(fns)


def gen_images(fns1, fns2):
    """
    Grayscale image generator
    """

    assert len(fns1) == len(fns2)

    for fn1, fn2 in zip(fns1, fns2):

        print(fn1, fn2)
        sys.stdout.flush()

        # Read files
        img1 = cv2.imread(fn1)
        img2 = cv2.imread(fn2)
        assert img2.shape == img1.shape

        # Convert to grayscale
        img1 = cv2.cvtColor(img1, cv2.COLOR_BGR2GRAY)
        img2 = cv2.cvtColor(img2, cv2.COLOR_BGR2GRAY)
        assert img2.shape == img1.shape

        yield(img1, img2)


def get_neighbors(noisy, sz):
    """
    Get sz by sz neighbors and for each pixel value
    Note: If grayscale, this will be sz x sz, if RGB, will be sz x sz x 3
    Use grayscale
    """
    window = (sz, sz)
    neighbors = [cv2.getRectSubPix(noisy, window, (y, x)).ravel() \
        for x, y in itertools.product(range(noisy.shape[0]), range(noisy.shape[1]))]
    neighbors = np.asarray(neighbors)
    return (neighbors / 255.0).astype('float32')        


def get_ground_truth(ground_truth):
    """
    Return the ground truth pixel values
    """
    return (ground_truth / 255.0).astype('float32').ravel()


def grayscale_to_csv(filename, img):
    """
    Convert a png file to a CSV file
    """

    csv = ''

    for j in range(img.shape[1]):
        for i in range(img.shape[0]):
            value = img[i, j]
            assert value[0] == value[1]
            assert value[0] == value[2]
            csv += '%s_%s_%s,%s\n' % (filename, i+1, j+1, value[0] / 255.0)
    return csv
