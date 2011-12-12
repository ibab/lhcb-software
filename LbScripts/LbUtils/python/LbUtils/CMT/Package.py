# local imports

import Common


# package imports
from LbUtils import Env
from LbUtils.Set import Set

# global imports

from time import gmtime, strftime
import os, re, sys
import logging
from subprocess import Popen, PIPE


class Package(object):
    def __init__(self, packagepath, parentproject=None, parentprojectpath=None):
        packagepath = os.path.realpath(packagepath)
        self._location = packagepath
        self._fulllocation = packagepath
        self._parentproject = parentproject
        self._parentprojectpath = parentprojectpath
        self._version = None
        self._realversion = None
        self._corename = None
        self._name = None
        self._hat = None
        self._constituents = None
        self._allconstituents = None
        self._usedsameprojpaklist = None
        self._usedpaklist = None
        self._allusedpaklist = None
        self._extpaklist = None
        self._allextpaklist = None
        self._binarylist = None
        self._hasconstituents = None
        self._usedbybinary = None
        self._release_date = None
    def __eq__(self, other):
        return self._fulllocation == other.fullLocation()
    def __str__(self):
        """ string repr. used for sorting """
        return self._fulllocation
    def setLocation(self):
        self._parentprojectpath = self.parentProjectPath()
        if self._parentprojectpath :
            strtoremove = self._parentprojectpath + os.sep
            self._location = self._location.replace(strtoremove,"")
    def location(self):
        if self._location == self._fulllocation :
            self._parentprojectpath = self.parentProjectPath()
            if self._parentprojectpath :
                strtoremove = self._parentprojectpath + os.sep
                self._location = self._location.replace(strtoremove,"")
        return self._location
    def fullLocation(self):
        return self._fulllocation
    def parentProject(self):
        if not self._parentproject :
            from Project import Project#IGNORE:E1102
            self._parentproject = Project(self.parentProjectPath())
        return self._parentproject
    def parentProjectPath(self):
        log = logging.getLogger()
        if self._parentprojectpath is None:
            if self._parentproject :
                self._parentprojectpath = self._parentproject.fullLocation()
            else:
                wdir = self.fullLocation()
                os.chdir(wdir)
                env = Env.getDefaultEnv()
                env["PWD"] = wdir

                p = Popen(["cmt", "show", "projects"], stdout=PIPE, stderr=PIPE, close_fds=True)
                for line in p.stdout:
                    line = line[:-1]
                    if line.find("(current)") != -1 :
                        m = re.compile(".+\(in\s+(.+)\s*\)\s*\(\s*current\s*\)").search(line)
                        if m :
                            self._parentprojectpath = os.path.realpath(m.group(1))
#                        words = line.split()
#                        self._parentprojectpath = os.path.realpath(words[-2].replace(")",""))
                for line in p.stderr:
                    Common.CMTLog(line[:-1])
                retcode = os.waitpid(p.pid, 0)[1]
                if retcode != 0:
                    log.warning("return code of 'cmt show project' in %s is %s", wdir, retcode)

        return self._parentprojectpath
    def version(self):
        """ Get the version of the package given by "cmt show version" """
        log = logging.getLogger()
        if self._version is None:
            wdir = os.path.join(self.fullLocation(), "cmt")
            os.chdir(wdir)
            env = Env.getDefaultEnv()
            env["PWD"] = wdir
            p = Popen(["cmt", "show", "version"], stdout=PIPE, stderr=PIPE, close_fds=True)
            for line in p.stdout:
                self._version = line[:-1]
            for line in p.stderr:
                Common.CMTLog(line[:-1])
            retcode = os.waitpid(p.pid, 0)[1]
            if retcode != 0:
                log.warning("return code of 'cmt show version' in %s is %s", wdir, retcode)

        return self._version

    def realVersion(self):
        """ get the real version if possible (look in the requirements file) """
        log = logging.getLogger()
        if self._realversion is None:
            self._realversion = self.version()
            if self._realversion == "v*":
                fname = os.path.join(self.fullLocation(), "cmt", "requirements" )
                f = open(fname, "r")
                for line in f:
                    line = line[:-1]
                    if line.find("version") != -1 :
                        words = line.split()
                        if words[0] == "version":
                            self._realversion = words[1]
                            log.debug("Found version %s in file %s" % (self._realversion, fname))
                            break
                f.close()
        return self._realversion

    def hasVersionDirectory(self):
        versiondir = False
        lastdir = os.path.split(self.location())[-1]
        if lastdir == self.realVersion():
            versiondir = True
        return versiondir

    def coreName(self):
        if self._corename is None:
            self._corename = self.getMacroValue("package")
            if self._corename == self.realVersion() :
                if self.hasVersionDirectory() :
                    self._corename = self.location().split(os.sep)[-2]
                else :
                    self._corename = self.location().split(os.sep)[-1]
        return self._corename

    def name(self):
        if self._name is None :
            self._name = self.location()
            if self.hasVersionDirectory():
                words = self.location().split(os.sep)[:-1]
                self._name = os.sep.join(words)
        return self._name

    def hat(self):
        if self._hat is None :
            if self.name() != self.coreName():
                words = self.name().split(os.sep)[:-1]
                self._hat = os.sep.join(words)
            else :
                self._hat = ""
        return self._hat

    def binaryList(self):
        if self._binarylist is None :
            self._binarylist = Set()
            wdir = os.path.join(self.fullLocation(), "cmt")
            subfiles = os.listdir(wdir)
            # the "default" value is used when no CMTCONFIG is set
            tmplist = ["default"]
            for f in subfiles:
                if f.endswith(".make"):
                    tmplist.append(f.replace(".make",""))
                elif f == "NMake":
                    tmplist.append("win32_")
            # verify that the binary directory exists
            for b in tmplist :
                topdirs = os.listdir(self.fullLocation())
                if b in topdirs:
                    self._binarylist.add(b)
                elif b == "win32_" :
                    for t in topdirs :
                        if t.startswith(b) :
                            self._binarylist.add(t)
        return self._binarylist

    def binaryConstituents(self, binary="default"):
        log = logging.getLogger()
        env = Env.getDefaultEnv()
        if binary == "default" :
            if env.has_key("CMTCONFIG"):
                del env["CMTCONFIG"] # ensures that no cmtconfig is set
        else :
            env["CMTCONFIG"] = binary
            if binary.find("win32") != -1 and sys.platform != "win32" :
                Common.addCMTTag("WIN32", env)
        if self._constituents is None :
            self._constituents = dict()
        if self._allconstituents is None :
            self._allconstituents = Set()
        if not self._constituents.has_key(binary):
            self._constituents[binary] = Set()
        if not self._constituents[binary] :
            const_val = self.getMacroValue("all_constituents")
            for c in const_val.split() :
                if c in self._allconstituents :
                    for a in self._allconstituents :
                        if a == c :
                            self._constituents[binary].add(a)
                            break
                else :
                    self._constituents[binary].add(c)
                    self._allconstituents.add(c)
            log.debug("Found %s constituents in %s for binary %s" % (len(self._constituents[binary]), self.name(), binary) )

        return self._constituents[binary]

    def constituents(self, binary_list=None):
        if self._constituents is None :
            self._constituents = dict()
        if binary_list is None :
            binary_list = self.binaryList()
        for b in binary_list :
            if not self._constituents.has_key(b) :
                self._constituents[b] = self.binaryConstituents(b)
        return self._constituents

    def hasBinaryConstituents(self, binary="default"):
        if self.binaryConstituents(binary) :
            return True
        else :
            return False

    def _setCMTPathEnv(self, cmtpath=None, cmtprojectpath=None):
        if cmtpath is None:
            cmtpath = self.parentProjectPath()
        Common.setCMTPathEnv(cmtpath, cmtprojectpath)


    def binaryUsedPackages(self, cmtpath=None, cmtprojectpath=None, binary="default"):
        env = Env.getDefaultEnv()
        log = logging.getLogger()
        self._setCMTPathEnv(cmtpath, cmtprojectpath)
        if binary == "default" :
            if env.has_key("CMTCONFIG"):
                del env["CMTCONFIG"] # ensures that no cmtconfig is set
        else :
            env["CMTCONFIG"] = binary
            if binary.find("win32") != -1 and sys.platform != "win32" :
                Common.addCMTTag("WIN32", env)
        if self._usedpaklist is None :
            self._usedpaklist = dict()
        if self._allusedpaklist is None :
            self._allusedpaklist = Set()
        indx = cmtpath, cmtprojectpath, binary
        if not self._usedpaklist.has_key(indx):
            self._usedpaklist[indx] = Set()
        if not self._usedpaklist[indx] :
            packagelist = Set()
            wdir = os.path.join(self.fullLocation(), "cmt")
            os.chdir(wdir)
            env["PWD"] = wdir
            p = Popen(["cmt", "show", "uses"], stdout=PIPE, stderr=PIPE, close_fds=True)
            for line in p.stdout:
                if not line.startswith("#"):
                    thepack = None
                    line = line.replace("(no_auto_imports)","").replace("(","").replace(")","")
                    words = line[:-1].split()
                    words = words[1:]
                    if words[-1].endswith("/") :
                        words[-1] = words[-1][:-1]
                    pakpath = ""
                    parentprojectpath = words[-1]
                    if len(words) == 4 :
                        pakpath = os.path.join(words[3], words[2], words[0])
                    elif len(words) == 3:
                        pakpath = os.path.join(words[2], words[0])
                    if isPackage(pakpath):
                        thepack = pakpath
                    else :
                        pakpath = os.path.join(pakpath,words[1])
                        if isPackage(pakpath):
                            thepack = pakpath
                    if thepack:
                        tmpack = self.__class__(thepack, parentprojectpath=parentprojectpath)
                        if tmpack in self._allusedpaklist :
                            for a in self._allusedpaklist :
                                if a == tmpack :
                                    a.addUsedByBinary(binary)
                                    packagelist.add(a)
                                    break
                        else :
                            packagelist.add(self.__class__(thepack, parentprojectpath=parentprojectpath))
            for line in p.stderr:
                Common.CMTLog(line[:-1])
            retcode = os.waitpid(p.pid, 0)[1]
            if retcode != 0:
                log.warning("return code of 'cmt show uses' in %s is %s", wdir, retcode)
            for pak in packagelist :
                if binary == "default" :
                    self._usedpaklist[indx].add(pak)
                    self._allusedpaklist.add(pak)
                else :
#                    if  pak.hasBinaryConstituents(binary):
                    self._usedpaklist[indx].add(pak)
                    self._allusedpaklist.add(pak)
        return self._usedpaklist[indx]

    def usedPackages(self, binary_list=None, cmtprojectpath=None, cmtpath=None):
        # require one of CMTPATH or CMTPROJECTPATH (or use current project path)
        self._setCMTPathEnv(cmtpath, cmtprojectpath)
        if self._usedpaklist is None:
            self._usedpaklist = dict()
        if binary_list is None :
            binary_list = self.binaryList()
        for b in binary_list :
            indx = cmtpath, cmtprojectpath, b
            if not self._usedpaklist.has_key(indx) :
                self._usedpaklist[indx] = self.binaryUsedPackages(cmtpath, cmtprojectpath, b)
        if not self._usedpaklist.has_key((cmtpath, cmtprojectpath, "default")) :
            self._usedpaklist[(cmtpath, cmtprojectpath, "default")] = self.binaryUsedPackages(cmtpath, cmtprojectpath)
        return self._usedpaklist

    def addUsedByBinary(self, binary):
        if self._usedbybinary is None :
            self._usedbybinary = Set()
        self._usedbybinary.add(binary)

    def usedByBinary(self):
        return self._usedbybinary

    def hasUsedByBinary(self):
        hasome = False
        if self._usedbybinary is not None :
            hasome = True
        return hasome

    def binaryExternalPackages(self, cmtprojectpath=None, cmtpath=None, binary="default"):
        env = Env.getDefaultEnv()
        self._setCMTPathEnv(cmtpath, cmtprojectpath)
        if binary == "default" :
            if env.has_key("CMTCONFIG"):
                del env["CMTCONFIG"] # ensures that no cmtconfig is set
        else :
            env["CMTCONFIG"] = binary
            if binary.find("win32") != -1 and sys.platform != "win32" :
                Common.addCMTTag("WIN32", env)
        if self._extpaklist is None :
            self._extpaklist = dict()
        if self._allextpaklist is None :
            self._allextpaklist = Set()
        indx = cmtpath, cmtprojectpath, binary
        if not self._extpaklist.has_key(indx):
            self._extpaklist[indx] = Set()
        if not self._extpaklist[indx] :
            for p in self.binaryUsedPackages(cmtpath, cmtprojectpath, binary):
                if self.parentProjectPath() != p.parentProjectPath():
                    self._allextpaklist.add(p)
                    self._extpaklist[indx].add(p)
        return self._extpaklist[indx]

    def externalPackages(self, cmtprojectpath=None, cmtpath=None, binary_list=None):
        # require one of CMTPATH or CMTPROJECTPATH (or use current project path)
        self._setCMTPathEnv(cmtpath, cmtprojectpath)
        if self._extpaklist is None:
            self._extpaklist = dict()
        if binary_list is None :
            binary_list = self.binaryList()
        for b in binary_list :
            indx = cmtpath, cmtprojectpath, b
            if not self._extpaklist.has_key(indx) :
                self._extpaklist[indx] = self.binaryExternalPackages(cmtpath, cmtprojectpath, b)
        if not self._extpaklist.has_key((cmtpath, cmtprojectpath, "default")) :
            indx = cmtpath, cmtprojectpath, "default"
            self._extpaklist[indx] = self.binaryExternalPackages(cmtpath, cmtprojectpath)
        return self._extpaklist[indx]

    def getMacroValue(self, macro_name):
        here = os.getcwd()
        log = logging.getLogger()
        wdir = os.path.join(self.fullLocation(), "cmt")
        env = Env.getDefaultEnv()
        env["PWD"] = wdir
        os.chdir(wdir)
        p = Popen(["cmt", "show", "macro_value", macro_name], stdout=PIPE, stderr=PIPE, close_fds=True)
        macro_val = p.stdout.read()[:-1]
        for line in p.stderr:
            Common.CMTLog(line[:-1])
        retcode = os.waitpid(p.pid, 0)[1]
        if retcode != 0:
            log.debug("return code of 'cmt show macro_value %s' in %s is %s" % (macro_name, wdir, retcode))
        os.chdir(here)
        return macro_val

    def getSetValue(self, set_name):
        here = os.getcwd()
        log = logging.getLogger()
        wdir = os.path.join(self.fullLocation(), "cmt")
        env = Env.getDefaultEnv()
        env["PWD"] = wdir
        os.chdir(wdir)
        p = Popen(["cmt", "show", "set_value", set_name], stdout=PIPE, stderr=PIPE, close_fds=True)
        set_val = p.stdout.read()[:-1]
        for line in p.stderr:
            Common.CMTLog(line[:-1])
        retcode = os.waitpid(p.pid, 0)[1]
        if retcode != 0:
            log.debug("return code of 'cmt show set_value %s' in %s is %s" % (set_name, wdir, retcode))
        os.chdir(here)
        return set_val
    def releaseDate(self):
        if self._release_date is None:
            self._release_date = strftime("%Y-%m-%d",gmtime(os.path.getmtime(os.path.join(self.fullLocation(), "cmt", "requirements"))))
        return self._release_date

def hasRequirementsFile(dirpath):
    hasfile = False
    log = logging.getLogger()
    try:
        subfiles = os.listdir(dirpath)
        for f in subfiles:
            if f == "cmt" and os.path.isdir(os.path.join(dirpath,f)) :
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
    ispak = False
    if os.path.islink(path):
        path = os.path.normpath(path)
    if os.path.isdir(path):
        if hasRequirementsFile(path):
            ispak = True
    return ispak

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


def getPackagesFromDir(directory, name=None, version=None, casesense=True, select=None, parentproject=None, pkgclass=Package):
    paklist = Set()
    log = logging.getLogger()
    try:
        for data in os.walk(directory):
            root, dirs = data[0], data[1]
            if isPackage(root):
                tobeadded = False
                if name is None :
                    tobeadded = True
                elif Common.doesDirMatchNameAndVersion(root, name, version, casesense) :
                    tobeadded = True
                if select is not None and tobeadded:
                    if Common.isDirSelected(root, select, casesense):
                        tobeadded = True
                if tobeadded :
                    tmpack = pkgclass(root, parentproject)
                    paklist.add(tmpack)
                dirs[:] = [] # don't visit anything else: a CMT package cannot contains another one.
            else :
                if 'CVS' in dirs:
                    dirs.remove('CVS')  # don't visit CVS directories
                if '.svn' in dirs:
                    dirs.remove('.svn')  # don't visit .svn directories
    except OSError, msg:
        log.warning("Cannot open path %s" % msg)

    log.debug("Found %s packages in %s" % (len(paklist), directory))

    return paklist


def onPackagePathError(error):
    log = logging.getLogger()
    log.warning("Cannot open path %s" % error.strerror)
