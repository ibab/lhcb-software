from os import environ
from Gaudi.Configuration import *
from GaudiConf.Configuration import *
import GaudiKernel.ProcessJobOptions
from TrackSys.Configuration import *
from GaudiKernel.SystemOfUnits import mm

from Configurables import ( ProcessPhase, 
                            Tf__PatVeloResetClusterUsed,
                            Tf__PatVeloRTracking, Tf__PatVeloSpaceTool,
                            Tf__PatVeloSpaceTracking, Tf__PatVeloTrackTool,
                            PatPV2D, PatPV3D,
                            PatForward, PatForwardTool,
                            HltTrackFilter)

GaudiSequencer("RecoHLTTrSeq").Members += [  Tf__PatVeloResetClusterUsed("HLT"),
                                             Tf__PatVeloRTracking("HltRecoRZVelo"),
                                             PatPV2D("HltRecoPV2D"),
                                             Tf__PatVeloSpaceTracking("HltRecoSpaceVelo"),
                                             HltTrackFilter("HltPrepareVelo"),
                                             PatPV3D("HltRecoPV3D"),
                                             PatForward("HltRecoForward")
                                             HltTrackFilter("HltPrepareForward")]


Tf__PatVeloResetClusterUsed("HLT").ResetRClusters = True; 
  
Tf__PatVeloRTracking("HltRecoRZVelo").OutputTracksName = "Hlt/Track/RZVelo";

PatPV2D("HltRecoPV2D").InputTracksName = "Hlt/Track/RZVelo";
PatPV2D("HltRecoPV2D").OutputVerticesName = "Hlt/Vertex/PV2D";

Tf__PatVeloSpaceTracking("HltRecoSpaceVelo").InputTracksName = "Hlt/Track/RZVelo";
Tf__PatVeloSpaceTracking("HltRecoSpaceVelo").OutputTracksName = "Hlt/Track/Velo";
Tf__PatVeloSpaceTracking("HltRecoSpaceVelo").addTool( Tf__PatVeloSpaceTool(), name="HLTVeloSpaceTool" )
Tf__PatVeloSpaceTracking("HltRecoSpaceVelo").HLTVeloSpaceTool.OutputLevel = 4;
Tf__PatVeloSpaceTracking("HltRecoSpaceVelo").HLTVeloSpaceTool.MarkClustersUsed = True;

PatForward("HltRecoForward").InputTracksName = "Hlt/Track/Velo";
PatForward("HltRecoForward").OutputTracksName = "Hlt/Track/Forward";
PatForward("HltRecoForward").addTool( PatForwardTool(), name="HLTForwardTool") 
PatForwardTool("HLTForwardTool").AddTTClusterName = "PatAddTTCoord"

HltTrackFilter("HltPrepareVelo").InputSelection = "TES:Hlt/Track/Velo";
HltTrackFilter("HltPrepareVelo").OutputSelection = "Velo1";
HltTrackFilter("HltPrepareVelo").RequirePositiveInputs = false;

HltTrackFilter("HltPrepareForward").InputSelection = "TES:Hlt/Track/Forward";
HltTrackFilter("HltPrepareForward").OutputSelection = "Forward1";
HltTrackFilter("HltPrepareForward").RequirePositiveInputs = false;
