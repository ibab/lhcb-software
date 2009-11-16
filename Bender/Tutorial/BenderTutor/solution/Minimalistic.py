#!/usr/bin/env python
# =============================================================================
# $Id: Minimalistic.py,v 1.13 2009-11-16 16:38:26 ibelyaev Exp $
# =============================================================================
"""
This is the simplest  Bender module to run the analysis job
using '*'.opts configuration
"""
# =============================================================================
## @file
#  Simple script to run 'DaVinci' job in Bender environment 
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-11-16
# =============================================================================
__author__  = ' Vanya BELYAEV  Ivan.Belyaev@nikhef.nl '
__version__ = ' CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.13 $  '  
# =============================================================================
## get EVERYTHING from Bender
from Bender.Main import *
# =============================================================================
## Job configuration 
def configure() :
    """
    The  basic configuration method
    """
    
    from Configurables import DaVinci
    
    DaVinci (
        DataType   = 'DC06' , # default  
        Simulation = True   ) 
    
    ## get/create application manager
    gaudi = appMgr() 
    
    ## redefine input files 
    evtSel = gaudi.evtSel()
    import LoKiExample.Bs2Jpsiphi_mm_data as data 
    evtSel.open( data.Files ) 
        
    return SUCCESS 
# =============================================================================


# =============================================================================
## Job steering :
if __name__ == '__main__' :

    ## job configuration
    configure()

    ## event loop 
    run(100)

# =============================================================================
# The END
# =============================================================================
