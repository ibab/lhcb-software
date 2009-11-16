#!/usr/bin/env python
# =============================================================================
# $Id: MCTrees.py,v 1.15 2009-11-16 16:38:26 ibelyaev Exp $ 
# =============================================================================
"""
'Solution'-file for 'MCTrees.py' example (Bender Tutorial)
"""
# =============================================================================
# @file
#
# "Solution"-file for 'MCTrees.py' example (Bender Tutorial)
#
# @author Vanya BELYAEV ibelyaev@physics.syr.edu
# @date   2004-10-12
# =============================================================================
__author__  = ' Vanya BELYAEV  Ivan.Belyaev@nikhef.nl '
__version__ = ' CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.15 $  '  
# =============================================================================
## import everything from BENDER
from Bender.MainMC import *
# =============================================================================
## @class MCTrees
#  The algorthmm itself 
class MCTrees( AlgoMC ) :
    """
    The algorthmm itself
    """
  
    ## the main analysis method 
    def analyse( self ) :
        """
        The main analysis method
        """ 
        ## get the MCDecayFinder wrapper
        finder = self.mcFinder()
        
        ## find all MC trees of interest 
        trees  = finder.find(
            ' [B_s0 -> ( J/psi(1S) ->  mu+  mu- ) phi(1020) ]cc' )
        
        ## get all kaons from the tree :
        phis   = finder.find(
            ' [B_s0 -> ( J/psi(1S) ->  mu+  mu- ) ^phi(1020) ]cc' )
        
        ## get marked particles from the tree:
        mus    = finder.find(
            ' [B_s0 -> ( J/psi(1S) -> ^mu+ ^mu- ) phi(1020) ]cc' )
        
        print ' found MCtrees/Phis/Mus: %s/%s/%s' % ( trees.size () ,
                                                      phis.size  () ,
                                                      mus.size   () )
        
        ## fill the histogram 
        for mu in mus :
            self.plot ( MCPT( mu ) / 1000 ,
                        ' PT of Muons from J/psi ' ,
                        0 , 10 )

        ## retrieve (bon-on-demand) N-Tuple 
        tup = self.nTuple( 'My N-Tuple' )
        zOrig = MCVXFUN( MCVZ )
        
        for mu in mus :
            tup.column ( 'P'   , MCP   ( mu ) / 1000 )
            tup.column ( 'PT'  , MCPT  ( mu ) / 1000 )
            tup.column ( 'ID'  , MCID  ( mu ) )
            tup.column ( 'Q3'  , MC3Q  ( mu ) )
            tup.column ( 'ZOR' , zOrig ( mu ) ) 
            tup.write() 
            
        return SUCCESS
# =============================================================================
    
# =============================================================================
## configure the job
def configure() :
    """
    Configure the job
    """
    
    from Configurables import DaVinci
    
    DaVinci (
        DataType   = 'DC06' , # default  
        Simulation = True   )
    
    ## configure histograms & n-tuples 
    from Gaudi.Configuration import NTupleSvc,HistogramPersistencySvc 
    NTupleSvc ( Output = [ "MC DATAFILE='MCTrees.root' OPT='NEW' TYP='ROOT'" ] )
    HistogramPersistencySvc ( OutputFile = 'MCTrees_histos.root' )

    ## get/create application manager
    gaudi = appMgr() 

    # 1) create the algorithm
    alg = MCTrees(
        'MCTree'         ,
        ## LUN for N-tuples 
        NTupleLUN = 'MC' ,
        ## MC-links 
        PP2MCs    = []
        )
    
    # 2) replace the list of top level algorithm by only *THIS* algorithm
    gaudi.setAlgorithms ( [ alg ] ) 

    ## redefine input files 
    evtSel = gaudi.evtSel()
    import LoKiExample.Bs2Jpsiphi_mm_data as data 
    evtSel.open( data.Files ) 
    
    return SUCCESS
# =============================================================================

# =============================================================================
## Job steering 
if __name__ == '__main__' :

    ## job configuration
    configure()

    ## event loop 
    run(100)

# =============================================================================
# The END 
# =============================================================================
