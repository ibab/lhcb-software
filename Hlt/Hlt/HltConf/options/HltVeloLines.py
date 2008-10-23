#
# VELO specific HLT trigger lines
#
from Configurables import VeloClusterFilter, DecodeVeloRawBuffer
from Configurables import Velo__VeloHltLiteClusterMonitor
from Configurables import Tf__PatVeloRTracking, Tf__PatVeloSpaceTracking, Tf__PatVeloGeneralTracking
from Configurables import Tf__PatVeloSpaceTool, Tf__PatVeloTrackTool
from Configurables import Tf__DefaultVeloRHitManager, Tf__DefaultVeloPhiHitManager
from Configurables import Tf__PatVeloRHitManager, Tf__PatVeloPhiHitManager
from Configurables import PatPV3D, PVOfflineTool
#from Configurables import PatVeloAlignTrackFilter
from Configurables import GaudiSequencer
from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import Hlt1Member as Member

### find primary vertices seperately in each side
for side in [ 'ASide', 'CSide' ] :
    cf = VeloClusterFilter( side + 'ClusterFilter'
                          , MinimumNumberOfRClusters = 12 # 4tracks with 3 hits
                          , MinimumNumberOfPhiClusters = 12 # 4tracks with 3 hits
                          , MaximumNumberOfClusters = 450 # 0.5% occupancy
                          , OutputLiteClusterLocation = "/Event/Raw/Velo/" + side + "LiteClusters"
                          , OutputClusterLocation = "/Event/Raw/Velo/" + side + "Clusters"
                          , FilterOption = { 'ASide' : 'Left', 'CSide' : 'Right' }[ side ]
                          )
    rt = Tf__PatVeloRTracking( side + 'RTracking'
                             , OutputTracksName = 'Hlt/Track/' + side + 'RZVelo'
                             , HitManagerName = side + 'DefaultVeloRHitManager')
    Tf__DefaultVeloRHitManager( side + 'DefaultVeloRHitManager'
                              , ClusterLocation = '/Event/Raw/Velo/' + side + 'Clusters'
                              , LiteClusterLocation = '/Event/Raw/Velo/' + side + 'LiteClusters' )
    Tf__DefaultVeloPhiHitManager( side + 'DefaultVeloPhiHitManager'
                                , ClusterLocation = '/Event/Raw/Velo/' + side + 'Clusters'
                                , LiteClusterLocation = '/Event/Raw/Velo/' + side + 'LiteClusters' )
    st = Tf__PatVeloSpaceTracking( side + 'SpaceTracking'
                                 , InputTracksName = 'Hlt/Track/'+side+'RZVelo'
                                 , OutputTracksName = 'Hlt/Track/'+side+'Velo'
                                 , SpaceToolName = side + "SpaceTool")
    Tf__PatVeloSpaceTool( side + 'SpaceTool'
                        , RHitManagerName= side+"RHitManager"
                        , PhiHitManagerName= side + "PhiHitManager"
                        , TrackToolName= side + "TrackTool" )
    gt = Tf__PatVeloGeneralTracking( side + 'GeneralTracking'
                                   , RHitManagerName = side + 'RHitManager'
                                   , PhiHitManagerName= side + 'PhiHitManager'
                                   , TrackToolName= side + 'TrackTool'
                                   , OutputTracksLocation = 'Hlt/Track/' + side + 'Velo')
    Tf__PatVeloTrackTool( side + 'TrackTool'
                        , RHitManagerName= side + "RHitManager"
                        , PhiHitManagerName= side + "PhiHitManager")
    Tf__PatVeloRHitManager(   side + 'RHitManager',   DefaultHitManagerName= side + "DefaultVeloRHitManager")
    Tf__PatVeloPhiHitManager( side + 'PhiHitManager', DefaultHitManagerName= side + "DefaultVeloPhiHitManager")

    pv3D = PatPV3D( side + 'PatPV3D'
                  , OutputVerticesName = 'Hlt/Vertex/' + side + 'PV3D')
    pv3D.addTool( PVOfflineTool, name = 'PVOfflineTool')
    pv3D.PVOfflineTool.InputTracks = ['Hlt/Track/' + side + 'Velo']
    pv3D.PVOfflineTool.PVFitterName = "LSAdaptPV3DFitter"
    pv3D.PVOfflineTool.PVSeedingName = "PVSeed3DTool"

    line =Line( 'Velo' + side
              ,  ODIN = "( ODIN_TRGTYP == LHCb.ODIN.PhysicsTrigger ) | ( ODIN_TRGTYP == LHCb.ODIN.NonZSupTrigger )"
              , algos =
              [ cf, rt, st, gt, pv3D
              , Member( 'VF' , 'Decision'
                      , OutputSelection = '%Decision'
                      , InputSelection  = 'TES:Hlt/Vertex/' + side + 'PV3D'
                      , FilterDescriptor = ['VertexNumberOf' + side + 'Tracks,>,4']
                      , HistogramUpdatePeriod = 1
                      , HistoDescriptor = {'VertexNumberOf' + side + 'Tracks' : ( 'VertexNumberOf'+side+'Tracks',-0.5,39.5,40)}
                      )
              ] )


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
#       [ DecodeVeloRawBuffer()
#       , VeloClusterFilter( 'OverlapTracksClusterFilter'
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
GaudiSequencer( 'HltVeloAligningSequence', IgnoreFilterPassed = True
              , Members = [ GaudiSequencer( 'OverlapTracksSequence' , MeasureTime=True  ) ]
              )

GaudiSequencer( 'HltVeloMonitoringSequence' , IgnoreFilterPassed=True
              , Members = [ GaudiSequencer('LiteClusterMonitorSequence', MeasureTime=True ) ]
              )

GaudiSequencer( 'LiteClusterMonitorSequence'
              , Members = [ DecodeVeloRawBuffer()
                          , Velo__VeloHltLiteClusterMonitor( 'VeloLiteClusterMonitor'
                                                           , HistogramByZone=True
                                                           , VeloLiteClusterLocation="Raw/Velo/LiteClusters" 
                                                           )
                          ]
              )

