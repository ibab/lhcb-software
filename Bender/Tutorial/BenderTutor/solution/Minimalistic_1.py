#!/usr/bin/env python
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
## import everything from bender 
from Bender.Main import *
# =============================================================================
## Job configuration:
def configure() :
    """
    This is the configuration method for module Minimalistic_1.py
    """
    
    ## get the confgigurtaion for *.opts file
    importOptions('$DAVINCIROOT/options/DaVinci.opts')

    ## get/create application manager
    gaudi = appMgr() 
   
    ## define input files 
    import BenderTutor.data_tutorial as data 
    evtSel = gaudi.evtSel()    
    evtSel.open( data.FILEs ) 

    return SUCCESS

# =============================================================================
## Job steering 
if __name__ == '__main__' :
    
    ## job configuration
    configure()
    
    ## event loop 
    run(50)
    
# =============================================================================
# The END 
# =============================================================================
