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
__author__  = ' Vanya BELYAEV  Ivan.Belyaev@nikhef.nl '
__version__ = ' CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.9 $  '  
# =============================================================================
## import everything from Bender 
from Bender.Main import *
# =============================================================================
## configure the application 
def configure() :
    """
    This is the configuration method for module Minimalistic_1.py
    """
    
    ## get the confgigurtaion for *.opts file
    importOptions( '$DAVINCIROOT/options/DaVinci.opts') 

    ## get/create application manager
    gaudi = appMgr() 
    
    ## redefine input files 
    evtSel = gaudi.evtSel()
    import LoKiExample.Bs2Jpsiphi_mm_data as data 
    evtSel.open( data.Files ) 
    
    return SUCCESS

# =============================================================================
## Job steeting  
if __name__ == '__main__' :
    
    # job configuration
    configure()
    
    # event loop 
    run(50)
    
# ======================================================-======================
# The END 
# =============================================================================


