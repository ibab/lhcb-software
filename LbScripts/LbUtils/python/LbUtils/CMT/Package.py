from LbUtils import Env
import os
import logging
from subprocess import Popen, PIPE

class Package(object):
    def __init__(self, packagepath, parentproject=None):
        self._location = packagepath
        self._fulllocation = packagepath
        self._parentproject = parentproject
        self.setLocation()
        self._usedpackagelist = {}
    def __eq__(self, other):
        return self._fulllocation == other.fullLocation()
    def __hash__(self):
        return self._fulllocation.__hash__()
    def setLocation(self):
        if self._parentproject :
            strtoremove = self._parentproject.location() + os.sep
            self._location = self._location.replace(strtoremove,"")
    def location(self):
        return self._location
    def fullLocation(self):
        return self._fulllocation
    def parentProject(self):
        return self._parentproject
    def showUsedPackages(self, binary_list):
        log = logging.getLogger()
        wdir = os.path.join(self._fulllocation,"cmt")
        os.chdir(wdir)
        env = Env.getDefaultEnv()
        env["PWD"] = wdir
        for b in binary_list :
            env["CMTCONFIG"] = b
            p = Popen(["cmt", "show", "uses"], stdout=PIPE, stderr=PIPE, close_fds=True)
            for line in p.stdout:
                if not line.startswith("#"):
                    print line[:-1]
            for line in p.stderr:
                log.warning(line[:-1])
            retcode = os.waitpid(p.pid, 0)[1]
            log.debug("return code of 'cmt show uses' in %s is %s", wdir, retcode)
            if retcode != 0:
                log.warning("return code of 'cmt show uses' in %s is %s", wdir, retcode)
    def getUsedPackages(self, binary_list, projectlist=None):
        for b in binary_list:
            self._usedpackagelist[b] = self.getBinaryUsedPackages(b, projectlist)
        return self._usedpackagelist
    def getBinaryUsedPackages(self, binary, projectlist):
        packagelist = set()
        return packagelist
        
        
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
