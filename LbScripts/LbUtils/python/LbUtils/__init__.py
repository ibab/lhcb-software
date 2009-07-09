try :
    import subprocess
    import optparse
except ImportError:
    from _helpers import subprocess
    from _helpers import optparse


import sys
sys.modules["subprocess"] = subprocess
sys.modules["optparse"] = optparse
del sys


import Env, Option
import CMT, Log, Set
import afs, castor
import Storage
import Script
import SelfExtract
import Temporary
import Tar

## Write the generic Makefile for CMT projects.
#  @param filename: the name of the destination file
#  @param overwrite: flag to decide if an already present file has to be kept or not (default is False)
def createProjectMakefile(dest, overwrite = False):
    import os
    if overwrite or not os.path.exists(dest):
        f = open(dest, "w")
        f.write("include ${LBUTILSROOT}/data/Makefile\n")
        f.close()
