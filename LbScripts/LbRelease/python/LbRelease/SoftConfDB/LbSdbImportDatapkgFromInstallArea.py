#!/usr/bin/env python
"""
A script to add the projects from the install area into the
Soft Configuration Database

"""
import logging
import os
import re
import sys
from datetime import datetime, timedelta

from LbConfiguration.Project import getProject
from LbUtils.Script import Script
from LbRelease.SoftConfDB.DatapkgImporter import DatapkgImporter
from LbRelease.Archive.ProjectManager import ProjectManager

class LbSdbImportDatapkgFIA(Script):
    """ Main scripts class for looking up dependencies.
    It inherits from """

    def defineOpts(self):
        """ Script specific options """
        parser = self.parser
        parser.add_option("-t",
                          dest = "nbdays",
                          action = "store",
                          default="-1",
                          help = "Number of days to include in the search. Only the ones more recent are added. -1 means all of them")
        parser.add_option("-a",
                          dest = "installarea",
                          action = "store",
                          default=None,
                          help = "Location of install area (defaults to $LHCBRELEASES)")
        parser.add_option("-d",
                          dest = "debug",
                          action = "store_true",
                          help = "Display debug output")
        parser.add_option("-b",
                          dest = "blank",
                          action = "store_true",
                          help = "Dry-run mode, just print out the actions to be executed")
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

        # Checking
        installareapath = opts.installarea
        if installareapath == None:
            installareapath = os.environ['LHCBRELEASES']
        if installareapath == None or installareapath == "":
            self.log.error("Could not find install area to process")
            sys.exit(1)
        self.log.warn("Processing Install Area: %s" % installareapath)
        self.installareapath = installareapath

        # Setting the number of days
        nbdays = int(opts.nbdays)

        # Creating the object to import dependencies
        self.mImporter = DatapkgImporter()

        if len(args) == 0:
            self.log.error("Please specify the data package projects to scan (DBASE or PARAM)")
            sys.exit(1)

        projects = [a.upper() for a in args]

        for p in projects:
            try:
                self.log.info("Processing %s" % p)
                # Actually import the data into the DB
                self.mImporter.importProjectDir(os.path.join(self.installareapath, p),
                                                None,
                                                nbdays)
            except Exception, e:
                self.log.error("Problem with release for %s" % p + str(e))
                


if __name__=='__main__':
    sUsage = """%prog
     -t <nb days>: Number of days in the history to be imported
     -d: Prints debug output.
      """
    s = LbSdbImportDatapkgFIA(usage=sUsage)
    sys.exit(s.run())


