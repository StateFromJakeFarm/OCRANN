#!/usr/bin/python

from funcs import *
from PIL import Image
import sys
import os
import argparse

def main():
    parser = argparse.ArgumentParser(prog='ocr')
    parser.add_argument('-s', '--save', type=str, default=None,
        help='save trained weights to provided filename')
    parser.add_argument('-i', '--iters', type=int, default=300,
        help='number of training iterations to run over provided training set (default is 300)')
    parser.add_argument('-t', '--train', type=str, default=None,
        help='folder with training inputs')
    parser.add_argument('-r', '--read', type=str, default=None,
        help='folder with images to read')
    parser.add_argument('-s', '--structure', type=str, default=None,
        help='ANN structure file')
    parser.add_argument('-w', '--weights', type=str, default='RANDOM',
        help='starting weights file (default is random starting weights)')
    parser.add_argument('-e', '--encoding', type=str, default='62encoding.txt',
        help='encodings file (default is \"62encodings.txt\"')
    parser.add_argument('-a', '--alpha', type=float, default=0.01,
        help='the alpha value (default is 0.01)')

    parser.parse_args()


if __name__ == '__main__':
    main()
