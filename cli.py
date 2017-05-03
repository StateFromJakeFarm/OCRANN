#!/usr/bin/python

from funcs import *
from PIL import Image
import sys

def main():
    img = toSquare('ImgTests/a/img037-003.png', 50)
    img.show()


if __name__ == '__main__':
    main()
