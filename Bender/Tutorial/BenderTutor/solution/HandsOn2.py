#!/usr/bin/env python
# =============================================================================
# $Id: HandsOn2.py,v 1.4 2005-01-24 17:29:40 ibelyaev Exp $
# =============================================================================
# CVS version $Revision: 1.4 $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
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
    gaudi.run(500)

    # for the interactive mode it is better to comment the last line
    gaudi.exit()
# =============================================================================

# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
