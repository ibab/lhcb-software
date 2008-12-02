#!/usr/bin/env
# =============================================================================
## @file PartProp/PartPropSvc.py
#  Demo-file for interactive work with new Particle Property Service
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-12-02
# =============================================================================
"""
Demo-file for interactive work with new Particle Property Service
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $" 
# =============================================================================
import PartProp.PartPropAlg
import PartProp.decorators 
import PartProp.Service
from GaudiPython.Bindings import AppMgr
# =============================================================================

gaudi = AppMgr()

gaudi.initialize()


## simple test-function 
def test () :
    """
    
    simple function for the test
    
    """
    pps = gaudi.ppSvc()

    
    for l in range(1,6) :
        
        # get 
        lst1 = pps.get ( lambda x : l == x.pid().lSpin() )
        print ' %s == 2*lSpin+1 '%l
        print lst1 

    for s in range(0,2) :
        
        # get 
        lst1 = pps.get ( lambda x : s == x.pid().sSpin() )
        print ' %s == 2*sSpin+1 '%s
        print lst1 

    for j in range(0,7) :
        
        # get 
        lst1 = pps.get ( lambda x : j == x.pid().jSpin() )
        print ' %s == 2*jSpi+1 '%j
        print lst1 


    
if '__main__' == __name__ :

    test() 

# =============================================================================
# The END 
# =============================================================================
