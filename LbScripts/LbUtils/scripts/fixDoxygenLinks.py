#!/usr/bin/env python
# $Id: fixDoxygenLinks.py,v 1.2 2008-10-31 12:31:05 hmdegaud Exp $
""" 
This tool intends to fix the wrong links to the external software created 
by Doxygen. Usually this appears with strings like "../../http://" in the href
of the links in the HTML files.
"""

from LbUtils.Script import Script
from LbUtils.CVS import CVS2Version

import os
import logging

__version__ = CVS2Version("$Name: not supported by cvs2svn $", "$Revision: 1.2 $")
__author__ = "hmd"

_doxfilesrc = ["html", "htm"]

def fixDoxygenLinksInFile(filepath, dryrun=False):
    nbreplace = 0
    log = logging.getLogger()
    linenb = 0
    if not dryrun :
        destpath = filepath+".tmp"
        fdest = open(destpath, "w")
    for line in open(filepath, "r") :
        linenb += 1
        if line.find('href="../../http:') != -1 :
            log.info("Found problem in file %s at line %s" % (filepath, linenb))
            log.info(line[:-1])
            nbreplace += 1
            if not dryrun :
                line = line.replace('href="../../http:', 'href="http:')
        if not dryrun :
            fdest.write(line)
    if not dryrun:
        fdest.close()
        os.remove(filepath)
        os.rename(destpath, filepath)
    return nbreplace

def fixDoxygenLinksInDir(dirpath, filext=None, dryrun=False):
    nbreplace = 0
    log = logging.getLogger()
    if not filext :
        filext = _doxfilesrc
    for root, dirs, files in os.walk(dirpath) :
        for f in files :
            fext = os.path.splitext(f)[1]
            if fext :
                fext = fext[1:]
            if fext in filext or "*" in filext:
                fpath = os.path.join(root, f)
                log.debug("Found file %s" % fpath )
                nbreplace += fixDoxygenLinksInFile(fpath, dryrun)
    return nbreplace

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
        parser.add_option("-a", "--add-ext",
                          action="append", 
                          dest="filext",
                          help="add extension for files to be processed "
                          "[default: %default]")

    def main(self):
        nbreplacetot = 0
        args = self.args
        opts = self.options
        for a in args :
            nbreplace = fixDoxygenLinksInDir(a, opts.filext, opts.dryrun)
            print "Number of replacements in %s: %s" % (a, nbreplace)
            nbreplacetot += nbreplace
        if len(args) > 1 :
            print "Total number of replacements: %s" % nbreplacetot            
        return 0


if __name__ == '__main__':
    s = FixDoxygenLinksScript(usage="%prog [options] [dir1 [dir2]]")
    s.run()
