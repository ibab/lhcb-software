__all__= ['path', 'stage', 'usage' ]
import os

def listdir(dirname=None ):
    dirlist = []
    nscmd = "nsls "
    if (dirname): 
        nscmd += dirname
    for line in os.popen(nscmd).xreadlines() :
        dirlist.append(line[:-1])
    return dirlist


def exists(pathname=None):
    ex = True
    nscmd = "nsls -l "
    if (pathname): 
        nscmd += pathname
    cmdout = os.popen(nscmd,'r')
    for line in cmdout.xreadlines() :
        if line.find("No such") != -1: ex = False
    return ex


def isdir(pathname=None):
    ex = False
    nscmd = "nsls -ld "
    if (pathname): nscmd += pathname
    for line in os.popen(nscmd).xreadlines() :
        if line[0]=="d" : ex = True
    return ex


def isfile(pathname=None):
    ex = False
    if not isdir(pathname) and exists(pathname): 
        ex = True
    return ex

def ismigrated(filename):
    ex = False
    nscmd = "nsls -l " + filename
    for line in os.popen(nscmd).xreadlines() :
        if line[0]=="m" : 
            ex = True
    return ex


def getsize(pathname=None):
    nscmd = "nsls -ld "
    if (pathname): nscmd += pathname
    for line in os.popen(nscmd).xreadlines() :
        elem = line.split()
        s = long(elem[4])
    return s


def _getdirs(pathname=None):
    ldir = []
    if isdir(pathname):
        nscmd = "nsls -l "
        if (pathname): nscmd += pathname
        for line in os.popen(nscmd).xreadlines() :
            if line[0]=="d" :
                elem = line.split()
                compname = elem[8]
                if (pathname) : compname = pathname + "/" + compname
                if isdir(compname): ldir.append(elem[8])
    return ldir

def _getfiles(pathname=None):
    lfile = []
    if isdir(pathname):
        nscmd = "nsls -l "
        if (pathname): nscmd += pathname
        for line in os.popen(nscmd).xreadlines() :
            if line[0]!="d" :
                elem = line.split()
                compname = elem[8]
                if (pathname) : compname = pathname + "/" + compname
                if not isdir(compname): lfile.append(elem[8])
    return lfile



def getchildren(pathname=None):
    lchild = []
    if isdir(pathname):
        nscmd = "nsls -l "
        if (pathname): nscmd += pathname
        for line in os.popen(nscmd).xreadlines() :
            elem = line.split()
            compname = elem[-1]
            if (pathname) : compname = pathname + "/" + compname
            lchild.append(elem[-1])
    return lchild

def walk(top, topdown=True, onerror=None):
    try:
        # Note that listdir and error are globals in this module due
        # to earlier import-*.
        names = listdir(top)
    except os.error, err:
        if onerror is not None:
            onerror(err)
        return

    dirs, nondirs = [], []
    for name in names:
        if isdir(top+"/"+name):
            dirs.append(name)
        else:
            nondirs.append(name)

    if topdown:
        yield top, dirs, nondirs
    for name in dirs:
        path = top + "/" + name
        for x in walk(path, topdown, onerror):
            yield x
    if not topdown:
        yield top, dirs, nondirs



__all__.append("walk")
