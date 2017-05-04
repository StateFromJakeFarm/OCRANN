#!/usr/bin/python

from PIL import Image
from os import listdir
import os
import re

def toSquare(path, newPath, sideLen, highLo=None):
    """Crop image to character and save as square bitmap"""
    img = None
    if type(path) == 'str':
        img = Image.open(path).convert('L')
    else:
        img = path

    imgX, imgY = img.size

    pixels = img.load()

    # high-pass/low-pass filter
    if highLo:
        for y in range(imgY):
            for x in range(imgX):
                if pixels[x,y] <= highLo:
                    pixels[x,y] = 0
                else:
                    pixels[x,y] = 255

    bounds = [-1,-1,-1,-1]
    # start x, end x
    for x in range(imgX):
        sawChar = False
        for y in range(imgY):
            if bounds[0] == -1:
                if pixels[x,y] == 0:
                    bounds[0] = x
                    sawChar = True
            else:
                if pixels[x,y] == 0:
                    sawChar = True
                elif y == imgY-1 and not sawChar:
                    bounds[2] = x-1

        if bounds[0] != -1 and not sawChar:
            break

    # start y, end y
    for y in range(imgY):
        sawChar = False
        for x in range(imgX):
            if bounds[1] == -1:
                if pixels[x,y] == 0:
                    bounds[1] = y
                    sawChar = True
            else:
                if pixels[x,y] == 0:
                    sawChar = True
                elif x == imgX-1 and not sawChar:
                    bounds[3] = y-1

        if bounds[1] != -1 and not sawChar:
            break

    img = img.crop(tuple(bounds)).resize((sideLen,sideLen), Image.LANCZOS)
    img.convert('RGB').save(newPath)

def createBmps(rawFolder, bmpFolder, sideLen):
    """Convert test images to bitmaps"""
    if not os.path.isdir(bmpFolder):
        os.makedirs(bmpFolder)

    for char in listdir(rawFolder):
        i = 0
        for f in listdir(os.path.join(rawFolder, char)):
            rawPath = os.path.join(rawFolder, char, f)
            bmpPath = os.path.join(bmpFolder, char+str(i)+'.bmp')
            print(rawPath + ' --(' + str(sideLen) + ',' + str(sideLen) + ')-> ' + bmpPath)
            toSquare(rawPath, bmpPath, sideLen)
            i += 1

def fullImgBmps(rawPath, bmpFolder, sideLen, highLo=40):
    if not os.path.isdir(bmpfolder):
        os.makedirs(bmpFolder)

    img = Image.open(rawPath)
    imgX, imgY = img.size
    pixels = img.load()

    for y in range(imgY):
        for x in range(imgX):
            if pixels[x,y] <= highLo:
                pixels[x,y] = 0
            else:
                pixels[x,y] = 255

    bounds = [-1,0,imgX,imgY]
    for x in range(imgX):
        for y in range(imgY):
            if bounds[0] == -1:
                if pixels[x,y] == 0:
                    bounds[0] = x
            else:
                if pixels[x,y] == 255:
                    bounds[2] = x

                    toSquare(img.crop(tuple(bounds)), bmpFolder, sideLen)
                    bounds = [-1,0,imgX,imgY]
