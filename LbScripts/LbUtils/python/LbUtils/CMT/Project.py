from LbUtils import Env
from LbUtils.CMT import Package
from LbUtils.Set import Set
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
    def _setContent(self):
        f = open(self.location(), "r")
        self._content = f.read()
        f.close()
    def content(self):
        if not self._content:
            self._setContent()
        return self._content

class Project(object):
    def __init__(self, projectpath, versiondir=False):
        self._location = self.location(projectpath)
        self._name = self.name(versiondir)
        self._version = self.version(versiondir)
        self._projectfile = self.projectFile()
        self._basenamelist = []
        self._baselist = Set()
        self._clientlist = Set()
        self._packagelist = []
        self._containedpackagelist = Set()
        self._usedpackagelist = {}
        
    def __eq__(self, other):
        return self.location() == other.location()
    
    def name(self, versiondir=None):
        if not hasattr(self, "_name") or versiondir is not None:
            self._setName(versiondir)
        return self._name
    def _setName(self, versiondir):
        dirspl = os.path.split(self.location())
        if versiondir :
            self._name = os.path.split(dirspl[0])[1]
        else : 
            self._name = dirspl[1]

    def version(self, versiondir=None):
        if versiondir is not None:
            self._setVersion(versiondir)
        return self._version
    def _setVersion(self, versiondir):
        if versiondir : 
            self._version = os.path.split(self.location())[1]
        else :
            self._version = None
            
    def location(self, projectpath=None):
        if not hasattr(self, "_location") or projectpath is not None:
            self._setLocation(projectpath)
        return self._location
    def _setLocation(self, projectpath):
        self._location = os.path.realpath(projectpath)

    def projectFile(self):
        if not hasattr(self, "_projectfile") or not self._projectfile :
            self._setProjectFile()
        return self._projectfile 
    def _setProjectFile(self):
        pfname = os.path.join(self.location(), "cmt", "project.cmt")
        self._projectfile = ProjectFile(pfname)
        
    def packages(self):
        return self._packagelist
    def getDependencies(self):
        log = logging.getLogger()
        wdir = os.path.join(self.location(),"cmt")
        os.chdir(wdir)
        env = Env.getDefaultEnv()
        env["PWD"] = wdir
        p = Popen(["cmt", "show", "projects"], stdout=PIPE, stderr=PIPE, 
                  close_fds=True)
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
        wdir = os.path.join(self.location(),"cmt")
        os.chdir(wdir)
        env = Env.getDefaultEnv()
        env["PWD"] = wdir
        parentlist = []
        p = Popen(["cmt", "show", "projects"], stdout=PIPE, stderr=PIPE, close_fds=True)
        for line in p.stdout:
            if re.compile("^"+self.name()).match(line):
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
    def baseNameList(self):
        if not self._basenamelist :
            self.getBaseNameList()
        return self._basenamelist
    def showBaseNameList(self):
        for pn in self.baseNameList() :
            print "\t%s" % pn
    def addClient(self, proj):
        self._clientlist.add(proj)
    def getBase(self, projectlist):
        for bn in self.baseNameList() :
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
            self.showContainedPackages()
        if showusedpackages :
            self.getUsedPackages(binary_list, project_list)
            self.printUsedPackages(binary_list, showcontainedpackages)
    def addContainedPackage(self, packagepath):
        newpack = Package.Package(packagepath, self)
        self._containedpackagelist.add(newpack)
    def getContainedPackages(self):
        log = logging.getLogger()
        for dn in os.walk(self.location(), 
                          topdown=True, 
                          onerror=Package.onPackagePathError):
            fullname = dn[0]
            if Package.isPackage(fullname):
                log.debug("found package: %s", fullname)
                self.addContainedPackage(fullname)
                dn[1][:] = []
    def hasContainedPackage(self, package):
        return package in self._containedpackagelist
    def containedPackages(self):
        if not self._containedpackagelist:
            self.getContainedPackages()
        return self._containedpackagelist
    def showContainedPackages(self):
        for p in self.containedPackages() :
            print "  %s" % p.location()
    def showUsedPackages(self, binary_list):
        for p in self.containedPackages() :
            p.showUsedPackages(binary_list)
    def getUsedPackages(self, binarylist, projectlist=None):
        if not projectlist :
            projectlist = Set()
            projectlist.add(self)
        for b in binarylist :
            self._usedpackagelist[b] = Set()
            for p in self.containedPackages() :
                self._usedpackagelist[b] |= p.getBinaryUsedPackages(b, projectlist)
        return self._usedpackagelist
    def printUsedPackages(self, binarylist, showcontained=False ):
        if not showcontained:
            all_packs = Set()
            for b in binarylist:
                all_packs |= self._usedpackagelist[b]
            for p in all_packs :
                if p.parentProject() == self:
                    print "    %s" % p.location(),
                else :
                    print "    %s" % p.fullLocation(),
                print "( ",
                for b in binarylist :
                    if p in self._usedpackagelist[b] :
                        print "%s " % b,
                print ")"
        else :
            for c in self.containedPackages() :
                print "  %s" %c.location()
                all_packs = Set()
                for b in binarylist:
                    all_packs |= c.binaryUsedPackages(b)
                for p in all_packs :
                    if p.parentProject() == self:
                        print "    %s" % p.location(),
                    else :
                        print "    %s" % p.fullLocation(),
                    print "( ",
                    for b in binarylist :
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
                    ssubf = os.listdir(os.path.join(dirpath, f))
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
    projlist = Set()
    selected = Set()
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
                    fn = os.path.join(fullname, ff)
                    if isProject(fn):
                        if not select :
                            log.debug("%s has a project version directory", fn)
                            projlist.add(Project(fn, True))
                        else :
                            if fn.find(select) != -1 :
                                log.debug("%s has a project version directory", fn)
                                projlist.add(Project(fn, True))
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


def walk(top, topdown=True, toclients=True, onerror=None, alreadyfound=None):
    if not alreadyfound:
        alreadyfound = Set()
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


