
import os
import sys
import logging

class TarBall:
    def __init__(self, filename, sourcedir=None, tag=None, targ_list=None):
        self._filename = filename 
        self._srcdir = sourcedir
        self._usetemp = True
    def dump(self):
        pass


def createTarBall(filename, sourcedir, tag=None, tag_list=None, usetemp=True):
    log = logging.getLogger()

    if os.path.exists(filename) :
        log.error("cannot create %s tarball. File %s already exists" % filename)
        sys.exit(1)

    tb = TarBall(filename, sourcedir, tag, tag_list)
    tb.dump()
