# loose quality conditions for PV vertex (for Collisions09)
from Configurables import PatPVOffline
from Configurables import PVOfflineTool, SimplePVSeedTool, SimplePVFitter

loosePVSeedTool = SimplePVSeedTool("SimplePVSeedTool")
looseLSAdaptPVFitter = SimplePVFitter("SimplePVFitter")
looseLSAdaptPVFitter.MinTracks = 3

loosePVOfflineTool = PVOfflineTool("PVOfflineTool")
loosePVOfflineTool.RequireVelo = False
loosePVOfflineTool.PVFitterName = "SimplePVFitter"
loosePVOfflineTool.PVSeedingName = "SimplePVSeedTool"


loosePVOfflineTool.addTool(loosePVSeedTool, "SimplePVSeedTool")
loosePVOfflineTool.addTool(looseLSAdaptPVFitter, "SimplePVFitter")

loosePVOffline = PatPVOffline("PatPVOffline")
loosePVOffline.addTool(loosePVOfflineTool, "PVOfflineTool")

#loosePVOffline.OutputLevel = 2

