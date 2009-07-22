############################################################
#
# Hlt2DisplVertices : a Line for displaced vertices hunting
#
# Maintainer : Neal Gauvin (Gueissaz)
# Date       : 16 july 2009
#
# 2 steps :
#      - Reconstruction of all vertices with >= 5 tracks
#        with PatPV3D, with optimized cuts for "smaller" vertices
#        than PV's.
#      - Hlt2DisplVertices loops on all reconstructed vertices.
#        The one with lowest z is not considered
#        Vertices with at least one backward track are not considered.
#        2 kinds of cuts are applied to the set of displaced vertices :
#           >1 prey passing thighter cuts
#               --> when hunting single long-lived particles
#           >2 preys passing looser cuts
#               --> when looking for 2 particles, coming from a mother one.
#        To allow for kinematical cuts, association is done between the Velo
#        tracks participating to the vertex and the reconstructed Hlt2Pions.
#        When no Hlt2Pions found, default pions with 0.4 GeV pt are created.
#        Vertices that are found to be close to the detector material
#        could be possibly eliminated.
#
#
############################################################

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from HltLine.HltLine import Hlt2Line
import GaudiKernel.SystemOfUnits as units

class Hlt2DisplVerticesLinesConf(HltLinesConfigurableUser) :
    
    __slots__ = {  "MinNbTracks"   : 0
                   ,"RMin"          : 0.3 * units.mm
                   ,"MinMass1"      : 9*units.GeV
                   ,"MinMass2"      : 4*units.GeV
                   ,"MinSumpt"      : 10.*units.GeV
                   ,"RemVtxFromDet" : 1*units.mm
                   }
    
    
    def __apply_configuration__(self) :
        
        from Configurables import HltANNSvc
        from Configurables import PatPV3D, PVOfflineTool, PVSeed3DTool, LSAdaptPV3DFitter, Hlt2DisplVertices
        from Hlt2SharedParticles.BasicParticles import NoCutsPions
        
        # Run PatPV3D
        Hlt2PatPV3D = PatPV3D("Hlt2PatPV3D")
        Hlt2PatPV3D.addTool(PVOfflineTool)
        Hlt2PatPV3D.PVOfflineTool.InputTracks = ["Hlt/Track/Velo"]
        Hlt2PatPV3D.PVOfflineTool.PVFitterName = "LSAdaptPV3DFitter"
        Hlt2PatPV3D.PVOfflineTool.PVSeedingName = "PVSeed3DTool"
        Hlt2PatPV3D.PVOfflineTool.addTool(PVSeed3DTool())
        Hlt2PatPV3D.PVOfflineTool.PVSeed3DTool.TrackPairMaxDistance = 0.2*units.mm
        Hlt2PatPV3D.PVOfflineTool.PVSeed3DTool.zMaxSpread = 1*units.mm
        Hlt2PatPV3D.PVOfflineTool.addTool( LSAdaptPV3DFitter())
        Hlt2PatPV3D.PVOfflineTool.LSAdaptPV3DFitter.maxIP2PV = 2*units.mm
        Hlt2PatPV3D.OutputVerticesName = "Rec/Vertices/Hlt2RV"
        Hlt2PatPV3D.OutputLevel = 3
        
        #Run Hlt2DisplVertices
        Hlt2LineDisplVertices = Hlt2DisplVertices("Hlt2LineDisplVertices")
        Hlt2LineDisplVertices.InputLocations = [NoCutsPions.outputSelection()]
        Hlt2LineDisplVertices.InputDisplacedVertices = "Rec/Vertices/Hlt2RV"
        Hlt2LineDisplVertices.OutputLevel = 3
        Hlt2LineDisplVertices.MinNbTracks = self.getProp('MinNbTracks')
        Hlt2LineDisplVertices.RMin = self.getProp('RMin')
        Hlt2LineDisplVertices.MinMass1 = self.getProp('MinMass1')
        Hlt2LineDisplVertices.MinMass2 = self.getProp('MinMass2')
        Hlt2LineDisplVertices.MinSumpt = self.getProp('MinSumpt')
        Hlt2LineDisplVertices.RemVtxFromDet = self.getProp('RemVtxFromDet')
        
        
        # Define the Hlt2 Line
        line = Hlt2Line('DisplVertices'
                        , prescale = 1
                        , algos = [Hlt2PatPV3D, NoCutsPions, Hlt2LineDisplVertices]
                        )
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DisplVerticesDecision" : 50280 } )
        
