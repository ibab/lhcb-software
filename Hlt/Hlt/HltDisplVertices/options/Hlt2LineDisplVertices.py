### @file
#
#  Hlt2DisplVerticesDEV Line
#
#  @author Neal Gueissaz
#  @date 2009-july-1
#
##
from Gaudi.Configuration import *
from HltLine.HltLine import Hlt2Line
from Configurables import PatPV3D, PVOfflineTool, Hlt2DisplVertices
from Hlt2SharedParticles.BasicParticles import NoCutsPions


# Run PatPV3D
Hlt2PatPV3D = PatPV3D("Hlt2PatPV3D")
Hlt2PatPV3D.addTool(PVOfflineTool)
Hlt2PatPV3D.PVOfflineTool.InputTracks = ["Hlt/Track/Velo"]
# DisplacedOff vertices
Hlt2PatPV3D.PVOfflineTool.PVFitterName = "LSAdaptPV3DFitter"
Hlt2PatPV3D.PVOfflineTool.PVSeedingName = "PVSeed3DTool"
Hlt2PatPV3D.PVOfflineTool.addTool(PVSeed3DTool())
Hlt2PatPV3D.PVOfflineTool.PVSeed3DTool.TrackPairMaxDistance = 0.2
Hlt2PatPV3D.PVOfflineTool.PVSeed3DTool.zMaxSpread = 1
Hlt2PatPV3D.PVOfflineTool.addTool( LSAdaptPV3DFitter())
Hlt2PatPV3D.PVOfflineTool.LSAdaptPV3DFitter.maxIP2PV = 2
Hlt2PatPV3D.OutputVerticesName = "Rec/Vertices/RV"
Hlt2PatPV3D.OutputLevel = 3

Hlt2LineDisplVertices = Hlt2DisplVertices("Hlt2LineDisplVertices")
Hlt2LineDisplVertices.InputLocations = [ NoCutsPions.outputSelection() ]


# Define the Hlt2 Line
#
# Add all needed intermediate states. They are defined in Hlt2Shared.py,
# but not necessarily run. So don't forget to add them.

line = Hlt2Line('DisplVertices'
                , prescale = 1
                , algos = [Hlt2PatPV3D, NoCutsPions, Hlt2LineDisplVertices]
               )
