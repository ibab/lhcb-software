#!/usr/bin/env python
# =============================================================================
"""
'Solution'-file for 'Hands-On 2' example (Bender Tutorial)
"""
# =============================================================================
# @file
#
# "Solution"-file for 'Hands-On 2' example (Bender Tutorial)
#
# @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
# @date   2004-10-12
# =============================================================================
__author__ = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
# =============================================================================
## import everything from BENDER
from Bender.MainMC import *
# =============================================================================
## @class HandOn2
#   solution for the second excersize 
class HandsOn2(AlgoMC):
    """
    Solution for the second excersize
    """
    ## the main analysis method 
    def analyse( self ) :
        """
        The main analysis method
        """ 
        ## get *ALL* Monte Carlo B_s0 and B_s~0
        Bs   = self.mcselect ( 'Bs'  ,'B_s0' == MCABSID )
        if Bs.empty()   : self.Warning ( 'No B_s0 are found!') 
        
        ## get all beauty baryions 
        lamb  = self.mcselect( 'beauty' , BARYON & BEAUTY )
        if lamb.empty() : self.Warning ( 'No beauty baryons are found!')
        
        nBs = self.counter("#Bs")
        nBs += Bs.size()

        nLb = self.counter("#LambdaB")
        nLb += lamb.size() 
        
        return SUCCESS
# =============================================================================

# =============================================================================
## configure the job
def configure() :
    """
    Configure the job
    """
    import BenderTutor.data_tutorial as data 
    
    ## general configuration :
    importOptions('$DAVINCIROOT/options/DaVinciCommon.opts')

    ## get/create the application manager
    gaudi = appMgr() 
    
    ## modify/update the configuration:
    
    ## 1) create the algorithm
    alg = HandsOn2( 'HandsOn2' )
    
    ## 2) replace the list of top level algorithm by only *THIS* algorithm
    gaudi.setAlgorithms( [alg] )
    
    ## define input data files :
    
    ##    1) get the Event Selector from Gaudi
    evtSel = gaudi.evtSel()
    ##    2) configure Event Selector
    evtSel.PrintFreq = 50 
    evtSel.open( data.FILEs ) 

        
    return SUCCESS
# =============================================================================

# =============================================================================
## Job steering 
if __name__ == '__main__' :
    
    ## job configuration
    configure()

    ## event loop 
    run(500)

# =============================================================================
# The END 
# =============================================================================
