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
from LbRelease.SoftConfDB.AppImporter import AppImporter
from LbRelease.Archive.ProjectManager import ProjectManager

class LbSdbImportFIA(Script):
    """ Main scripts class for looking up dependencies.
    It inherits from """

    def defineOpts(self):
        """ Script specific options """
        parser = self.parser
        parser.add_option("-t",
                          dest = "nbdays",
                          action = "store",
                          default="30",
                          help = "Number of days to include in the search. Only the ones more recent are added. -1 means all of them")
        parser.add_option("-c",
                          dest = "checkactive",
                          action = "store_true",
                          help = "reset the active flag for all entries in the install area")
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
        self.mAppImporter = AppImporter()
        self.mAppImporter.installArea = self.installareapath

        if len(args) > 0 and len(args) < 1:
            self.log.error("Please specify the -c or -t options, or a project and version")
            sys.exit(1)

        projects = []
        if len(args) > 0:
            projects.append((args[0].upper(), args[1]))
        else:
            if self.options.checkactive:
                self.log.warn("Checking the install area to know which projects are installed" % nbdays)
                projects = self.resetActiveFlag(installareapath)
                sys.exit(0)
            else:
                self.log.warn("Importing projects newer than %s days" % nbdays)
                projects = self.findRecentReleases(installareapath, nbdays)

        for (p, v) in projects:
            if opts.blank:
                self.log.warning("Would add %s %s" % (p,v))
            else:
                self.log.warning("Adding %s %s" % (p,v))
                # Actually import the data into the DB
                self.mAppImporter.recurseDependencies(p, v)
                try:
                    iaproj = self.loadProject(p, v)
                    # Setting the project as active...
                    self.mAppImporter.mConfDB.setPVActive(p, v)

                    for cfg in iaproj.releasedConfigs:
                        # Skip the noarch fake config
                        if cfg == "noarch":
                            continue
                        self.mAppImporter.mConfDB.addPVPlatform(p, v, cfg)
                        self.log.warning("Added %s %s %s" % (p, v, cfg))
                except:
                    self.log.error("Problem with release for %s %s" % (p, v))



    def findRecentReleases(self, path, nbdays):
        """ Iterate though the install area to find the projects
        newer than nbdays """
        ret = []
        for projdir in os.listdir(path):
            # Checking if this corersponds to an existing project...
            proj = None
            try:
                proj = getProject(projdir)
            except:
                pass

            # If yes, then process it...
            if proj != None:
                newprojs = self.findRecentReleasesForProject(os.path.join(path, projdir), projdir, nbdays)
                ret += newprojs
        return ret


    def resetActiveFlag(self, path):
        """ Set the Active flag correctly based on an install area """

        # First clear everything...
        # We use a btach to submit all actions at the same time
        confDB = self.mAppImporter.mConfDB
        batch = confDB.getWriteBatch()
        confDB.deleteActiveLinks(batch)

        projCounter = 0
        # First find projects
        for projdir in os.listdir(path):
            # Checking if this corresponds to an existing project...
            proj = None
            try:
                proj = getProject(projdir)
            except:
                pass

            # If yes, then process it...
            if proj != None:
                releases = self.findRecentReleasesForProject(os.path.join(path, projdir), projdir, -1)
                for (p,v) in releases:
                    try:
                        confDB.setPVActive(p, v, batch)
                        projCounter += 1
                    except:
                        self.log.warning("Ignoring %s %s" % (p, v))
        # Now submitting the batch
        batch.submit()
        self.log.warning("Added %s projects" %projCounter)




    def findRecentReleasesForProject(self, path, project, nbdays):
        """ Iterate though the projects directories
        newer than nbdays (-1 means all...)"""
        ret = []
        now = datetime.now()
        delta =timedelta (days = nbdays)
        for pver in os.listdir(path):
            # Checking if this corresponds to an existing project...
            m = re.match("%s_([\d\w]+)" % project, pver)
            if m != None:
                version = m.group(1)
                then = datetime.fromtimestamp(os.path.getctime(os.path.join(path, pver)))
                if (now - then) <  delta or nbdays < 0:
                    self.log.info("Found project %s %s" % (project, version))
                    ret.append((project, version))
                    #print "%s -- %s -- %s" % (project, m.group(1), time.ctime(os.path.getctime(os.path.join(path, pver))))
        return ret


    def loadProject(self, projectName, projectVersion):
        """ Inspect the Install Area to find all files related to the project"""
        p = ProjectManager.loadFromDisk(projectName, projectVersion)
        return p


if __name__=='__main__':
    sUsage = """%prog
     -t <nb days>: Number of days in the history to be imported
     -d: Prints debug output.
      """
    s = LbSdbImportFIA(usage=sUsage)
    sys.exit(s.run())


