#!/usr/bin/env python
# $Id: fixDoxygenLinks.py,v 1.1 2008-10-30 18:43:00 hmdegaud Exp $
""" 
This tool intends to fix the wrong links to the external software created 
by Doxygen. Usually this appears with strings like "../../http://" in the href
of the links in the HTML files.
"""

from LbUtils.Script import Script
from LbUtils.CVS import CVS2Version

import os

__version__ = CVS2Version("$Name: not supported by cvs2svn $", "$Revision: 1.1 $")
__author__ = "hmd"

_doxfilesrc = ["html", "htm"]

def fixDoxygenLinksInFile(filepath, dryrun=False):
    pass


def fixDoxygenLinksInDir(dirpath, filext=None, dryrun=False):
    if not filext :
        filext = _doxfilesrc
    for root, dirs, files in os.walk(dirpath) :
        for f in files :
            fext = os.path.splitext(f)[1]
            if fext :
                fext = fext[1:]
            if fext in filext :
                fixDoxygenLinksInFile(f, dryrun)


class FixDoxygenLinksScript(Script):
    _version = __version__
    _description = __doc__
    def defineOpts(self):
        parser = self.parser 
        parser.set_defaults(dryrun=False)
        parser.add_option("--dry-run",
                          action="store_true", 
                          dest="dryrun",
                          help="Dry run mode. Doesn't modify anything. Only prints "
                          "[default: %default]")
        parser.set_defaults(filext=_doxfilesrc)
        parser.add_option("-e", "--file-ext",
                          action="append", 
                          dest="filext",
                          help="add extension for files to be processed "
                          "[default: %default]")

    def main(self):
        return 0


if __name__ == '__main__':
    s = FixDoxygenLinksScript(usage="%prog [options] [dir1 [dir2]]")
    s.run()
