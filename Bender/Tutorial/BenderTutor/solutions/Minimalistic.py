#!/usr/bin/env python
# =============================================================================
# $Id: Minimalistic.py,v 1.3 2004-10-27 14:08:19 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================

# =============================================================================
# @file
#
# "Solution"-file for 'Minimalistic' example (Bender Tutorial)
#
# @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
# @date   2004-10-12
# =============================================================================

# get EVERYTHING from Bender
from bendermodule import *

# get the CONFIGURATION utilities
import benderconfig as bender


# =============================================================================
# The configuration of the job 
# =============================================================================
def configure() :
    """
    The  basic configruation method
    """
    bender.config( files = ['$BENDERTUTOROPTS/BenderTutor.opts' ] )
    return SUCCESS 
# =============================================================================



# =============================================================================
# The control flow 
# =============================================================================
if __name__ == '__main__' :

    # job configuration
    configure()

    # event loop 
    g.run(10)

    # for the interactive mode it is better to comment the last line
    g.exit()
# =============================================================================


# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.1.1.1  2004/10/13 17:45:21  ibelyaev
# New package: Star-Up Tutorial for Bender
# 
# =============================================================================
# The END
# =============================================================================
