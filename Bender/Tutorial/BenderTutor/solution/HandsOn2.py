#!/usr/bin/env python
# =============================================================================
# $Id: HandsOn2.py,v 1.3 2004-11-08 17:02:45 ibelyaev Exp $
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
# define the primitive algorithm 
# =============================================================================
class HandsOn2(Algo):
    def analyse( self ) :
        
        # get *ALL* Monte Carlo B_s0 and B_s~0
        Bs   = self.mcselect ( tag = 'Bs'   , cuts = 'B_s0' == MCABSID )
        if not Bs.empty()   : self.Warning( message = 'Bs' , prints = 0 ) 
        
        # get all beauty hadrons 
        lamb  = self.mcselect( tag = 'beauty' , cuts = BARYON & BEAUTY )
        if not lamb.empty() : self.Warning( message = 'LambdaB' , prints = 0 ) 
        
        return SUCCESS
# =============================================================================

# =============================================================================
# configure the job
# =============================================================================
def configure() :
    
    gaudi.config ( files = ['$BENDERTUTOROPTS/BenderTutor.opts' ] )
    
    # modify/update the configuration:
    
    # 1) create the algorithm
    alg = HandsOn2( 'HandsOn2' )
    
    # 2) replace the list of top level algorithm by only *THIS* algorithm
    gaudi.setAlgorithms( [alg] )
    
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
