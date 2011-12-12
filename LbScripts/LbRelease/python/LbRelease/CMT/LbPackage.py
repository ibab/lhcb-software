from LbRelease.ReleaseNotes import extract


from LbUtils.Set import Set
from LbUtils.CMT import Package
from LbConfiguration.Platform import binary_list
import os
import logging


class LbPackage(Package):
    def __init__(self, packagepath, parentproject=None, parentprojectpath=None):
        super(LbPackage, self).__init__(packagepath, parentproject, parentprojectpath)
        self._release_notes = None
    def binaryList(self):
        """ override binary list: use the standard LHCb binary list as base """
        if self._binarylist is None :
            self._binarylist = Set()
            # the "default" value is used when no CMTCONFIG is set
            tmplist = ["default"]
            for f in binary_list :
                tmplist.append(f)
            # verify that the binary directory exists
            for b in tmplist :
                topdirs = os.listdir(self.fullLocation())
                if b in topdirs:
                    self._binarylist.add(b)
        return self._binarylist
    def releaseNotes(self):
        log = logging.getLogger()
        if self._release_notes is None :
            relnote_path = os.path.join(self.fullLocation(), "doc","release.notes")
            self._release_notes = ""
            if os.path.exists(relnote_path):
                self._release_notes = extract(relnote_path, self.version())
            else :
                log.warn("The file %s doesn't exist" % relnote_path)
        return self._release_notes
