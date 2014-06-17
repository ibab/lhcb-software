#!/usr/bin/env python
"""

Script to set the release request flag on a project/version

"""
import logging
import sys

from LbUtils.Script import Script
from LbRelease.SoftConfDB.SoftConfDB import SoftConfDB


class LbSdbCMake(Script):
    """ Update information about a project / version to say whether it was built with CMake"""

    def defineOpts(self):
        """ Script specific options """
        parser = self.parser
        parser.add_option("-d",
                          dest = "debug",
                          action = "store_true",
                          help = "Display debug output")

        parser.add_option("-r",
                          dest = "remove",
                          action = "store_true",
                          default = False,
                          help = "Remove the link to the CMake node")


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

        if self.options.remove:
            self.mConfDB.unsetCMakeBuild(project, version)
        else:
            self.mConfDB.setCMakeBuild(project, version)


if __name__=='__main__':
    sUsage = """%prog [-r] project
    Sets the flag indicating that the project was built with CMake"""
    s = LbSdbCMake(usage=sUsage)
    sys.exit(s.run())


