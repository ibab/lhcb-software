#!/usr/bin/env python
# $Id: makeSelfExtractScript.py,v 1.2 2008-10-29 17:43:40 hmdegaud Exp $

from LbUtils.Script import Script
from LbUtils.SelfExtract import createSelfExtractFile
from LbUtils.CVS import CVS2Version

__version__ = CVS2Version("$Name: not supported by cvs2svn $", "$Revision: 1.2 $")

class makeSelfExtractScript(Script):
    _version = __version__
    def defineOpts(self):
        parser = self.parser 
    def main(self):
        return 0


if __name__ == '__main__':
    s = makeSelfExtractScript(usage="%prog [options]")
    s.run()


