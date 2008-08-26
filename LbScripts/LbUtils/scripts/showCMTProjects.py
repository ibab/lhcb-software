#!/usr/bin/env python
# $Id: showCMTProjects.py,v 1.13 2008-08-26 08:46:53 hmdegaud Exp $

from LbUtils.CMT import Project, Graph
from LbUtils.Script import Script
from LbUtils.Set import Set


import copy
import os

class showCMTProjScript(Script):
    _version = "fake version"
    def defineOpts(self): 
        parser = self.parser
        parser.set_defaults(casesensitive=True)
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

        parser.add_option("-t", "--tags",
                          dest="tags",
                          help="set CMT extra tags" )


        parser.set_defaults(directory=None)
        parser.add_option("-D", "--directory",
                          dest = "directory",
                          help = "directory to look for projects")


#------------------------------------------------------------------------------ 

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
        g.show(options.packages)
        
#------------------------------------------------------------------------------ 
if __name__ == '__main__':

    s = showCMTProjScript(usage="%prog [options] projectname version")
    s.run()
    

