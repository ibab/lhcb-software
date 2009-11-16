#!/usr/bin/env python
# =============================================================================
# $Id: HandsOn3.py,v 1.15 2009-11-16 16:38:26 ibelyaev Exp $ 
# =============================================================================
"""
'Solution'-file for 'Hands-On-3' example (Bender Tutorial)
"""
# =============================================================================
## @file
#
#  "Solution"-file for 'Hands-On-3' example (Bender Tutorial)
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date   2004-10-12
# =============================================================================
__author__  = ' Vanya BELYAEV  Ivan.Belyaev@nikhef.nl '
__version__ = ' CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.15 $  '  
# =============================================================================
## import everything from BENDER
from Bender.MainMC import *
# =============================================================================
## @class MCKaons
#  the solution for the third excersize 
class MCKaons(AlgoMC) :
    """
    The solution for the third excersize
    """
    
    ## the main analysus method 
    def analyse( self ) :
        """
        The main analysis method
        """ 
        ## get the helper object 
        finder = self.mcFinder()
        
        ## get all kaons from the tree :
        kaons  = finder.find( ' [B_s0 -> J/psi(1S) ( phi(1020) -> ^K+ ^K- ) ]cc' )

        ## retrive (book-on-demand) N-tuple objects 
        tup = self.nTuple( 'My N-Tuple' )
        for K in kaons :            
            print ' Kaon: %s PT=%s MeV' % ( K.name() , K.momentum().pt() ) 
            
            tup.column ( 'P'  , MCP ( K ) / 1000 )
            tup.column ( 'PT' , MCPT( K ) / 1000 )
            tup.column ( 'ID' , MCID( K )       )
            tup.column ( 'Q3' , MC3Q( K )       )
            # 4-momentum
            mom = K.momentum() 
            tup.column ( 'px' , mom.Px() )
            tup.column ( 'py' , mom.Py() )
            tup.column ( 'pz' , mom.Pz() )
            # origin vertex 
            ov = K.originVertex().position()
            tup.column ( 'vx' , ov.X() )
            tup.column ( 'vy' , ov.Y() )
            tup.column ( 'vz' , ov.Z() )
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
    from Gaudi.Configuration import NTupleSvc
    NTupleSvc (
        Output = [
        "MCK DATAFILE='HandsOn3.root' OPT='NEW' TYP='ROOT'"
        ]
        )
    
    ## get/create application manager
    gaudi = appMgr() 
    
    # modify/update the configuration:
    
    ## 1) create & configure the algorithm
    alg = MCKaons(
        'MCKaons'                  , ## algorithm name 
        NTupleLUN = 'MCK'          , ## logical file unit for N-tuples  
        PP2MCs = []                  ## list of MC-truth links
        )
    
    ## 2) replace the list of top level algorithm by only *THIS* algorithm
    gaudi.setAlgorithms( [alg ])
    
    ## redefine input files 
    evtSel = gaudi.evtSel()
    evtSel.PrintFreq = 50
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
    run(500)
# =============================================================================


# =============================================================================
# The END 
# =============================================================================
