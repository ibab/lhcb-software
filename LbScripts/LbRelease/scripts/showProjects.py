#!/usr/bin/env python
# $Id: showProjects.py,v 1.1.1.1 2008-02-04 14:41:09 hmdegaud Exp $

from LbUtils.CMT import Project
from LbUtils.Script import Script

import os

class showProjScript(Script):
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
                          fallback_env="LHCBPROJECTPATH")
        
        parser.add_option("-S", "--select",
                          dest = "selection",
                          help = "filter project path according to the selection",
                          fallback_conf="/lhcb/")
        
        parser.set_defaults(showdependencies=False)
        parser.add_option("-d", "--dependencies",
                          action = "store_true",
                          dest = "showdependencies",
                          help = "show project dependencies")
    def main(self):
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
        
        for p in projlist:
            print p.location()
            if options.showdependencies : 
                p.getDependencies()


if __name__ == '__main__':

    s = showProjScript(usage="%prog [options] projectname version")
    s.run()
    

