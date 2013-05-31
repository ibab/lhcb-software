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
                          help = "Set the action to be done: a: clear active flags, u: clear used flags")


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
            if self.options.action.lower() == "a":
                self.mConfDB.setPVActive(project, version)
            elif self.options.action.lower() == "u":
                self.mConfDB.setPVUsed(project, version)
            else:
                self.log.error("Unknown action %s " %  self.options.action)

if __name__=='__main__':
    sUsage = """%prog project version  """
    s = LbSdbClear(usage=sUsage)
    sys.exit(s.run())


