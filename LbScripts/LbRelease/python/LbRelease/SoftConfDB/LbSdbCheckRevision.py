#!/usr/bin/env python
"""
Script to update links to project version nodes

"""
import logging
import sys

from LbUtils.Script import Script
from LbRelease.SoftConfDB.SoftConfDB import SoftConfDB
from LbRelease.SoftConfDB.AppImporter import getProjectLastRev

class LbSdbCheckRevision(Script):
    """ Update information about a project / version """

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
            project   = args[0].upper()
            version   = args[1]

            # Connect to the ConfDB to update the platform
            self.mConfDB = SoftConfDB()
            n = self.mConfDB.mNeoDB.get_indexed_node("ProjectVersion",
                                                 "ProjectVersion",
                                                 project + "_" + version)
            dbRev = n['Rev']
            svnRev = getProjectLastRev(project, version)

            status = "ERROR"
            if dbRev == svnRev:
                status = "OK"
            print "%5s %s %s" % (status, dbRev, svnRev)
            sys.exit(dbRev != svnRev)
            
if __name__=='__main__':
    sUsage = """%prog project version  """
    s = LbSdbCheckRevision(usage=sUsage)
    sys.exit(s.run())


