#!/usr/bin/env python2.4
# =============================================================================
# $Id: Minimalistic_2.py,v 1.5 2006-11-17 11:59:47 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.5 $
# =============================================================================
""" Simple script to run 'DaVinci' job in Bender environment """
# =============================================================================
## @file
#  Simple script to run 'DaVinci' job in Bender environment 
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-11-16
# =============================================================================
__author__ = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
# =============================================================================

## import everything from Bender 
from bendermodule import *

# =============================================================================
# configure the application 
# =============================================================================
def configure() :
    """
    This is the configuration method for module Minimalistic_1.py
    """
    ## get the confgigurtaion for *.opts file
    gaudi.config( files = ['$DAVINCIROOT/options/DaVinci.opts'] )
    
    ## define input files 
    evtSel = gaudi.evtSel()
    
    import data_tutorial as data 
    evtSel.open( data.FILES ) 
  
    return SUCCESS

# =============================================================================
# Job steeting  
# =============================================================================
if __name__ == '__main__' :
    
    # job configuration
    configure()
    
    # event loop 
    gaudi.run(50)
    

# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.4  2006/06/06 20:03:27  ibelyaev
#  update for Bender v4r8p5
#
# =============================================================================
# The END 
# =============================================================================


