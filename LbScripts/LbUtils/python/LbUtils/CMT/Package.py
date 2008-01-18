import os
import logging

class Package(object):
    def __init__(self, packagepath, parentproject=None):
        self._location = packagepath
        self._parentproject = parentproject
        self.setLocation()
    def setLocation(self):
        if self._parentproject :
            strtoremove = self._parentproject.location() + os.sep
            self._location = self._location.replace(strtoremove,"")
    def location(self):
        return self._location
    def parentProject(self):
        return self._parentproject
        
def hasRequirementsFile(dirpath):
    hasfile = False
    log = logging.getLogger()
    try:
        subfiles = os.listdir(dirpath)
        for f in subfiles:
            if f == "cmt" :
                try :
                    ssubf = os.listdir(os.path.join(dirpath,f))
                    for i in ssubf:
                        if i == "requirements" :
                            hasfile = True
                            return hasfile
                except OSError, msg :
                    log.warning("Cannot open path %s" % msg)
    except OSError, msg :
        log.warning("Cannot open path %s" % msg)
    return hasfile


def isPackage(path):
    isproj = False
    if os.path.islink(path):
        path = os.path.normpath(path)
    if os.path.isdir(path):
        if hasRequirementsFile(path):
            isproj = True
    return isproj


def onPackagePathError(error):
    log = logging.getLogger()
    log.warning("Cannot open path %s" % error.strerror)
