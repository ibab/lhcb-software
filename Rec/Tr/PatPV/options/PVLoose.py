#loose quality conditions for PV vertex (for Collisions09)
from Configurables import PatPVOffline
from Configurables import LSAdaptPVFitter, PVOfflineTool, PVSeedTool

loosePVSeedTool = PVSeedTool("PVSeedTool")
looseLSAdaptPVFitter = LSAdaptPVFitter("LSAdaptPVFitter")

loosePVOfflineTool = PVOfflineTool("PVOfflineTool")

looseLSAdaptPVFitter.MinTracks = 2
loosePVSeedTool.minClusterMult = 2
loosePVSeedTool.minCloseTracksInCluster = 2
loosePVSeedTool.ratioSig2HighMult = 1
loosePVSeedTool.ratioSig2LowMult =1

loosePVOfflineTool.addTool(loosePVSeedTool, "PVSeedTool")
loosePVOfflineTool.addTool(looseLSAdaptPVFitter, "LSAdaptPVFitter")

loosePVOffline = PatPVOffline("PatPVOffline")
loosePVOffline.addTool(loosePVOfflineTool, "PVOfflineTool")

#loosePVOffline.OutputLevel = 4
