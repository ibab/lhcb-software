#!/usr/bin/env python2.4
# =============================================================================
# $Id: HandsOn2.py,v 1.7 2006-11-17 11:59:47 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $ 
# =============================================================================
""" 'Solution'-file for 'Hands-On 2' example (Bender Tutorial) """
# =============================================================================
# @file
#
# "Solution"-file for 'Hands-On 2' example (Bender Tutorial)
#
# @author Vanya BELYAEV  ibelyaev@physics.syr.edu
# @date   2004-10-12
# =============================================================================
__author__ = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
# =============================================================================

# import everything from BENDER
from bendermodule import *

# =============================================================================
## @class HandOn2
#   solution for the second excersize 
class HandsOn2(AlgoMC):
    """ solution for the second excersize """

    ## the main analysis method 
    def analyse( self ) :
        """ the main analysis method """
        
        ## get *ALL* Monte Carlo B_s0 and B_s~0
        Bs   = self.mcselect ( 'Bs'  ,'B_s0' == MCABSID )
        if Bs.empty()   :
            self.Warning ( 'No Bs is found!') 
        
        ## get all beauty baryions 
        lamb  = self.mcselect( 'beauty' , BARYON & BEAUTY )
        if lamb.empty() :
            self.Warning ( 'No beutry baryon are found!')
        
        nBs = self.counter("#Bs")
        nBs += Bs.size()

        nLb = self.counter("#LambdaB")
        nLb += lamb.size() 
        
        return SUCCESS
# =============================================================================

# =============================================================================
## configure the job
def configure() :
    """ configure the job """
    
    ## general configuration :
    gaudi.config ( files = [ '$DAVINCIROOT/options/DaVinciCommon.opts' ] )
    
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
    import data_tutorial as data 
    evtSel.open( data.FILES ) 

        
    return SUCCESS
# =============================================================================

# =============================================================================
## Job steering 
if __name__ == '__main__' :
    
    ## job configuration
    configure()

    ## event loop 
    gaudi.run(500)

# =============================================================================
# $Log: not supported by cvs2svn $
#
# =============================================================================
# The END 
# =============================================================================
