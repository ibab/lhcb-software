#!/usr/bin/env python


from LbUtils import castor
from os import environ
from LbUtils.Storage import humanPrint


def getdirsize(pathname=None,verbose=False,printfunc=humanPrint):
    size = 0
    if castor.isdir(pathname):
        for child in castor.getchildren(pathname):
            if (pathname) : child = pathname + "/" + child
            size += getdirsize(child,verbose,printfunc)
    else:
        size = castor.getsize(pathname)
    if verbose:
        if pathname: 
            printfunc(pathname, size)
        else:
            printfunc(environ["CASTOR_HOME"], size)
    return size


