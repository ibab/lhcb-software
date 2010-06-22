#!/usr/bin/env python
# $Id: ScriptTest.py,v 1.2 2009-11-09 10:37:32 hmdegaud Exp $
""" Don't forget about the description """

from LbUtils.Script import ConfigScript as Script 
from LbUtils.CVS import CVS2Version

import logging
import os

__version__ = CVS2Version("$Name: not supported by cvs2svn $", "$Revision: 1.2 $")

class ScriptTest(Script):
    _version = __version__    
    _description = __doc__
    def defineOpts(self):
        parser = self.parser
        parser.set_defaults(toto_val="bla")
        parser.add_option("-t", "--toto-val", help="set toto value [default %default]")
        
    def main(self):
        log = logging.getLogger()
        log.debug("This is a debug message")
        log.info("This is an info message")
        log.warning("This is a warning message")
        print self.options.toto_val
        return 0

if __name__ == '__main__':
    s = ScriptTest(usage="ScriptTest [options] path", use_config_file=True)
    s.run()
