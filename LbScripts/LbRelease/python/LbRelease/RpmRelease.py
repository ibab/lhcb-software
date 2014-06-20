#!/usr/bin/env python
# $Id: $

import os, sys, re
from subprocess import Popen, PIPE
import shutil

from LbUtils.Script import Script
from LbConfiguration.Version import ParseSvnVersion

## @class ReleaseSlot
# Main script class for to release RPMs to the repository.
class ReleaseSlot(Script):
    _version = ParseSvnVersion("$Id: $", "$URL: $")
    def __init__(self):
        Script.__init__(self, usage = "\n\t%prog [options] releasedir",
                              description = "Script to copy RPMs to the LHCb RPM repository and reindex the DB")

    def defineOpts(self):
        ''' User options '''
        self.parser.add_option("-i", "--interactive", action = "store_true", default=False,
                               help = "Prompt before copying the files")
        self.parser.add_option("-r", "--rpm-dir", action = "store", default="/afs/cern.ch/lhcb/distribution/rpm/lhcb",
                               help = "Prompt before copying the files")
        self.parser.add_option("--rpm-regex", action = "store", default=None,
                               help = "Regexp for the RPM names to copy")
        self.parser.add_option("-c", "--copy", action = "store_true", default=False,
                               help = "Really copy the files, in dry-run mode otherwise")

    def releaseRpms(self, builddir, repodir, copymode, rpmre):
        ''' Release the RPMs in builddir to the RPM repo ''' 

        builddir = os.path.abspath(builddir)
        repodir = os.path.abspath(repodir)

        self.log.warning("Build dir: %s" % builddir)
        self.log.warning("Repo  dir: %s" % repodir)
        
        if not os.path.exists(builddir):
            raise Exception("The build directory %s does not exist" % builddir)
        
        if not os.path.exists(repodir):
            raise Exception("The RPM repository %s does not exist" % repodir)

        # Listing the RPMs in the build dir
        rpms = [ f for f in os.listdir(builddir) if f.endswith(".rpm") ]

        # Iterating on the rpms
        newrpms = []
        import re
        for rpm in rpms:
            if rpmre != None:
                if not re.match(rpmre, rpm):
                    continue
                
            rpminbuild = os.path.join(builddir, rpm)
            rpminrepo  = os.path.join(repodir, rpm)
            if os.path.exists(rpminrepo):
                self.log.warning("RPM EXISTS: %s already in repository" % rpm)
            else:
                self.log.warning("RPM NEW   : %s will be copied to repository" % rpm)
                if copymode:
                    shutil.copy(rpminbuild, repodir)
                    newrpms.append(rpm)

        # Returning 
        return newrpms

    def updateRepoDB(self, repodir):
        ''' Recreate/Update the YUM repository DB  '''

        if not os.path.exists(repodir):
            raise Exception("The RPM repository %s does not exist" % repodir)
        
        self.log.warning("Updating RPM repository %s" % repodir)
        os.system("createrepo --workers=20 --update %s" % repodir)


    def main(self):
        ''' Main method for the script '''
        if len(self.args) != 1:
            self.parser.error('Please specify the directory with the RPMs')

        if not self.options.copy:
            self.log.warning("In dry-run mode. use --copy to perform the actual copy")
        else:
            self.log.warning("Copying RPMs to the YUM repository")

        copiedrpms = self.releaseRpms(self.args[0], self.options.rpm_dir, self.options.copy, self.options.rpm_regex)
        if len(copiedrpms) > 0 and self.options.copy:
            self.updateRepoDB(self.options.rpm_dir)
        
