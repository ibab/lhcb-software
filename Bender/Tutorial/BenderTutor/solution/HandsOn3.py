#!/usr/bin/env python2.4
# =============================================================================
# $Id: HandsOn3.py,v 1.8 2006-11-17 11:59:47 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.8 $
# =============================================================================
""" 'Solution'-file for 'HandsOn3.py' example (Bender Tutorial) """
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
from bendermodule import *

# =============================================================================
## @class MCKaons
#  the solution for the third excersize 
class MCKaons(AlgoMC) :
    """  the solution for the third excersize """

    ## the main analysus method 
    def analyse( self ) :
        """ the main analysus method """
        
        ## get the helper object 
        finder = self.mcFinder()
        
        ## get all kaons from the tree :
        kaons  = finder.find( ' [B_s0 -> J/psi(1S) ( phi(1020) -> ^K+ ^K- ) ]cc' )

        ## retrive (book-on-demand) N-tuple objects 
        tup = self.nTuple( 'My N-Tuple' )
        for K in kaons :            
            print ' Kaon: %s PT=%s MeV' % ( nameFromPID( K.particleID() ) ,
                                            K.momentum().pt() ) 

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
    """ configure the job """ 
    
    gaudi.config ( files =
                   [ '$DAVINCIROOT/options/DaVinciCommon.opts' ] )
    
    # modify/update the configuration:
    
    ## 1) create the algorithm
    alg = MCKaons( 'MCKaons' )
    
    ## 2) replace the list of top level algorithm by only *THIS* algorithm
    gaudi.setAlgorithms( [alg ])
    

    ## configure the histograms:
    if 'HbookCnv' not in gaudi.DLLs : gaudi.DLLs += ['HbookCnv']
    gaudi.HistogramPersistency = "HBOOK"
    hps = gaudi.service('HistogramPersistencySvc')
    hps.OutputFile = 'HandOn3_histos.hbook'
    
    ## add the printout of the histograms
    hsvc = gaudi.service( 'HbookHistSvc' )
    hsvc.PrintHistos = True
    
    ## configure the N-Tuples:
    ntsvc = gaudi.ntuplesvc()
    ntsvc.Output = [ "MC DATAFILE='HandsOn3.hbook' OPT='NEW' TYP='HBOOK'" ]     

    ## configure own algorithm 
    myAlg = gaudi.algorithm('MCKaons')
    myAlg.NTupleLUN = 'MC'
    myAlg.PP2MCs = []
    
    
    ## redefine input files 
    evtSel = gaudi.evtSel()
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

    
# =============================================================================
# $Log: not supported by cvs2svn $
#
# =============================================================================
# The END 
# =============================================================================
