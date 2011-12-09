from LbUtils.Set import Set
from LbUtils.CMT import Package
from LbConfiguration.Platform import binary_list
import os


class LbPackage(Package):
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
        notes = ""
        return notes
