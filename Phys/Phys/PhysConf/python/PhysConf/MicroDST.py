# $Id:$
## =============================================================================
# @file PhysConf/MicroDST.py
#
# Helper module to deal with uDST
#
# The module is copied from Phys/BenderTools package
#
# @author VanyaBELYAEV Ivan.Belyaev@itep.ru
# @date   2011-10-05
#
#                    $Revision: 152811 $
#  Last modification $Date: 2013-02-23 15:05:41 +0100 (Sat, 23 Feb 2013) $
#                 by $Author: ibelyaev $ 
# =============================================================================
"""
Helper module to deal with uDST, mainly TisTos

The module is copied from Phys/BenderTools package
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-10-05"
__version__ = "$Revision: 152811 $"
__all__     = (
    'uDstConf'    ,
    'uDstConf13b'    ## version for Stripping13b processing
    )
# =============================================================================
from Gaudi.Configuration import log 
# =============================================================================
## Configure node killing agents for uDST processing
#  @author VanyaBELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-10-05
def killDAQ ( nodes = [ '/Event/DAQ' , '/Event/pRec' ] , logger = None )  :
    """
    Configure node killing agents for uDST processing
    """
    #
    if not logger : logger = log
    #
    from Configurables import EventNodeKiller
    killer = EventNodeKiller ( "KillDAQ" )
    killer.Nodes += nodes 
    try : 
        from Gaudi.Configuration import getConfigurable
        conf   = getConfigurable ( 'DaVinciEventInitSeq' ) 
        conf.Members.insert ( 0 , killer ) 
        logger.info  ( "Add           killer agent for: %s" % killer.Nodes )
    except:
        logger.error ( "Failed to add killer agent for: %s" % killer.Nodes )

# ======================================================================================
## configure various uDst settings from RootInTES
#
#  @code
#
#  from PhysConf.MicroDST import uDstConf
#  uDstConf ( '/Event/Leptonic' )
#
#  @endcode 
#
#  @author VanyaBELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-10-05  
def uDstConf ( rootInTes        ,
               killNodes = [ '/Event/DAQ'    , '/Event/pRec' ] ,
               logger    = None )  :
    """
    Various settings for mDst
    
    #  from PhysConf.MicroDST import uDstConf
    #  uDstConf ( '/Event/Leptonic' )
    #
    """
    #
    if not logger : logger = log
    #
    #
    ## Never use gfal ...
    #
    from Configurables import Gaudi__IODataManager as IODataManager 
    IODataManager( "IODataManager" ).UseGFAL = False
    logger.info  ( "Disable GFAL"  )

    #
    ## pick up all goodies from Juan
    #
    import MicroDSTConf.TriggerConfUtils as TCU
    #
    ## 'rootify'
    #
    if 0 != rootInTes.find ( '/Event/') : rootInTes = '/Event/' + rootInTes
    rootInTes = TCU.fixTrunk ( rootInTes )

    logger.info  ( "Reconfigure uDST to use    RootInTES='%s'" % rootInTes )
    #
    ## configure L0,Hlt,e tc using Juan's functions:
    #
    TCU.configureL0AndHltDecoding ( rootInTes )
    logger.info  ( "Configure L0&HLT decoding  RootInTES='%s'" % rootInTes )
    
    from Configurables import DataOnDemandSvc
    dod = DataOnDemandSvc( Dump = True )
    
    #
    ## extra configuration:
    #
    #   it is more relevant for Stripping-17 data,
    #   however it is backard compatible with other uDsts 
    #
    
    #
    ## 0. Kill Event/DAQ
    #
    from Gaudi.Configuration import appendPostConfigAction
    def _killDAQ_ ( ) :
        return  killDAQ ( nodes = killNodes , logger = logger )     
    appendPostConfigAction ( _killDAQ_ )
    logger.info  ( "Configure DAQ-killer agent for %s " % killNodes )
    
    #
    ## 1. Copy/Link ODIN, RawEvent and RecSummary
    #
    from Configurables import Gaudi__DataLink as Link
    rawEvt  = Link ( 'LinkRawEvent'                      ,
                     What   =  '/Event/Trigger/RawEvent' ,
                     Target = rootInTes + 'DAQ/RawEvent' )
    odin    = Link ( 'LinkODIN'                          ,
                     What   =          '/Event/DAQ/ODIN' ,
                     Target = rootInTes +     'DAQ/ODIN' )
    summary = Link ( 'LinkSummary'                       ,
                     What   =      '/Event/Rec/Summary'  ,
                     Target = rootInTes + 'Rec/Summary'  )    
    header  = Link ( 'LinkHeader'                        ,
                     What   =       '/Event/Rec/Header'  ,
                     Target = rootInTes +  'Rec/Header'  )
    reports = Link ( 'LinkStripReports',
                     What =        '/Event/Strip/Phys/DecReports' ,
                     Target = rootInTes + 'Strip/Phys/DecReports' )
    
    dod.AlgMap [ rawEvt  . Target ] = rawEvt 
    dod.AlgMap [ odin    . Target ] = odin
    dod.AlgMap [ summary . Target ] = summary
    dod.AlgMap [ header  . Target ] = header
    dod.AlgMap [ reports . Target ] = reports
    
    logger.info  ( "Configure ODIN,Raw,Summary&Reports RootInTES='%s'" % rootInTes )
    
    from Configurables import DecodePileUpData
    pileupDec = DecodePileUpData()
    pileupDec.RawEventLocation = rootInTes + 'DAQ/RawEvent' 


# ======================================================================================
## configure various uDst settings from RootInTES (Stripping-13b version)
#
#  @code
#
#  from PhysConf.MicroDST import uDstConf
#  uDstConf ( '/Event/Leptonic' )
#
#  @endcode 
#
#  @author VanyaBELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-10-05  
def uDstConf13b ( rootInTes , logger = None )  :
    """
    Various settings for mDst (stripping 13b version)
    
    #  from PhysConf.MicroDST import uDstConf13b as uDstConf 
    #  uDstConf ( '/Event/Leptonic' )
    #
    """
    # =========================================================================
    return uDstConf ( rootInTes , [ '/Event/pRec' ] , logger ) 
    
# =============================================================================
if __name__ == '__main__' :
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120

# =============================================================================
# The END 
# =============================================================================
