'''

Utility classes to interface with Castor.

to use:

  archiver = CastorProjectArchiver(projectName, version)
  res = archiver.checkArchiveStatus()

Created on Mar 6, 2012

@author: Ben Couturier
'''
import logging
import os
from ProjectManager import ProjectManager
from LbLegacy.Utils import getStatusOutput
#
# Main Archiving class
###############################################################################
class CastorArchiver(object):

    def __init__(self):
        """ Constructor for the archiver which initializes the necessary
        dependencies """
        self.log = logging.getLogger()

    def pathExists(self, castor_path):
        """ Checks whether given file exists """
        ret = False
        # OK key tells us that the call went fine
        cmd = "nsls -l %s" % castor_path
        self.log.debug("Running '%s'" % cmd)
        (retval, output) = getStatusOutput(cmd)
        self.log.debug("Command returned '%s'" % output)
        if retval == 0:
            ret = True
        return ret

    def dirname(self, castor_path):
        """ Returns the CASTOR dirname """
        # XXX Should not use os.path actually, would not work on windows...
        return os.path.dirname(castor_path)

    def mkdir(self, castor_dir):
        """ Checks whether given file exists """
        ret = False
        # OK key tells us that the call went fine
        self.log.info("Creating CASTOR dir %s" % castor_dir)
        cmd = "nsmkdir -p %s" % castor_dir
        self.log.debug("Running '%s'" % cmd)
        (retval, output) = getStatusOutput(cmd)
        if retval == 0:
            ret = True
        return ret


    def archiveFile(self, filename, castor_filename):
        """ Utility function wrapping rfcp """
        self.log.info("CASTOR Filename %s" % castor_filename)
        cmd = "rfcp %s %s" % (filename, castor_filename)
        self.log.debug("Running '%s'" % cmd)
        (retval, output) = getStatusOutput(cmd)
        if retval != 0:
            raise Exception("Copy to CASTOR failed: %s" % output)
            self.log.error("Copy to CASTOR failed: %s" % output)
#
# Specific lass to Archive projects
###############################################################################
class CastorProjectArchiver(CastorArchiver):
    """ Specific class to archive projects. It uses the project manager to
    know what to archive for a specific project """

    def __init__(self, project, version=None):
        """ Constructor for the project archiver. """
        super(CastorProjectArchiver, self).__init__()

        # Either we are passed a couple projectName/version
        if  isinstance(project, str) and version != None:
            # Loading the project
            self.project = ProjectManager.loadFromDisk(project, version)
        elif isinstance(project, ProjectManager):
            self.project = project
        else:
            raise Exception("CastorProjectArchivers constructs takes ProjectManager|(name, version)")

    def checkArchiveStatus(self):
        """ method that checks whether all necessary files are archived correctly
        It takes into account:
        - The dist tar.gz files and associated MD5s
        - the HTML files and associated MD5s
        It returns a list contains couples: CASTOR Filename, Archived(True or False)
        """
        p = self.project
        res = []
        for f in p.getDistFiles():
            cfn = f.getCastorFilename()
            self.log.debug("Checking CASTOR filename %s" % cfn)
            state = self.pathExists(cfn)
            res.append((f, state))
        return res

    def removeArchivedFiles(self):
        """ Method that archives all files for a project to DIRAC """
        p = self.project
        res = []
        for f in p.getDistFiles():
            cfn = f.getCastorFilename()
            self.log.info("Checking if %s is archived" % f.getDistFileName())
            if self.pathExists(cfn):
                self.log.info("File already archived %s - removing" % f.getDistFileName())
                print "Running: rm -i %s" % f.getDistFileName()
                os.system("rm -i %s" % f.getDistFileName())

            else:
                # Checking if the target directory exists...
                self.log.info("Cannot remove file not archived:%s" % f.getDistFileName())


    def archiveProject(self):
        """ Method that archives all files for a project to DIRAC """
        p = self.project
        res = []
        for f in p.getDistFiles():
            cfn = f.getCastorFilename()
            self.log.info("Archiving %s to %s" % (f.getDistFileName(), cfn))
            if self.pathExists(cfn):
                self.log.info("File already archived %s, %s" % (f.getDistFileName(), cfn))
            else:
                # Checking if the target directory exists...
                dirname = f.getCastorDirname()
                if not self.pathExists(dirname):
                    self.mkdir(dirname)

                # Now copying the file
                res = self.archiveFile(f.getFullPath(), cfn)
                self.log.debug(res)
                self.log.info("Archived %s to %s" % (f.getDistFileName(), cfn))
