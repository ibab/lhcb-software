'''

Utility classes to interface with Dirac.

to use:

  archiver = DiracProjectArchiver(projectName, version)
  res = archiver.checkArchiveStatus()

Created on Mar 6, 2012

@author: Ben Couturier
'''
import logging
from DIRAC.Interfaces.API.Dirac import Dirac
from ProjectManager import ProjectManager, SOFTWARE_SE

#
# Main Archiving class
###############################################################################
class DiracArchiver(object):

    def __init__(self):
        """ Constructor for the archiver which initializes the necessary
        dependencies """
        self.dirac = Dirac()
        self.log = logging.getLogger()

    def lfnExists(self, lfn):
        """ Checks whether a given LFN already exists in DIRAC """
        ret = False
        result = self.dirac.getMetadata(lfn)
        # OK key tells us that the call went fine
        if result["OK"]:
            # The result has a key called Value.
            # It contains maps called Successful and Failed
            # with the results for each lfn passed
            if lfn in result["Value"]["Successful"].keys():
                ret = True
        else:
            raise Exception("DIRAC getMetadata error:" + result)
        return ret

    def archiveFile(self, filename, lfn, storageElement=SOFTWARE_SE):
        """ Archive a file to the storage element """
        ret = self.dirac.addFile(lfn, filename, storageElement)
        return ret


#
# Specific lass to Archive projects
###############################################################################
class DiracProjectArchiver(DiracArchiver):
    """ Specific class to archive projects. It uses the project manager to
    know what to archive for a specific project """

    def __init__(self, project, version=None):
        """ Constructor for the project archiver. """
        super(DiracProjectArchiver, self).__init__()

        # Either we are passed a couple projectName/version
        if  isinstance(project, str) and version != None:
            # Loading the project
            self.project = ProjectManager.loadFromDisk(project, version)
        elif isinstance(project, ProjectManager):
            self.project = project
        else:
            raise Exception("DiractProjectArchivers constructur takes ProjectManager|(name, version)")

            # Or a projectManager directly

    def checkArchiveStatus(self):
        """ method that checks whether all necessary files are archived correctly
        It takes into account:
        - The dist tar.gz files and associated MD5s
        - the HTML files and associated MD5s
        It returns a list contains couples: LFN , Archived(True or False)
        """
        p = self.project
        res = []
        for f in p.getDistFiles():
            lfn = f.getLFN()
            self.log.debug("Checking LFN %s" % lfn)
            state = self.lfnExists(lfn)
            res.append((f, state))
        return res

    def archiveProject(self):
        """ Method that archives all files for a project to DIRAC """
        p = self.project
        res = []
        for f in p.getDistFiles():
            lfn = f.getLFN()
            self.log.info("Archiving %s to %s" % (f.getDistFileName(), lfn))
            if self.lfnExists(lfn):
                self.log.info("File already archived %s" % f.getDistFileName())
            else:
                res = self.archiveFile(f.getFullPath(), lfn)
                self.log.debug(res)
                if res['OK'] == False:
                    self.log.error("Archive Error: " + res['Message'])
                else:
                    self.log.info("Archived %s" % f.getDistFileName())


