#!/usr/bin/env python
"""
A script to add a project to the Software Configuration DB

"""
import logging
import sys

from LbConfiguration.Repository import getRepositories
from LbUtils.Script import Script
from LbRelease import rcs
from LbRelease.SoftConfDB.AppImporter import AppImporter


class LbSdbImportProject(Script):
    """ Main scripts class for looking up dependencies.
    It inherits from """


    def defineOpts(self):
        """ Script specific options """
        parser = self.parser
        parser.add_option("--norelease",
                          dest = "norelease",
                          default = False,
                          action = "store_true",
                          help = "Disable automatic release of projects not yet in DB")

    def main(self):
        """ Main method for bootstrap and parsing the options.
        It invokes the appropriate method and  """
        self.log = logging.getLogger()
        opts = self.options
        args = self.args

        if len(args) < 2 :
            self.log.error("Not enough arguments")
            sys.exit(1)
        else :
            project   = args[0]
            version   = args[1]

            # Creating the object to import dependencies
            self.mAppImporter = AppImporter(not opts.norelease)

            self.log.warning("Checking SoftConfDB for %s %s" % (project, version))
            self.mAppImporter.processProjectVersion(project, version)

if __name__=='__main__':
    sUsage = """%prog [-n] project version  """
    s = LbSdbImportProject(usage=sUsage)
    sys.exit(s.run())


