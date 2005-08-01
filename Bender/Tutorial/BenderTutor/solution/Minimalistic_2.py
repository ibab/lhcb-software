#!/usr/bin/env python
# =============================================================================
# $Id: Minimalistic_2.py,v 1.3 2005-08-01 16:04:24 ibelyaev Exp $ 
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

# =============================================================================
# configure the application 
# =============================================================================
def configure() :
    """
    This is the configuration method for module Minimalistic_1.py
    """
    # get the confgigurtaion for *.opts file
    gaudi.config( files = ['$DAVINCIROOT/options/DaVinci.opts'] )
    
    # define input data files :
    #    1) get the Event Selector from Gaudi
    evtSel = gaudi.evtSel()
    #    2) configure Event Selector 
    #       files from $DAVINCIROOT/options/DaVinciTestData.opts 
    evtSel.open( [
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000665_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000645_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000648_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000652_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000656_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000658_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000659_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000667_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000670_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000672_9.dst' ] ) 
    

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
# =============================================================================
# The END 
# =============================================================================


