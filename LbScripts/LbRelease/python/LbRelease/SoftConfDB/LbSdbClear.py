#!/usr/bin/env python
"""
Script to update links to project version nodes

"""
import logging
import sys

from LbUtils.Script import Script
from LbRelease.SoftConfDB.SoftConfDB import SoftConfDB


class LbSdbClear(Script):
    """ Update information about a project / version """

    def defineOpts(self):
        """ Script specific options """
        parser = self.parser
        parser.add_option("-d",
                          dest = "debug",
                          action = "store_true",
                          help = "Display debug output")

        parser.add_option("-s",
                          dest = "action",
                          action = "store",
                          default = "a",
                          help = "Set the action to be done: a: active, u: used")


    def main(self):
        """ Main method for bootstrap and parsing the options.
        It invokes the appropriate method and  """
        self.log = logging.getLogger()

        # Connect to the ConfDB to update the platform
        self.mConfDB = SoftConfDB()
        if self.options.action.lower() == "a":
            self.mConfDB.deleteActiveLinks()
        elif self.options.action.lower() == "u":
            self.mConfDB.deleteUsedLinks()
        else:
            self.log.error("Unknown action %s " %  self.options.action)


if __name__=='__main__':
    sUsage = """%prog project version  """
    s = LbSdbClear(usage=sUsage)
    sys.exit(s.run())


