#!/usr/bin/env python
# =============================================================================
# $Id: Bs2PhiMM.py,v 1.1 2005-02-16 10:14:45 ibelyaev Exp $
# =============================================================================
# CVS version $Revision: 1.1 $
# =============================================================================
# CVS tag     $Name: not supported by cvs2svn $
# =============================================================================
# @file 
# "Demo" algorithm for Bs -> phi mu+ mu- decay  
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2003-12-04
# =============================================================================
"""
Trivial algorithm to study the major featured of Bs -> phi mu+ mu- decay
"""
# =============================================================================
__author__ = 'Vanya BELYAEV belyaev@lapp.in2p3.fr'
# =============================================================================

# import the Bender itself  
from   bendermodule  import *
import benderPreLoad as preload

# =============================================================================
# Specific physics analysis algorithm 
# =============================================================================

class Bs2PhiMM(Algo):
    """
    My own analysis algorithm for selection of Bs - > ( phi -> K+ K- ) mu+ mu- 
    """
    def analyse ( self ) :
        
        # get MC truth Bs
        mc    = self.mctruth()
        # true Bs 
        mcBs  = mc.find ( decay = ' [B_s0 ->  mu+   mu- (  phi(1020) ->  K+  K- ) ]cc' )
        # true phis from this decay  
        mcPhi = mc.find ( decay = ' [B_s0 ->  mu+   mu- ( ^phi(1020) ->  K+  K- ) ]cc' )
        # true muons from this decay
        mcMu  = mc.find ( decay = ' [B_s0 -> ^mu+  ^mu- (  phi(1020) ->  K+  K- ) ]cc' )
        # true kaons from this decay
        mcK   = mc.find ( decay = ' [B_s0 ->  mu+   mu- (  phi(1020) -> ^K+ ^K- ) ]cc' )

        # create MC cuts/predicates 
        mcCutBs  = MCTRUTH( mc , mcBs  )
        mcCutPhi = MCTRUTH( mc , mcPhi )
        mcCutMu  = MCTRUTH( mc , mcMu  )
        mcCutK   = MCTRUTH( mc , mcK   )
        
        # get all reconstructed primary vertices:
        primaries = self.vselect ( tag  = "PV"                         ,
                                   cuts =  VertexType.Primary == VTYPE ) 
        if primaries.empty() :
            return self.Warning( "No primary vertex is found!" , SUCCESS )
        
        # minimal impact parameter of tarck with respect to ALL primary vertices
        mips = MIPCHI2( primaries , self.geo() )
        
        # get all reconstructed muons with loose criteria 
        muons   = self.select ( tag    = 'mu' ,
                                cuts   = ( 'mu+' == ABSID ) & ( PIDmu > -5 ) )
        
        muplus  = self.select ( tag    = 'mu+' ,
                                source = muons ,
                                cuts   = Q > 0 ) 
        muminus = self.select ( tag    = 'mu-' ,
                                source = muons ,
                                cuts   = Q < 0 ) 

        # get all reconstructed kaons with loose criteria 
        kaons   = self.select( tag = "K"  , cuts = ( 'K+'  == ABSID ) & ( PIDK  > -5 ) )
        
        kplus   = self.select ( tag    = 'K+'  ,
                                source = kaons ,
                                cuts   = Q > 0 )
        
        kminus  = self.select ( tag    = 'K-'  ,
                                source = kaons ,
                                cuts   = Q < 0 ) 
        
        # retrive  N-Tuple 
        tup = self.nTuple( title = 'Bs -> Phi Mu+ Mu-' )
        
        # make a loop over 4 particles 
        for Bs in self.loop( formula = 'K+ K- mu+ mu-' , pid = 'B_s0' ) :
            
            # rough estimate for invariant mass 
            mass = Bs.mass(0)
            if not 3 * GeV < mass < 8.0 * GeV : continue
            
            # rough estimate for invariant KK-mass 
            m12   = Bs.mass(1,2)
            if not             m12 < 1.1 * GeV : continue
            
            K1  = Bs.particle(1)
            K2  = Bs.particle(2)
            Mu1 = Bs.particle(3)
            Mu2 = Bs.particle(4)

            # from upstream PVxs select PVx with minimal B-impact parameter
            B = Bs.particle()
            if not B       : continue                                # CONTINUE 
            vipChi2 = VIPCHI2( B , self.geo() ) 
            vCut    = ( vipChi2 >= 0 ) & ( VPSD( B , self.geo() ) > ( -0.5 * mm ) )  
            primary = SelectPrimaryVertexMin( primaries , # primary vertices 
                                              vipChi2   , # to be minimized 
                                              vCut      ) # cut 
            if not primary :
                self.Warning("Could not find the approproate primary vertex")
                continue
            
            # fill n-Tuple
            tup.column_ ( "mass"  , M     ( Bs  ) / GeV ) # KKmm mass
            tup.column_ ( "chi2"  , VCHI2 ( Bs )        ) # chi2 of vertex fit 
            tup.column_ ( "m12"   , M12   ( Bs  ) / GeV ) # KK   mass
            tup.column_ ( "m34"   , M34   ( Bs  ) / GeV ) #   mm mass

            # transverse momenta 
            tup.column_ ( "pt1"   , PT    ( K1  ) / GeV ) # pt of K1 
            tup.column_ ( "pt2"   , PT    ( K2  ) / GeV ) # pt of K2 
            tup.column_ ( "pt3"   , PT    ( Mu1 ) / GeV ) # pt of Mu1 
            tup.column_ ( "pt4"   , PT    ( Mu2 ) / GeV ) # pt of Mu2

            # impact parameter significances for daughter particles 
            tup.column_ ( "mip1"  , mips  ( K1  )       )  
            tup.column_ ( "mip2"  , mips  ( K2  )       ) 
            tup.column_ ( "mip3"  , mips  ( Mu1 )       ) 
            tup.column_ ( "mip4"  , mips  ( Mu2 )       )
            
            # impact parameter significance for the whole Bs
            tup.column_ ( "mips"  , mips  ( Bs  )       )
            
            # identification variables
            tup.column_ ( "pidk1" , PIDK  ( K1  )       ) # DLL(K-pi)  for K1 
            tup.column_ ( "pidk2" , PIDK  ( K2  )       ) # DLL(K-pi)  for K2 
            tup.column_ ( "muid1" , PIDmu ( Mu1 )       ) # DLL(mu-pi) for Mu1 
            tup.column_ ( "muid2" , PIDmu ( Mu2 )       ) # DLL(mu-pi) for Mu2
            
            # mctruth flags
            tup.column_ ( "mcbs"   , mcCutBs ( Bs  ) , 0 , 1 )
            tup.column_ ( "mck1"   , mcCutK  ( K1  ) , 0 , 1 )
            tup.column_ ( "mck2"   , mcCutK  ( K2  ) , 0 , 1 )
            tup.column_ ( "mcm1"   , mcCutMu ( Mu1 ) , 0 , 1 )
            tup.column_ ( "mcm2"   , mcCutMu ( Mu2 ) , 0 , 1 )

            dz   = VDSIGN  ( primary )  # z(Bs) - z(PV)
            tof  = VDTIME  ( primary )  # c*tau evaluator 
            dtof = VDDTIME ( primary )  # c*tau evaluator
            # the cosine of angle between Bs momentum and 
            # the vector from primary vertex to decay vertex 
            dira = DIRA    ( primary )

            tup.column_ ( "dz"     , dz      ( Bs )  ) # z(Bs)-z(PV)
            tup.column_ ( "tof"    , tof     ( Bs )  ) # c*tau for Bs 
            tup.column_ ( "dtof"   , dtof    ( Bs )  ) # c*tau for Bs 
            tup.column_ ( "dira"   , dira    ( Bs )  ) # angle 
            
            tup.write()

        return SUCCESS 
        

# =============================================================================
# Generic job configuration 
# =============================================================================
def configure() :
    gaudi.config( files   =
                  [ '$BENDEREXAMPLEOPTS/BenderExample.opts' ,   # general options 
                    '$BENDEREXAMPLEOPTS/PoolCatalogs.opts'  ,   # pool catalogs
                    '$LOKIEXAMPLEOPTS/Bs_phimumu_DC04.opts' ] , # input data 
                  options =
                  [ 'EcalPIDmu.OutputLevel     =   5  ' ,
                    'HcalPIDmu.OutputLevel     =   5  ' ,
                    'EcalPIDe.OutputLevel      =   5  ' ,
                    'HcalPIDe.OutputLevel      =   5  ' ,
                    'BremPIDe.OutputLevel      =   5  ' ,
                    'PrsPIDe.OutputLevel       =   5  ' ,
                    'NeutralPP2MC.OutputLevel  =   5  ' ,
                    'Hadrons.OutputLevel       =   5  ' ,
                    'EventSelector.PrintFreq   = 100  ' ] )
    
    # specific job configuration 
    
    # preload algorithm(s)
    gaudi.addAlgorithm ( 'LoKiPreLoad/Charged' ) 
    preload.Charged( Particles = [ 'muon' , 'kaon'] )
    
    # create analysis algorithm and add it to the list of
    alg = Bs2PhiMM('Bs2PhiMM')    
    gaudi.addAlgorithm ( alg ) 
    
    desktop = gaudi.tool('Bs2PhiMM.PhysDesktop')
    desktop.InputLocations  = [ '/Event/Phys/Charged']

    # output histogram file 
    hsvc = gaudi.service( 'HistogramPersistencySvc' )
    hsvc.HistogramPersistency = "HBOOK" 
    hsvc.OutputFile = 'bsphimmhistos.hbook'
    
    # add the printout of the histograms
    hsvc = gaudi.service( 'HbookHistSvc' )
    hsvc.PrintHistos = True

    # switch off Davinci histograms
    dv = gaudi.algorithm('DaVinci')
    dv.doHistos = False
    
    # configure the N-Tuples:
    ntsvc = gaudi.nTupleSvc()
    ntsvc.defineOutput( { 'PHIMM' : 'bsphimmtuples.hbook' } , 'HBOOK' )
    
    # configure my own algorithm
    myAlg = gaudi.algorithm('Bs2PhiMM')
    myAlg.NTupleLUN = 'PHIMM'

    return SUCCESS

# =============================================================================
# job execution 
# =============================================================================

if __name__ == '__main__' :
    import sys 
    # configure the job
    configure() 
    # run job 
    gaudi.run  ( 500 )
    # terminate the Application Manager 
    gaudi.exit ()
    
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
