#!/usr/bin/env python
# =============================================================================
# $Id: Bs2PhiPhi_Tags.py,v 1.1 2004-08-06 12:12:03 ibelyaev Exp $
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
# Specific physics analysis algorithm 
# =============================================================================

class Bs2PhiPhiTags(Algo):
    """
    Simple algorithm to write Event Tag Collections for Bs -> Phi Phi 
    """
    def analyse ( self ) :
        " The 'main' analyse method "
        # reset filter
        self.setFilterPassed ( FALSE )
        
        PV  = self.vselect ( tag = "PVs" , cuts = VertexType.Primary == VTYPE ) 
        Bs  = self.select  ( tag = 'Bs'  , cuts = 531         == ABSID        ) 
        Phi = self.select  ( tag = 'phi' , cuts = 'phi(1020)' ==    ID        )
        
        evtcol = self.evtCol( title = "MyCol" )
        
        iop    = self.get( address = '/Event')
        iop    = iop.registry()
        iop    = iop.address()
        evtcol.column( name = "Address" , value = iop )

        evt    = self.get( address = '/Event/Header')
        evtcol.column ( name = "evtNum"  , value = evt.evtNum() )
        evtcol.column ( name = "runNum"  , value = evt.runNum() )
        
        evtcol.column ( name = "nBs"     , value = Bs.size()    ) 
        evtcol.column ( name = "nPhi"    , value = Phi.size()   )
        
        self.setFilterPassed( TRUE )
            
        return SUCCESS 

# =============================================================================
# job configuration
# =============================================================================
def configure () :
    # Generic job configuration & input data 
    
    bender.config( files   =
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
    g.topAlg += ['LoKiPreLoad/Hadrons']
    preload.Hadrons( Particles = [ 'kaon' , 'pion'] )
    
    # create analysis algorithm and add it to the list of
    bs = Bs2PhiPhi('Bs2PhiPhi')
    
    g.topAlg += [ 'Bs2PhiPhi' ]
    
    bs = gaudi.iProperty('Bs2PhiPhi')
    bs.OutputLevel = 5
    bs.NTupleLUN  = "PHIPHI"
    desktop                 = g.property('Bs2PhiPhi.PhysDesktop')
    desktop.InputLocations  = [ "/Event/Phys/Hadrons"]
    
    a2 = Bs2PhiPhiTags('TagCreator')
    g.OutStream += [ 'Sequencer/SeqEvtTags']
    
    seq = gaudi.iProperty('SeqEvtTags')
    seq.Members = [ 'TagCreator' , 'EvtCollectionStream/TagsWriter' ]
    tag1 = gaudi.iProperty('TagCreator')
    tagd = gaudi.iProperty('TagCreator.PhysDesktop')
    tagd.InputLocations = [ '/Event/Phys/Bs2PhiPhi' ]
    tag1.EvtColLUN      = 'EVTTAGS' ;
    tag1.EvtColsProduce = TRUE 
    tag1.EvtColSplitDir = FALSE
    
    tagw = gaudi.iProperty('TagsWriter')
    tagw.ItemList    = [ '/NTUPLES/EVTTAGS/TagCreator/1' ]
    tagw.EvtDataSvc  =   'EvtTupleSvc' ;
    
    # output histogram file 
    hsvc = g.property( 'HistogramPersistencySvc' )
    hsvc.OutputFile = 'phi.hbook'
    # ntuples 
    nsvc = gaudi.iProperty( 'NTupleSvc' )
    nsvc.Output = [ "PHIPHI  DATAFILE='bs2phiphi_tup.hbook' TYP='HBOOK' OPT='NEW'" ]
    # event collections
    tsvc = gaudi.iProperty('EvtTupleSvc')
    tsvc.Output =  [ "EVTTAGS DATAFILE='PFN:EventTags.tags' TYP='POOL_ROOTTREE' OPT='RECREAT' " ]
    
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
