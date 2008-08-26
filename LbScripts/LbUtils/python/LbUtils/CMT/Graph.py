from LbUtils.CMT import getProjectsFromDir, getProjectsFromPath, Project
from LbUtils.CMT.Project import walk
from LbUtils.Set import Set
import logging
import os

class Graph(object):
    def __init__(self, projectpath):
        self._projectpath = projectpath
        self._prjclass = Project
        self._projectlist = None
        self._startprojlist = None

    def getStartProjectsFromDir(self, directory=None, name=None, version=None, casesense=True, select=None):
        if self._startprojlist is None:
            self._startprojlist = Set()
        dirprojlist = Set()
        if directory is not None :
            dirprojlist.update(self.getProjectsFromDir(directory, name, version, casesense, select))
        else :
            for d in self._projectpath.split(os.pathsep) :
                dirprojlist.update(self.getProjectsFromPath(d, name, version, casesense, select))
        self._startprojlist.update(dirprojlist)
        return dirprojlist

    def getProjectsFromDir(self, directory, name=None, version=None, casesense=True, select=None):
        dirprojlist = getProjectsFromDir(directory, 
                                         name=name, version=version, 
                                         casesense=casesense, select=select,
                                         prjclass=self._prjclass)
        if self._projectlist is None :
            self._projectlist = Set()
        else :
            dirprojlist.override(self._projectlist)             
        self._projectlist.update(dirprojlist)
        return dirprojlist
    def getProjectsFromPath(self, directory, name=None, version=None, casesense=True, select=None):
        dirprojlist = getProjectsFromPath(directory, 
                                         name=name, version=version, 
                                         casesense=casesense, select=select,
                                         prjclass=self._prjclass)
        if self._projectlist is None :
            self._projectlist = Set()
        else :
            dirprojlist.override(self._projectlist)             
        self._projectlist.update(dirprojlist)
        return dirprojlist
    def dependencies(self):
        log = logging.getLogger()
        for p in self._startprojlist :
            log.info("Calculating dependencies for %s %s" % (p.name(), p.version()))
            for data in walk(top=p, cmtprojectpath=self._projectpath) :
                deps = data[1]
                deps.override(self._projectlist)
                self._projectlist.update(deps)

    def show(self, showpackages=False):
        for p in self._projectlist:
            if p in self._startprojlist :
                print p.summary(cmtprojectpath=self._projectpath, showpackages=showpackages)
            else :
                print p.summary(cmtprojectpath=self._projectpath, dependent=True, showpackages=showpackages)

    