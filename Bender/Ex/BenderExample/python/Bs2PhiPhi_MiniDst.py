#!/usr/bin/env python
# =============================================================================
# $Id: Bs2PhiPhi_MiniDst.py,v 1.1 2004-08-06 12:12:03 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $
# =============================================================================
# @file 
# "Demo" algorithm for Bs->phi phi stripping using Event Tag Collection
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2003-12-04
# =============================================================================

# import the Bender itself  
from   bendermodule  import *
import benderconfig  as     bender 
import benderPreLoad as     preload
from   Bs2PhiPhi     import Bs2PhiPhi

# =============================================================================
# job configuration
# =============================================================================
def configure () :
    # Generic job configuration & input data 
    
    bender.config( files   =
                   [ '$BENDEREXAMPLEOPTS/BenderExample.opts' ,     # general options 
                     '$BENDEREXAMPLEOPTS/PoolCatalogs.opts'  ,     # pool catalogs
                     '$DAVINCIROOT/options/DaVinciMiniDst.opts'  , # dst content
                     '$LOKIEXAMPLEOPTS/Bs_phiphi_DC04.opts'  ]   , # input data 
                   options =
                   [ 'EcalPIDmu.OutputLevel     =   5  ' ,
                     'HcalPIDmu.OutputLevel     =   5  ' ,
                     'EcalPIDe.OutputLevel      =   5  ' ,
                     'HcalPIDe.OutputLevel      =   5  ' ,
                     'BremPIDe.OutputLevel      =   5  ' ,
                     'PrsPIDe.OutputLevel       =   5  ' ,
                     'NeutralPP2MC.OutputLevel  =   5  ' ,
                     'Hadrons.OutputLevel       =   5  ' ,
                     'MiniDstWriter.OptItemList += {"/Event/Rec/ProtoParticle2MC#*" }      ' ,
                     'MiniDstWriter.OptItemList += {"/Event/Rec/NeutralPPs2MCParticles#*"} ' ,
                     'EventSelector.PrintFreq   = 100  ' ] )
    
    # specific job configuration 
    # preload algorithm(s)
    g.topAlg += ['LoKiPreLoad/Hadrons']
    preload.Hadrons( Particles = [ 'kaon' , 'pion'] )
    
    # create analysis algorithm and add it to the list of
    bs = Bs2PhiPhi('Bs2PhiPhi')
    
    #g.topAlg += [ 'Bs2PhiPhi' ]
    
    bs = gaudi.iProperty('Bs2PhiPhi')
    bs.OutputLevel = 5
    bs.NTupleLUN  = "PHIPHI"
    desktop                 = g.property('Bs2PhiPhi.PhysDesktop')
    desktop.InputLocations  = [ "/Event/Phys/Hadrons"]
    
    # output histogram file 
    hsvc = g.property( 'HistogramPersistencySvc' )
    hsvc.OutputFile = 'phi_writedst.hbook'
    # ntuples 
    nsvc = gaudi.iProperty( 'NTupleSvc' )
    nsvc.Output = [ "PHIPHI  DATAFILE='bs2phiphi_writedst_tup.hbook' TYP='HBOOK' OPT='NEW'" ]

    # output stream configuration 
    g.OutStream     = [ 'Sequencer/DstSeq' ]
    seq = gaudi.iProperty('DstSeq')
    seq.Members     = [ 'Bs2PhiPhi' , 'OutputStream/MiniDstWriter' ]
    dst = gaudi.iProperty( 'MiniDstWriter' )
    dst.Preload     = FALSE
    dst.Output      = `"DATAFILE='PFN:Bs2PhiPhi.mdst' TYP='POOL_ROOTTREE' OPT='CREATE'"` ;

    return SUCCESS 
    
# =============================================================================
# job execution 
# =============================================================================

if __name__ == '__main__' :
    import sys 
    # analyse the options
    nEvents = bender.getNEvents( sys.argv[1:] )
    if not nEvents : nEvents = 10000 
    # configure the job
    configure() 
    # run job 
    g.run  ( nEvents )
    # terminate Application Manager 
    g.exit ()
    
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
