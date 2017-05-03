#!/usr/bin/python

from PIL import Image

def imageCrop(path, sideLen, highLo=None):
    img = Image.open(path).convert('L')
    imgX, imgY = img.size

    pixels = img.load()
    bounds = (0,0,0,0)
    # upper bound
    for y in range(imgY):
        for x in range(imgX):
            if pixels[x,y] == 0:
                bounds[0] = y

    # left bound
    for x in range(imgX):
        for y in range(imgY):
            if pixels[x,y] == 0:
                bounds[1] = x

    # lower bound
    for y in reversed(range(imgY)):
        for x in range(imgX):
            if pixels[x,y] == 0:
                bounds[2] = y

    # right bound
    for x in reversed(range(imgX)):
        for y in range(imgY):
            if pixels[x,y] == 0:
                bounds[3] = x
