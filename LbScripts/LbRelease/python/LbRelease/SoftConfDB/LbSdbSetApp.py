#!/usr/bin/env python
"""
Script to update links to project version nodes

"""
import logging
import sys

from LbUtils.Script import Script
from LbRelease.SoftConfDB.SoftConfDB import SoftConfDB


class LbSdbSetApp(Script):
    """ Update information about a project / version """

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
                          help = "Remove the link to the application node")


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
        if len(args) < 1 :
            self.log.error("Not enough arguments")
            sys.exit(1)
        else :
            project   = args[0].upper()

                # Connect to the ConfDB to update the platform
        self.mConfDB = SoftConfDB()

        if self.options.remove:
            self.mConfDB.unsetApplication(project)
        else:
            self.mConfDB.setApplication(project)


if __name__=='__main__':
    sUsage = """%prog [-r] project
    Sets the project as an Application """
    s = LbSdbSetApp(usage=sUsage)
    sys.exit(s.run())


