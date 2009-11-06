#!/usr/bin/env python
# $Id: ScriptTest.py,v 1.1 2009-11-06 14:30:55 hmdegaud Exp $
""" Don't forget about the description """

from LbUtils.Script import ConfigScript as Script 
from LbUtils.CVS import CVS2Version

import os

__version__ = CVS2Version("$Name: not supported by cvs2svn $", "$Revision: 1.1 $")

class ScriptTest(Script):
    _version = __version__    
    _description = __doc__
    def defineOpts(self):
#        self.setConfigFile(config_file=os.path.join(os.environ["HOME"], ".totorc"))
        parser = self.parser
        parser.set_defaults(toto_val="bla")
        parser.add_option("-t", "--toto-val", help="set toto value [default %default]")
        
        pass
    def main(self):
        return 0

if __name__ == '__main__':
    s = ScriptTest(usage="ScriptTest [options] path", use_config_file=True)
    s.run()
