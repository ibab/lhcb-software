#!/usr/bin/env python2.4
# =============================================================================
# $Id: Minimalistic.py,v 1.7 2006-11-17 11:59:47 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.7 $
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

## get EVERYTHING from Bender
from bendermodule import *

# =============================================================================
## Job configuration 
def configure() :
    """ The  basic configuration method """
    
    ## get the confgigurtaion for *.opts file
    gaudi.config( files = ['$DAVINCIROOT/options/DaVinci.opts'] )

    ## define input files 
    evtSel = gaudi.evtSel()    
    import data_tutorial as data 
    evtSel.open( data.FILES ) 
    
    
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
# $Log: not supported by cvs2svn $
#
# =============================================================================
# The END
# =============================================================================
