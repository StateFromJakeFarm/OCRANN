#!/usr/bin/python

from PIL import Image
from os import listdir
import os
import sys
import subprocess
import shutil
import glob

def toSquare(path, newPath, sideLen, highLo=None):
    """Crop image to character and save as square bitmap"""
    img = None
    if type(path) is str:
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
                    if x != imgX-1:
                        sawChar = True
                    else:
                        bounds[2] = x-1
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
                    if y != imgY-1:
                        sawChar = True
                    else:
                        bounds[3] = imgY-1
                elif x == imgX-1 and not sawChar:
                    bounds[3] = y-1

        if bounds[1] != -1 and not sawChar:
            break

    # bad strip
    if -1 in bounds:
        return

    img = img.crop(tuple(bounds)).resize((sideLen,sideLen), Image.LANCZOS)
    img.convert('RGB').save(newPath)

def createBmps(rawFolder, bmpFolder, sideLen):
    """Convert test images to bitmaps"""
    clearFolder(bmpFolder)

    for char in listdir(rawFolder):
        i = 0
        for f in listdir(os.path.join(rawFolder, char)):
            rawPath = os.path.join(rawFolder, char, f)
            bmpPath = os.path.join(bmpFolder, char+str(i)+'.bmp')
            print(rawPath + ' --(' + str(sideLen) + ',' + str(sideLen) + ')-> ' + bmpPath)
            toSquare(rawPath, bmpPath, sideLen)
            i += 1

def fullImgBmp(rawPath, bmpFolder, sideLen, highLo):
    clearFolder(bmpFolder)

    img = Image.open(rawPath).convert('L')
    imgX, imgY = img.size
    pixels = img.load()

    for y in range(imgY):
        for x in range(imgX):
            if pixels[x,y] <= highLo:
                pixels[x,y] = 0
            else:
                pixels[x,y] = 255

    bounds = [-1,0,imgX,imgY]
    i = 0
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
            i += 1
            toSquare(img.crop(tuple(bounds)), os.path.join(bmpFolder, str(i) + '.bmp'), sideLen)
            bounds = [-1,0,imgX,imgY]

def clearFolder(folderPath):
    if os.path.isdir(folderPath):
        shutil.rmtree(folderPath)
    os.makedirs(folderPath)

def runANN(argsList, giveString=False):
    ann = subprocess.Popen(argsList, stdout=subprocess.PIPE)
    outStr = ''
    curStr = ''
    while True:
        out = ann.stdout.read(1)
        if out == '' and ann.poll() != None:
            break
        if out != '':
            if giveString:
                if str(out) == '\n' or str(out) == '\0':
                    if len(curStr) == 1:
                        outStr += str(curStr)
                    curStr = ''
                else:
                    curStr += str(out)
            else:
                sys.stdout.write(out)
                sys.stdout.flush()

    return outStr
