#!/usr/bin/env python
"""
Script to update links to project version nodes

"""
import logging
import sys

from LbUtils.Script import Script
from LbRelease.SoftConfDB.SoftConfDB import SoftConfDB


class LbSdbSetAllAppUsed(Script):
    """ Script to set all apps versiosn as used """

    def defineOpts(self):
        """ Script specific options """
        parser = self.parser
        parser.add_option("-d",
                          dest = "debug",
                          action = "store_true",
                          help = "Display debug output")


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

        # Connect to the ConfDB to update the platform
        self.mConfDB = SoftConfDB()
        self.mConfDB.setupDB()
        self.mConfDB.setAllAppVersionsUsed()

if __name__=='__main__':
    sUsage = """%prog [-r] project
    Sets the project as an Application """
    s = LbSdbSetAllAppUsed(usage=sUsage)
    sys.exit(s.run())


