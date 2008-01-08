""" utilities for storage manipulation """

try:
    from Numeric import log10
except ImportError:
    from math import log10

def conv2terabytes(sizeinbytes):
    tbfact = 2**40
    sizeinTBytes = float(sizeinbytes)/tbfact  
    return round(sizeinTBytes,int(log10(tbfact)))

def conv2gigabytes(sizeinbytes):
    gbfact = 2**30
    sizeinGBytes = float(sizeinbytes)/gbfact  
    return round(sizeinGBytes,int(log10(gbfact)))


def conv2megabytes(sizeinbytes):
    mbfact = 2**20
    sizeinMBytes = float(sizeinbytes)/mbfact  
    return round(sizeinMBytes,int(log10(mbfact)))


def conv2kilobytes(sizeinbytes):
    kbfact = 2**10
    sizeinKBytes = float(sizeinbytes)/kbfact  
    return round(sizeinKBytes,int(log10(kbfact)))

def printInByte(pathname,size):
    print pathname,size, "B"

def printInKByte(pathname,size):
    print pathname, conv2kilobytes(size), "KB"

def printInMByte(pathname,size):
    print pathname, conv2megabytes(size), "MB"

def printInGByte(pathname,size):
    print pathname, conv2gigabytes(size), "GB"

def printInTByte(pathname,size):
    print pathname, conv2terabytes(size), "TB"


def humanPrint(pathname,size):
    if (size>10**9): 
        printInGByte(pathname,size)
    elif (size>10**6): 
        printInMByte(pathname,size)
    elif (size>10**3): 
        printInKByte(pathname,size)
    else: 
        printInByte(pathname,size)
