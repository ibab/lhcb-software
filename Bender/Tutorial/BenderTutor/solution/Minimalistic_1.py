#!/usr/bin/env python
# =============================================================================
# $Id: Minimalistic_1.py,v 1.2 2005-01-24 17:29:40 ibelyaev Exp $ 
# =============================================================================
# CVS version $Revison: $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================
"""
Simple script to run 'DaVinci' job in Bender environment
"""
__author__ = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'

from bendermodule import *


def configure() :
    """
    This is the configuration method for module Minimalistic_1.py
    """
    # get the confgigurtaion for *.opts file
    gaudi.config( files = ['$BENDERTUTOROPTS/BenderTutor.opts'] )
    return SUCCESS

if __name__ == '__main__' :
    
    # job configuration
    configure()
    
    # event loop 
    gaudi.run(50)
    
    # for the interactive mode it is better to comment the last line
    gaudi.exit()


# =============================================================================
# $Log: not supported by cvs2svn $ 
# =============================================================================
# The END 
# =============================================================================
