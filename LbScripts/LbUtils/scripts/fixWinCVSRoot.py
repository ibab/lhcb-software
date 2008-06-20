#!/usr/bin/env python
# $Id: fixWinCVSRoot.py,v 1.1 2008-06-20 17:46:49 hmdegaud Exp $

from LbUtils.Script import Script
from LbUtils.CVS import fixRoot
import os, logging

def dryRunCallBack(option, opt_str, value, parser):
    log = logging.getLogger()
    setattr(parser.values, option.dest, True)
    if log.getEffectiveLevel() > logging.INFO :
        parser.values.loglevel = "INFO"
        log.setLevel(logging.INFO)
    
class FixWinCVSRootScript(Script):
    _version = "Jun 20, 2008 1:46:45 PM"
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
