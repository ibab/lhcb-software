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

LCGRPMURL = "http://service-spi.web.cern.ch/service-spi/external/rpms"
INSTALLERNAME = "lcg_install.sh"
RELEASEDIRNAME = "releases"
PACKAGE_EXCLUSION_LIST = ['etc', 'lib', 'tmp',  'usr',  'var', INSTALLERNAME ]


class LCGRpmInstaller(object):
    '''
    Class to install a set of LCG packages to a specific area
    '''
    def __init__(self, siteroot):
        '''
        Initialize the siteroot, and downloads the installer
        '''
        self.log  = logging.getLogger(__name__)
        # Making sure we have an abs path
        self._siterootBase = os.path.abspath(siteroot)
        self._siteroot = os.path.join(self._siterootBase, RELEASEDIRNAME)
        self._installer = os.path.join(self._siteroot, INSTALLERNAME)
        self.log.warning("Preparing LCG RPM install area: %s" % self._siteroot)
        
        # Checking the directory
        if not os.path.exists(self._siteroot):
            os.makedirs(self._siteroot)

        # Downloading the installer
        import urllib
        urllib.urlretrieve ("/".join([LCGRPMURL, INSTALLERNAME]),
                            self._installer)
        if not os.path.exists(self._installer):
            raise Exception("Failed downloading installer, cannot continue")
        st = os.stat(self._installer)
        os.chmod(self._installer, st.st_mode | stat.S_IEXEC)

    def run(self, commands):
        '''
        Run an install comamnd on the install area
        '''
        cmd = [ self._installer, "--root", self._siteroot] + commands
        self.log.debug("Running: %s" % " ".join(cmd))
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        out, err = p.communicate()
        rcode = p.returncode
        return (out, err, rcode )


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
        topdirfiles = set(os.listdir(RELEASEDIRNAME)) - set(subdirs.keys())
        unpackaged = topdirfiles - topdirfilesrequested
        missing = topdirfilesrequested - topdirfiles
        if len(unpackaged) > 0:
            self.log.error("Unpackaged files:" + ",".join(unpackaged))
        if len(missing) > 0:
            self.log.error("Missing    files:" + ",".join(missing))

        finallist.extend(topdirfiles - missing)
            
        # Now doing the same on subdirs
        for k, v in subdirs.items():
            files = set(os.listdir(os.path.join(RELEASEDIRNAME, k)))
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
        os.chdir(self._siterootBase)
        allfiles = os.listdir(RELEASEDIRNAME)
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
                str += ' %s ' % os.path.join(RELEASEDIRNAME, f)

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
        return (len(os.listdir(self._siterootBase)) == 0)
