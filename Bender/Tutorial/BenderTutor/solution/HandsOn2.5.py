#!/usr/bin/env python
# =============================================================================
# $Id: HandsOn2.5.py,v 1.3 2004-11-08 17:02:45 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================

# =============================================================================
# @file
#
# "Solution"-file for 'Hands-On 2' example (Bender Tutorial)
#
# @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
# @date   2004-10-12
# =============================================================================

# import everything from BENDER
from bendermodule import *

# =============================================================================
# configure the job
# =============================================================================
def configure() :
    
    # reuse the configuration from HandsOn2.py example
    import HandsOn2
    HandsOn2.configure()
    
    # redefine input files 
    evtsel = gaudi.evtSel()
    evtsel.open( stream = [ 'LFN:/lhcb/production/DC04/v1/DST/00000543_00000017_5.dst', 
                            'LFN:/lhcb/production/DC04/v1/DST/00000543_00000018_5.dst',
                            'LFN:/lhcb/production/DC04/v1/DST/00000543_00000016_5.dst',
                            'LFN:/lhcb/production/DC04/v1/DST/00000543_00000020_5.dst',
                            'LFN:/lhcb/production/DC04/v1/DST/00000543_00000024_5.dst',
                            'LFN:/lhcb/production/DC04/v1/DST/00000543_00000019_5.dst',
                            'LFN:/lhcb/production/DC04/v1/DST/00000543_00000021_5.dst',
                            'LFN:/lhcb/production/DC04/v1/DST/00000543_00000022_5.dst',
                            'LFN:/lhcb/production/DC04/v1/DST/00000543_00000001_5.dst',
                            'LFN:/lhcb/production/DC04/v1/DST/00000543_00000002_5.dst' ] )
    
    return SUCCESS
# =============================================================================

# =============================================================================
# The control flow 
# =============================================================================
if __name__ == '__main__' :

    # job configuration
    configure()

    # event loop 
    gaudi.run(100)

    # for the interactive mode it is better to comment the last line
    gaudi.exit()
# =============================================================================

# =============================================================================
# $Log: not supported by cvs2svn $ 
# =============================================================================
# The END 
# =============================================================================
