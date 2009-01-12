#!/usr/bin/env python
# $Id: makeSelfExtractScript.py,v 1.3 2009-01-12 10:40:02 hmdegaud Exp $
""" 
Script to create self-extracting scripts 
"""

from LbUtils.Script import Script
from LbUtils.SelfExtract import createSelfExtractFile
from LbUtils.CVS import CVS2Version

__version__ = CVS2Version("$Name: not supported by cvs2svn $", "$Revision: 1.3 $")

class makeSelfExtractScript(Script):
    _version = __version__
    _description = __doc__
    def defineOpts(self):
        parser = self.parser 
    def main(self):
        return 0


if __name__ == '__main__':
    s = makeSelfExtractScript(usage="%prog [options]")
    s.run()


