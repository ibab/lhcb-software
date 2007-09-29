#!/usr/bin/env python2.4
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
__author__ = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
# =============================================================================

# =============================================================================
## get EVERYTHING from Bender
from Bender.Main import *

# =============================================================================
## Job configuration 
def configure() :
    """
    The  basic configuration method
    """
    
    import data_tutorial as data 

    ## get the confgigurtaion for *.opts file
    gaudi.config( files = ['$DAVINCIROOT/options/DaVinci.opts'] )

    ## define input files 
    evtSel = gaudi.evtSel()    
    evtSel.open( data.FILEs ) 
    
    
    return SUCCESS 
# =============================================================================


# =============================================================================
## Job steering :
if __name__ == '__main__' :

    ## job configuration
    configure()

    ## event loop 
    g.run(100)

# =============================================================================
# The END
# =============================================================================
