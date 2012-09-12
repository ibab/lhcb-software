#!/bin/env python
#
# Script to copy the data files to the InstallArea
#

import os
import logging
import shutil
from stat import S_IMODE, ST_MODE

# Preparing the option parser
from optparse import OptionParser
parser = OptionParser(usage = "%prog [options]",
                      description = "Script o update the externals with teh data")
parser.add_option("--debug", action = "store_true",
                  help = "Increase the verbosity level.")

# Parse command line
options, args = parser.parse_args()

# Preparing the logger
log = logging.getLogger("Patch")
if options.debug:
    verbosity = logging.DEBUG
else:
    verbosity = logging.INFO
log.setLevel(verbosity)

# Set global stream handler
hndlr = logging.StreamHandler()
hndlr.setFormatter(logging.Formatter('%(levelname)s: (%(name)s) %(message)s'))
logging.getLogger().handlers = [hndlr]

# Set umask in the same way install_project does it
# ( the code is taken from install_project.getGlobalMask() )
themask = os.umask(0)
os.umask(themask)
log.debug("Default umask is '%04o'" %themask)
if os.environ.get('MYSITEROOT'):
    authbits1 = 0777 - themask
    basedir = os.environ["MYSITEROOT"].split(os.path.pathsep)[0]
    authbits2 = S_IMODE(os.stat(basedir)[ST_MODE])
    log.debug("Found MYSITEROOT permissions imply '%04o' umask" % (0777 - authbits2))
    authbits = authbits1 | authbits2
    newmask = 0777 - authbits
    if themask != newmask:
        os.umask(newmask)
        themask = newmask
log.debug("Using '%04o' umask" %themask)

# Looking up the data directory, and teh destination area
log.info("In InstallAreaPatch PostInstall")
datadir = os.path.realpath(os.path.join(os.path.dirname(__file__), "..", "data"))
externaldir = os.environ['LCG_external_area']
log.info("Data dir is    : %s" % datadir)
log.info("External dir is: %s" % externaldir)

# Now iterating over teh source content to overwrite files
srcdir = datadir
dstdir = externaldir
ignorelist = [ ".svn" ]
for root, dirs, files in os.walk(srcdir, topdown=True) :
    dirs[:] = [ dn for dn in dirs if dn not in ignorelist ]
    for d in dirs:
        log.debug("Found dir: %s" % d)
        src = os.path.join(root, d)
        dst = src.replace(srcdir + os.sep, "")
        dst = os.path.join(dstdir, dst)
        #if not os.path.exists(dst) :
        #    log.info("Directory %s is missing, creating" % dst)
        #    os.makedirs(dst)

    for f in files :
        log.info("==> Processing file: %s" % f)
        src = os.path.join(root, f)
        dst = src.replace(srcdir + os.sep, "")
        dst = os.path.join(dstdir, dst)
        if os.path.exists(dst):
            log.info("%s Old size: %s" % (dst, os.path.getsize(dst)))
            log.info("Copying %s to %s" % (src, dst))
            shutil.copy2(src, dst)
            log.info("%s New size: %s" % (dst, os.path.getsize(dst)))
        else:
            log.info("Target does not exist skipping")


