#!/usr/bin/env python


from common import isdir, getchildren, getsize
from os import environ
from LbUtils.Storage import humanPrint


def getdirsize(pathname=None,verbose=False,printfunc=humanPrint):
    size = 0
    if isdir(pathname):
        for child in getchildren(pathname):
            if (pathname) : child = pathname + "/" + child
            size += getdirsize(child,verbose,printfunc)
    else:
        size = getsize(pathname)
    if verbose:
        if pathname: 
            printfunc(pathname, size)
        else:
            printfunc(environ["CASTOR_HOME"], size)
    return size


