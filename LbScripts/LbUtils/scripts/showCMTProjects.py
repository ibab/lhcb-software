#!/usr/bin/env python
# $Id: showCMTProjects.py,v 1.9 2008-01-23 15:06:15 hmdegaud Exp $

from LbUtils.CMT import Project
from LbUtils.Script import Script

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
        
        parser.set_defaults(showdependencies=False)
        parser.add_option("-d", "--dependencies",
                          action = "store_true",
                          dest = "showdependencies",
                          help = "show project dependencies")
        parser.set_defaults(showbase=False)
        parser.add_option("-b", "--base",
                          action = "store_true",
                          dest = "showbase",
                          help = "show base projects")

        parser.set_defaults(showclient=False)
        parser.add_option("-c", "--client",
                          action = "store_true",
                          dest = "showclient",
                          help = "show client projects")

        parser.set_defaults(recursebase=False)
        parser.add_option("-B", "--recurse-base",
                          action = "store_true",
                          dest = "recursebase",
                          help = "recurse in the base direction")


        parser.set_defaults(recurseclient=False)
        parser.add_option("-C", "--recurse-clients",
                          action = "store_true",
                          dest = "recurseclient",
                          help = "recurse in the client direction")

        parser.set_defaults(showcontainedpackages=False)
        parser.add_option("--show-contained-packages",
                          action = "store_true",
                          dest = "showcontainedpackages",
                          help = "show contained packages in the selected project(s)")

        parser.set_defaults(showusedpackages=False)
        parser.add_option("--show-used-packages",
                          action = "store_true",
                          dest = "showusedpackages",
                          help = "show used packages in the selected project(s) according to the list of binaries")

        parser.add_option("--binaries",
                          dest="binaries",
                          help="set binaries platforms", 
                          fallback_env="CMTCONFIG")

        parser.add_option("-t", "--tags",
                          dest="tags",
                          help="set CMT extra tags" )
        
    def main(self):
        if self.env.has_key("CMTPATH"): 
            del self.env["CMTPATH"]
    
        projname = None
        if len(self.args) > 0:
            projname = self.args[0]
        
        projvers = None
        if len(self.args) > 1:
            projvers = self.args[1]
        
        options = self.options
    
        projlist = Project.getProjects(options.cmtprojectpath, 
                                       projname, projvers, 
                                       options.casesensitive, 
                                       options.selection)

        if options.binaries :
            binary_list = options.binaries.split(",")
        if options.tags :
            tag_list = options.tags.split(",")
        
        for p in projlist:
            if not options.recurseclient and not options.recursebase:
                p.show(options.showdependencies, options.showbase, options.showclient)
                if options.showcontainedpackages :
                    p.getContainedPackages()
                    p.showContainedPackages()
                if options.showusedpackages :
                    p.getContainedPackages()
                    p.showUsedPackages(binary_list)
            else :
                if options.recurseclient :
                    for proj, deps, packs in Project.walk(p, topdown=True, toclients=True):
                        proj.show(options.showdependencies, options.showbase, True)
                        if options.showcontainedpackages :
                            p.getContainedPackages()
                            p.showContainedPackages()
                        if options.showusedpackages :
                            p.getContainedPackages()
                            p.showUsedPackages(binary_list)
                else :
                    for proj, deps, packs in Project.walk(p, topdown=True, toclients=False):
                        proj.show(options.showdependencies, True, options.showclient)
                        if options.showcontainedpackages :
                            p.getContainedPackages()
                            p.showContainedPackages()
                        if options.showusedpackages :
                            p.getContainedPackages()
                            p.showUsedPackages(binary_list)

        

if __name__ == '__main__':

    s = showCMTProjScript(usage="%prog [options] projectname version")
    s.run()
    

