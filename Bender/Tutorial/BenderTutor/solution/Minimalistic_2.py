#!/usr/bin/env python
# =============================================================================
# $Id: Minimalistic_2.py,v 1.2 2005-01-24 17:29:40 ibelyaev Exp $ 
# =============================================================================
# CVS version $Revision: 1.2 $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $
# =============================================================================
"""
Simple script to run 'DaVinci' job in Bender environment
"""
__author__ = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'


from bendermodule import *

# =============================================================================
# configure the application 
# =============================================================================
def configure() :
    """
    This is the configuration method for module Minimalistic_1.py
    """
    # get the confgigurtaion for *.opts file
    gaudi.config( files = ['$BENDERTUTOROPTS/BenderTutor.opts'] )
    
    # define input data file:
    evtsel = gaudi.evtSel()
    evtsel.open( [ 'LFN:/lhcb/production/DC04/v1/DST/00000543_00000017_5.dst' , 
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000018_5.dst' ,
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000016_5.dst' ,
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000020_5.dst' ] )
    
    return SUCCESS

# =============================================================================
# Job steeting  
# =============================================================================
if __name__ == '__main__' :
    
    # job configuration
    configure()
    
    # event loop 
    gaudi.run(50)
    
    # for the interactive mode it is better to comment the last line
    gaudi.exit()

# =============================================================================
# $Log: not supported by cvs2svn $ 
# =============================================================================
# The END 
# =============================================================================


