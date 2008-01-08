from LbUtils.Links import showLinks

import sys
from optparse import OptionParser

if __name__ == '__main__':
    usage = "usage: %prog [options] package version hat"
    parser = OptionParser(usage=usage, version="%prog v1r0")
    
    
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
