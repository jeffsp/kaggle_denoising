from __future__ import print_function
import cv2
import glob
import sys


def ground_truth_fns():
    """
    Return list of ground truth filenames
    """
    return sorted(glob.glob('train_cleaned/*.png'))


def noisy_fns():
    """
    Return list of noisy filenames
    """
    return sorted(glob.glob('train/*.png'))


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
    fns = [fn.replace('train_cleaned', 'train_denoised') for fn in fns]
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


def grayscale_to_csv(filename, img):
    """
    Convert a png file to a CSV file
    """

    csv = ''

    for i in range(img.shape[0]):
        for j in range(img.shape[1]):
            value = img[i, j]
            assert value[0] == value[1]
            assert value[0] == value[2]
            csv += '%s_%s_%s,%s\n' % (filename, i, j, value[0])
    return csv
