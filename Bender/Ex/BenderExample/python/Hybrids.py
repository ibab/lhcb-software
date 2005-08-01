#!/usr/bin/env python
# =============================================================================
# $Id: Hybrids.py,v 1.5 2005-08-01 09:50:19 ibelyaev Exp $
# =============================================================================
# CVS version $Revision: 1.5 $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================
# @file
# Simple script for LoKiHybrid demo  
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2003-12-04 
# =============================================================================

# import the Bender itself  
from   bendermodule import *
import benderPreLoad as preload

# =============================================================================
# Specific physics analysis algorithm 
# =============================================================================

# create my own algorithm  
class Hybrids(Algo):
    " Trivial algorithm to text LoKiHybrid approach "
    def analyse ( self ) :
        
        self.setFilterPassed ( TRUE )
        
        pions = self.select( tag = 'pions' , cuts = ABSID == 'pi+' )
        
        cut   = FILTER ( self.filterCriterion(0) )
        sel   = self.select( tag    = 'sel'   ,
                             source = pions   ,
                             cuts   = cut     )
        
        self.Print( message = "Selected:\t#%d\tparticles from \t#%d pions" % ( sel.size() , pions.size() ) )
        
        return SUCCESS 

def configure() :
    
    # Generic job configuration     
    gaudi.config( files   =
                  [ '$DAVINCIROOT/options/DaVinciCommon.opts'   ,   # common options 
                    '$DAVINCIROOT/options/DaVinciReco.opts'     ,   # general 'Reco' options 
                    '$DAVINCIROOT/options/DaVinciTestData.opts' ] ) 
    
    # specific job configuration 
    gaudi.addAlgorithm( 'LoKiPreLoad/Hadrons' ) 
    preload.Hadrons( Particles = [ 'kaon' , 'pion'] )
    
    # create analysis algorithm and add it to the list of
    alg  = Hybrids('Hybrids')
    gaudi.addAlgorithm( alg )    
    alg  = gaudi.algorithm('Hybrids')
    alg.FilterCriteria = [ 'HybridFilterCriterion/Hybrid']
    fltr = gaudi.tool('Hybrids.Hybrid')
    fltr.Code = " PT > ( 1 * GeV )   "                  # NB!!!
    desktop = gaudi.tool('Hybrids.PhysDesktop')
    desktop.InputLocations  = [ "/Event/Phys/Hadrons"]

    alg  = Hybrids('Hybrids1')
    gaudi.addAlgorithm( alg )    
    alg  = gaudi.algorithm('Hybrids1')
    alg.FilterCriteria = [ 'HybridFilterCriterion/Hybrid']    
    fltr = gaudi.tool('Hybrids1.Hybrid')
    fltr.Code = " PT < ( 1 * GeV )   "                  # NB !
    desktop = gaudi.tool('Hybrids1.PhysDesktop')
    desktop.InputLocations  = [ "/Event/Phys/Hadrons"]
    
    return SUCCESS

# =============================================================================
# job execution 
# =============================================================================

if __name__ == '__main__' :
    # configure the job
    configure() 
    # run job 
    g.run  ( 100  )
 
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
