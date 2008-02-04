from LbUtils import Env
from LbUtils.CMT import Package
import logging
import os
import re
from subprocess import Popen, PIPE

class ProjectFile(object):
    def __init__(self, filename):
        self._location = filename
        self._name = None
        self._version = None
        self._container = None
        self._file = None
        self.readFile()
    def readFile(self):
        f = open(self._location, "r")
        self._file = f.read()
        f.close()

class Project(object):
    def __init__(self, projectpath, versiondir=False):
        self._name = None
        self._version = None
        self._location = projectpath
        self._versiondir = versiondir
        self._projectfile = None
        self._basenamelist = []
        self._baselist = set()
        self._clientlist = set()
        self._packagelist = []
        self._containedpackagelist = set()
        self._usedpackagelist = {}
        self.setName(versiondir)
        self.setVersion()
        self.setProjectFile()
        self.getBaseNameList()
    def __eq__(self, other):
        return self._location == other._location
    def __hash__(self):
        return self._location.__hash__()
    def name(self):
        return self._name
    def setName(self, versiondir):
        dirspl = os.path.split(self._location)
        if versiondir :
            self._name = os.path.split(dirspl[0])[1]
        else : 
            self._name = dirspl[1]
    def version(self):
        return self._version
    def setVersion(self):
        if self._versiondir : 
            self._version = os.path.split(self._location)[1]
    def location(self):
        return self._location
    def packages(self):
        return self._packagelist
    def getDependencies(self):
        log = logging.getLogger()
        wdir = os.path.join(self._location,"cmt")
        os.chdir(wdir)
        env = Env.getDefaultEnv()
        env["PWD"] = wdir
        p = Popen(["cmt", "show", "projects"], stdout=PIPE, stderr=PIPE, close_fds=True)
        for line in p.stdout:
            print line[:-1]
        for line in p.stderr:
            log.warning(line[:-1])
        retcode = os.waitpid(p.pid, 0)[1]
        log.debug("return code of 'cmt show projects' in %s is %s", wdir, retcode)
        if retcode != 0:
            log.warning("return code of 'cmt show projects' in %s is %s", wdir, retcode)
    def getBaseNameList(self):
        log = logging.getLogger()
        wdir = os.path.join(self._location,"cmt")
        os.chdir(wdir)
        env = Env.getDefaultEnv()
        env["PWD"] = wdir
        parentlist = []
        p = Popen(["cmt", "show", "projects"], stdout=PIPE, stderr=PIPE, close_fds=True)
        for line in p.stdout:
            words = line[:-1].split(" ")
            if re.compile("^"+self._name).match(line):
                parmatch = re.compile("C=[a-zA-Z]+").findall(line)
                for w in parmatch :
                    parentlist.append(w.replace("C=",""))
            elif parentlist :
                for par in parentlist:
                    if re.compile("\s+" + par).match(line):
                        m = re.compile("\(in\s+(.+)\s*\)").search(line)
                        if m : 
                            self._basenamelist.append(m.group(1))
        for line in p.stderr:
            log.warning(line[:-1])
        retcode = os.waitpid(p.pid, 0)[1]
        log.debug("return code of 'cmt show projects' in %s is %s", wdir, retcode)
        if retcode != 0:
            log.warning("return code of 'cmt show projects' in %s is %s", wdir, retcode)
    def showBaseNameList(self):
        for pn in self._basenamelist :
            print "\t%s" % pn
    def addClient(self, proj):
        self._clientlist.add(proj)
    def getBase(self, projectlist):
        for bn in self._basenamelist :
            for p in projectlist :
                if p.location() == bn :
                    self._baselist.add(p)
                    p.addClient(self)
    def showBase(self):
        if self._baselist :
            for b in self._baselist :
                print "\t%s" % b.location()
    def base(self):
        return self._baselist
    def clients(self):
        return self._clientlist
    def showClient(self):
        if self._clientlist :
            for c in self._clientlist :
                print "\t%s" % c.location() 
    def setProjectFile(self):
        pfname = os.path.join(self._location, "cmt", "project.cmt")
        self._projectfile = ProjectFile(pfname)
    def show(self, showdeps=False, 
             showbase=False, 
             showclient=False, 
             showcontainedpackages=False, 
             showusedpackages= False,
             binary_list=None,
             project_list=None):
        print self.location()
        if showdeps : 
            self.getDependencies()
        if showbase :
            self.showBase()
        if showclient :
            self.showClient()
        if showcontainedpackages and not showusedpackages:
            self.getContainedPackages()
            self.showContainedPackages()
        if showusedpackages :
            self.getContainedPackages()
            self.getUsedPackages(binary_list, project_list)
            self.printUsedPackages(binary_list, showcontainedpackages)
    def addContainedPackage(self, packagepath):
        newpack = Package.Package(packagepath, self)
        self._containedpackagelist.add(newpack)
    def getContainedPackages(self):
        log = logging.getLogger()
        for dn in os.walk(self._location,topdown=True,onerror=Package.onPackagePathError):
            fullname = dn[0]
            if Package.isPackage(fullname):
                log.debug("found package: %s", fullname)
                self.addContainedPackage(fullname)
                dn[1][:] = []
    def hasContainedPackage(self, package):
        return package in self._containedpackagelist
    def containedPackages(self):
        return self._containedpackagelist
    def showContainedPackages(self):
        for p in self._containedpackagelist :
            print "  %s" % p.location()
    def showUsedPackages(self, binary_list):
        for p in self._containedpackagelist :
            p.showUsedPackages(binary_list)
    def getUsedPackages(self, binary_list, projectlist=None):
        if not projectlist :
            projectlist = set()
            projectlist.add(self)
        if not self._containedpackagelist:
            self.getContainedPackages()
        for b in binary_list :
            self._usedpackagelist[b] = set()
            for p in self._containedpackagelist :
                self._usedpackagelist[b] |= p.getBinaryUsedPackages(b, projectlist)
        return self._usedpackagelist
    def printUsedPackages(self, binary_list, showcontained=False ):
        if not showcontained:
            all_packs = set()
            for b in binary_list:
                all_packs |= self._usedpackagelist[b]
            for p in all_packs :
                if p.parentProject() == self:
                    print "    %s" % p.location(),
                else :
                    print "    %s" % p.fullLocation(),
                print "( ",
                for b in binary_list :
                    if p in self._usedpackagelist[b] :
                        print "%s " % b,
                print ")"
        else :
            for c in self._containedpackagelist :
                print "  %s" %c.location()
                all_packs = set()
                for b in binary_list:
                    all_packs |= c.binaryUsedPackages(b)
                for p in all_packs :
                    if p.parentProject() == self:
                        print "    %s" % p.location(),
                    else :
                        print "    %s" % p.fullLocation(),
                    print "( ",
                    for b in binary_list :
                        if p in self._usedpackagelist[b] :
                            print "%s " % b,
                    print ")"
    
def hasProjectFile(dirpath):
    hasfile = False
    log = logging.getLogger()
    try:
        subfiles = os.listdir(dirpath)
        for f in subfiles:
            if f == "cmt" :
                try :
                    ssubf = os.listdir(os.path.join(dirpath,f))
                    for i in ssubf:
                        if i == "project.cmt" :
                            hasfile = True
                            return hasfile
                except OSError, msg :
                    log.warning("Cannot open path %s" % msg)
    except OSError, msg :
        log.warning("Cannot open path %s" % msg)
    return hasfile



def isProject(path):
    isproj = False
    if os.path.islink(path):
        path = os.path.normpath(path)
    if os.path.isdir(path):
        if hasProjectFile(path):
            isproj = True
    return isproj

def getProjectsFromPath(path, name=None, version=None, casesense=False, select=None):
    projlist = set()
    selected = set()
    log = logging.getLogger()
    try:
        lsdir = os.listdir(path)
        for f in lsdir:
            fullname = os.path.join(path,f)
            if isProject(fullname):
                if not select :
                    log.debug("%s has no project version directory", fullname)
                    projlist.add(Project(fullname))
                else :
                    if fullname.find(select) != -1 :
                        log.debug("%s has no project version directory", fullname)
                        projlist.add(Project(fullname))                        
            if os.path.isdir(fullname):
                lsintdir = os.listdir(fullname)
                for ff in lsintdir:
                    fn = os.path.join(fullname,ff)
                    if isProject(fn):
                        if not select :
                            log.debug("%s has a project version directory", fn)
                            projlist.add(Project(fn,True))
                        else :
                            if fn.find(select) != -1 :
                                log.debug("%s has a project version directory", fn)
                                projlist.add(Project(fn,True))
    except OSError, msg:
        log.warning("Cannot open path %s" % msg)
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
    projlist = set()
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
    selected = set()
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


def walk(top, topdown=True, toclients=True, onerror=None, alreadyfound=None):
    if not alreadyfound:
        alreadyfound = set()
    alreadyfound.add(top)
    proj = top
    if toclients :
        deps = proj.clients()
    else :
        deps = proj.base()
    packs = proj.packages()
    if topdown :
        yield (proj, deps, packs)
    for d in deps :
        if d not in alreadyfound :
            for w in walk(d, topdown, toclients, onerror, alreadyfound) :
                yield w
    if not topdown :
        yield (proj, deps, packs)
