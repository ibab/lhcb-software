###############################################################################
# (c) Copyright 2014 CERN                                                     #
#                                                                             #
# This software is distributed under the terms of the GNU General Public      #
# Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
"""

Class to extract install LCG RPMs and repackage them as classic
LHCb tar files

"""

import logging
import os
import urllib
import subprocess
import stat

from LbRelease.LbYum.LbInstall import LbInstallConfig, InstallArea
from LbRelease.LbYum.LHCbConfig import Config

INSTALLERNAME = "lcg_install.sh"
LCGDIRNAME = "lcg"
PACKAGE_EXCLUSION_LIST = ['etc', 'lib', 'tmp',  'usr',  'var', INSTALLERNAME ]

class LCGYumInstaller(object):
    '''
    Class to install a set of LCG packages to a specific area
    '''
    def __init__(self, siteroot):
        '''
        Initialize the siteroot an the client
        '''
        self.log  = logging.getLogger(__name__)
        self._siterootBase = os.path.abspath(siteroot)
        self._siteroot = self._siterootBase
        self._lcgDir = os.path.join(self._siterootBase, LCGDIRNAME)
        self._config = LbInstallConfig("LHCbConfig")
        self._config.configInst = Config()            
        self._config.siteroot = self._siteroot
        self._installArea = InstallArea(self._config)

    def install(self, rpm, nodeps=False):
        '''
        Run an install command on the install area
        '''
        if isinstance(rpm, str) or isinstance(rpm, unicode):
            return self._installArea.installRpm(rpm, nodeps=nodeps)
        else:
            return self._installArea.installPackage(rpm, nodeps=nodeps)
    

    def findRpm(self, rpm):
        '''
        Fin a specific RPM instance
        '''
        package =  self._installArea.lbYumClient.findLatestMatchingName(rpm)
        return package

    def checkInstallArea(self, packageList):
        '''
        Check which files are packages, which are NOT
        '''
        # First we parse the package list to see which top dirs we should go down into!
        # Only works 1st level is needed!
        subdirs = {}
        topdirfilesrequested = set()
        finallist = []
        for p in packageList:
            # First a safety check
            if p.count(os.sep) > 1:
                raise Exception("Error: install area check only works for top level directory")

            # Check subdirs
            if p.count(os.sep) == 1:
                topdirname = p.split(os.sep)[0]
                extname = p.split(os.sep)[1]
                tmplist = subdirs.get(topdirname)
                if tmplist == None:
                    tmplist = []
                tmplist.append(extname)
                subdirs[topdirname]=tmplist
            # And topdir
            else:
                topdirfilesrequested.add(p)
                
        # Now comparing the requested files with the files on disk...
        # First the topdir....
        topdirfiles = set(os.listdir(LCGDIRNAME)) - set(subdirs.keys())
        unpackaged = topdirfiles - topdirfilesrequested
        missing = topdirfilesrequested - topdirfiles
        if len(unpackaged) > 0:
            self.log.error("Unpackaged files:" + ",".join(unpackaged))
        if len(missing) > 0:
            self.log.error("Missing    files:" + ",".join(missing))

        finallist.extend(topdirfiles - missing)
            
        # Now doing the same on subdirs
        for k, v in subdirs.items():
            files = set(os.listdir(os.path.join(LCGDIRNAME, k)))
            requested = set(v)
            unpackaged = files - requested
            missing = requested - files
            if len(unpackaged) > 0:
                self.log.error("Subdir: %s Unpackaged files: %s" % (k, ",".join(unpackaged)))
            if len(missing) > 0:
                self.log.error("Subdir: %s Missing    files: %s" % (k,  ",".join(missing)))

            finallist.extend(list(requested - missing))

        # Now returning the actual list of files to package
        return finallist

    def prepareLCGTar(self, targetfile, packageList):
        '''
        Create the tar file of the install area
        '''
        # Checking which files to package
        os.chdir(self._lcgDir)
        allfiles = os.listdir(self._lcgDir)
        packfiles = allfiles
        if packageList != None:
            packfiles = packageList
            actuallist = self.checkInstallArea(packageList)
        else:
            actuallist = packfiles
   
        str = "tar zcf " + targetfile
        str += ' --exclude "*/InstallArea*" '
        str += ' --exclude "*.tar.gz" '
        str += ' --exclude "*.tgz" '
        str += ' --exclude "*.rpm" '
        for f in actuallist:
            if f not in PACKAGE_EXCLUSION_LIST:
                str += ' %s ' % f

        self.log.warning("In directory: %s" % self._siterootBase)
        self.log.warning("Running command: %s" % str)
        rc = os.system(str)
        if rc == 0:
            self.log.warning("Tar file creation successful, now creating md5sum file")
        else:
            raise Exception("Problem creating tar file")
 
    def isAreaEmpty(self):
        '''
        Check whether an install was already done or not
        '''
        isEmpty = False
        return (len([ f for f in os.listdir(self._siteroot)
                      if f not in PACKAGE_EXCLUSION_LIST] ) == 0)


