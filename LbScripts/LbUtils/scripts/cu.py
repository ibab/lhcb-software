#!/usr/bin/env python
"""
Main program for castor usage.
"""

from LbUtils.Storage import humanPrint, printInByte, printInKByte
from LbUtils.Storage import printInMByte, printInGByte, printInTByte
from LbUtils.castor.usage import getdirsize
from LbUtils.CVS import CVS2Version
from optparse import OptionParser
from os import environ

__version__ = CVS2Version("$Name: not supported by cvs2svn $", "$Revision: 1.3 $")


def print_in_bytes(option, opt_str, value, parser):
    """ callback to print in bytes """
    setattr(parser.values, option.dest, printInByte)
def print_in_kilobytes(option, opt_str, value, parser):
    """ callback to print in kilobytes """    
    setattr(parser.values, option.dest, printInKByte)
def print_in_megabytes(option, opt_str, value, parser):
    """ callback to print in megabytes """
    setattr(parser.values, option.dest, printInMByte)
def print_in_gigabytes(option, opt_str, value, parser):
    """ callback to print in gigabytes """
    setattr(parser.values, option.dest, printInGByte)
def print_in_terabytes(option, opt_str, value, parser):
    """ callback to print in terabytes """
    setattr(parser.values, option.dest, printInTByte)
def print_for_human(option, opt_str, value, parser):
    """ callback to print in human readable way """
    setattr(parser.values, option.dest, humanPrint)

if __name__ == '__main__':
    
    usage = "usage: %prog [options] castor_path1 castor_path2 ..."
    parser = OptionParser(usage=usage, version=__version__, description=__doc__)
    
    
    parser.set_defaults(verbose=False)
    parser.add_option("-s", "--silent",
                      action="store_false", 
                      dest="verbose",
                      help="silent")    
    parser.add_option("-q", "--quiet",
                      action="store_false", 
                      dest="verbose",
                      help="silent")
    parser.add_option("-v", "--verbose",
                      action="store_true", 
                      dest="verbose",
                      help="verbose")
    
    
    parser.set_defaults(printfunc=humanPrint)
    parser.add_option("-b", "-B", "--bytes",
                      action="callback", 
                      callback=print_in_bytes,
                      dest="printfunc",
                      help="result in Bytes")
    parser.add_option("-k", "-K", "--kilo",
                      action="callback", 
                      callback=print_in_kilobytes,
                      dest="printfunc",
                      help="result in KiloBytes")
    parser.add_option("-m", "-M", "--mega",
                      action="callback", 
                      callback=print_in_megabytes,
                      dest="printfunc",
                      help="result in MegaBytes")
    parser.add_option("-g", "-G", "--giga",
                      action="callback", 
                      callback=print_in_gigabytes,
                      dest="printfunc",
                      help="result in GigaBytes")
    parser.add_option("-t", "-T", "--tera",
                      action="callback", 
                      callback=print_in_terabytes,
                      dest="printfunc",
                      help="result in TeraBytes")
    parser.add_option("-H", "--human",
                      action="callback", 
                      callback=print_for_human,
                      dest="printfunc",
                      help="human readable results")
    
    (options, args) = parser.parse_args()
    
    if args:
        tot = 0
        size = dict()
        for p in args:
            size[p] = getdirsize(p, options.verbose, options.printfunc)
            tot += size[p]
        if len(args) > 1 :
            for p in args:
                options.printfunc(p, size[p])
            options.printfunc("Total size:", tot)
            
    else:
        size  = getdirsize(None, options.verbose, options.printfunc)
        if not options.verbose:
            options.printfunc(environ["CASTOR_HOME"], size)



