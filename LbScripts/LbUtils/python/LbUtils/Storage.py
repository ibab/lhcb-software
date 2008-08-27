""" utilities for storage manipulation """

try:
    from Numeric import log10
except ImportError:
    from math import log10

Factor = dict()
Factor["B"]  = 1
Factor["KB"] = 2**10
Factor["MB"] = 2**20
Factor["GB"] = 2**30
Factor["TB"] = 2**40
Factor["PB"] = 2**50

def convert(size, srcunit, trgunit):
    value = size
    if srcunit != trgunit :
        sizeinbytes = size * Factor[srcunit]
        trgfact = Factor[trgunit]
        value = round(float(sizeinbytes)/trgfact,int(log10(trgfact)))
    return value

def customPrint(pathname, size, unit):
    print pathname, convert(size, "B", unit)

def getHumanUnit(size, unit="B"):
    size = convert(size, unit, "B")
    hu = "B"
    for u in Factor.keys() :
        if size > 10**(int(log10(Factor[u]))) :
            if Factor[u] > Factor[hu] :
                hu = u
    return hu
        
class Unit(object):
    def __init__(self, size, size_unit=None):
        self._size = size # always in bytes
        self._display = None # display unit
        if size_unit is not None :
            self._size = convert(size, size_unit ,"B")
    def setDisplay(self, display):
        self._display = display
    def __str__(self):
        if self._display == "human" or self._display is None :
            n = getHumanUnit(self._size)
            strtoprint = "%s %s" % (convert(self._size, "B", n), n)
        elif self._display == "B" :
            strtoprint = "%s B" % self._size
        else :
            strtoprint = "%s %s" % (convert(self._size, "B", self._display), self._display)
        return strtoprint


def conv2petabytes(sizeinbytes):
    return convert(sizeinbytes, "B", "PB")
    
def conv2terabytes(sizeinbytes):
    return convert(sizeinbytes, "B", "TB")

def conv2gigabytes(sizeinbytes):
    return convert(sizeinbytes, "B", "GB")

def conv2megabytes(sizeinbytes):
    return convert(sizeinbytes, "B", "MB")

def conv2kilobytes(sizeinbytes):
    return convert(sizeinbytes, "B", "KB")

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

def printInPByte(pathname,size):
    print pathname, conv2petabytes(size), "PB"


def humanPrint(pathname,size):
    n = getHumanUnit(size)
    customPrint(pathname, size, n)


