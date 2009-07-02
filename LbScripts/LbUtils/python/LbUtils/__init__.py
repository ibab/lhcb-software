try :
    import subprocess
    import optparse
except ImportError:
    from _helpers import subprocess
    from _helpers import optparse


import sys
sys.modules["subprocess"] = subprocess
sys.modules["optparse"] = optparse
import subprocess
import optparse
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
    # Directory of the module
    basedir = os.path.dirname(__file__)
    # path to the source file
    src = os.path.join(basedir, "CMT_Project_Makefile")
    if overwrite or not os.path.exists(dest):
        # copy the content of
        from shutil import copyfile
        copyfile(src, dest)
