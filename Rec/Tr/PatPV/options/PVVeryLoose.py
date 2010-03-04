# loose quality conditions for PV vertex (for Collisions09)
from Configurables import PatPVOffline
from Configurables import PVOfflineTool, SimplePVSeedTool, SimplePVFitter

loosePVSeedTool      = PVSeed3DOfflineTool("PVSeed3DOfflineTool")
looseLSAdaptPVFitter = LSAdaptPVFitter("LSAdaptPVFitter")
looseLSAdaptPVFitter.MinTracks    = 3
looseLSAdaptPVFitter.acceptTrack  = 0.000000000001
looseLSAdaptPVFitter.trackMaxChi2 = 25.
looseLSAdaptPVFitter.zMaxSpread   = 1000.
looseLSAdaptPVFitter.minIter      = 3
   
loosePVOfflineTool = PVOfflineTool("PVOfflineTool")
loosePVOfflineTool.RequireVelo = False
loosePVOfflineTool.PVFitterName = "LSAdaptPVFitter"
loosePVOfflineTool.PVSeedingName = "PVSeed3DOfflineTool"


loosePVOfflineTool.addTool(loosePVSeedTool, "PVSeed3DOfflineTool")
loosePVOfflineTool.addTool(looseLSAdaptPVFitter, "LSAdaptPVFitter")

loosePVOffline = PatPVOffline("PatPVOffline")
loosePVOffline.addTool(loosePVOfflineTool, "PVOfflineTool")

#loosePVOffline.OutputLevel = 2

