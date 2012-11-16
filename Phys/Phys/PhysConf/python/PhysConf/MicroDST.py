# =============================================================================
"""
Helper module to deal with uDST
"""
# =============================================================================
## configure variopsi uDst settings from RootInTES 
def uDstConf ( rootInTes ) :
    """
    Various settings for mDst
    
    """
    # Never use gfal ...
    from Configurables import Gaudi__IODataManager as IODataManager 
    IODataManager("IODataManager").UseGFAL = False
    #from Configurables import Gaudi__RootCnvSvc    as RootCnvSvc 
    #RootCnvSvc    ( "RootCnvSvc"  )

    #
    ## pick up all goodies from Juan
    #
    import MicroDSTConf.TriggerConfUtils as TCU

    #
    ## 'rootify'
    #
    if 0 != rootInTes.find ( '/Event/') : rootInTes = '/Event/' + rootInTes
    rootInTes = TCU.fixTrunk ( rootInTes )

    #
    ## configure L0,Hlt,e tc using Juan's functions:
    #
    TCU.configureL0AndHltDecoding ( rootInTes )

    from Configurables import DataOnDemandSvc
    dod = DataOnDemandSvc( Dump = True )
    #
    ## extra configuration:
    #   it is more relevant for Stripping-17 data,
    #   however it is backard compatible with other uDsts 
    #

    #
    ## 0. Kill Event/DAQ and Event/pRec
    #
    def killMissingNodes ()  : 
        from Configurables import EventNodeKiller
        killer = EventNodeKiller ( "KillMissingNodes" )
        killer.Nodes += [ '/Event/DAQ', '/Event/pRec', 'Event/Link' ]
        try : 
            from Gaudi.Configuration import getConfigurable
            conf   = getConfigurable ( 'DaVinciEventInitSeq' ) 
            conf.Members.insert ( 0 , killer ) 
        except:
            print "Failed to add killer agent for", killer.Nodes
            
    from Gaudi.Configuration import appendPostConfigAction
    appendPostConfigAction ( killMissingNodes )
 
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
    
    #
    ## CRJ No longer needed.
    #
    #from Configurables import OdinTimeDecoder, ODINDecodeTool
    #odinDec = OdinTimeDecoder ( )
    #odinDec.addTool( ODINDecodeTool, 'ODINDecodeTool' )
    #odinDec.ODINDecodeTool.RawEventLocations = [
    #    rootInTes + 'DAQ/RawEvent' ] + odinDec.ODINDecodeTool.RawEventLocations
    
    from Configurables import DecodePileUpData
    pileupDec = DecodePileUpData()
    pileupDec.RawEventLocation = rootInTes + 'DAQ/RawEvent' 
    
# =============================================================================
# The END 
# =============================================================================
