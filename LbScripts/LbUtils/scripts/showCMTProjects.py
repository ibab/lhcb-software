#!/usr/bin/env python

from LbUtils.CMT import Project
from LbUtils.Option import Parser
from LbUtils import Env
from optparse import OptionParser
import logging
import os

if __name__ == '__main__':
    
    usage = "%prog [options] projectname version"
    parser = Parser(usage=usage, version="%prog v1r0")

    env = Env.getDefaultEnv()
    log = logging.getLogger()
        
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
    
    (options, args) = parser.parse_args()

    del env["CMTPATH"]
    
    projname = None
    if len(args) > 0:
        projname = args[0]
    
    projvers = None
    if len(args) > 1:
        projvers = args[1]

    projlist = Project.getProjects(options.cmtprojectpath, 
                                   projname, projvers, 
                                   options.casesensitive, 
                                   options.selection)
    
    for p in projlist:
        print p.location()
        if options.showdependencies : 
            p.getDependencies()
        

