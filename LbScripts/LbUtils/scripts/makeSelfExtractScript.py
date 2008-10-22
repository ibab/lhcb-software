#!/usr/bin/env python
# $Id: makeSelfExtractScript.py,v 1.1 2008-10-22 15:58:35 hmdegaud Exp $

from LbUtils.Script import Script
from LbUtils.SelfExtract import createSelfExtractFile

class makeSelfExtractScript(Script):
    _version = "Oct 6, 2008 5:12:36 PM"
    def defineOpts(self):
        parser = self.parser 
    def main(self):
        return 0


if __name__ == '__main__':
    s = makeSelfExtractScript(usage="%prog [options]")
    s.run()


