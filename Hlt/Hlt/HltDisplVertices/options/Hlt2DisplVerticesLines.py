############################################################
#
# Hlt2DisplVertices : a Line for displaced vertices hunting
#
# Maintainer : Neal Gauvin 
# Date       : 16 july 2009
# Revised    : 12 November 2009
#
# 3 steps :
#      - Reconstruction of all vertices with >= 5 tracks
#        with PatPV3D, with optimized cuts for "smaller" vertices
#        than PV's.
#      - Hlt2DisplVertices loops on all reconstructed vertices.
#        The one with lowest z (upstream) is not considered
#        Vertices with at least one backward track are not considered.
#        Vertices close to the beam line removed.
#        To allow for kinematical cuts, association is done between the Velo
#        tracks participating to the vertex and the reconstructed Hlt2Pions.
#        When no Hlt2Pions found, default pions with 0.4 GeV pt are created.
#        Vertices that are found to be close to the detector material
#        could be possibly eliminated.
#      - Hlt2LonglivedParts : basic cuts on the selected candidates
#        2 kinds of cuts are applied to the set of displaced vertices :
#           >1 prey passing thighter cuts
#               --> when hunting single long-lived particles
#           >2 preys passing looser cuts
#               --> when looking for 2 particles, coming from a mother one.
#
#
############################################################

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
import GaudiKernel.SystemOfUnits as units

class Hlt2DisplVerticesLinesConf(HltLinesConfigurableUser) :
    
    __slots__ = {  "MinNbTracks"   : 0
                ,  "RCutMethod"    : "FromUpstreamPV"
                ,  "RMin"         : { 'Hlt2LineDisplVertices' : 0.3 * units.mm
                                    , 'Hlt2SingleLonglived' : 0.5 * units.mm
                                    , 'Hlt2DoubleLonglived' : 0.3 * units.mm
                                      }
                ,  "MinMass"      : { 'Hlt2LineDisplVertices' : 2*units.GeV
                                    , 'Hlt2SingleLonglived' : 6.5*units.GeV
                                    , 'Hlt2DoubleLonglived' : 2*units.GeV
                                      }
                ,  "MinSumpt"     :{ 'Hlt2LineDisplVertices' : 0*units.GeV
                                   , 'Hlt2SingleLonglived' : 6.*units.GeV
                                   , 'Hlt2DoubleLonglived' : 2*units.GeV
                                      } 
                ,  "RemVtxFromDet" : 1*units.mm
                }
    
    
    def __apply_configuration__(self) :
        
        from HltLine.HltLine import Hlt2Line
        from Configurables import GaudiSequencer, HltANNSvc
        from Configurables import PatPV3D, PVOfflineTool, PVSeed3DTool, LSAdaptPV3DFitter, Hlt2DisplVertices, Hlt2DisplVerticesDEV, Hlt2LonglivedPartsDEV
        from Hlt2SharedParticles.BasicParticles import NoCutsPions
        from HltLine.HltReco import PV3D

	from HltLine.Hlt2TrackingConfigurations import Hlt2UnfittedForwardTracking
        Hlt2UnfittedForwardTracking = Hlt2UnfittedForwardTracking()

        #######################################################################
        # Eventually get primary vertices
        DVSeq = []
        if( self.getProp('RCutMethod') == "FromUpstreamPV" ):
            DVSeq.append( PV3D )
        
        ######################################################################
        # Run PatPV3D : reconstruction of displaced vertices
        Hlt2PatPV3D = PatPV3D("Hlt2DisplVerticesV3D")
        DVSeq.append( Hlt2PatPV3D )
        Hlt2PatPV3D.addTool(PVOfflineTool)
        Hlt2PatPV3D.PVOfflineTool.InputTracks = [(Hlt2UnfittedForwardTracking.hlt2VeloTracking()).outputSelection()]
        Hlt2PatPV3D.PVOfflineTool.PVFitterName = "LSAdaptPV3DFitter"
        Hlt2PatPV3D.PVOfflineTool.PVSeedingName = "PVSeed3DTool"
        Hlt2PatPV3D.PVOfflineTool.addTool(PVSeed3DTool)
        Hlt2PatPV3D.PVOfflineTool.PVSeed3DTool.TrackPairMaxDistance = 0.2*units.mm
        Hlt2PatPV3D.PVOfflineTool.PVSeed3DTool.zMaxSpread = 1*units.mm
        Hlt2PatPV3D.PVOfflineTool.addTool( LSAdaptPV3DFitter)
        Hlt2PatPV3D.PVOfflineTool.LSAdaptPV3DFitter.maxIP2PV = 2*units.mm
        Hlt2PatPV3D.OutputVerticesName = "Rec/Vertices/Hlt2DisplVerticesV3D"

        #######################################################################
        #Get the pions
        DVSeq.append( NoCutsPions )
        
        #######################################################################
        #Run Hlt2DisplVertices : get the candidates
        #Hlt2LineDisplVertices = Hlt2DisplVertices("Hlt2LineDisplVertices")
        Hlt2LineDisplVertices = Hlt2DisplVerticesDEV("Hlt2LineDisplVertices")
        DVSeq.append( Hlt2LineDisplVertices )
        Hlt2LineDisplVertices.InputLocations = [NoCutsPions.outputSelection()]
        Hlt2LineDisplVertices.InputDisplacedVertices = Hlt2PatPV3D.OutputVerticesName 
        Hlt2LineDisplVertices.RCutMethod = self.getProp('RCutMethod')
        Hlt2LineDisplVertices.RMin = self.getProp('RMin')['Hlt2LineDisplVertices']
        Hlt2LineDisplVertices.MinMass = self.getProp('MinMass')['Hlt2LineDisplVertices']
        Hlt2LineDisplVertices.RemVtxFromDet = self.getProp('RemVtxFromDet')
        Hlt2LineDisplVertices.SaveHidValSelCut = False
        Hlt2LineDisplVertices.OutputLevel = 2

        #######################################################################
        #Run Hlt2LongLivedParts : single LLP selection
        InputParts = ["HLT/Hlt2LineDisplVertices"]
        Hlt2SingleLonglived = Hlt2LonglivedPartsDEV("Hlt2SingleLonglived")
        Hlt2SingleLonglived.InputLocations = InputParts
        Hlt2SingleLonglived.MinNbParts = 1
        Hlt2SingleLonglived.RMin = self.getProp('RMin')['Hlt2SingleLonglived']
        Hlt2SingleLonglived.MinMass = self.getProp('MinMass')['Hlt2SingleLonglived']
        Hlt2SingleLonglived.MinSumpt = self.getProp('MinSumpt')['Hlt2SingleLonglived']
        Hlt2SingleLonglived.OutputLevel = 3

        line = Hlt2Line('DisplVerticesSingle'
                        , prescale = self.prescale
                        , HLT = 'HLT_ALL'
                        #, ODIN = 'ODIN_TRGTYP == LHCb.ODIN.TechnicalTrigger'
                        , algos = DVSeq + [ Hlt2SingleLonglived ]
                        , postscale = self.postscale
                        )
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DisplVerticesSingleDecision" : 50280 } )

        #######################################################################
        #Run Hlt2LonglivedParts : double LLP selection
        Hlt2DoubleLonglived = Hlt2LonglivedPartsDEV("Hlt2DoubleLonglived")
        Hlt2DoubleLonglived.InputLocations = InputParts
        Hlt2DoubleLonglived.MinNbParts = 2
        Hlt2DoubleLonglived.RMin = self.getProp('RMin')['Hlt2DoubleLonglived']
        Hlt2DoubleLonglived.MinMass = self.getProp('MinMass')['Hlt2DoubleLonglived']
        Hlt2DoubleLonglived.MinSumpt = self.getProp('MinSumpt')['Hlt2DoubleLonglived']
        Hlt2DoubleLonglived.OutputLevel = 3

        line = Hlt2Line('DisplVerticesDouble'
                        , prescale = self.prescale
                        , HLT = 'HLT_ALL'
                        #, ODIN = 'ODIN_TRGTYP == LHCb.ODIN.TechnicalTrigger'
                        , algos = DVSeq + [ Hlt2DoubleLonglived ]
                        , postscale = self.postscale
                        )
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DisplVerticesDoubleDecision" : 50281 } )

        #######################################################################
