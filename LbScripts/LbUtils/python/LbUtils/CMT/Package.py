from LbUtils import Env
from LbUtils.Set import Set
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
                    line = line.replace("(no_auto_imports)","").replace("(","").replace(")","")
                    words = line[:-1].split()
                    words = words[1:]
                    if words[-1].endswith("/") :
                        words[-1] = words[-1][:-1]
                    pakpath =""
                    if len(words) == 4 :
                        pakpath = os.path.join(words[3], words[2], words[0])
                    elif len(words) == 3:
                        pakpath = os.path.join(words[2], words[0])
                        
                    if isPackage(pakpath):
                        print pakpath
                    else :
                        pakpath = os.path.join(pakpath,words[1])
                        if isPackage(pakpath):
                            print pakpath
                    
            for line in p.stderr:
                if line.startswith("#CMT> Warning:") and line.find("not found") != -1 :
                    log.debug(line[:-1])
                else : 
                    log.warning(line[:-1])
            retcode = os.waitpid(p.pid, 0)[1]
            log.debug("return code of 'cmt show uses' in %s is %s", wdir, retcode)
            if retcode != 0:
                log.warning("return code of 'cmt show uses' in %s is %s", wdir, retcode)
    def getUsedPackages(self, binary_list, projectlist=None):
        for b in binary_list:
            self._usedpackagelist[b] = self.getBinaryUsedPackages(b, projectlist)
        return self._usedpackagelist
    def getBinaryUsedPackages(self, binary, projectlist=None):
        from LbUtils.CMT.Project import Project, isProject, getProjectInstance
        packagelist = Set()
        log = logging.getLogger()
        wdir = os.path.join(self._fulllocation,"cmt")
        os.chdir(wdir)
        env = Env.getDefaultEnv()
        env["PWD"] = wdir
        env["CMTCONFIG"] = binary
        p = Popen(["cmt", "show", "uses"], stdout=PIPE, stderr=PIPE, close_fds=True)
        for line in p.stdout:
            if not line.startswith("#"):
                thepack = None
                theproj = None
                line = line.replace("(no_auto_imports)","").replace("(","").replace(")","")
                words = line[:-1].split()
                words = words[1:]
                if words[-1].endswith("/") :
                    words[-1] = words[-1][:-1]
                pakpath = ""
                if isProject(words[-1]):
                    theproj = Project(words[-1])
                    if projectlist :
                        if theproj not in projectlist:
                            projectlist.add(theproj)
                        else:
                            theproj = getProjectInstance(projectlist, words[-1])
                    
                if len(words) == 4 :
                    pakpath = os.path.join(words[3], words[2], words[0])
                elif len(words) == 3:
                    pakpath = os.path.join(words[2], words[0])
                if isPackage(pakpath):
                    thepack = Package(pakpath)
                else :
                    pakpath = os.path.join(pakpath,words[1])
                    if isPackage(pakpath):
                        thepack = Package(pakpath)
                if theproj and thepack :
                    if not theproj.hasContainedPackage(thepack):
                        theproj.addContainedPackage(pakpath)
                    thepack = getPackageInstance(theproj.containedPackages(), pakpath)
                if thepack:
                    packagelist.add(thepack)
        for line in p.stderr:
            if line.startswith("#CMT> Warning:") and line.find("not found") != -1 :
                log.debug(line[:-1])
            else : 
                log.warning(line[:-1])
        retcode = os.waitpid(p.pid, 0)[1]
        log.debug("return code of 'cmt show uses' in %s is %s", wdir, retcode)
        if retcode != 0:
            log.warning("return code of 'cmt show uses' in %s is %s", wdir, retcode)
        self._usedpackagelist[binary] = Set()
        for pak in packagelist :
            if  pak.hasConstituents(binary):
                self._usedpackagelist[binary].add(pak) 
        return self._usedpackagelist[binary]
    
    def binaryUsedPackages(self, binary):
        return self._usedpackagelist[binary]
    def showConstituents(self, binary):
        log = logging.getLogger()
        wdir = os.path.join(self._fulllocation,"cmt")
        os.chdir(wdir)
        env = Env.getDefaultEnv()
        env["PWD"] = wdir
        env["CMTCONFIG"] = binary
        p = Popen(["cmt", "show", "macro_value","all_constituents"], stdout=PIPE, stderr=PIPE, close_fds=True)
        for line in p.stdout:
            result = line[:-1].strip()
        for line in p.stderr:
            log.warning(line[:-1])
        retcode = os.waitpid(p.pid, 0)[1]
        log.debug("return code of 'cmt show macro_value all_constituents' in %s is %s", wdir, retcode)
        if retcode != 0:
            log.warning("return code of 'cmt show macro_value all_constituents' in %s is %s", wdir, retcode)
        return result
    def hasConstituents(self, binary):
        log = logging.getLogger()
        wdir = os.path.join(self._fulllocation,"cmt")
        os.chdir(wdir)
        env = Env.getDefaultEnv()
        env["PWD"] = wdir
        env["CMTCONFIG"] = binary
        p = Popen(["cmt", "show", "macro_value","all_constituents"], stdout=PIPE, stderr=PIPE, close_fds=True)
        for line in p.stdout:
            result = line[:-1].strip()
        for line in p.stderr:
            log.warning(line[:-1])
        retcode = os.waitpid(p.pid, 0)[1]
        log.debug("return code of 'cmt show macro_value all_constituents' in %s is %s", wdir, retcode)
        if retcode != 0:
            log.warning("return code of 'cmt show macro_value all_constituents' in %s is %s", wdir, retcode)
        return (result != "")
        
        
        
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

def getPackageInstance(packlist, packpath, absolute=True):
    for p in packlist :
        if absolute :
            if p.fullLocation() == packpath :
                return p
        else:
            if p.location() == packpath:
                return p
    else:
        return None

def onPackagePathError(error):
    log = logging.getLogger()
    log.warning("Cannot open path %s" % error.strerror)
