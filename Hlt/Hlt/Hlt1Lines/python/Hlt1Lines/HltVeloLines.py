# =============================================================================
# $Id: HltVeloLines.py,v 1.2 2009-06-01 15:32:32 graven Exp $
# =============================================================================
## @file
#  Configuration of Hlt Lines for the VELO closing proceure
# =============================================================================
"""
 script to configure Hlt lines for the VELO closing procedure
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.2 $"
# =============================================================================

#
# VELO (closing procedure) specific HLT trigger lines
#
from HltLine.HltLinesConfigurableUser import *
from HltLine.HltLine import Hlt1Line   as Line
from HltLine.HltLine import Hlt1Member as Member

class HltVeloLinesConf(HltLinesConfigurableUser):
   __slots__ = { 'Prescale' : { 'Hlt1VeloClosing' : 1, 'Hlt1Velo.Side' : 0.0001 } 
               , 'Postscale' : {}
               , 'MinimumNumberOfRClusters'   : 12 # 4 tracks with 3 hits
               , 'MinimumNumberOfPhiClusters' : 12 # 4 tracks with 3 hits
               , 'MaxNumberOfClusters'        : 450 # 0.5% occupancy
               , 'ODIN'                       :"( ODIN_TRGTYP != LHCb.ODIN.RandomTrigger )" # on what trigger types do we run?
               }

   def __apply_configuration__(self):
        from Configurables import VeloClusterFilter
        from Configurables import Tf__DefaultVeloRHitManager as DefaultVeloRHitManager
        from Configurables import Tf__DefaultVeloPhiHitManager as DefaultVeloPhiHitManager
        from Configurables import Tf__PatVeloTrackTool as PatVeloTrackTool
        from Configurables import Tf__PatVeloRTracking as PatVeloRTracking
        from Configurables import Tf__PatVeloSpaceTracking as PatVeloSpaceTracking
        from Configurables import Tf__PatVeloSpaceTool as PatVeloSpaceTool
        from Configurables import Tf__PatVeloGeneralTracking as PatVeloGeneralTracking
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
            rt = PatVeloRTracking( 'Hlt1Velo' + side + 'RTracking'
                                     , OutputTracksName = 'Hlt/Track/' + side + 'RZVelo'
                                     , HitManagerName = side + 'DefaultVeloRHitManager')
            DefaultVeloRHitManager( side + 'DefaultVeloRHitManager'
                                      , ClusterLocation = '/Event/Raw/Velo/' + side + 'Clusters'
                                      , LiteClusterLocation = '/Event/Raw/Velo/' + side + 'LiteClusters' )
            DefaultVeloPhiHitManager( side + 'DefaultVeloPhiHitManager'
                                        , ClusterLocation = '/Event/Raw/Velo/' + side + 'Clusters'
                                        , LiteClusterLocation = '/Event/Raw/Velo/' + side + 'LiteClusters' )
            st = PatVeloSpaceTracking('Hlt1Velo' +  side + 'SpaceTracking'
                                         , InputTracksName = 'Hlt/Track/'+side+'RZVelo'
                                         , OutputTracksName = 'Hlt/Track/'+side+'Velo'
                                         , SpaceToolName = 'Tf__PatVeloSpaceTool/' + side + "SpaceTool")
            PatVeloSpaceTool( side + 'SpaceTool'
                                , RHitManagerName= side+"RHitManager"
                                , PhiHitManagerName= side + "PhiHitManager"
                                , TrackToolName= side + "TrackTool" )
            gt = PatVeloGeneralTracking( 'Hlt1Velo' + side + 'GeneralTracking'
                                           , RHitManagerName = side + 'RHitManager'
                                           , PhiHitManagerName= side + 'PhiHitManager'
                                           , TrackToolName= side + 'TrackTool'
                                           , OutputTracksLocation = 'Hlt/Track/' + side + 'Velo')
            PatVeloTrackTool( side + 'TrackTool'
                                , RHitManagerName= side + "RHitManager"
                                , PhiHitManagerName= side + "PhiHitManager")
            PatVeloRHitManager(   side + 'RHitManager',   DefaultHitManagerName= side + "DefaultVeloRHitManager")
            PatVeloPhiHitManager( side + 'PhiHitManager', DefaultHitManagerName= side + "DefaultVeloPhiHitManager")

            pv3D = PatPV3D( 'Hlt1Velo' + side + 'PatPV3D', OutputVerticesName = 'Hlt/Vertex/' + side + 'PV3D')
            pv3D.addTool( PVOfflineTool, name = 'PVOfflineTool')
            pv3D.PVOfflineTool.InputTracks = ['Hlt/Track/' + side + 'Velo']
            pv3D.PVOfflineTool.PVFitterName = "LSAdaptPV3DFitter"
            pv3D.PVOfflineTool.PVSeedingName = "PVSeed3DTool"

            Line( 'Velo' + side
                , ODIN = self.getProp('ODIN')
                , prescale = self.prescale
                , algos =
                [ cf, rt, st, gt, pv3D
                , Member( 'VF' , 'Decision'
                        , OutputSelection = '%Decision'
                        , InputSelection  = 'TES:Hlt/Vertex/' + side + 'PV3D'
                        , FilterDescriptor = ['VertexNumberOf' + side + 'Tracks,>,4']
                        , HistogramUpdatePeriod = 1
                        , HistoDescriptor = {'VertexNumberOf' + side + 'Tracks' : ( 'VertexNumberOf'+side+'Tracks',-0.5,39.5,40)}
                        )
                ]
                , postscale = self.postscale
                )

        Line( 'VeloClosing' 
            , prescale = self.prescale
            , HLT = "HLT_PASS('Hlt1VeloCSideDecision','Hlt1VeloASideDecision')" 
            , postscale = self.postscale
            )
