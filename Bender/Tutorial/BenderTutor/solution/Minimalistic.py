#!/usr/bin/env python
# =============================================================================
# $Id: Minimalistic.py,v 1.3 2004-11-08 17:02:46 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
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

# get EVERYTHING from Bender
from bendermodule import *

# =============================================================================
# The configuration of the job 
# =============================================================================
def configure() :
    """
    The  basic configuration method
    """
    # get the major configuration from '*'.opts file 
    gaudi.config( files = ['$BENDERTUTOROPTS/BenderTutor.opts' ] )
    
    # define input data files :
    #    1) get the Event Selector from Gaudi
    #    2) configure Event Selector 
    evtSel = gaudi.evtSel()
    evtSel.open( [ 'LFN:/lhcb/production/DC04/v1/DST/00000543_00000017_5.dst' , 
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000018_5.dst' ,
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000016_5.dst' ,
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000020_5.dst' ] )

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

    # terminate  Gaudi application  
    g.exit()
# =============================================================================

# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END
# =============================================================================
