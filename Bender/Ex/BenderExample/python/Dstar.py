#!/usr/bin/env python
# =============================================================================
# $Id: Dstar.py,v 1.14 2006-06-07 09:22:12 ibelyaev Exp $
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

import benderPreLoad as preload

global h1 
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
        tup1    = self.nTuple ( title = "D0 Tuple" )
        dm      = ADMASS("D0") < 30 * MeV   # create Delta Mass cut
        
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
        
        tup2    = self.nTuple ( title = "D* Tuple" )
        for Dpi in self.loop( formula = "D0 pi" , pid='D*(2010)-' ) :            
            # use delta mass trick instead of mass-constrained fit
            dm = Dpi.mass(1,2) - Dpi.mass(1)
            if dm > (170. * MeV) : continue
            if VCHI2( Dpi )  > 25  : continue
            #
            global h1 
            h1 = self.plot( title = ' m(D0pi+) - m(D0) ' , value = dm ,
                            low =  120 * MeV , high = 170 * MeV )   
            # daughter particles of D0 
            k1 = child( Dpi , 1 , 1 )
            p1 = child( Dpi , 1 , 2 )
            #if not k1 : continue
            #if not p1 : continue 
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
                          
        # get all saved D*+
        Dpi = self.selected( "D*+" )
        
        return SUCCESS 

def configure() :
    # Generic job configuration 
    gaudi.config( files   =
                  [ '$BENDEREXAMPLEOPTS/BenderExample.opts'     ,  # common options
                    '$STDOPTS/Hbook.opts'                       ,
                    '$DAVINCIROOT/options/DaVinciTestData.opts' ] ) 
    
    # specific job configuration 
    # preload algorithm(s)
    gaudi.addAlgorithm( 'LoKiPreLoad/Hadrons' ) 
    preload.Hadrons( Particles = [ 'kaon' , 'pion'] )
    
    # create analysis algorithm and add it to the list of
    alg = Dstar( 'Dstar' )
    gaudi.addAlgorithm( alg )
    
    alg = gaudi.algorithm('Dstar')
    alg.OutputLevel = 5
    alg.NTupleLUN = 'DSTAR'
    alg.OutputLocation = '/Event/Phys/Dstar2HH'
    
    desktop = gaudi.tool('Dstar.PhysDesktop')
    desktop.InputLocations  = [ "/Event/Phys/Hadrons"]
    
    # output histogram file 
    hsvc = gaudi.service( 'HistogramPersistencySvc' )
    hsvc.OutputFile = 'dstar.hbook'
    
    nsvc = gaudimodule.iProperty( 'NTupleSvc' )
    gaudi.HistogramPersistency = "HBOOK" 
    #nsvc = gaudi.service( 'NTupleSvc' )
    nsvc.Output =[ "DSTAR DATAFILE='dstar_tup.hbook' TYP='HBOOK' OPT='NEW'" ]
    

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
    
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
