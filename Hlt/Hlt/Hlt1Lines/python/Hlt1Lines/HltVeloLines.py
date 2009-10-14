# =============================================================================
# $Id: HltVeloLines.py,v 1.13 2009-10-14 11:54:50 sredford Exp $
# =============================================================================
## @file
#  Configuration of Hlt Lines for the VELO closing proceure
# =============================================================================
"""
 script to configure Hlt lines for the VELO closing procedure
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.13 $"
# =============================================================================

#
# VELO (closing procedure) specific HLT trigger lines
#
from HltLine.HltLinesConfigurableUser import *

class HltVeloLinesConf(HltLinesConfigurableUser):
   __slots__ = { 'Prescale'                   : { 'Hlt1Velo(.Side|TrTr)' : 0.0001 } 
               , 'MinimumNumberOfRClusters'   : 12 # 4 tracks with 3 hits
               , 'MinimumNumberOfPhiClusters' : 12 # 4 tracks with 3 hits
               , 'MaxNumberOfClusters'        : 450 # 0.5% occupancy
               , 'ODIN'                       :"( ODIN_TRGTYP != LHCb.ODIN.RandomTrigger )" # on what trigger types do we run?
               }

   def __apply_configuration__(self):
        from HltLine.HltLine import Hlt1Line   as Line
        from HltLine.HltLine import Hlt1Member as Member
        # from Configurables import DecodeVeloRawBuffer
        from Configurables import VeloClusterFilter
        from Configurables import Tf__DefaultVeloRHitManager as DefaultVeloRHitManager
        from Configurables import Tf__DefaultVeloPhiHitManager as DefaultVeloPhiHitManager
        from Configurables import Tf__PatVeloTrackTool as PatVeloTrackTool
        from Configurables import Tf__PatVeloRTracking as PatVeloRTracking
        from Configurables import Tf__PatVeloSpaceTracking as PatVeloSpaceTracking
        from Configurables import Tf__PatVeloSpaceTool as PatVeloSpaceTool
        from Configurables import Tf__PatVeloGeneralTracking as PatVeloGeneralTracking
        from Configurables import Tf__PatVeloTraversingTracking as PatVeloTraversingTracking
        from Configurables import Tf__PatVeloRHitManager as PatVeloRHitManager
        from Configurables import Tf__PatVeloPhiHitManager as PatVeloPhiHitManager
        from Configurables import PatPV3D, PVOfflineTool
        ### find primary vertices seperately in each side
        for side in [ 'ASide', 'CSide' ] :
            cf = VeloClusterFilter('Hlt1Velo' +  side + 'ClusterFilter'
                                  , MinimumNumberOfRClusters = self.getProp('MinimumNumberOfRClusters')
                                  , MinimumNumberOfPhiClusters = self.getProp('MinimumNumberOfPhiClusters')
                                  , MaximumNumberOfClusters = self.getProp('MaxNumberOfClusters') 
                                  , OutputLiteClusterLocation = "/Event/Raw/Velo/" + side + "LiteClusters"
                                  , OutputClusterLocation = "/Event/Raw/Velo/" + side + "Clusters"
                                  , FilterOption = { 'ASide' : 'Left', 'CSide' : 'Right' }[ side ]
                                  )
            ## do the R tracking
            rm = DefaultVeloRHitManager( side + 'DefaultVeloRHitManager'
                                         , ClusterLocation = cf.OutputClusterLocation 
                                         , LiteClusterLocation = cf.OutputLiteClusterLocation )
            rt = PatVeloRTracking( 'Hlt1Velo' + side + 'RTracking'
                                     , OutputTracksName = 'Hlt/Track/' + side + 'RZVelo'
                                     , HitManagerName = rm.splitName()[-1] )

            ## do the space tracking
            pm = DefaultVeloPhiHitManager( side + 'DefaultVeloPhiHitManager'
                                      , ClusterLocation = cf.OutputClusterLocation
                                      , LiteClusterLocation = cf.OutputLiteClusterLocation )

            # wrap a new level of managment around the previous ones
            rm = PatVeloRHitManager(   side + 'RHitManager',   DefaultHitManagerName = rm.splitName()[-1] )
            pm = PatVeloPhiHitManager( side + 'PhiHitManager', DefaultHitManagerName = pm.splitName()[-1] )

            tt = PatVeloTrackTool( side + 'TrackTool'
                                , RHitManagerName = rm.splitName()[-1]
                                , PhiHitManagerName = pm.splitName()[-1]   
                                , TracksInHalfBoxFrame = True)

            spacetool = PatVeloSpaceTool( side + 'SpaceTool'
                                , RHitManagerName = rm.splitName()[-1]
                                , PhiHitManagerName = pm.splitName()[-1]          
                                , TrackToolName = tt.splitName()[-1] )          

            tracks = 'Hlt/Track/%sVelo' % side
            st = PatVeloSpaceTracking('Hlt1Velo' +  side + 'SpaceTracking'
                                         , InputTracksName = rt.OutputTracksName
                                         , OutputTracksName = tracks
                                         , SpaceToolName = spacetool.getFullName() )

            gt = PatVeloGeneralTracking( 'Hlt1Velo' + side + 'GeneralTracking'
                                           , RHitManagerName = rm.splitName()[-1]
                                           , PhiHitManagerName = pm.splitName()[-1]
                                           , TrackToolName = tt.splitName()[-1]
                                           , OutputTracksLocation = tracks )

            pv3D = PatPV3D( 'Hlt1Velo' + side + 'PatPV3D', OutputVerticesName = 'Hlt/Vertex/' + side + 'PV3D', OutputLevel = 3)
            pv3D.addTool( PVOfflineTool, name = 'PVOfflineTool')
            pv3D.PVOfflineTool.InputTracks = [tracks]
            pv3D.PVOfflineTool.PVFitterName = "LSAdaptPV3DFitter"
            pv3D.PVOfflineTool.PVSeedingName = "PVSeed3DTool"
 
            from HltLine.HltDecodeRaw import DecodeVELO
            Line( 'Velo' + side
                , ODIN = self.getProp('ODIN')
                , prescale = self.prescale
                , algos =
                [ DecodeVELO, cf, rt, st, gt, pv3D
                , Member( 'VF' , 'Decision'
                        , OutputSelection = '%Decision'
                        , InputSelection  = 'TES:%s' % pv3D.OutputVerticesName
                        , FilterDescriptor = ['VertexNumberOf' + side + 'Tracks,>,4']
                        , HistogramUpdatePeriod = 1
                        , HistoDescriptor = {'VertexNumberOf' + side + 'Tracks' : ( 'VertexNumberOf'+side+'Tracks',-0.5,39.5,40)}
                        )
                ]
                , postscale = self.postscale
                )

            # The following code leaks memory, and is NOT yet ready for inclusion
            if False :
                 ### find traversing tracks across both sides                              
                 tgt = PatVeloGeneralTracking( 'Hlt1VeloGeneralTracking'
                                             , TrackToolName = 'TrackTool'
                                             , OutputTracksLocation = '/Event/Hlt/Track/VeloGeneral'
                                             )
                 
                 PatVeloTrackTool( 'TrackTool', TracksInHalfBoxFrame = True)
                 
                 tt = PatVeloTraversingTracking('TraversingTracking'
                                                , InputTracksLocation = tgt.OutputTracksLocation
                                                , DistanceGuess = 0
                                                , IncludeLuminousRegion = 'False'
                                                , OutputTracksLocation = '/Event/Hlt/Track/TraversingTrack'
                                                )
                 
                 Line( 'VeloTrTr'
                       , ODIN = self.getProp('ODIN')
                       , prescale = self.prescale
                       , algos =
                       [ DecodeVELO, tgt, tt
                         , Member( 'TF' , 'Decision'
                                   , OutputSelection = '%Decision'
                                   , InputSelection  = 'TES:' + tt.OutputTracksLocation
                                   , FilterDescriptor = ["NumberOfASideVeloHits,>,4", "NumberOfCSideVeloHits,>,4" ]
                                   , HistogramUpdatePeriod = 1
                                   )
                         ]
                       , postscale = self.postscale
                       )
