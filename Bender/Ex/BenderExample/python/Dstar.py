# $Id: Dstar.py,v 1.1 2003-12-07 16:14:30 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================
# @file
# Simple script to run D*+ -> ( D0 -> K- pi+ ) pi+ selection with Bender
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2003-12-04 
# =============================================================================

# import the bender itself  
from BenderModule import *

# create my own algorithm  
class Dstar(Bender):
    " My own analysis algorithm for selection of D*+ -> D0(K- pi+) pi+ "
    def analyse ( self ) :

        # select all primary vertices 
        PVs    = self.vselect( tag = "PVs" , cuts = VTYPE == VertexType.Primary )
        if PVs.empty() : return SUCCESS
        
        # minimal IP significance with respect to ALL primary vertices 
        mips    = MIPCHI2( PVs , self.geo() )
        
        # reconstructed particles
        charged = self.select( tag = "C" , cuts = ( 0 != Q ) )
        pion    = 211 == abs( ID )
        kaon    = 321 == abs( ID )
        hadrons = self.select( tag    ="H"      ,
                               source = charged ,
                               cuts   = pion | kaon )
        kaons   = self.select( tag = "K"  , source = hadrons , cuts = kaon )
        pions   = self.select( tag = "pi" , source = hadrons , cuts = pion )
        
        # some MC truth information
        mc     = self.mctruth()
        # lists of all true B0, true K0S and true Phi  
        D0mc   = mc.findDecays ( decay = " [D0 -> K- pi+ ]cc" )  
        mcD0   = MCTRUTH       ( mc , D0mc   )
        Dstmc  = mc.findDecays ( decay = " [D*(2010)+ -> D0 pi+ ]cc" )  
        # MC cuts for true D0 and truth D* 
        mcD0   = MCTRUTH       ( mc , D0mc   )
        mcDst  = MCTRUTH       ( mc , Dstmc  )  
        
        d0s     = self.loop( formula = "K pi" , pid= "D0" )
        tup1    = self.ntuple ( name = "D0 ntuple" )
        dm      = DMASS("D0") < 60 * MeV   # create Delta Mass cut 
        for D0 in d0s:
            if 0 != Q( D0(1) ) + Q(D0(2) ) : continue 
            if D0.mass(1,2) > 2.010 * GeV  : continue  
            if D0.mass(1,2) < 1.700 * GeV  : continue
            if VCHI2( D0 )  > 25           : continue
            tup1.column( name = "mass"   , value = M(D0) / GeV   )
            tup1.column( name = "vchi2"  , value = VCHI2( D0 )   )
            tup1.column( name = "mipK"   , value = mips( D0(1) ) )
            tup1.column( name = "mippi"  , value = mips( D0(2) ) )
            tup1.column( name = "mipD0"  , value = mips( D0    ) )
            tup1.column( name = "id1"    , value = ID( D0(1)   ) ) 
            tup1.column( name = "id2"    , value = ID( D0(2)   ) )
            # MC truth flag 
            tup1.column( name = "mcD0"   , value = mcD0( D0    ) )
            # write N-tuple 
            tup1.write() 
            if not dm( D0 ) : continue  # apply Delta Mass cut 
            D0.save("D0")

        # get all saved D0 
        D0s = self.selected( "D0" )
        
        tup2   = self.ntuple ( name = "D* ntuple" )
        dstars = self.loop   ( formula =  "D0 pi" , pid = "D*(2010)-" )
        for dstar in dstars :
            # use delta mass trick instead of mass-constrained fit
            if dstar.mass(1,2) - dstar.mass(1) > 170 * MeV  : continue
            if VCHI2( dstar )  > 25  : continue
            # daughter particles of D0 
            k1 = child( dstar , 1 , 1 )
            p1 = child( dstar , 1 , 2 )
            #
            tup2.column ( name = "mass"  , value = M    ( dstar    ) / GeV  )
            tup2.column ( name = "mD0"   , value = M    ( dstar(1) ) / GeV  )
            tup2.column ( name = "vchi2" , value = VCHI2 ( dstar ) )
            # "pointing" to primary vertices 
            tup2.column ( name = "mipDs" , value = mips ( dstar    ) )
            tup2.column ( name = "mipD0" , value = mips ( dstar(1) ) )
            tup2.column ( name = "mipp2" , value = mips ( dstar(2) ) )
            tup2.column ( name = "mipk1" , value = mips  ( k1 ) )
            tup2.column ( name = "mipp1" , value = mips  ( p1 ) )
            # particle identifiers 
            tup2.column ( name = "idk1"  , value = ID( k1  ) ) 
            tup2.column ( name = "idp1"  , value = ID( p1  ) ) 
            tup2.column ( name = "idp2"  , value = ID( D0(2) ) ) 
            # MC truth flag for D0
            tup2.column ( name = "mcD0"  , value = mcD0  ( dstar ( 1 )  ) )            
            # MC truth flag for D*
            tup2.column ( name = "mcDst" , value = mcDst ( dstar ) )
            # write N-tuple
            tup2.write()
            dstar.save("D*-")
            
        # get all saved D*
        dstars = self.selected( "D*-" )
        if not dstars.empty() : self.setFilterPassed( 1 < 2 )
            
        return SUCCESS 

#    create my algorithm 
myAlg = Dstar('Dstar')

#def property( tag ):   # temporary trick!
#    return Service(tag)

# configure my algorithm 
p1 = Property('Dstar')
p1.OutputLevel = 4       # printout level 
p1.TupleLUN    = 'DSTAR' # logical name for N-Tuple file, see NTupleSvc.Output

# configure my desktop 
desktop                 = Property('Dstar.PhysDesktop')
desktop.InputLocations  = [ "/Event/Phys/Charged" ]
desktop.OutputLocation  =   "/Event/Phys/Dstar"

# initialize the algorithms
myAlg.initialize()

# append my algorithm to the list of top level algorithms
g.topAlg = g.topAlg + [ 'Dstar' ] 

# execute 1 event 
g.execute(1)

#myAlg.finalize()
#g.exit()

# =============================================================================
# The END 
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
