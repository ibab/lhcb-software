#!/usr/bin/env python
# =============================================================================
"""
'Solution'-file for 'HandsOn3.py' example (Bender Tutorial)
"""
# =============================================================================
## @file
#
#  "Solution"-file for 'HandsOn3.py' example (Bender Tutorial)
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date   2004-10-12
# =============================================================================
__author__ = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
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
            print ' Kaon: %s PT=%s MeV' % (
                LoKi.Particles.nameFromPID( K.particleID() ) , K.momentum().pt() ) 
            
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
    import BenderTutor.data_tutorial as data 
    
    gaudi.config ( files =
                   [ '$DAVINCIROOT/options/DaVinciCommon.opts' ] )
    
    # modify/update the configuration:
    
    ## 1) create the algorithm
    alg = MCKaons( 'MCKaons' )
    
    ## 2) replace the list of top level algorithm by only *THIS* algorithm
    gaudi.setAlgorithms( [alg ])
    

    ## configure the histograms:
    hps = gaudi.service('HistogramPersistencySvc')
    hps.OutputFile = 'HandOn3_histos.root'
    
    ## configure the N-Tuples:
    ntsvc = gaudi.ntuplesvc()
    ntsvc.Output = [ "MC DATAFILE='HandsOn3.root' OPT='NEW' TYP='ROOT'" ]     

    ## configure own algorithm 
    alg.NTupleLUN = 'MC'
    alg.PP2MCs = []
        
    ## redefine input files 
    evtSel = gaudi.evtSel()
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
    gaudi.run(500)
# =============================================================================


# =============================================================================
# The END 
# =============================================================================
