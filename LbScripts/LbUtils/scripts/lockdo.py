#!/usr/bin/env python
"""
lockdo is used to execute shell commands, providing a file-based locking mechanism.
To avoid execution of the same <command> by different threads or machines in the same
time, each thread creates a `.lock_*` file in the current working directory based
on <launcher_unique_id> and <launcher_common_id> to decide if it is its turn to start
a <command>. If <launcher_common_id> is not provided - <command> is its default value.
"""
from LbUtils.Lock import Lock
import sys

def printUsage():
    print 'usage: lockdo.py command launcher_unique_id [launcher_common_id]'
    print __doc__
    print 'Please report bugs to karol.kruzelecki@cern.ch'
    print

if __name__ == '__main__':
    if len(sys.argv) < 3 or len(sys.argv) > 4:
        printUsage()
        sys.exit()
    if len(sys.argv) > 3: commonId = sys.argv[3]
    else: commonId = None
    Lock(sys.argv[1], sys.argv[2], commonId)
