#!/usr/bin/env python
# =============================================================================
# $Id: HandsOn4.py,v 1.5 2005-01-24 17:29:40 ibelyaev Exp $
# =============================================================================
# CVS version $Revision: 1.5 $ 
# =============================================================================
# CVS tag     $Name: not supported by cvs2svn $ 
# =============================================================================
""" 'Solution'-file for 'RCMCselect.py' example (Bender Tutorial) """ 
# =============================================================================
# @file
#
# "Solution"-file for 'RCMCselect.py' example (Bender Tutorial)
#
# @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
# @date   2004-10-12
# =============================================================================
__author__ = 'Vanya BELYAEV  belyaev@lapp.in2p3.fr'

# import everything from BENDER
from bendermodule import *

# =============================================================================
# the algorithm itself 
# =============================================================================
class RCKaons(Algo):
    def analyse( self ) :

        # helper MC object 
        mc = self.mctruth()
        
        # get all MC phis from the tree :
        mcPhi = mc.find( decay = ' phi(1020) : [B_s0 -> J/psi(1S) ( phi(1020) -> K+ K- ) ]cc' )
        
        # get all MC kaons  from the tree :
        mcK = mc.find( decay = ' [B_s0 -> J/psi(1S) ( phi(1020) -> ^K+ ^K- ) ]cc' )

        # prepare Monte Carlo "Cuts" 
        mcCutPhi = MCTRUTH( mc , mcPhi )
        mcCutK   = MCTRUTH( mc , mcK   )

        # get reconstructed kaons
        kplus  = self.select ( tag = 'K+' , cuts = 'K+' == ID  )
        kminus = self.select ( tag = 'K-' , cuts = 'K-' == ID  )

        tup = self.nTuple ( title = 'My N-Tuple' )
        
        # loop over K K combinations
        phis = self.loop( formula = 'K+ K-' , pid = 'phi(1020)' )
        for phi in phis :
            
            if phi.mass(1,2) > 1050 * MeV : continue
            if not 0 < VCHI2( phi ) < 100 : continue
            
            self.plot ( title = 'Di kaon mass '  ,
                        value = M(phi)/ GeV      ,
                        low = 1.0 , high = 1.050 )
            if mcCutPhi( phi ) :
                self.plot ( title = 'Di kaon mass (MCTruth)'  ,
                            value = M(phi)/ GeV      ,
                            low = 1.0 , high = 1.050 )
                
            tup.column( name = 'M'      , value = M(phi) / GeV )
            tup.column( name = 'chi2v'  , value = VCHI2(phi)   )
            
            kaon1 = phi(1)
            kaon2 = phi(2)
            
            # identification variables 
            tup.column( name = 'dllk1'  , value = PIDK( kaon1 ) )
            tup.column( name = 'dllk2'  , value = PIDK( kaon2 ) )

            # MC truth flags:
            #   true phi
            tup.column( name = 'mc'     , value = mcCutPhi ( phi ) )
            #   the first kaon is true 
            tup.column( name = 'mck1'   , value = mcCutK   ( kaon1 ) )
            #   the seons kaon is true   
            tup.column( name = 'mck2'   , value = mcCutK   ( kaon2 ) )
            tup.write()
            
        return SUCCESS 
# =============================================================================

# =============================================================================
# configure the job
# =============================================================================
def configure() :
    
    gaudi.config ( files = ['$BENDERTUTOROPTS/BenderTutor.opts' ] )
    
    # modify/update the configuration:
    
    # Preload all charged particles 
    gaudi.addAlgorithm( 'LoKiPreLoad/Charged' )
    
    import benderPreLoad as preload
    preload.Charged( Name  = 'Charged' ,
                     Kaons = [ "det='RICH'  k-pi='0.0' k-p='-5.0' " ] )
    
    # 1) create the algorithm
    alg = RCKaons( 'RCKaons' )
    
    # 2) replace the list of top level algorithm by only *THIS* algorithm
    gaudi.addAlgorithm( alg )
    
    # 3) configure algorithm
    desktop = gaudi.tool('RCKaons.PhysDesktop')
    desktop.InputLocations = [ "/Event/Phys/Charged" ]
    
    # configure the histograms:
    hsvc = gaudi.histoSvc('HistogramPersistencySvc')
    hsvc.setOutput( 'rckaonhistos.hbook' , 'HBOOK' )    

    # add the printout of the histograms
    hsvc = gaudi.service( 'HbookHistSvc' )
    hsvc.PrintHistos = True
    
    # configure the N-Tuples:
    ntsvc = gaudi.nTupleSvc()
    ntsvc.defineOutput( { 'RC' : 'rckaontuples.hbook' } , 'HBOOK' )
    
    myAlg = gaudi.algorithm('RCKaons')
    myAlg.NTupleLUN = 'RC'
    
    # redefine input files 
    evtsel = gaudi.evtSel()
    evtsel.open( [ 'LFN:/lhcb/production/DC04/v1/DST/00000543_00000017_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000018_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000016_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000020_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000024_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000019_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000021_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000022_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000001_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000002_5.dst' ] )
    
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
# The END 
# =============================================================================
