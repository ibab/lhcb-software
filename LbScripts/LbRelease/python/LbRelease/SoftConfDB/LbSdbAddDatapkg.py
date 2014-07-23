#!/usr/bin/env python
"""
A script to add a project to the Software Configuration DB

"""
import logging
import sys

from LbConfiguration.Repository import getRepositories
from LbConfiguration.Package import getPackage
from LbUtils.Script import Script
from LbRelease import rcs
from LbRelease.SoftConfDB.DatapkgImporter import DatapkgImporter

class LbSdbAddDatapkg(Script):
    """ Main script to add a Datapkg to the DB, checking the dependencies from SVN """

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
        if len(args) < 2 :
            self.log.error("Not enough arguments")
            sys.exit(1)
        else :
            fullname = args[0]
            hat = None
            datapkg   = fullname
            version   = args[1]

            # Checking if there was a / in the name to find hats
            if "/" in datapkg:
                (har, datapkg) = fullname.split("/")

            # Creating the object to import dependencies
            self.mImporter = DatapkgImporter()
            self.log.warning("Processing %s %s" % (fullname, version))

            # Getting the project from metadata
            pack = getPackage(datapkg)
            if pack == None:
                self.log.error("Could not find package %s in configuration" % datapkg)

            # Now performing the import...
            self.mImporter.importpkg(pack.project(), datapkg, version, hat)

if __name__=='__main__':
    sUsage = """%prog [-r] project version
     -d: Prints debug output.
      """
    s = LbSdbAddDatapkg(usage=sUsage)
    sys.exit(s.run())


