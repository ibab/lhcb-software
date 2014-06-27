#!/usr/bin/env python
"""
A script to add a project to the Software Configuration DB

"""
import logging
import sys

from LbConfiguration.Repository import getRepositories
from LbUtils.Script import Script
from LbRelease import rcs
from LbRelease.SoftConfDB.SoftConfDB import SoftConfDB

# SVN Repositories definition
url = str(getRepositories(protocol='anonymous')["lbsvn"])
lbsvn = rcs.connect(url)

gaudiurl = str(getRepositories(protocol='anonymous')["gaudi"])
gaudisvn = rcs.connect(gaudiurl)

class LbSdbAddPlatform(Script):
    """ Main scripts class for looking up dependencies.
    It inherits from """

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
        if len(args) < 3 :
            self.log.error("Not enough arguments")
            sys.exit(1)
        else :
            project   = args[0].upper()
            version   = args[1]
            platform  = args[2]

            # Connect to the ConfDB to update the platform
            self.mConfDB = SoftConfDB()
            self.mConfDB.addPVPlatform(project, version, platform)

if __name__=='__main__':
    sUsage = """%prog project version platform   """
    s = LbSdbAddPlatform(usage=sUsage)
    sys.exit(s.run())


