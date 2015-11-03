#!/usr/bin/env python
"""

Script to set the release request flag on a project/version

"""
import logging
import sys

from LbUtils.Script import Script
from LbRelease.SoftConfDB.SoftConfDB import SoftConfDB


class LbSdbTag(Script):
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

        parser.add_option("-p", "--platform",
                          dest = "platform",
                          action = "store",
                          default = None,
                          help = "Set the tag to the project/version/platform")

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
        if len(args) < 3 :
            self.log.error("Not enough arguments")
            sys.exit(1)
        else :
            project   = args[0].upper()
            version   = args[1]
            tag = args[2].upper()

        # Connect to the ConfDB to update the platform
        self.mConfDB = SoftConfDB()
       
        if self.options.remove:
            if self.options.platform != None:
                self.mConfDB.unsetPlatformTag(project, version, self.options.platform, tag)
            else:
                self.mConfDB.unsetTag(project, version, tag)
        else:
            if self.options.platform != None:
                self.mConfDB.setPlatformTag(project, version, self.options.platform, tag)
            else:
                self.mConfDB.setTag(project, version, tag)


if __name__=='__main__':
    sUsage = """%prog [-r] project version tag
    Sets the flag indicating which buildtool is used for the build"""
    s = LbSdbTag(usage=sUsage)
    sys.exit(s.run())


