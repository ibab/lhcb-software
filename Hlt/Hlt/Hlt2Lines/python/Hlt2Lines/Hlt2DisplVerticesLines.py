############################################################
#
# Hlt2DisplVertices : a Line for displaced vertices hunting
#
# Maintainer : Neal Gauvin 
# Date       : 16 july 2009
# Revised    : 12 November 2009
#
# 2 steps :
#      - Reconstruction of all vertices with >= 5 tracks
#        with PatPV3D, with optimized cuts for "smaller" vertices
#        than PV's.
#      - Hlt2DisplVertices loops on all reconstructed vertices.
#        The one with lowest z (upstream) is not considered
#        Vertices with at least one backward track are not considered.
#        Vertices close to the beam line removed.
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
import GaudiKernel.SystemOfUnits as units

class Hlt2DisplVerticesLinesConf(HltLinesConfigurableUser) :
    
    __slots__ = {  "MinNbTracks"   : 0
                ,  "RCutMethod"    : "FromUpstreamPV"
                ,  "RMin1"         : 0.5 * units.mm
                ,  "RMin2"         : 0.3 * units.mm
                ,  "MinMass1"      : 6.5*units.GeV
                ,  "MinMass2"      : 2*units.GeV
                ,  "MinSumpt1"     : 6.*units.GeV
                ,  "MinSumpt2"     : 2.*units.GeV
                ,  "RemVtxFromDet" : 1*units.mm
                }
    
    
    def __apply_configuration__(self) :
        
        from HltLine.HltLine import Hlt2Line
        from Configurables import HltANNSvc
        from Configurables import PatPV3D, PVOfflineTool, PVSeed3DTool, LSAdaptPV3DFitter, Hlt2DisplVertices, Hlt2DisplVerticesDEV
        from Hlt2SharedParticles.BasicParticles import NoCutsPions
        from HltLine.HltPVs import PV3D
        from Configurables import Hlt2Tracking
        
        # Run PatPV3D
        Hlt2PatPV3D = PatPV3D("Hlt2DisplVerticesV3D")
        Hlt2PatPV3D.addTool(PVOfflineTool)
        #Get the correct input location automatically from the Hlt2 reconstruction instead of hardocoding
        Hlt2PatPV3D.PVOfflineTool.InputTracks = [(Hlt2Tracking().hlt2VeloTracking()).outputSelection()]
        Hlt2PatPV3D.PVOfflineTool.PVFitterName = "LSAdaptPV3DFitter"
        Hlt2PatPV3D.PVOfflineTool.PVSeedingName = "PVSeed3DTool"
        Hlt2PatPV3D.PVOfflineTool.addTool(PVSeed3DTool)
        Hlt2PatPV3D.PVOfflineTool.PVSeed3DTool.TrackPairMaxDistance = 0.2*units.mm
        Hlt2PatPV3D.PVOfflineTool.PVSeed3DTool.zMaxSpread = 1*units.mm
        Hlt2PatPV3D.PVOfflineTool.addTool( LSAdaptPV3DFitter)
        Hlt2PatPV3D.PVOfflineTool.LSAdaptPV3DFitter.maxIP2PV = 2*units.mm
        Hlt2PatPV3D.OutputVerticesName = "Rec/Vertices/Hlt2DisplVerticesV3D"
        
        #Run Hlt2DisplVertices
        Hlt2LineDisplVertices = Hlt2DisplVertices("Hlt2LineDisplVertices")
        Hlt2LineDisplVertices.InputLocations = [NoCutsPions.outputSelection()]
        Hlt2LineDisplVertices.InputDisplacedVertices = Hlt2PatPV3D.OutputVerticesName 
        Hlt2LineDisplVertices.MinNbTracks = self.getProp('MinNbTracks')
        Hlt2LineDisplVertices.RCutMethod = self.getProp('RCutMethod')
        Hlt2LineDisplVertices.RMin1 = self.getProp('RMin1')
        Hlt2LineDisplVertices.RMin2 = self.getProp('RMin2')
        Hlt2LineDisplVertices.MinMass1 = self.getProp('MinMass1')
        Hlt2LineDisplVertices.MinMass2 = self.getProp('MinMass2')
        Hlt2LineDisplVertices.MinSumpt1 = self.getProp('MinSumpt1')
        Hlt2LineDisplVertices.MinSumpt2 = self.getProp('MinSumpt2')
        Hlt2LineDisplVertices.RemVtxFromDet = self.getProp('RemVtxFromDet')
        
        # Define the Hlt2 Line
        if( self.getProp('RCutMethod') == "FromUpstreamPV" ):
            line = Hlt2Line('DisplVertices'
                            , prescale = self.prescale
                            , algos = [ PV3D, Hlt2PatPV3D, NoCutsPions, Hlt2LineDisplVertices]
                            , postscale = self.postscale
                            )
        else :
           line = Hlt2Line('DisplVertices'
                            , prescale = self.prescale
                            , algos = [ Hlt2PatPV3D, NoCutsPions, Hlt2LineDisplVertices]
                            , postscale = self.postscale
                            ) 
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DisplVerticesDecision" : 50280 } )
