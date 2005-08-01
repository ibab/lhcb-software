#!/usr/bin/env python
# =============================================================================
# $Id: Minimalistic_1.py,v 1.3 2005-08-01 16:04:24 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.3 $
# =============================================================================
"""
Simple script to run 'DaVinci' job in Bender environment
"""
# =============================================================================
__author__ = 'Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr'
# =============================================================================

from bendermodule import *


def configure() :
    """
    This is the configuration method for module Minimalistic_1.py
    """
    # get the confgigurtaion for *.opts file
    gaudi.config( files = ['$DAVINCIROOT/options/DaVinci.opts'] )
    return SUCCESS

if __name__ == '__main__' :
    
    # job configuration
    configure()
    
    # event loop 
    gaudi.run(50)
    
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
