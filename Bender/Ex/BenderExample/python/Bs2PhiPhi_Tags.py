#!/usr/bin/env python
# =============================================================================
# $Id: Bs2PhiPhi_Tags.py,v 1.4 2004-11-25 14:55:05 ibelyaev Exp $
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

        print ' i am here!!! ' 
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
                    'HcalPIDe.OutputLevel      =   5  ' ,
                    'EvtTupleSvc.OutputLevel   =   1  ' ,
                    'NTupleSvc.OutputLevel     =   1  ' ,
                    'BremPIDe.OutputLevel      =   5  ' ,
                    'PrsPIDe.OutputLevel       =   5  ' ,
                    'NeutralPP2MC.OutputLevel  =   5  ' ,
                    'Hadrons.OutputLevel       =   5  ' ,
                    'EventSelector.PrintFreq   = 100  ' ] )

    gaudi.HistogramPersistency = "ROOT"

    
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
    
    a2 = Bs2PhiPhiTags('TagCreator')
    gaudi.OutStream = [ 'Sequencer/SeqEvtTags']
    
    seq = gaudi.algorithm('SeqEvtTags')
    seq.Members = [ 'TagCreator' , 'EvtCollectionStream/TagsWriter' ]
    tag1 = gaudi.algorithm('TagCreator')
    tagd = gaudi.tool('TagCreator.PhysDesktop')
    tagd.InputLocations = [ '/Event/Phys/Bs2PhiPhi' ]
    tag1.EvtColLUN      = 'EVTTAGS' ;
    tag1.EvtColsProduce = TRUE 
    tag1.EvtColSplitDir = FALSE
    
    tagw = gaudi.algorithm('TagsWriter')
    tagw.ItemList    = [ '/NTUPLES/EVTTAGS/TagCreator/1' ]
    tagw.EvtDataSvc  =   'EvtTupleSvc' ;
    
    # ntuples 
    nsvc = gaudi.nTupleSvc()
    nsvc.defineOutput( { 'PHIPHI' : 'bs2phiphi_tup.hbook' } , 'HBOOK' ) 
    
    # event collections
    tsvc = gaudi.evtcolsvc()
    tsvc.defineOutput( { 'EVTTAGS' : 'EventTags.tags' } , 'POOL_ROOTTREE' )
    
    return SUCCESS 
    
# =============================================================================
# job execution 
# =============================================================================

if __name__ == '__main__' :
    # configure the job
    configure() 
    # run job 
    gaudi.run  ( 100 )
    # terminate Application Manager 
    gaudi.exit ()
    
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.3  2004/11/14 16:56:49  ibelyaev
# *** empty log message ***
#
# Revision 1.2  2004/11/12 14:24:42  ibelyaev
#  v4r2
#
# =============================================================================
# The END 
# =============================================================================
