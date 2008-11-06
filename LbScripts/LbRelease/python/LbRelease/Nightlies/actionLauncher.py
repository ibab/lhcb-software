#!/usr/bin/env python
"""
script to be launched only by LCG Nightlies mechanisms.

Provides a set of handles for LCG Nightlies "builders" generated automatically
for LHCb projects to start get, config, install, make, and test cmt actions.
"""

import sys
from functions import *

def printUsage():
    print 'usage: actionLauncher.py get|config|install|test projectName [slotName]'
    print '                         make projectName minus_j_value [slotName]'
    print '       - `slotName` if provided overrides LCG_SLOT env. variable'

if __name__ == '__main__':
    if len(sys.argv) <3:
        printUsage()
        sys.exit()
    function = {
                 'config' : config,
                 'get' : get,
                 'install' : install,
                 'test' : test,
                }
    if sys.argv[1] == 'make' and len(sys.argv) > 3:
        if len(sys.argv) > 4: slotName = sys.argv[4]
        else: slotName = os.environ['LCG_SLOT']
        make(slotName, sys.argv[2], int(sys.argv[3]))
    elif sys.argv[1] in function.keys():
        if len(sys.argv) > 3: slotName = sys.argv[3]
        else: slotName = os.environ['LCG_SLOT']
        projectName = sys.argv[2]
        function[sys.argv[1]](slotName, projectName)
    else:
        printUsage()
        sys.exit()
