
# local import
from Package import Package, getPackagesFromDir
import Common


# package imports
from LbUtils import Env
from LbUtils.Set import Set
from LbUtils.afs.directory import isAFSDir, Directory, isMountPoint

from LbUtils.Common import versionSort

# global import
import logging
import os
import re
from subprocess import Popen, PIPE

class ProjectFile(object):
    def __init__(self, filename):
        self._location = self.location(filename)
        self._content = None
        self._name = None
        self._version = None
        self._container = None
    def _setLocation(self, filename):
        self._location = filename
    def location(self, filename=None):
        if filename is not None:
            self._setLocation(filename)
        return self._location
    def container(self):
        if self._container is None :
            m = re.compile("container\s+(.+)\s*").search(self.content())
            if m :
                self._container = m.group(1)
            else :
                self._container = ""
        return self._container
    def _setContent(self):
        f = open(self.location(), "r")
        self._content = f.read()
        f.close()
    def content(self):
        if not self._content:
            self._setContent()
        return self._content

class Project(object):
    def __init__(self, projectpath):
        projectpath = os.path.realpath(projectpath)
        self._fulllocation = projectpath
        self._pkgclass = Package
        self._name = None
        self._projectfile = None
        self._version = None
        self._location = None
        self._baselist = None
        self._clientlist = None
        self._packagelist = None
        self._binarylist = None
        self._usedpaklist = None
        self._allusedpaklist = None
        self._extpaklist = None
        self._allextpaklist = None
        self._container = None
        self._container_pkg = None
        self._afsmount = None
    def __eq__(self, other):
        return self.fullLocation() == other.fullLocation()

    def fullLocation(self):
        return self._fulllocation

    def __str__(self):
        """ string repr. used for sorting """
        return self._fulllocation

    def __name(self): # internal name, not to be overridden by subclasses
        log = logging.getLogger()
        if self._name is None:
            env = Env.getDefaultEnv()
            wdir = os.path.join(self.fullLocation(), "cmt")
            if not os.path.exists(wdir) : # special case for CMTUSERCONTEXT
                wdir = self.fullLocation()
            os.chdir(wdir)
            env["PWD"] = wdir
            p = Popen(["cmt", "show", "projects"], stdout=PIPE, stderr=PIPE, close_fds=True)
            line = p.stdout.readline()[:-1]
            self._name = line.split()[0]
            for line in p.stderr:
                Common.CMTLog(line[:-1])
            retcode = os.waitpid(p.pid, 0)[1]
            if retcode != 0 :
                log.warning("return code of 'cmt show projects' in %s is %s", wdir, retcode)
        return self._name

    def name(self): # displayed name. Can be overridden by subclasses
        return self.__name()

    def projectFile(self):
        if self._projectfile is None :
            self._projectfile = ProjectFile(os.path.join(self.fullLocation(), "cmt", "project.cmt"))
        return self._projectfile

    def container(self):
        if self._container is None :
            self._container = self.projectFile().container()
        return self._container

    def containerPackage(self):
        if self._container_pkg is None :
            cont_name = self.container()
            if cont_name :
                for p in self.packages() :
                    if p.name() == cont_name :
                        self._container_pkg = p
                        break ;
        return self._container_pkg



    def __version(self):
        if self._version is None :
            tmlist = self.fullLocation().split(os.sep)
            if tmlist[-2] == self.__name():
                self._version = tmlist[-1]
            else :
                self._version = ""
        return self._version

    def version(self):
        return self.__version()

    def location(self):
        if self._location is None :
            tmlist = self.fullLocation().split(os.sep)
            if self.__version() == "":
                self._location = tmlist[-1]
            else :
                self._location = os.sep.join([tmlist[-2],tmlist[-1]])
        return self._location

    def _setCMTPathEnv(self, cmtpath=None, cmtprojectpath=None):
        if cmtprojectpath is None:
            tmlist = self.fullLocation().split(os.sep)
            if self.__version() == "" :
                cmtprojectpath = os.sep.join(tmlist[:-1])
            else :
                cmtprojectpath = os.sep.join(tmlist[:-2])
        Common.setCMTPathEnv(cmtpath, cmtprojectpath)
        return cmtprojectpath

    def base(self, cmtpath=None, cmtprojectpath=None):
        self._setCMTPathEnv(cmtpath, cmtprojectpath)
        if self._baselist is None :
            self._baselist = Set()
            wdir = os.path.join(self.fullLocation(),"cmt")
            if not os.path.exists(wdir) :
                wdir = self.fullLocation()
            os.chdir(wdir)
            env = Env.getDefaultEnv()
            env["PWD"] = wdir
            parentlist = []
            p = Popen(["cmt", "show", "projects"], stdout=PIPE, stderr=PIPE, close_fds=True)
            for line in p.stdout:
                if re.compile("^"+self.__name()).match(line):
                    parmatch = re.compile("C=[a-zA-Z]+").findall(line)
                    for w in parmatch :
                        parentlist.append(w.replace("C=",""))
                elif parentlist :
                    for par in parentlist:
                        if re.compile("\s+" + par).match(line):
                            m = re.compile("\(in\s+(.+)\s*\)").search(line)
                            if m :
                                self._baselist.add(self.__class__(m.group(1)))
            for line in p.stderr:
                Common.CMTLog(line[:-1])
            os.waitpid(p.pid, 0)[1]
        return self._baselist

    def overrideBaseProject(self, other, cmtpath=None, cmtprojectpath=None):
        self._setCMTPathEnv(cmtpath, cmtprojectpath)
        self._baselist.override(other)

    def clients(self, cmtpath=None, cmtprojectpath=None):
        """ get all the client projects. Pretty slow. Use with care """
        cmtprojectpath = self._setCMTPathEnv(cmtpath, cmtprojectpath)
        if self._clientlist is None:
            self._clientlist = Set()
            tmplist = Set()
            projectpath_list = cmtprojectpath.split(os.pathsep)
            for c in projectpath_list :
                tmplist |= getProjectsFromDir(directory=c, prjclass=self.__class__)
            for p in tmplist :
                for b in p.base() :
                    if b == self:
                        self._clientlist.add(p)
        return self._clientlist

    def packages(self):
        if self._packagelist is None :
            self._packagelist = getPackagesFromDir(directory=self.fullLocation(), parentproject=self, pkgclass=self._pkgclass)
        else :
            self.updatePackages(getPackagesFromDir(directory=self.fullLocation(), parentproject=self, pkgclass=self._pkgclass))
        return self._packagelist

    def hasPackages(self):
        haspack = False
        if self._packagelist is not None :
            haspack = True
        return haspack

    def addPackage(self, pak):
        if self._packagelist is None :
            self._packagelist = Set()
        if pak.parentProjectPath() == self.fullLocation() :
            self._packagelist.add(pak)

    def updatePackages(self, paklist):
        if self._packagelist is None :
            self._packagelist = Set()
        for p in paklist :
            if p.parentProjectPath() == self.fullLocation() :
                self._packagelist.add(p)

    def hasPackage(self, package):
        return package in self.packages()

    def findPackage(self, name, version=None):
        pkg = None
        pkglist = getPackagesFromDir(self.fullLocation(), name, version, parentproject=self, pkgclass=self._pkgclass)
        if len(pkglist) :
            srtlist = versionSort(pkglist, reverse=True)
            pkg = srtlist[0]
            self.updatePackages(pkglist)

        return pkg



    def binaryList(self):
        if self._binarylist is None :
            self._binarylist = Set()
            for p in self.packages() :
                self._binarylist |= p.binaryList()
        return self._binarylist


    def binaryUsedPackages(self, cmtpath=None, cmtprojectpath=None, binary="default"):
        env = Env.getDefaultEnv()
        log = logging.getLogger()
        self._setCMTPathEnv(cmtpath, cmtprojectpath)
        if binary == "default" :
            if env.has_key("CMTCONFIG"):
                del env["CMTCONFIG"] # ensures that no cmtconfig is set
        else :
            env["CMTCONFIG"] = binary
        if self._usedpaklist is None :
            self._usedpaklist = dict()
        if self._allusedpaklist is None :
            self._allusedpaklist = Set()
        indx = cmtpath, cmtprojectpath, binary
        if not self._usedpaklist.has_key(indx):
            self._usedpaklist[indx] = Set()
        if not self._usedpaklist[indx] :
            containedpackages = self.packages()
            if self.container() == "" :
                log.warning("No container package found @ %s" % self.fullLocation())
                log.warning("Parsing all packages for %s binary" % binary)
                for p in containedpackages :
                    tmplist = p.binaryUsedPackages(cmtpath, cmtprojectpath, binary)
                    # override the found packages by the one in the contained package list
                    tmplist.override(containedpackages)
                    # update the packages of the base projects
                    for data in walk(top=self, cmtpath=cmtpath, cmtprojectpath=cmtprojectpath):
                        deps = data[1]
                        for b in deps :
                            if b.hasPackages() :
                                tmplist.override(b.packages())
                            b.updatePackages(tmplist)
                    for t in tmplist :
                        t.addUsedByBinary(binary)
                    self._allusedpaklist |= tmplist
                    self._usedpaklist[indx] |= tmplist
            else :
                log.info("Container package %s found @ %s" % (self.container(), self.fullLocation()))
                log.info("Extracting used packages for %s binary" % binary)
                for p in containedpackages :
                    if p.name() == self.container() :
                        tmplist = p.binaryUsedPackages(cmtpath, cmtprojectpath, binary)
                        # override the found packages by the one in the contained package list
                        tmplist.override(containedpackages)
                        # update the packages of the base projects
                        for data in walk(top=self, cmtpath=cmtpath, cmtprojectpath=cmtprojectpath):
                            deps = data[1]
                            for b in deps :
                                if b.hasPackages() :
                                    tmplist.override(b.packages())
                                b.updatePackages(tmplist)
                        for t in tmplist :
                            t.addUsedByBinary(binary)
                        self._allusedpaklist |= tmplist
                        self._usedpaklist[indx] |= tmplist

        return self._usedpaklist[indx]

    def usedPackages(self, binary_list=None, cmtpath=None, cmtprojectpath=None):
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

    def allUsedPackages(self):
        return self._allusedpaklist


    def binaryExternalPackages(self, cmtpath=None, cmtprojectpath=None, binary="default"):
        env = Env.getDefaultEnv()
        self._setCMTPathEnv(cmtpath, cmtprojectpath)
        if binary == "default" :
            if env.has_key("CMTCONFIG"):
                del env["CMTCONFIG"] # ensures that no cmtconfig is set
        else :
            env["CMTCONFIG"] = binary
        if self._extpaklist is None :
            self._extpaklist = dict()
        if self._allextpaklist is None :
            self._allextpaklist = Set()
        indx = cmtpath, cmtprojectpath, binary
        if not self._extpaklist.has_key(indx):
            self._extpaklist[indx] = Set()
        if not self._extpaklist[indx] :
            for p in self.binaryUsedPackages(cmtpath, cmtprojectpath, binary):
                if self.fullLocation() != p.parentProjectPath():
                    self._allextpaklist.add(p)
                    self._extpaklist[indx].add(p)
        return self._extpaklist[indx]


    def externalPackages(self, cmtpath=None, cmtprojectpath=None, binary_list=None):
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

    def allExternalPackages(self):
        return self._allextpaklist

    def afsMountPoint(self):
        log = logging.getLogger()
        if self._afsmount is None :
            if isAFSDir(self.fullLocation()) :
                if isMountPoint(self.fullLocation()):
                    self._afsmount = Directory(self.fullLocation())
                else :
                    log.info("Directory %s in not a mount point" % self.fullLocation())
            else :
                log.info("Directory %s in not on AFS" % self.fullLocation())

        return self._afsmount

    def summary(self, cmtpath=None, cmtprojectpath=None, dependent=False, showpackages=False, afsinfo=False):
        summary  = "Name                 : %s\n" % self.name()
        summary += "Version              : %s\n" % self.version()
        summary += "Location             : %s\n" % self.fullLocation()
        summary += "Used projects        : "
        tmplist = []
        for p in self.base(cmtpath, cmtprojectpath) :
            tmplist.append("%s %s" % (p.name(), p.version()) )
        summary += ", ".join(tmplist) + "\n"
        if not dependent :
            tmplist = []
            for b in self.binaryList() :
                tmplist.append("%s" % b )
            if tmplist :
                summary += "Built configurations : "
                summary += ", ".join(tmplist) + "\n"
            tmpacksum = ""
            if showpackages :
                tmpacksum += "Contained packages:\n"
                for p in self.packages():
                    tmpacksum += "  %-35s%s" % (p.name(), p.version())
                    if not p.hasUsedByBinary() :
                        tmpacksum += "\n"
                    else :
                        tmpacksum += ", ".join(p.usedByBinary()) + "\n"
                self.usedPackages(cmtpath=cmtpath, cmtprojectpath=cmtprojectpath)
            if afsinfo :
                afsmount = self.afsMountPoint()
                if afsmount is not None :
                    summary += "AFS Mount Point      : %s\n" % afsmount.getVolumeName()
        else :
            tmpacksum = ""
            if showpackages :
                tmpacksum += "Used packages:\n"
                if self.hasPackages() :
                    for p in self.packages() :
                        tmpacksum += "  %-35s%s  " % (p.name(), p.version())
                        if not p.hasUsedByBinary() :
                            tmpacksum += "\n"
                        else :
                            tmpacksum += ", ".join(p.usedByBinary()) + "\n"
        if self._packagelist is not None :
            summary += "Number of Packages   : %s\n" % len(self._packagelist)
            summary += tmpacksum
        if dependent :
            sumlist = summary.split("\n")
            summary2 = ""
            for l in sumlist :
                summary2 += "     %s\n" % l
            summary = summary2
        return summary

def hasProjectFile(dirpath):
    return os.path.exists(os.path.join(dirpath, "cmt", "project.cmt"))

def isProject(path):
    isproj = False
    if os.path.islink(path):
        path = os.path.normpath(path)
    if os.path.isdir(path):
        if hasProjectFile(path):
            isproj = True
    return isproj

def getProjectsFromPath(path, name=None, version=None, casesense=False, select=None, prjclass=Project):
    projlist = Set()
    log = logging.getLogger()
    try:
        lsdir = os.listdir(path)
        for f in lsdir:
            fullname = os.path.join(path,f)
            if isProject(fullname):
                tobeadded = False
                if Common.doesDirMatchNameAndVersion(fullname, name, version, casesense) :
                    tobeadded = True
                if select is not None and tobeadded:
                    if Common.isDirSelected(fullname, select, casesense):
                        tobeadded = True
                    else :
                        tobeadded = False
                if tobeadded :
                    tmproj = prjclass(fullname)
                    projlist.add(tmproj)
                    log.debug("Found project at %s" % fullname)

            elif os.path.isdir(fullname):
                lsintdir = os.listdir(fullname)
                for ff in lsintdir:
                    fn = os.path.join(fullname, ff)
                    if isProject(fn):
                        tobeadded = False
                        if Common.doesDirMatchNameAndVersion(fn, name, version, casesense) :
                            tobeadded = True
                        if select is not None and tobeadded:
                            if Common.isDirSelected(fn, select, casesense):
                                tobeadded = True
                            else :
                                tobeadded = False
                        if tobeadded :
                            tmproj = prjclass(fn)
                            projlist.add(tmproj)
                            log.debug("Found project at %s" % fn)

    except OSError, msg:
        log.warning("Cannot open path %s" % msg)

    log.info("Found %s projects in %s" % (len(projlist), path))

    return projlist


def getProjectsFromDir(directory, name=None, version=None, casesense=True, select=None, prjclass=Project):
    projlist = Set()
    log = logging.getLogger()
    try:
        for data in os.walk(directory):
            root, dirs = data[0], data[1]
            log.debug("Entering directory %s" % root)
            if isProject(root):
                tobeadded = False
                tmproj = prjclass(root)
                if name is None :
                    tobeadded = True
                elif Common.doesDirMatchNameAndVersion(root, name, version, casesense) :
                    tobeadded = True
                if select is not None and tobeadded:
                    if Common.isDirSelected(root, select, casesense):
                        tobeadded = True
                if tobeadded :
                    projlist.add(tmproj)
                    log.debug("Found project at %s" % root)
                dirs[:] = [] # don't visit anything else: a CMT package cannot contains another one.
            else :
                if 'CVS' in dirs:
                    dirs.remove('CVS')  # don't visit CVS directories
                if '.svn' in dirs :
                    dirs.remove('.svn')
                for d in dirs : # don't visit hidden directories
                    if d.startswith(".") :
                        dirs.remove(d)
    except OSError, msg:
        log.warning("Cannot open path %s" % msg)

    log.info("Found %s projects in %s" % (len(projlist), directory))

    return projlist


def _getProjects(cmtprojectpath, name=None, version=None, casesense=False, select=None):
    log = logging.getLogger()
    projlist = []
    pathcomponents = cmtprojectpath.split(os.pathsep)
    for p in pathcomponents:
        log.info("looking for projects in %s", p)
        pl = getProjectsFromPath(p, name, version, casesense, select)
        if pl:
            log.info("Found %s project in %s", len(pl), p)
            projlist += pl
    return projlist

def getAllProjects(cmtprojectpath, select=None):
    log = logging.getLogger()
    projlist = Set()
    pathcomponents = cmtprojectpath.split(os.pathsep)
    for p in pathcomponents:
        log.info("looking for projects in %s", p)
        pl = getProjectsFromPath(p, select=select)
        if pl:
            log.info("Found %s project in %s", len(pl), p)
            projlist |= pl
    return projlist

def getProjectInstance(projlist, projpath):
    for p in projlist :
        if p.location() == projpath :
            return p
    else:
        return None

def FilterProjects(projlist, name=None, version=None, casesense=False ):
    selected = Set()
    if not name and not version:
        return projlist

    # filter according to name and version
    if not casesense:
        name = name.upper()
    for p in projlist:
        if not casesense:
            prname = p.name().upper()
        else:
            prname = p.name()
        if prname == name:
            if not version:
                selected.add(p)
            else :
                if p.version() == version:
                    selected.add(p)
    return selected

def getProjects(cmtprojectpath, name=None, version=None,
                    casesense=False, select=None ):
    projlist = getAllProjects(cmtprojectpath, select=select)
    for p in projlist :
        p.getBase(projlist)
    return FilterProjects(projlist, name=name, version=version, casesense=casesense)

def findProject(cmtprojectpath, name, version=None, casesense=False,
                project_class=Project):
    """ find project in the path. if version is None find a versionless project
    or the latest version of a regular project """
    prj = None
    for p in cmtprojectpath.split(os.pathsep) :
        prjlist = getProjectsFromPath(p, name, version, casesense, prjclass=project_class)
        if len(prjlist) :
            srtlist = versionSort(prjlist, reverse=True)
            prj = srtlist[0]
            break
    return prj



def walk(top, topdown=True, toclients=False,
         onerror=None, alreadyfound=None,
         cmtpath=None, cmtprojectpath=None,
         light=False):
    if not alreadyfound:
        alreadyfound = Set()
    alreadyfound.add(top)
    proj = top
    if toclients :
        deps = proj.clients(cmtpath, cmtprojectpath)
    else :
        deps = proj.base(cmtpath, cmtprojectpath)
    if not light and proj.hasPackages() :
        packs = proj.packages()
    else :
        packs = Set()
    if topdown :
        yield (proj, deps, packs)
    for d in deps :
        if d not in alreadyfound :
            for w in walk(d, topdown, toclients, onerror, alreadyfound, cmtpath,
                          cmtprojectpath) :
                yield w
    if not topdown :
        yield (proj, deps, packs)


def CMTWhich(project, package=None, version=None, all_occurences=False, casesense=True,
             project_class=Project):
    """ function to extract the project or package class"""
    if version :
        ver = version
        pak = package
    else :
        # case where no 3rd argument is passed
        # try package as a version
        ver = package
        pak = None

    prj = findProject(os.environ["CMTPROJECTPATH"], project, ver, casesense=casesense,
                      project_class=project_class)

    if not prj :
        # try versionless project or latest version of a project
        ver = None
        pak = package
        prj = findProject(os.environ["CMTPROJECTPATH"], project, ver, casesense=casesense,
                          project_class=project_class)

    result = prj

    if prj and pak :
        # case where there is a second argument
        pkg =None
        for p, _, _ in walk(prj, cmtprojectpath=os.environ["CMTPROJECTPATH"], light=True) :
            if p.version() :
                pkg = p.findPackage(pak)
            else :
                pkg = p.findPackage(pak, version)
            if pkg :
                break
        result = pkg


    return result


