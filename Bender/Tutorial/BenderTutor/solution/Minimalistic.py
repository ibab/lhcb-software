#!/usr/bin/env python
# =============================================================================
# $Id: Minimalistic.py,v 1.5 2005-08-01 16:04:24 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.5 $
# =============================================================================
"""
This is the simplest  Bender module to run the analysis job
using '*'.opts configuration
"""
# =============================================================================
# @file
#

# @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
# @date   2004-10-12
# =============================================================================
__author__ = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'

# get EVERYTHING from Bender
from bendermodule import *

# =============================================================================
# The configuration of the job 
# =============================================================================
def configure() :
    """
    The  basic configuration method
    """
    # get the confgigurtaion for *.opts file
    gaudi.config( files = ['$DAVINCIROOT/options/DaVinci.opts'] )

        
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


# =============================================================================
# The control flow 
# =============================================================================
if __name__ == '__main__' :

    # job configuration
    configure()

    # event loop 
    g.run(100)


# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END
# =============================================================================
