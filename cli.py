#!/usr/bin/python

from funcs import *
from PIL import Image
import sys

def main():
    img = toSquare('ImgTests/a/img037-003.png', 10)
    img.show()
    imageToInput(img, 'lol')


if __name__ == '__main__':
    main()
