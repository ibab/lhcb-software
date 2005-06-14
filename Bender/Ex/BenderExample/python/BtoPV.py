#!/usr/bin/env python
# =============================================================================
# $Id: BtoPV.py,v 1.1 2005-06-14 16:17:19 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1 $
# =============================================================================
# @file 
# "Demo" algorithm for B -> PV association 
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2005-06-06
# =============================================================================

# import the Bender itself  
from   bendermodule  import *

import benderPreLoad as preload

# =============================================================================
# Specific physics analysis algorithm 
# =============================================================================

class BtoPV(Algo):
    """
    Simple algorithm for B -> PV associations 
    """
    def analyse ( self ) :
        " The 'main' analyse method "
        # reset filter
        self.setFilterPassed ( FALSE )
        # get all primary vertices
        prims = self.vselect( tag = 'PVs' , cuts = VertexType.Primary == VTYPE )
        if prims.empty() :
            return self.Error ( message = "N Promary vertices found!" , code = SUCCESS )
        
        piplus  = self.select( tag = "pi+" , cuts = 'pi+' == ID ) ;
        piminus = self.select( tag = "pi-" , cuts = 'pi-' == ID ) ;
        
        bs   = self.loop   ( formula = "pi+ pi+ pi- pi-" , pid = "B_s0" )
        for b in bs :
            m12 = b.mass(1,2,3,4)
            if m12 > 6 * GeV : continue
            if m12 < 2 * GeV : continue
            chi2 = VCHI2 ( b )
            if chi2 < 0      : continue
            if chi2 > 10000  : continue
            
            b.save( "Bs" ) ;
            for pv in prims : self.asctPVsIPCHI2( b.particle() , pv )

        bs = self.selected('Bs')
        for b in bs :
            pvs = self.PVs( b )
            print 'Number of associated primary vertices: ', pvs.size()
            for pv in pvs :
                v = pv.to_     ()
                w = pv.weight_ () 
                print v.position() , w
                
        return SUCCESS 


# =============================================================================
# job configuration
# =============================================================================
def configure () :
    # Generic job configuration & input data 
    gaudi.config( files   =
                  [ '$BENDEREXAMPLEOPTS/BenderExample.opts'        , # general options 
                    '$DAVINCIROOT/options/DaVinciTestData.opts'  ] , # input data 
                  options =
                  [ 'EcalPIDmu.OutputLevel     =   5  ' ,
                    'HcalPIDmu.OutputLevel     =   5  ' ,
                    'EcalPIDe.OutputLevel      =   5  ' ,
                    'NTupleSvc.OutputLevel     =   2  ' ,
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
    bs = BtoPV('BtoPV')
    gaudi.addAlgorithm( bs ) 
    
    #N-Tuple configuration 
    nsvc = gaudi.service ('NTupleSvc')
    nsvc.Output = ["B2PV DATAFILE='b2pv.hbook' OPT='NEW' TYP='HBOOK'"]
    
    # output histogram file 
    hsvc = gaudi.histoSvc()
    hsvc.setOutput( 'b2pv_his.hbook' , 'HBOOK')
    
    
    bs = gaudi.algorithm('BtoPV')
    bs.OutputLevel = 5
    bs.NTupleLUN  = "B2PV"
    
    desktop = gaudi.tool('BtoPV.PhysDesktop')
    desktop.InputLocations  = [ "/Event/Phys/Hadrons"]
    
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
# Revision 1.4  2005/01/24 17:33:00  ibelyaev
#  update for v4r5
#
# =============================================================================
# The END 
# =============================================================================
