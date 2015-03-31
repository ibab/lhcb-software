#=== options for reconstruction all PV including displaced PVs
from Configurables import PatPV3D, PVOfflineTool, LSAdaptPV3DFitter, PVSeed3DTool
from GaudiKernel.SystemOfUnits import mm

#=== reconstruction of PV
PV = PatPV3D( "PV" )
PV.addTool(PVOfflineTool)
PV.PVOfflineTool.InputTracks = ["Rec/Track/Best"]
PV.OutputVerticesName = "Rec/Vertices/PV"

#=== reconstruction of displaced vertices
RV = PatPV3D( "RV" )
RV.addTool(PVOfflineTool)
RV.PVOfflineTool.InputVerticesName = "Rec/Vertices/PV" #takes PVs as input
RV.PVOfflineTool.LookForDisplaced = True
RV.PVOfflineTool.InputTracks = ["Rec/Track/Best"]
RV.PVOfflineTool.PVSeedingName = "PVSeed3DTool"
RV.PVOfflineTool.addTool(PVSeed3DTool())
RV.PVOfflineTool.PVSeed3DTool.TrackPairMaxDistance = 0.2*mm
RV.PVOfflineTool.PVSeed3DTool.zMaxSpread = 1*mm
RV.PVOfflineTool.addTool( LSAdaptPV3DFitter())
RV.OutputVerticesName = "Rec/Vertices/RV"
