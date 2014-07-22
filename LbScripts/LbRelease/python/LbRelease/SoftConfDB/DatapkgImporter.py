#!/usr/bin/env python
"""
A script to add a project to the Software Configuration DB

"""
import logging
import os
import re

from LbConfiguration.Repository import getRepositories
from LbRelease import rcs
from LbConfiguration.Package import getPackage, PackageConfException
from LbConfiguration.Version import getVersionsFromDir

from LbUtils.Processes import callCommand
from LbRelease.SoftConfDB.SoftConfDB import SoftConfDB


# SVN Repositories definition
url = str(getRepositories(protocol='anonymous')["lbsvn"])
lbsvn = rcs.connect(url)


class DatapackageImporter:
    """ Main script to look up package """

    def __init__(self):
        # Creating the SoftConfDB Object
        self.mConfDB = SoftConfDB()
        self.log = logging.getLogger()
        self.installArea = None

    def importpkg(self, project, datapkg, version, hat=None):
        """ Import a datapackage version """

        project = project.upper()
        tmp = datapkg
        if hat != None:
            tmp = "/".join([hat, datapkg])
        self.log.warning("Checking %s %s %s" % (project, tmp, version))

        # Loading the package from LbScripts
        pack = getPackage(datapkg)
        if pack.hat()!= hat:
            raise Exception("Error importing: hat does not match conf data!")

        if pack.project().upper() != project:
            raise Exception("Error importing: project does not match conf data!")
        
        
        # Check if the package actually exist
        found = lbsvn.hasVersion(tmp, version)
        if not found:
            raise Exception("Could not find package  %s %s %s in SVN" % (project, tmp, version))

        # Now creating the package if it does not exist
        self.mConfDB.getOrCreateDatapkgVersion(project, hat, datapkg, version)


    def importProjectDir(self, dirname, hat=None):
        """ Import all projects in a directory """
        import os

        # Checking existence
        if not os.path.exists(dirname):
            raise Exception("Directory %s does not exist" % dirname)

        # Getting the absolute path 
        dirabspath = os.path.realpath(os.path.abspath(dirname))

        # Now iterating on the subdirs, we try and see if they are data packages
        # That's how we separate datapackages from hats...
        for entry in os.listdir(dirabspath):
            path = os.path.join(dirabspath, entry)

            # We ignores files, just processing directories...
            if not os.path.isdir(path):
                continue

            pack = None
            try:
                pack = getPackage(entry)
            except PackageConfException:
                # This is not a package, it maybe a hat for other packages...
                # but only if we're not trying to process a hat already...
                # CAREFUL: only handle one level hats...
                if hat == None:
                    self.log.warning("Processing  subdirectory %s" % entry)
                    self.importProjectDir(os.path.join(dirname, entry), entry)

            if pack != None:
                # Now performing the import
                self.log.warning("Processing package: %s" % entry)
                self._importPackage(os.path.join(dirname, entry), hat, pack)
   
    def _importPackage(self, datapkgdir, hat, pack):
        """ Import all the versions of a package from a specific directory """
        # First some checks...
        project = pack.project()
        datapkg = pack.Name()
        if hat != pack.hat():
            raise Exception("Mismatch in hats: %s vs %s" % (hat, pack.hat()))
        if hat == None:
            har = ""

        # Now adding all the versions from the directory
        # Using LbCOnfigurations tool for that
        for ver in  getVersionsFromDir(datapkgdir):
            self.log.warning("Adding package %s %s %s %s" % (project, hat, datapkg, ver.name()))
            self.importpkg(project, datapkg, ver.name(), hat)

if __name__ == "__main__":
    dimp = DatapackageImporter()
    #dimp.importpkg("DBASE", "HltTCK", "v2r9", "TCK")
    dimp.importProjectDir("/afs/cern.ch/lhcb/software/releases/DBASE")

