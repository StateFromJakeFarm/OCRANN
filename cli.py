#!/usr/bin/python

from funcs import *
import os
import argparse

def main():
    # Parse command line args
    parser = argparse.ArgumentParser(prog='ocr')
    parser.add_argument('-i', '--iters', type=int, default=300,
        help='number of training iterations to run over provided training set (default is 300)')
    parser.add_argument('-t', '--train', type=str,
        help='folder with training inputs')
    parser.add_argument('-r', '--read', type=str,
        help='folder with images to read')
#    parser.add_argument('-s', '--structure', type=str,
#        help='ANN structure file')
    parser.add_argument('-w', '--weights', type=str, default='RANDOM',
        help='starting weights file (default is random starting weights)')
#    parser.add_argument('-e', '--encoding', type=str, default='62encoding.txt',
#        help='encodings file (default is \"62encodings.txt\"')
#    parser.add_argument('-a', '--alpha', type=float, default=0.01,
#        help='the alpha value (default is 0.01)')
    parser.add_argument('-b', '--bmp', type=str, default=None,
        help='convert images in directory to bmps')
    parser.add_argument('-s', '--side', type=int, default=10,
        help='length of side of bmp image (default is 10)')
    parser.add_argument('-l', '--highlo', type=int, default=60,
        help='high-pass/low-pass threshold (default is 60)')
    args = parser.parse_args()

    if args.bmp:
        createBmps(args.bmp, args.bmp.strip('/') + 'Bmps', args.side)

    if args.read:
        fullImgBmps(args.read, args.read.split('.')[0] + 'Bmps', args.side, args.highlo)


if __name__ == '__main__':
    main()
