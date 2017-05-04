#!/usr/bin/python

from PIL import Image
from os import listdir
import os
import re

def toSquare(path, newPath, sideLen, highLo=None):
    img = Image.open(path).convert('L')
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

def createTestBmps(rawFolder, bmpFolder):
    for char in listdir(rawFolder):
        for f in listdir(os.path.join(rawFolder, char)):
            i = 0
            print(os.path.join(bmpFolder, char+str(i)+'.bmp'))
            toSquare(os.path.join(rawFolder, char, f), os.path.join(bmpFolder, char+str(i)+'.bmp'), 32)
            i += 1

def imageToInput(image, filePath):
    f = open(filePath, 'a')
    for val in image.getdata():
        if val == 255:
            f.write('0.1 ')
        else:
            f.write('0.9 ')

    f.write('\n')
    f.close()

def charOutIndex(char):
    if re.match('^[0-9]$', char):
        return int(char)
    elif re.match('^[A-Z]$', char):
        return int(char) - 55
    else:
        return int(char) - 32
