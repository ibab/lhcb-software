#!/usr/bin/env python


from LbUtils import castor
from os import environ
from LbUtils.Storage import humanPrint


def getdirsize(pathname=None,verbose=False,printfunc=humanPrint):
    size = 0
    if castor.path.isdir(pathname):
        for child in castor.path.getchildren(pathname):
            if (pathname) : child = pathname + "/" + child
            size += getdirsize(child,verbose,printfunc)
    else:
        size = castor.path.getsize(pathname)
    if verbose:
        if pathname: 
            printfunc(pathname, size)
        else:
            printfunc(environ["CASTOR_HOME"], size)
    return size


