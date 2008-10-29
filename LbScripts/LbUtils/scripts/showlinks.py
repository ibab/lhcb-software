from LbUtils.Links import showLinks
from LbUtils.CVS import CVS2Version

__version__ = CVS2Version("$Name: not supported by cvs2svn $", "$Revision: 1.2 $")

import sys
from optparse import OptionParser

if __name__ == '__main__':
    usage = "usage: %prog [options] package version hat"
    parser = OptionParser(usage=usage, version=__version__)
    
    
    parser.set_defaults(verbose=False)
    parser.add_option("-q", "--quiet",
                      action="store_false", 
                      dest="verbose",
                      help="silent")
    parser.add_option("-v", "--verbose",
                      action="store_true", 
                      dest="verbose",
                      help="verbose")
    for d in sys.argv[1:]:
        showLinks(d)
