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

# SVN Repositories definition
url = str(getRepositories(protocol='anonymous')["lbsvn"])
lbsvn = rcs.connect(url)

gaudiurl = str(getRepositories(protocol='anonymous')["gaudi"])
gaudisvn = rcs.connect(gaudiurl)

class LbSdbImportProject(Script):
    """ Main scripts class for looking up dependencies.
    It inherits from """

    def defineOpts(self):
        """ Script specific options """
        parser = self.parser
        parser.add_option("-r",
                          dest = "recursive",
                          action = "store_true",
                          help = "Display dependencies recursively")
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
        if len(args) < 2 :
            self.log.error("Not enough arguments")
            sys.exit(1)
        else :
            project   = args[0]
            version   = args[1]

            # Creating the object to import dependencies
            self.mAppImporter = AppImporter()

            # Now get the dependencies
            if self.options.recursive:
                self.mAppImporter.recurseDependencies(project, version)
            else:
                self.mAppImporter.processDependencies(project, version)

if __name__=='__main__':
    sUsage = """%prog [-r] project version
     -r: Recursively lists all dependencies.
     -d: Prints debug output.
      """
    s = LbSdbImportProject(usage=sUsage)
    sys.exit(s.run())


