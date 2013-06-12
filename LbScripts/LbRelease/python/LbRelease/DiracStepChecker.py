#!/usr/bin/env python
"""
A script to check whether

"""
import logging
import os
import sys

from LbConfiguration.Project import getProject
from LbUtils.Script import Script

class DiracStepChecker(Script):
    """ Main scripts class to check the applications used by DIRAC """

    def defineOpts(self):
        """ Script specific options """
        parser = self.parser
        parser.add_option("-d",
                          dest = "debug",
                          action = "store_true",
                          help = "Display debug output")

    def diracInit(self):
        """ Initialise DIRAC and the book keeping """
        from DIRAC.Core.Base.Script import parseCommandLine
        from LHCbDIRAC.BookkeepingSystem.Client.BookkeepingClient import BookkeepingClient
        parseCommandLine()
        self.bkClient = BookkeepingClient()

    def main(self):
        """  Parse the args, validate the project and start the DIRAC method """
        self.log = logging.getLogger()
        opts = self.options
        args = self.args
        if opts.debug:
            self.log.setLevel(logging.DEBUG)
        else:
            self.log.setLevel(logging.WARNING)

        if len(args) < 2 :
            # Now calling the bookkeeping
            self.diracInit()
            self.listAppsInUSedSteps()
            exit(0)

            #self.log.error("Not enough arguments")
            #self.parser.print_help()
            #sys.exit(1)
        else :
            projectNameCmd   = args[0]
            version   = args[1]

        # First validating the project name
        project = getProject(projectNameCmd, True)
        if project == None:
            self.log.error("Could not find project: %s" % projectNameCmd)
            sys.exit(1)
        projectName = project.Name()


        # Now calling the bookkeeping
        self.diracInit()
        isUsed = self.isProjectUsed(projectName, version)
        if isUsed:
            sys.exit(1)
        else:
            sys.exit(0)

    def isProjectUsed(self, projectName, version):
        """ Check in the book keeping if there is a step using the application """

        isUsed = True

        # Connection to Book Keeping
        rc = self.bkClient.getAvailableSteps({'ApplicationName': projectName, 'ApplicationVersion': version, 'Usable':'Yes'})
        if (not rc['OK']):
            raise Exception("Error connecting to BookKeeping: %s" % rc['Message'])

        # Checking the dictionary to find the number of steps
        for r in rc['Value']['Records']:
            self.log.debug(r)

        # Checking the number the Total numebr of records
        nbSteps = rc['Value']['TotalRecords']
        self.log.info("Number of steps using %s %s: %s" % (projectName, version, nbSteps))
        if nbSteps == 0:
            isUsed = False

        return isUsed


    def listAppsInUSedSteps(self):
        """ List all used applications """

        # Connection to Book Keeping
        rc = self.bkClient.getAvailableSteps({'Usable':'Yes'})
        if (not rc['OK']):
            raise Exception("Error connecting to BookKeeping: %s" % rc['Message'])

        # Checking the dictionary to find the number of steps
        allrecords = rc['Value']['Records']
        allapps = set()
        for r in allrecords:
            self.log.debug(r)
            allapps.add((r[2], r[3]))

        for (p,v) in sorted(allapps):
            print "%s %s" % (p, v)

if __name__=='__main__':
    sUsage = """%prog <project> <version>
     -d: Prints debug output.
     -l: Print all projects used
      """
    s = DiracStepChecker(usage=sUsage)
    sys.exit(s.run())


