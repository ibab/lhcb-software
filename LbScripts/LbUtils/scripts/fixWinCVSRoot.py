#!/usr/bin/env python
# $Id: fixWinCVSRoot.py,v 1.2 2008-10-29 17:43:40 hmdegaud Exp $

from LbUtils.Script import Script
from LbUtils.CVS import fixRoot, CVS2Version

import os, logging

__version__ = CVS2Version("$Name: not supported by cvs2svn $", "$Revision: 1.2 $")


def dryRunCallBack(option, opt_str, value, parser):
    log = logging.getLogger()
    setattr(parser.values, option.dest, True)
    if log.getEffectiveLevel() > logging.INFO :
        parser.values.loglevel = "INFO"
        log.setLevel(logging.INFO)
    
class FixWinCVSRootScript(Script):
    _version = __version__
    def defineOpts(self):
        parser = self.parser
        parser.set_defaults(dryrun=False)
        parser.add_option("--dry-run",
                          action="callback",
                          callback = dryRunCallBack, 
                          dest="dryrun",
                          help="Only simulates the change [default: %default]")
        parser.set_defaults(recursive=False)
        parser.add_option("-r","--recursive",
                          action="store_true",
                          dest="recursive",
                          help="recurse to subdirectories [default: %default]")        
    def main(self):
        location = os.getcwd()
        if len(self.args) > 0 :
            location = self.args[0]
        return fixRoot(location,
                        ":kserver:", ":ext:",
                        self.options.recursive, 
                        self.options.dryrun)


if __name__ == '__main__':
    s = FixWinCVSRootScript(usage="%prog [options]")
    s.run()
