#!/usr/bin/env python
# =============================================================================
# $Id: Bs2PhiPhi.py,v 1.2 2004-11-12 14:24:42 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $
# =============================================================================
# @file 
# "Demo" algorithm for Bs->phi phi stripping 
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2003-12-04
# =============================================================================

# import the Bender itself  
from   bendermodule  import *

import benderPreLoad as preload

# =============================================================================
# Specific physics analysis algorithm 
# =============================================================================

class Bs2PhiPhi(Algo):
    """
    Simple algorithm for Bs -> Phi Phi stripping
    """
    def analyse ( self ) :
        " The 'main' analyse method "
        # reset filter
        self.setFilterPassed ( FALSE )
        # get all primary vertices
        prims = self.vselect( tag = 'PVs' , cuts = VertexType.Primary == VTYPE )
        if prims.empty() :
            return self.Error ( message = "N Promary vertices found!" , code = SUCCESS )
        
        kplus  = self.select( tag = "K+" , cuts = -321 == ID ) ;
        kminus = self.select( tag = "K-" , cuts = 321 == ID ) ;
        
        tuple1 = self.nTuple ( title   = "Phi" ) ;
        phis   = self.loop   ( formula = "K+ K-" , pid = "phi(1020)" )
        for phi in phis :
            if phi.mass(1,2) > ( 1020 + 40 ) * MeV : continue
            tuple1.column ( name = 'mass' , value = M     ( phi ) ) 
            tuple1.column ( name = 'chi2' , value = VCHI2 ( phi ) )
            tuple1.column ( name = 'p'    , value = P     ( phi ) )
            tuple1.column ( name = 'pt'   , value = PT    ( phi ) )
            tuple1.column ( name = 'lv01' , value = LV01  ( phi ) )
            tuple1.write  () 
            phi.save( "phi" ) ;

        tuple2 = self.nTuple ( title = "Bs" ) ;
        BSs    = self.loop   ( formula = "phi phi" , pid = "B_s0" )
        for Bs in BSs :
            mass = Bs.mass(1,2)
            if mass < 4.0 * GeV        : continue
            if mass > 7.0 * GeV        : continue
            chi2 = VCHI2( Bs  )
            if chi2 <   0              : continue
            if chi2 > 100              : continue
            tuple2.column ( name = "mass" , value = M    ( Bs ) / GeV )
            tuple2.column ( name = "p"    , value = P    ( Bs ) / GeV )
            tuple2.column ( name = "pt"   , value = PT   ( Bs ) / GeV )
            tuple2.column ( name = "lv01" , value = LV01 ( Bs )       )
            tuple2.column ( name = "chi2" , value = chi2              )
            tuple2.write  ()
            Bs.save( "Bs" )

        BS = self.selected( "Bs" ) ;
        if not BS.empty() : self.setFilterPassed( TRUE )
            
        return SUCCESS 


# =============================================================================
# job configuration
# =============================================================================
def configure () :
    # Generic job configuration & input data 
    gaudi.config( files   =
                  [ '$BENDEREXAMPLEOPTS/BenderExample.opts' ,   # general options 
                    '$BENDEREXAMPLEOPTS/PoolCatalogs.opts'  ,   # pool catalogs
                    '$LOKIEXAMPLEOPTS/Bs_phiphi_DC04.opts'  ] , # input data 
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
    gaudi.addAlgorithm( 'LoKiPreLoad/Hadrons' ) 
    preload.Hadrons( Particles = [ 'kaon' , 'pion'] )
    
    # create analysis algorithm and add it to the list of
    bs = Bs2PhiPhi('Bs2PhiPhi')
    gaudi.addAlgorithm( bs ) 
    
    bs = gaudi.algorithm('Bs2PhiPhi')
    bs.OutputLevel = 5
    bs.NTupleLUN  = "PHIPHI"
    
    desktop = gaudi.tool('Bs2PhiPhi.PhysDesktop')
    desktop.InputLocations  = [ "/Event/Phys/Hadrons"]
    
    # output histogram file 
    hsvc = gaudi.service( 'HistogramPersistencySvc' )
    hsvc.OutputFile = 'phi.hbook'
    
    nsvc = gaudimodule.iProperty( 'NTupleSvc' )
    nsvc.Output += [ "PHIPHI DATAFILE='bs2phiphi_tup.hbook' TYP='HBOOK' OPT='NEW'" ]

    return SUCCESS

# =============================================================================
# job execution 
# =============================================================================
if __name__ == '__main__' :
    import sys 
    # configure the job 
    configure()
    # execute 
    gaudi.run( 100  )
    # terminate 
    gaudi.exit()
  
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
