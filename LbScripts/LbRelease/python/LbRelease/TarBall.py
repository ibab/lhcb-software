"""
Generic Tar Ball module
"""

import os.path
from LbUtils.Temporary import TempDir
import tarfile
import shutil

class TarBall(object):
    def __init__(self, name, srcloc=None):
        self._name = ""
        self.setName(name)
        self._srcLoc = []
        self.addSourceLocation(srcloc)
        self._exclude = list()
        self._converter = dict()
        self._destLoc = None
        if not self._destLoc:
            self._destLoc = os.path.abspath(os.getcwd())
        self._tmpdir = TempDir(self._name)
        print self._tmpdir
    def setName(self, name):
        self._name = name
    def getName(self):
        return self._name
    def addSourceLocation(self, srcloc):
        self._srcLoc += os.path.abspath(srcloc)
    def getSourceLocation(self):
        return self._srcLoc
    def addExclusion(self, pattern):
        self._exclude += pattern
    def addConverter(self, target, source):
        """ will convert the source into the target before the 
            creation of the tar ball. A target can have many sources """
        self._converter.setdefault(target, []).append(source)
    def explodeSources(self):
        for s in self._srcLoc:
            if os.path.isfile(s):
                if tarfile.is_tarfile(s):
                    srctar = tarfile.open(s,"r:*")
                    srctar.extractall(self._tmpdir.getName())
                    srctar.close()
                else:
                    print "%s is not a tar file. Skipping ..." % s
            else:
                shutil.copytree(s, self._tmpdir.getName())

    def create(self):
        curdir = os.getcwd()
        os.chdir(self._tmpdir.getName())
        os.chdir(curdir)


class BinaryTarBall(TarBall):
    """ Binary tar ball. Only include the selected binary. Thus
        filter out the unwanted other binaries """
    def __init__(self, name, srcloc, binary):
        super(BinaryTarBall, self).__init__(name, srcloc)
        self._binaries = [binary]
    def addBinary(self, binary):
        self._binaries += binary

if __name__ == '__main__':
    pass