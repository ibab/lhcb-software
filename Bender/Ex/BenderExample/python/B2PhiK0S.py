# $Id: B2PhiK0S.py,v 1.1 2003-12-07 16:14:30 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $
# =============================================================================
# @file 
# The simple selection/analysis algorithm for Bd -> Phi K0S selection/analysis
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# date    2003-12-04
# =============================================================================

from BenderModule import *
 
class B2PhiK0S(Bender):
    "Simple selection/analysis algorithm for Bd -> Phi(K+K-) K0S(pi+pi-) decay"
    def analyse ( self ) :

        # reset the filter 
        self.setFilterPassed( FALSE )
        
        # all primary vertices
        primaries = self.vselect ( tag = "PVs" , cuts = VTYPE == VertexType.Primary )
        if primaries.empty() : return SUCCESS                                    # RETURN
        
        # select all reconstructed K0S 
        K0S       = self.select  ( tag = "K0S" , cuts =  ID == "KS0" )
        if K0S.empty()     : return SUCCESS                                      # RETURN
    
        # select kaons for Phi-> K+ K-  reconstruction  
        mips = MIPCHI2( primaries , self.geo() )
        kaons     = self.select  ( tag = "K"   , cuts = abs( ID ) == 321   )
        if kaons.empty()   : return SUCCESS                                      # RETURN
        kplus     = self.select  ( tag = "K+"  , source = kaons , cuts = Q >  0.5 )
        kminus    = self.select  ( tag = "K-"  , source = kaons , cuts = Q < -0.5 )
        
        # some MC truth indfromation
        mc     = self.mctruth()
        # lists of all true B0, true K0S and true Phi  
        B0mc   = mc.findDecays ( decay = " B0        -> phi(1020) KS0 " )  
        K0Smc  = mc.findDecays ( decay = " KS0       -> pi+       pi- " )
        PHImc  = mc.findDecays ( decay = " phi(1020) -> K+        K-  " )

        # MC cut for true B0, true K0S and true Phi  
        mcB0   = MCTRUTH       ( mc , B0mc  )
        mcK0S  = MCTRUTH       ( mc , K0Smc )
        mcPHI  = MCTRUTH       ( mc , PHImc )
        
        # cut on phi delta mass 
        dmPHI  = abs( DMASS("phi(1020)") ) <= 20 * MeV 
        
        phis  = self.loop( formula='K+ K-' , pid="phi(1020)" )
        for phi in phis :
            if phi.mass(1,2) > 1050 * MeV             : continue
            if VCHI2( phi )  > 100                    : continue
            if not dmPHI( phi )                       : continue
            phi.save("phi")

        # check selected phis 
        Phis = self.selected("phi") 
        if Phis.empty()    :  return SUCCESS                                    # RETURN

        # some general information
        evtHdr = self.evthdr_1()
        genHdr = self.genhdr_1()
        l0     = self.l0_1    ()
        l1     = self.l1_1    ()
        
        tuple = self.ntuple ( name = "B0 -> phi K0S analysis " )
        B0s   = self.loop( formula = "phi K0S" , pid = "B0")
        for B0 in B0s :
            if B0.mass(1,2)   > 6 * GeV          : continue
            if B0.mass(1,2)   < 4 * GeV          : continue
            if VCHI2( B0 )    > 49               : continue
            Phi = B0(1)
            K0S = B0(2)
            K1  = child( Phi , 1 ) 
            K2  = child( Phi , 2 ) 
            pi1 = child( K0S , 1 ) 
            pi2 = child( K0S , 2 ) 

            # find 'own' primary vertex
            vip = VIP( particle( B0 ) , self.geo() )
            primary = selectPrimaryVertexMin ( primaries , vip  )
            if None == primary                   : continue
            B0.setPV( primary )

            # general information 
            tuple.column  ( name = ""         , value = evtHdr           )
            tuple.column  ( name = "EvType"   , value = genHdr.evType () )
            tuple.column  ( name = "L0"       , value = l0.decision   () )
            tuple.column  ( name = "L0Type"   , value = l0.typeL0Trig () )
            tuple.column  ( name = "L1"       , value = l1.decision   () )

            # multiplicity of primary vertices 
            tuple.column  ( name = "nPVs"     , value = primaries.size() )

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
            
            ctau = VDTIME ( self.point( vertex = primary ) )
            tuple.column  ( name = "B0Ctau"   , value = ctau( B0 ) )

            dira = DIRA   ( self.point( vertex = primary ) )
            tuple.column  ( name = "B0DirA"   , value = dira( B0 ) )
            
            # vertex positions 
            tuple.column  ( name = "B0Vx"      , value = vertex( B0  ) .position() )
            tuple.column  ( name = "PhiVx"     , value = vertex( Phi ) .position() )
            tuple.column  ( name = "K0SVx"     , value = vertex( K0S ) .position() )
            tuple.column  ( name = "PVVx"      , value = primary       .position() )
            
            tuple.column  ( name = "B0P"       , value = B0  .momentum() ) ;
            tuple.column  ( name = "PhiP"      , value = Phi .momentum() ) ;
            tuple.column  ( name = "K0SP"      , value = K0S .momentum() ) ;
            
            tuple.column  ( name = "LV01"      , value = LV01( B0  ) ) ;
            tuple.column  ( name = "LV01K0"    , value = LV01( K0S ) ) ;

            tuple.column  ( name = "B0MC"      , value = mcB0  ( B0  ) ) ;
            tuple.column  ( name = "PHIMC"     , value = mcPHI ( Phi ) ) ;
            tuple.column  ( name = "K0SMC"     , value = mcK0S ( K0S ) ) ;
            
            tuple.write()
            B0.save("B0")

        # all selected B0
        B0S = self.selected("B0")
        if not B0S.empty() : self.setFilterPassed( TRUE )

        return SUCCESS

    
    
# create my algorithm 
myAlg = B2PhiK0S('B2PhiK0S')

# configure my algorithm 
prop1 = Property('B2PhiK0S')
# prop1.OutputLevel = 4
prop1.TupleLUN    = 'PHIK0S'
# confugure my desktop

# configure own desktop 
desktop                 = Property('B2PhiK0S.PhysDesktop')
desktop.InputLocations  = [ "/Event/Phys/Charged" ,
                            "/Event/Phys/LLKs"    ,
                            "/Event/Phys/UUKs"    ,
                            "/Event/Phys/LUKs"    ,
                            "/Event/Phys/LVKs"    ]
desktop.OutputLocation  =   "/Event/Phys/B2PhiK0S"

# initialize my algorithm 
myAlg.initialize()

# append my algorithm to the list of top level algorithms 
g.topAlg = g.topAlg + [ 'B2PhiK0S' ] 

# execute algorithm (main Gaudi event loop)
g.execute(1)

# finalize my algorithm
# myAlg.finalize()

# say: "Good Bye, everybody!"
# g.exit()

# =============================================================================
# The END 
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
