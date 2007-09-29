#!/usr/bin/env python2.4
# =============================================================================
"""
'Solution'-file for 'RCMCselect.py' example (Bender Tutorial)
""" 
# =============================================================================
# @file
#
# "Solution"-file for 'RCMCselect.py' example (Bender Tutorial)
#
# @author Vanya BELYAEV ibelyaev@physics.syr.edu
# @date   2004-10-12
# =============================================================================
__author__ = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
# =============================================================================

# =============================================================================
## import everything from BENDER
from Bender.MainMC import *

# =============================================================================
## @class RCKaons
#  solution for the fourth exersize 
class RCKaons(AlgoMC):
    """
    The solution for the fourth exersize
    """
    
    ## the main analysis method 
    def analyse( self ) :
        """
        The main analysis method
        """

        ## helper MC object 
        finder = self.mcFinder()
        
        ## get all MC phis from the tree :
        mcPhi = finder.find( ' phi(1020) : [B_s0 -> J/psi(1S) ( phi(1020) -> K+ K- ) ]cc' )
        
        ## get all MC kaons  from the tree :
        mcK   = finder.find( ' [B_s0 -> J/psi(1S) ( phi(1020) -> ^K+ ^K- ) ]cc' )

        ## prepare Monte Carlo "Cuts" 
        mcCutPhi = MCTRUTH( self.mcTruth() , mcPhi )
        mcCutK   = MCTRUTH( self.mcTruth() , mcK   )

        ## get reconstructed kaons
        kplus  = self.select ( 'K+' , 'K+' == ID  )
        kminus = self.select ( 'K-' , 'K-' == ID  )

        ## retrive (boon-on-demand) N-tuple 
        tup = self.nTuple ( 'My N-Tuple' )
        
        ## loop over K K combinations
        phis = self.loop( 'K+ K-' , 'phi(1020)' )
        for phi in phis :
            
            if phi.mass(1,2) > 1050       : continue
            if not 0 < VCHI2( phi ) < 100 : continue
            
            m = M( phi ) / 1000
            
            self.plot ( m , 'Di kaon mass ', 1.0 , 1.050 )
            if mcCutPhi( phi ) :
                self.plot ( m , 'Di kaon mass (MCTruth)'  ,
                            1.0 , 1.050 )
                
            tup.column( 'M'      , m )
            tup.column( 'chi2v'  , VCHI2(phi)   )
            
            kaon1 = phi(1)
            kaon2 = phi(2)
            
            ## identification variables 
            tup.column ( 'dllk1'  , PIDK( kaon1 ) )
            tup.column ( 'dllk2'  , PIDK( kaon2 ) )

            ## pt-momenta
            tup.column ( 'pt1'    , PT ( kaon1 ) ) 
            tup.column ( 'pt2'    , PT ( kaon2 ) )
            
            ## MC truth flags:
            ##  true phi
            tup.column ( 'mc'     , mcCutPhi ( phi   ) )
            ##   the first kaon is true 
            tup.column ( 'mck1'   , mcCutK   ( kaon1 ) )
            ##   the second kaon is true   
            tup.column ( 'mck2'   , mcCutK   ( kaon2 ) )
            
            ## commit the fileld row
            tup.write()
            
        return SUCCESS 
# =============================================================================

# =============================================================================
## configure the job
def configure() :
    """
    configure the job
    """

    import data_tutorial as data 

    line = 'NTupleSvc.Output += { "RC DATAFILE=\'%s\' TYP=\'%s\' OPT=\'NEW\'" }'
    line = line % ( 'HandsOn4.hbook' , 'HBOOK' )
    
    gaudi.config ( files = [
        '$DAVINCIROOT/options/DaVinci.opts'                 ,
        '$COMMONPARTICLESROOT/options/StandardKaons.opts' ] ,
                   options = [ line ] )
    
    ## modify/update the configuration:
    
    ## 1) create the algorithm
    alg = RCKaons( 'RCKaons' )
    
    ## 2) add algorithm to the list of TopLevel algorithms 
    gaudi.addAlgorithm( alg )
    
    ## 3) configure algorithm
    desktop = gaudi.tool('RCKaons.PhysDesktop')
    desktop.InputLocations = [
        'Phys/StdLooseKaons' ,
        'Phys/StdLooseMuons' 
        ]
    
    ## configure the histograms:
    if 'HbookCnv' not in gaudi.DLLs : gaudi.DLLs += ['HbookCnv']
    gaudi.HistogramPersistency = "HBOOK"
    hsvc = gaudi.service('HistogramPersistencySvc')
    hsvc.OutputFile =  'rskaons_histos.hbook' 
    
    ## add the printout of the histograms at the end of the job 
    hsvc = gaudi.service( 'HbookHistSvc' )
    hsvc.PrintHistos = True
    
    ## configure own algorithm 
    alg.PP2MCs = ['Relations/Rec/ProtoP/Charged'] # only charged 
    alg.NTupleLUN = 'RC'
    
    # redefine input files 
    evtSel = gaudi.evtSel()
    evtSel.PrintFreq = 50
    
    ## redefine input files 
    evtSel.open( data.FILEs ) 

    return SUCCESS
# =============================================================================

# =============================================================================
# The job steering 
# =============================================================================
if __name__ == '__main__' :
    
    # job configuration
    configure()
    
    # event loop 
    gaudi.run(500)

# =============================================================================
# The END 
# =============================================================================
