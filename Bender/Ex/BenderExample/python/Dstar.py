# $Id: Dstar.py,v 1.4 2004-02-10 12:37:34 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================
# @file
# Simple script to run D*+ -> ( D0 -> K- pi+ ) pi+ selection with Bender
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2003-12-04 
# =============================================================================

# import the Bender itself  
from   bendermodule import *
import benderconfig as bender 

g.JobOptionsType = 'NONE'
g.OutputLevel = 3
g.config()

# =============================================================================
# Specific physics analysis algorithm 
# =============================================================================

# create my own algorithm  
class Dstar(Algo):
    " My own analysis algorithm for selection of D*+ -> D0(K- pi+) pi+ "
    def analyse ( self ) :

        # select all primary vertices 
        PVs    = self.vselect( tag = "PVs" , cuts = VTYPE == VertexType.Primary )
        if PVs.empty() : return SUCCESS
        
        # minimal IP significance with respect to ALL primary vertices 
        mips    = MIPCHI2( PVs , self.geo() )
        
        # reconstructed particles
        pions = self.select( tag = "pi" , cuts = 211 == ABSID )
        kaons = self.select( tag = "K"  , cuts = 321 == ABSID )

        # Monte-Carlo truth 
        tup1    = self.ntuple ( title = "D0 Tuple" )
        dm      = ADMASS("D0") < 60 * MeV   # create Delta Mass cut
        
        # Loop over all (K pi)  combinations 
        for D0 in self.loop( formula = "K pi" , pid= "D0" ) :
            if not 1.7 * GeV < D0.mass(1,2) < 2.010 * GeV : continue
            if 0 != SUMQ( D0 )                            : continue  
            if VCHI2( D0 )  > 25                          : continue
            tup1.column( name = "mass"   , value = M(D0) / GeV   )
            tup1.column( name = "vchi2"  , value = VCHI2( D0 )   )
            tup1.column( name = "mipK"   , value = mips( D0(1) ) )
            tup1.column( name = "mippi"  , value = mips( D0(2) ) )
            tup1.column( name = "mipD0"  , value = mips( D0    ) )
            tup1.column( name = "id1"    , value = ID( D0(1)   ) ) 
            tup1.column( name = "id2"    , value = ID( D0(2)   ) )
            # write N-tuple 
            tup1.write() 
            # if not dm( D0 ) : continue  # apply Delta Mass cut 
            D0.save("D0")

        # get all saved D0 
        D0s = self.selected( "D0" )
        if D0s.empty() : return SUCCESS                            # RETURN
        
        tup2    = self.ntuple ( title = "D* Tuple" )
        for Dpi in self.loop( formula = "D0 pi" , pid='D*(2010)-' ) :            
            # use delta mass trick instead of mass-constrained fit
            if Dpi.mass(1,2) - Dpi.mass(1) > (170. * MeV) : continue
            if VCHI2( Dpi )  > 25  : continue
            # daughter particles of D0 
            k1 = child( Dpi , 1 , 1 )
            p1 = child( Dpi , 1 , 2 )
            #
            tup2.column ( name = "mass"  , value = M     ( Dpi    ) / GeV  )
            tup2.column ( name = "mD0"   , value = M     ( Dpi(1) ) / GeV  )
            tup2.column ( name = "vchi2" , value = VCHI2 ( Dpi    )        )
            # "pointing" to primary vertices 
            tup2.column ( name = "mipDs" , value = mips  ( Dpi    ) )
            tup2.column ( name = "mipD0" , value = mips  ( Dpi(1) ) )
            tup2.column ( name = "mipp2" , value = mips  ( Dpi(2) ) )
            tup2.column ( name = "mipk1" , value = mips  ( k1     ) )
            tup2.column ( name = "mipp1" , value = mips  ( p1     ) )
            # particle identifiers 
            tup2.column ( name = "idk1"  , value = ID    ( k1     ) ) 
            tup2.column ( name = "idp1"  , value = ID    ( p1     ) ) 
            tup2.column ( name = "idp2"  , value = ID    ( Dpi(2) ) ) 
            tup2.write()
            Dpi.save("D*+")
                          
        # get all saved D0 
        Dpi = self.selected( "D*+" )
        
        return SUCCESS 

# =============================================================================
# Generic job configuration 
# =============================================================================

bender.config( files   = [ '$DAVINCIROOT/options/DaVinci.opts' ] ,
               options = [ 'EcalPIDmu.OutputLevel     =   5  ' ,
                           'HcalPIDmu.OutputLevel     =   5  ' ,
                           'EcalPIDe.OutputLevel      =   5  ' ,
                           'HcalPIDe.OutputLevel      =   5  ' ,
                           'BremPIDe.OutputLevel      =   5  ' ,
                           'PrsPIDe.OutputLevel       =   5  ' ,
                           'EventSelector.PrintFreq   =  10  ' ] )

# define input data channel B0 -> ( D*- -> D0bar(K+ pi-) pi- ) pi+  
g.readOptions('/afs/cern.ch/lhcb/project/web/cards/415000.opts')

g.HistogramPersistency = "HBOOK" ;

# =============================================================================
# specific job configuration 
# =============================================================================

# preload algorithm(s)
g.topAlg += [ "LoKiPreLoad/Charged"]

# configure the preload algorithm(s) 
preload  = g.property('Charged')
preload.OutputLevel = 4

desktop                   = g.property('Charged.PhysDesktop')
desktop.ParticleMakerType = "CombinedParticleMaker"
desktop.OutputLocation    = "/Event/Phys/Charged"

maker                    = g.property('Charged.PhysDesktop.CombinedParticleMaker')
maker.ExclusiveSelection = FALSE
maker.Particles          = [ "kaon" , "pion" ]
maker.KaonSelection      = [ "det='RICH' k-pi='-5.0'   k-p='-5.0' "]
maker.PionSelection      = [ "det='RICH' pi-k='-5.0' "]

# create analysis algorithm and add it to the list of
alg = Dstar('Dstar')

g.topAlg += [ 'Dstar' ]

alg = gaudi.iProperty('Dstar')
alg.OutputLevel = 5
alg.TupleLUN    = 'DSTAR'

desktop                 = g.property('Dstar.PhysDesktop')
desktop.InputLocations  = [ "/Event/Phys/Charged"]
desktop.OutputLocation  =   "/Event/Phys/Dstar"

# output histogram file 
hsvc = g.property( 'HistogramPersistencySvc' )
hsvc.OutputFile = 'dstar.hbook'

nsvc = gaudi.iProperty( 'NTupleSvc' )
nsvc.Output =[ "DSTAR DATAFILE='dstar_tup.hbook' TYP='HBOOK' OPT='NEW'" ]

# =============================================================================
# job execution 
# =============================================================================


g.initialize()


## g.run(100)  ## crash !!
g._evtpro.executeRun(100)

g.exit()

# =============================================================================
# The END 
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.3  2004/01/24 23:38:51  ibelyaev
#  update and fixes
#
# =============================================================================
