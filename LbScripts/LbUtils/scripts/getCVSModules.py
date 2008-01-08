#!/usr/bin/env python

import os
from LbUtils import CVS
from optparse import OptionParser

if __name__ == '__main__':
    usage = "%prog cvsroot"

    parser = OptionParser(usage=usage, version="%prog v1r0")
    
    
    parser.set_defaults(verbose=False)
    parser.add_option("-q", "--quiet",
                      action="store_false", 
                      dest="verbose",
                      help="silent mode")
    parser.add_option("-v", "--verbose",
                      action="store_true", 
                      dest="verbose",
                      help="verbose mode")
    
    (options, args) = parser.parse_args()
    
    if not args:
        args.append(os.environ["CVSROOT"])
        
    Roots = []
    for a in args:
        Roots.append(CVS.Root(a))
        
    for r in Roots:
        print r.Modules()
        
