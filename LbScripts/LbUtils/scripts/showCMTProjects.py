#!/usr/bin/env python
# $Id: showCMTProjects.py,v 1.14 2008-08-27 09:18:39 hmdegaud Exp $

from LbUtils.CMT import showCMTProjScript

if __name__ == '__main__':

    s = showCMTProjScript(usage="%prog [options] projectname version")
    s.run()
    

