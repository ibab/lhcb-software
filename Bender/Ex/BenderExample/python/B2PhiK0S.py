# $Id: B2PhiK0S.py,v 1.2 2004-01-22 22:35:43 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $
# =============================================================================
# @file 
# The simple selection/analysis algorithm for Bd -> Phi K0S selection/analysis
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# date    2003-12-04
# =============================================================================

# import the Bender itself  
from   bendermodule import *
import benderconfig as bender 

g.JobOptionsType = 'NONE'
g.OutputLevel = 3
g.config()

class B2PhiK0S(Algo):
    "Simple selection/analysis algorithm for Bd -> Phi(K+K-) K0S(pi+pi-) decay"
    def analyse ( self ) :

        # reset the filter 
        self.setFilterPassed( FALSE )
        
        # all primary vertices
        # primaries = self.vselect ( tag = "PVs" , cuts = VTYPE == VertexType.Primary )
        primaries = self.vselect ( tag = "PVs" , cuts = VTYPE == 1 )
        if primaries.empty() : return SUCCESS                                    # RETURN
        
        # select all reconstructed K0S 
        K0S       = self.select  ( tag = "K0S" , cuts =  ID == "KS0" )
        if K0S.empty()     : return SUCCESS                                      # RETURN
    
        # select kaons for Phi-> K+ K-  reconstruction  
        mips = MIPCHI2( primaries , self.geo() )
        # kaons     = self.select  ( tag = "K"   , cuts = abs( ID ) == 321   )
        kplus     = self.select  ( tag = "K+"  , cuts = ID ==  321  )
        kminus    = self.select  ( tag = "K-"  , cuts = ID == -321 )
        if kplus.empty()  : return SUCCESS                                      # RETURN
        if kminus.empty() : return SUCCESS                                      # RETURN
        
        # some MC truth indfromation
        mc     = self.mctruth()
        # lists of all true B0, true K0S and true Phi  
        B0mc   = mc.find ( decay = " B0        -> phi(1020) KS0 " )  
        K0Smc  = mc.find ( decay = " KS0       -> pi+       pi- " )
        PHImc  = mc.find ( decay = " phi(1020) -> K+        K-  " )

        # MC cut for true B0, true K0S and true Phi  
        mcB0   = MCTRUTH       ( mc , B0mc  )
        mcK0S  = MCTRUTH       ( mc , K0Smc )
        mcPHI  = MCTRUTH       ( mc , PHImc )
        
        # cut on phi delta mass 
        # dmPHI  = abs( DMASS("phi(1020)") ) <= 20 * MeV 

        phis  = self.loop( formula='K+ K-' , pid="phi(1020)" )
        for phi in phis :
            if phi.mass(1,2) > 1050 * MeV             : continue
            if not phi()                              : continue 
            if VCHI2( phi )  > 100                    : continue
            #if not dmPHI( phi )                       : continue
            phi.save( "phi" )
            
        # check selected phis 
        Phis = self.selected("phi")
        if Phis.empty()    :  return SUCCESS                                    # RETURN

        # some general information
        
        tuple = self.ntuple ( title = "B0 -> phi K0S analysis " )
        B0s   = self.loop( formula = "phi K0S" , pid = "B0")
        for B0 in B0s :
            if B0.mass(1,2)   > 6 * GeV          : continue
            if B0.mass(1,2)   < 4 * GeV          : continue
            if VCHI2( B0 )    > 49               : continue
            
            if not B0() : continue 
            Phi = B0(1)
            K0S = B0(2)
            if not Phi  : continue 
            if not K0S  : continue 
            K1  = child( Phi , 1 ) 
            K2  = child( Phi , 2 ) 
            pi1 = child( K0S , 1 ) 
            pi2 = child( K0S , 2 ) 

            if not  K1  : continue 
            if not  K2  : continue 
            if not pi1  : continue 
            if not pi2  : continue 

            # find 'own' primary vertex
            vip = VIP( particle( B0 ) , self.geo() )
            primary = SelectPrimaryVertexMin ( primaries , vip  )
            if not primary : continue
            
            B0.setPV( primary )

            # multiplicity of primary vertices 
            #tuple.column  ( name = "nPVs"     , value = primaries.size() )

            # masses 
            tuple.column  ( name = "B0Mass"   , value = M     ( B0  )   /  GeV   ) 
            tuple.column  ( name = "PhiMass"  , value = M12   ( Phi )   /  GeV   )
            tuple.column  ( name = "K0SMass"  , value = M12   ( K0S )   /  GeV   )
            tuple.column  ( name = "K0SM"     , value = M     ( K0S )   /  GeV   )
            
            tuple.column  ( name = "B0Chi2"   , value = VCHI2 ( vertex ( B0  ) ) )
            tuple.column  ( name = "PhiChi2"  , value = VCHI2 ( vertex ( Phi ) ) )
            tuple.column  ( name = "K0SChi2"  , value = VCHI2 ( vertex ( K0S ) ) )

            tuple.column  ( name = "mipK1"    , value = mips ( K1  ) )
            tuple.column  ( name = "mipK2"    , value = mips ( K2  ) )
            tuple.column  ( name = "mipPhi"   , value = mips ( Phi ) )
            tuple.column  ( name = "mipK0S"   , value = mips ( K0S ) )
            tuple.column  ( name = "mipB0"    , value = mips ( B0  ) )
            
            #ctau = VDTIME ( self.point( vertex = primary ) )
            #tuple.column  ( name = "B0Ctau"   , value = ctau( B0 ) )
            
            #dira = DIRA   ( self.point( vertex = primary ) )
            #tuple.column  ( name = "B0DirA"   , value = dira( B0 ) )
            
            # vertex positions 
            #tuple.column  ( name = "B0Vx"      , value = vertex( B0  ) .position() )
            #tuple.column  ( name = "PhiVx"     , value = vertex( Phi ) .position() )
            #tuple.column  ( name = "K0SVx"     , value = vertex( K0S ) .position() )
            #tuple.column  ( name = "PVVx"      , value = primary       .position() )
            
            #tuple.column  ( name = "B0P"       , value = B0  .momentum() ) ;
            #tuple.column  ( name = "PhiP"      , value = Phi .momentum() ) ;
            #tuple.column  ( name = "K0SP"      , value = K0S .momentum() ) ;
            
            tuple.column  ( name = "LV01"      , value = LV01( B0  ) ) ;
            tuple.column  ( name = "LV01K0"    , value = LV01( K0S ) ) ;

            #tuple.column  ( name = "B0MC"      , value = mcB0  ( B0  ) ) ;
            #tuple.column  ( name = "PHIMC"     , value = mcPHI ( Phi ) ) ;
            #tuple.column  ( name = "K0SMC"     , value = mcK0S ( K0S ) ) ;
            
            tuple.write()
            B0.save("B0")

        # all selected B0
        B0S = self.selected("B0")
        if not B0S.empty() : self.setFilterPassed( TRUE )
        
        return SUCCESS


# =============================================================================
# Generic job configuration 
# =============================================================================

bender.config( files   = [ '$DAVINCIROOT/options/DaVinci.opts' ] ,
               options = [ 'EcalPIDmu.OutputLevel   =   5  '   ,
                           'HcalPIDmu.OutputLevel   =   5  '   ,
                           'EcalPIDe.OutputLevel    =   5  '   ,
                           'HcalPIDe.OutputLevel    =   5  '   ,
                           'BremPIDe.OutputLevel    =   5  '   ,
                           'PrsPIDe.OutputLevel     =   5  '   ,
                           'EventSelector.PrintFreq =  10  '   ] )

g.HistogramPersistency = "HBOOK" ;

# =============================================================================
# specific job configuration 
# =============================================================================

bender.config( files   = [ '$KS2PIPISELROOT/options/PreLoadPions.opts'  ,
                           '$KS2PIPISELROOT/options/bestKs2PiPi.opts' ] ,
               options = [ 'loadUPions.OutputLevel   = 4 ' ,
                           'loadLPions.OutputLevel   = 4 ' ,
                           'loadVTTPions.OutputLevel = 4 ' ,
                           'LLKs2PiPi.OutputLevel    = 4 ' , 
                           'UUKs2PiPi.OutputLevel    = 4 ' , 
                           'LVKs2PiPi.OutputLevel    = 4 ' , 
                           'LUKs2PiPi.OutputLevel    = 4 ' , 
                           'LLKs2PiPi.UnconstVertexFitter.OutputLevel = 5 ' ,
                           'LVKs2PiPi.UnconstVertexFitter.OutputLevel = 5 ' ,
                           'LUKs2PiPi.UnconstVertexFitter.OutputLevel = 5 ' , 
                           'UUKs2PiPi.UnconstVertexFitter.OutputLevel = 5 ' ,
                           'NeutralPP2MC.InputData = {} '   ] )

# preload algorithm(s)
g.topAlg += [ 'LoKiPreLoad/Charged']

# configure the preload algorithm(s) 
preload  = g.property('Charged')
preload.OutputLevel = 4

desktop                   = g.property('Charged.PhysDesktop')
desktop.ParticleMakerType = "CombinedParticleMaker"
desktop.OutputLocation    = "/Event/Phys/Charged"

maker                    = g.property('Charged.PhysDesktop.CombinedParticleMaker')
maker.ExclusiveSelection = 0
maker.Particles          = [ "kaon" , "pion" ]
maker.KaonSelection      = [ "det='RICH' k-pi='-5.0'   k-p='-5.0' "]
maker.PionSelection      = [ "det='RICH' pi-k='-5.0' "]
 

# create my algorithm and add it to top level algorithms 
alg = B2PhiK0S('B2PhiK0S')
g.topAlg += ['B2PhiK0S']

# configure my algorithm 
alg = gaudi.iProperty('B2PhiK0S')
alg.TupleLUN    = 'PHIK0S'
# confugure my desktop

# configure own desktop 
desktop                 = g.property('B2PhiK0S.PhysDesktop')
desktop.InputLocations  = [ "/Event/Phys/Charged" ,
                            "/Event/Phys/LLKs"    ,
                            "/Event/Phys/UUKs"    ,
                            "/Event/Phys/LUKs"    ,
                            "/Event/Phys/LVKs"    ]
desktop.OutputLocation  =   "/Event/Phys/B2PhiK0S"


# output histogram file 
hsvc = g.property( 'HistogramPersistencySvc' )
hsvc.OutputFile = 'B2PhiK0S.hbook'

# input data 
g.readOptions('/afs/cern.ch/lhcb/project/web/cards/412000.opts')


# N-tuples
tup = gaudi.iProperty( 'NTupleSvc' )
tup.Output = [ "PHIK0S DATAFILE='B2PhiK0S_tup.hbook' TYP='HBOOK' OPT='NEW'" ]

# =============================================================================
# job execution 
# =============================================================================

g.initialize()

#g.run(100)  ## crash !!
g._evtpro.executeRun(100)

g.exit()
# =============================================================================
# The END 
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.1  2003/12/07 16:14:30  ibelyaev
#  update for Bender application and project
#
# =============================================================================
