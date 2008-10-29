#!/usr/bin/env python
# $Id: showCMTProjects.py,v 1.15 2008-10-29 15:40:41 hmdegaud Exp $

from LbUtils.CMT import showCMTProjScript
from LbUtils.CVS import CVS2Version

__version__ = CVS2Version("$Name: not supported by cvs2svn $", "$Revision: 1.15 $")

if __name__ == '__main__':

    s = showCMTProjScript(usage="%prog [options] projectname version", version=__version__)
    s.run()
    

