#!/usr/bin/env python
# $Id: showCMTProjects.py,v 1.16 2008-10-30 09:11:10 hmdegaud Exp $
""" this is a fake documentation """


from LbUtils.CMT import showCMTProjScript
from LbUtils.CVS import CVS2Version

__version__ = CVS2Version("$Name: not supported by cvs2svn $", "$Revision: 1.16 $")

if __name__ == '__main__':

    s = showCMTProjScript(usage="%prog [options] projectname version", version=__version__, description=__doc__)
    s.run()
    
