# =============================================================================
## @file
#  Configuration of Hlt Lines for the VELO closing proceure
# =============================================================================
"""
 script to configure Hlt lines for the VELO closing procedure
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.10 $"
# =============================================================================

#
# VELO (closing procedure) specific HLT trigger lines
#
from HltLine.HltLinesConfigurableUser import *

class Hlt1VeloLinesConf(HltLinesConfigurableUser):
   __slots__ = { 'Prescale'                   : { 'Hlt1Velo(.Side|TrTr).*' : 0.0 } 
               , 'Postscale'                  : { 'Hlt1Velo(.Side|TrTr).*' : 'RATE(1000)' } 
               , 'MinimumNumberOfRClusters'   : 12 # 4 tracks with 3 hits
               , 'MinimumNumberOfPhiClusters' : 12 # 4 tracks with 3 hits
               , 'MaxNumberOfClusters'        : 450 # 0.5% occupancy
               , 'MinTrksPerVtx'              : 2   # 4 is the minumum value you can put here  
               , 'ODIN'                       : ""
               , 'L0DU'                       : ""
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
            drm = DefaultVeloRHitManager( side + 'DefaultVeloRHitManager'
                                         , ClusterLocation = cf.OutputClusterLocation 
                                         , LiteClusterLocation = cf.OutputLiteClusterLocation )
            
            rm = PatVeloRHitManager(   side + 'RHitManager',   DefaultHitManagerName = drm.splitName()[-1] )
            
            rt = PatVeloRTracking( 'Hlt1Velo' + side + 'RTracking'
                                     , OutputTracksName = 'Hlt/Track/' + side + 'RZVelo'
                                     , HitManagerName = rm.splitName()[-1] )

            ## do the space tracking
            dpm = DefaultVeloPhiHitManager( side + 'DefaultVeloPhiHitManager'
                                      , ClusterLocation = cf.OutputClusterLocation
                                      , LiteClusterLocation = cf.OutputLiteClusterLocation )

            pm = PatVeloPhiHitManager( side + 'PhiHitManager', DefaultHitManagerName = dpm.splitName()[-1] )

            tt = PatVeloTrackTool( side + 'TrackTool'
                                , RHitManagerName = rm.splitName()[-1]
                                , PhiHitManagerName = pm.splitName()[-1]   
                                , TracksInHalfBoxFrame = True )

            tracks = 'Hlt/Track/%sVelo' % side
            st = PatVeloSpaceTracking('Hlt1Velo' +  side + 'SpaceTracking'
                                         , InputTracksName = rt.OutputTracksName
                                         , OutputTracksName = tracks
                                         , SpaceToolName = 'Tf__PatVeloSpaceTool/' + side + 'SpaceTool' )

            # need to knock off the type name here
            spaceToolName = st.SpaceToolName[st.SpaceToolName.find('/')+1:]
            st.addTool(PatVeloSpaceTool, name = spaceToolName)
            # need to access the tool with its name as code not as a string...
            spaceTool = eval('st.'+spaceToolName)
            spaceTool.RHitManagerName = rm.splitName()[-1]
            spaceTool.PhiHitManagerName = pm.splitName()[-1]
            spaceTool.TrackToolName = 'Tf__PatVeloTrackTool/'+side+'TrackTool'
            
            gt = PatVeloGeneralTracking( 'Hlt1Velo' + side + 'GeneralTracking'
                                           , RHitManagerName = rm.splitName()[-1]
                                           , PhiHitManagerName = pm.splitName()[-1]
                                           , TrackToolName = tt.splitName()[-1]
                                           , OutputTracksLocation = tracks )

            pv3D = PatPV3D( 'Hlt1Velo' + side + 'PatPV3D', OutputVerticesName = 'Hlt/Vertex/' + side + 'PV3D')
            pv3D.addTool( PVOfflineTool, name = 'PVOfflineTool')
            pv3D.PVOfflineTool.InputTracks = [tracks]
            pv3D.PVOfflineTool.PVSeedingName = "PVSeed3DTool"
            from Configurables import PVSeed3DTool, LSAdaptPV3DFitter
            pv3D.PVOfflineTool.addTool( PVSeed3DTool )
            pv3D.PVOfflineTool.PVSeed3DTool.MinCloseTracks = 1 # default is 4
            pv3D.PVOfflineTool.PVFitterName = "LSAdaptPV3DFitter"
            pv3D.PVOfflineTool.addTool( LSAdaptPV3DFitter )
            pv3D.PVOfflineTool.LSAdaptPV3DFitter.TrackErrorScaleFactor = 2.0
            pv3D.PVOfflineTool.LSAdaptPV3DFitter.MinTracks = 3 # default is 5
 
            from HltLine.HltDecodeRaw import DecodeVELO
            if False :
                Line( 'Velo' + side
                    , ODIN = self.getProp('ODIN')
                    , L0DU = self.getProp('L0DU')
                    , prescale = self.prescale
                    , algos =
                    [ DecodeVELO, cf, rt, st, gt, pv3D  
                    , Member( 'VF' , 'Decision'
                            , OutputSelection = '%Decision'
                            , InputSelection  = 'TES:%s' % pv3D.OutputVerticesName
                            , FilterDescriptor = ['VertexNumberOf' + side + 'Tracks,>,%s' %self.getProp('MinTrksPerVtx')]
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
