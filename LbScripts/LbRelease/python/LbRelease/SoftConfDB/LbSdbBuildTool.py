#!/usr/bin/env python
"""

Script to set the release request flag on a project/version

"""
import logging
import sys

from LbUtils.Script import Script
from LbRelease.SoftConfDB.SoftConfDB import SoftConfDB


class LbSdbBuildTool(Script):
    """ Update information about a project / version to say whether it was built with CMake or CMT"""

    def defineOpts(self):
        """ Script specific options """
        parser = self.parser
        parser.add_option("-d",
                          dest = "debug",
                          action = "store_true",
                          help = "Display debug output")

        parser.add_option("-r", "--remove",
                          dest = "remove",
                          action = "store_true",
                          default = False,
                          help = "Remove the link to the CMake node")
        parser.add_option("--cmt",
                          dest = "usecmt",
                          action = "store_true",
                          default = False,
                          help = "Set the build tool to be CMT")
        parser.add_option("--cmake",
                          dest = "usecmake",
                          action = "store_true",
                          default = False,
                          help = "Set the build tool to be CMT")

    def main(self):
        """ Main method for bootstrap and parsing the options.
        It invokes the appropriate method and  """
        self.log = logging.getLogger()

        opts = self.options
        args = self.args
        if opts.debug:
            self.log.setLevel(logging.DEBUG)
        else:
            self.log.setLevel(logging.WARNING)
        if len(args) < 2 :
            self.log.error("Not enough arguments")
            sys.exit(1)
        else :
            project   = args[0].upper()
            version   = args[1]

        # Connect to the ConfDB to update the platform
        self.mConfDB = SoftConfDB()

        buildtool = "cmake"
        if self.options.usecmt:
            if self.options.usecmake:
                raise Exception("Please specify one build system only")
            buildtool = "cmt"
        
        if self.options.remove:
            self.mConfDB.unsetTool(project, version)
        else:
            self.mConfDB.setBuildTool(project, version, buildtool)


if __name__=='__main__':
    sUsage = """%prog [-r] project
    Sets the flag indicating which buildtool is used for the build"""
    s = LbSdbBuildTool(usage=sUsage)
    sys.exit(s.run())


