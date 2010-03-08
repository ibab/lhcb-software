#!/usr/bin/env python
"""
script to be launched only by LCG Nightlies mechanisms.

Provides a set of handles for LCG Nightlies "builders" generated automatically
for LHCb projects to start get, config, install, make, and test cmt actions.
"""

import os, sys
try:
    from functions import *
except ImportError:
    # This is needed to be able to launch the submodule with "python -m",
    # because it seems the sys.path is not the same than what we get when
    # simply importing it.
    from LbRelease.Nightlies.functions import *

def printUsage():
    print 'usage: actionLauncher.py get|config|install|test projectName [slotName]'
    print '                         make projectName minus_j_value minus_l_value [slotName]'
    print '       - `slotName` if provided overrides LCG_SLOT env. variable'

if __name__ == '__main__':
    if len(sys.argv) <3:
        printUsage()
        sys.exit()
    if 'PWD' in os.environ:
        del os.environ['PWD']
    function = {
                 'config' : config,
                 'get' : get,
                 'install' : install,
                 'test' : test,
                }
    if sys.argv[1] == 'make' and len(sys.argv) > 4:
        if len(sys.argv) > 5: slotName = sys.argv[5]
        else: slotName = os.environ['LCG_SLOT']
        make(slotName, sys.argv[2], int(sys.argv[3]), float(sys.argv[4]))
    elif sys.argv[1] in function.keys():
        if len(sys.argv) > 3: slotName = sys.argv[3]
        else: slotName = os.environ['LCG_SLOT']
        projectName = sys.argv[2]
        function[sys.argv[1]](slotName, projectName)
    else:
        printUsage()
        sys.exit()
