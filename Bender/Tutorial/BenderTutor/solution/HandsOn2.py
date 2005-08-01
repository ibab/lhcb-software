#!/usr/bin/env python
# =============================================================================
# $Id: HandsOn2.py,v 1.5 2005-08-01 16:04:24 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $ 
# =============================================================================
""" 'Solution'-file for 'Hands-On 2' example (Bender Tutorial) """
# =============================================================================
# @file
#
# "Solution"-file for 'Hands-On 2' example (Bender Tutorial)
#
# @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
# @date   2004-10-12
# =============================================================================
__author__ = 'Vanya BELYAEV  belyaev@lapp.in2p3.fr'
# =============================================================================

# import everything from BENDER
from bendermodule import *

# =============================================================================
# define the primitive algorithm 
# =============================================================================
class HandsOn2(Algo):
    def analyse( self ) :
        
        # get *ALL* Monte Carlo B_s0 and B_s~0
        Bs   = self.mcselect ( tag = 'Bs'   , cuts = 'B_s0' == MCABSID )
        if not Bs.empty()   : self.Warning( message = 'Bs' , prints = 0 ) 
        
        # get all beauty baryions 
        lamb  = self.mcselect( tag = 'beauty' , cuts = BARYON & BEAUTY )
        if not lamb.empty() : self.Warning( message = 'LambdaB' , prints = 0 ) 
        
        return SUCCESS
# =============================================================================

# =============================================================================
# configure the job
# =============================================================================
def configure() :
    
    # general configuration :
    gaudi.config ( files = [ '$DAVINCIROOT/options/DaVinciCommon.opts' ] )
    
    # modify/update the configuration:
    
    # 1) create the algorithm
    alg = HandsOn2( 'HandsOn2' )
    
    # 2) replace the list of top level algorithm by only *THIS* algorithm
    gaudi.setAlgorithms( [alg] )
    
    # define input data files :
    #    1) get the Event Selector from Gaudi
    evtSel = gaudi.evtSel()
    evtSel.PrintFreq = 50 
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
    gaudi.run(500)

# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
