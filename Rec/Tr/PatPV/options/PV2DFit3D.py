from Configurables import PatPV2DFit3D, PVOfflineTool

# reconstruction
myPV2DFit3D = PatPV2DFit3D("myPV2DFit3D")
myPV2DFit3D.addTool(PVOfflineTool)
myPV2DFit3D.PVOfflineTool.PVFitterName = "LSAdaptPV3DFitter"
#myPV2DFit3D.OutputVerticesName = "Rec/Vertex/myPV2DFit3D"
