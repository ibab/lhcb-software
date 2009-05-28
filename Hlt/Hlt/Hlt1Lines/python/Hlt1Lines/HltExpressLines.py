from HltLine.HltLinesConfigurableUser import *
from HltLine.HltLine import Hlt1Line as Line
#
# Define lines that define the XPress stream
#
class HltExpressLinesConf(HltLinesConfigurableUser) :
    # steering variables
    __slots__ = { 'Prescale' : { 'Hlt1XPress' : 0.0025
                               , 'Hlt1VeloAlign' : 0.000001 } } # overrule inherited default

    def __apply_configuration__(self) : 
        from Configurables import Tf__PatVeloAlignTrackFilter as PatVeloAlignTrackFilter
        Line( 'VeloAlign'
            , ODIN = "( ODIN_TRGTYP != LHCb.ODIN.RandomTrigger )"
            , prescale = self.prescale
            , postscale = self.postscale
            , algos = [ PatVeloAlignTrackFilter('HltVeloAlignFilter') ] 
            )

        Line( 'XPress' 
            , ODIN = "( ODIN_TRGTYP != LHCb.ODIN.RandomTrigger )"
            , prescale = self.prescale
            , postscale = self.postscale
            )


        #Line( 'VeloAlignTracks' 
        #    , algos = [ PatVeloAlignTrackFilter( 'AlignTrackFilter' ) ] )
        #
        #
        #Line( 'VeloOverlapTracks' 
        #    ,  algos = [ PatVeloAlignTrackFilter( 'OverlapAlignTrackFilter', Overlap = True ) ] )
        #
        #
        # Overlap-track-finding sequence
        #
        #HltLine( 'VeloOverlap'
        #       , algos =
        #       [ VeloClusterFilter( 'OverlapTracksClusterFilter'
        #                 , OutputLiteClusterLocation = "/Event/Raw/Velo/OverlapTracksLiteClusters"
        #                 , OutputClusterLocation = "/Event/Raw/Velo/OverlapTracksClusters"
        #                 , FilterOption = "Overlap"
        #                 , MinimumNumberOfPhiClusters =18 # 1 track  with 18 hits
        #                 , MaximumNumberOfPhiClusters =84 # 4 tracks with 21 hits
        #                 )
        #       , Tf__PatVeloGeneric( 'OverlapTracksGenericTracking'
        #                  , RHitManagerName = "OverlapTracksRHitManager"
        #                  , PhiHitManagerName= "OverlapTracksPhiHitManager"
        #                  , Output = "Hlt/Track/OverlapTracks"
        #                  , ConsiderOverlaps = True
        #                  , CheckReadOut = True
        #                  , ForwardProp = True
        #                  , MinModules=10
        #                  )
        #       , Member( 'TF','Decision' ,"HltTrackFilter/OverlapTrackTrigger"
        #               , InputSelection  = "TES:Hlt/Track/OverlapTracks"
        #               , FilterDescriptor = ["NumberOfASideVeloHits,>,20", "NumberOfCSideVeloHits,>,20" ]
        #               , HistogramUpdatePeriod = 1
        #               , HistoDescriptor = { "NumberOfASideVeloHits" : [ "NumberOfASideVeloHits",0.,100.,100],
        #                                     "NumberOfCSideVeloHits" : [ "NumberOfCSideVeloHits",0.,100.,100] }
        #               )
        #       ])
        #
        #ottt = OverlapTracksTrackTool( RHitManagerName="OverlapTracksRHitManager", PhiHitManagerName="OverlapTracksPhiHitManager")
        #ottt.addTool( name = OverlapTracksRHitManager )
        #ottt.OverlapTracksRHitManager.DefaultHitManagerName="OverlapTracksDefaultVeloRHitManager"
        #ottt.addTool( name = OverlapTracksPhiHitManager )
        #ottt.OverlapTracksPhiHitManager.DefaultHitManagerName="OverlapTracksDefaultVeloPhiHitManager"
        #
        #OverlapTracksDefaultVeloRHitManager( ClusterLocation = "/Event/Raw/Velo/OverlapTracksClusters"
        #                                   , LiteClusterLocation = "/Event/Raw/Velo/OverlapTracksLiteClusters" )
        #OverlapTracksDefaultVeloPhiHitManager( ClusterLocation = "/Event/Raw/Velo/OverlapTracksClusters"
        #                                     , LiteClusterLocation = "/Event/Raw/Velo/OverlapTracksLiteClusters" )
        #
        #OverlapTracksAssociator( TracksInContainer = "Hlt/Track/OverlapTracks" )
        #OverlapTracksPatChecker( VeloTrackLocation = "Hlt/Track/OverlapTracks" )
        #
        #
        ##
        ## VELO-specific, HLT-resident monitoring sequence
        #
        #GaudiSequencer( 'HltVeloAligningSequence', IgnoreFilterPassed = True
        #              , Members = [ GaudiSequencer( 'OverlapTracksSequence' , MeasureTime=True  ) ]
        #              )
        #
        #GaudiSequencer( 'HltVeloMonitoringSequence' , IgnoreFilterPassed=True
        #              , Members = [ GaudiSequencer('LiteClusterMonitorSequence', MeasureTime=True ) ]
        #              )
        #
        #GaudiSequencer( 'LiteClusterMonitorSequence'
        #              , Members = [ Velo__VeloHltLiteClusterMonitor( 'VeloLiteClusterMonitor'
        #                                                           , HistogramByZone=True
        #                                                           , VeloLiteClusterLocation="Raw/Velo/LiteClusters" 
        #                                                           )
        #                          ]
        #              )
        #
        #
