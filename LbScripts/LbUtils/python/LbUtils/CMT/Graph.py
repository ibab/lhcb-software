# local import
from Project import getProjectsFromDir, getProjectsFromPath, Project
from Project import walk

# package import
from LbUtils.Set import Set
from LbUtils.Script import Script

# global import
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

    def show(self, showpackages=False, afsinfo=False):
        for p in self._projectlist:
            if p in self._startprojlist :
                print p.summary(cmtprojectpath=self._projectpath, showpackages=showpackages, afsinfo=afsinfo)
            else :
                print p.summary(cmtprojectpath=self._projectpath, dependent=True, showpackages=showpackages)

    
    
class showCMTProjScript(Script):
    def defineOpts(self): 
        parser = self.parser
        parser.set_defaults(casesensitive=False)
        parser.add_option("-s", "--case-sensitive",
                          action="store_true", 
                          dest="casesensitive",
                          help="matching of the project name in case sensitive mode "
                          "[default: %default]")
        parser.add_option("-i", "--case-insensitive",
                          action="store_false", 
                          dest="casesensitive",
                          help="matching of the project name in case insensitive mode")
    
        parser.add_option("-P", "--project-path",
                          dest="cmtprojectpath",
                          help="set CMTPROJECTPATH", 
                          fallback_env="CMTPROJECTPATH")
        
        parser.add_option("-S", "--select",
                          dest = "selection",
                          help = "filter project path according to the selection")
        
        parser.set_defaults(dependencies=False)
        parser.add_option("-d", "--dependencies",
                          action = "store_true",
                          dest = "dependencies",
                          help = "show project dependencies")

        parser.set_defaults(packages=False)
        parser.add_option("-p","--packages",
                          action = "store_true",
                          dest = "packages",
                          help = "show packages in the selected project(s)")

        parser.set_defaults(afsinfo=False)
        parser.add_option("-a","--afs-info",
                          action = "store_true",
                          dest = "afsinfo",
                          help = "Show AFS volume informations if any")

        
        parser.add_option("-t", "--tags",
                          dest="tags",
                          help="set CMT extra tags" )


        parser.set_defaults(directory=None)
        parser.add_option("-D", "--directory",
                          dest = "directory",
                          help = "directory to look for projects")



    def main(self):
        if self.env.has_key("CMTPATH"): 
            del self.env["CMTPATH"]
    
        name = None
        if len(self.args) > 0:
            name = self.args[0]
        
        version = None
        if len(self.args) > 1:
            version = self.args[1]
        
        options = self.options
        
        g=Graph(options.cmtprojectpath)
    
        g.getStartProjectsFromDir(directory=options.directory, 
                                  name=name, version=version,
                                  casesense=options.casesensitive,
                                  select=options.selection)

        if options.dependencies is not None:
            g.dependencies()
        g.show(options.packages, options.afsinfo)
    