#!/usr/bin/env python2.4
# =============================================================================
"""
Simple script to run 'DaVinci' job in Bender environment
"""
# =============================================================================
## @file
#  Simple script to run 'DaVinci' job in Bender environment 
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-11-16
# =============================================================================
__author__ = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
# =============================================================================

# =============================================================================
## import everything from bender 
from Bender.Main import *

# =============================================================================
## Job configuration:
def configure() :
    """
    This is the configuration method for module Minimalistic_1.py
    """
    import data_tutorial as data 
    
    ## get the confgigurtaion for *.opts file
    gaudi.config( files = ['$DAVINCIROOT/options/DaVinci.opts'] )

    ## define input files 
    evtSel = gaudi.evtSel()
    
    evtSel.open( data.FILEs ) 

    return SUCCESS

# =============================================================================
## Job steering 
if __name__ == '__main__' :
    
    ## job configuration
    configure()
    
    ## event loop 
    gaudi.run(50)
    
# =============================================================================
# The END 
# =============================================================================
