"""
Module for the selection of events with displaced vertices.

The selection of displaced vertices is achieved in 3 steps :
      - Reconstruction of all vertices with >= 4 tracks
        with PatPV3D, with optimized cuts for smaller vertices
        than PV's.
      - Hlt2PreSelDV loops on all reconstructed vertices.
        The one with lowest z (upstream) is not considered
        Vertices with at least one backward track are not considered.
        Vertices close to the beam line removed.
        To allow for kinematical cuts, association is done between the Velo
        tracks participating to the vertex and the reconstructed Hlt2Pions.
        When no Hlt2Pions found, default pions with 0.4 GeV pt are created.
        Vertices that are found to be close to the detector material
        could be possibly eliminated.
      - Hlt2SelDV : basic cuts on the selected candidates
        2 kinds of cuts are applied to the set of displaced vertices :
           >1 prey passing thighter cuts
               --> when hunting single long-lived particles
           >2 preys passing looser cuts
               --> when looking for 2 particles, coming from a mother one.

Maintainer : Neal Gauvin
Please do not edit without the permission of the author.

Exported symbols (use python help!):
      - Hlt2DisplVerticesLinesConf

"""

__author__ = ['Neal Gauvin']
__date__ = '12/01/2009'
__version__ = '$Revision: 1.22 $'
__all__ = ('Hlt2DisplVerticesLinesConf')

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
import GaudiKernel.SystemOfUnits as units

class Hlt2DisplVerticesLinesConf(HltLinesConfigurableUser) :
    
    __slots__ = {  "MinNbTracks"  : { 'Hlt2RV2P' : 4
                                    , 'Hlt2SingleLonglived' : 5
                                    , 'Hlt2SinglePSLonglived' : 4  
                                    , 'Hlt2DoubleLonglived' : 4
                                      }
                ,  "RCutMethod"   : "FromUpstreamPV"
                ,  "RMin"         : { 'Hlt2RV2P' : 0.3 * units.mm
                                    , 'Hlt2SingleLonglived' : 0.3 * units.mm
                                    , 'Hlt2SinglePSLonglived' : 0.3 * units.mm
                                    , 'Hlt2DoubleLonglived' : 0.3 * units.mm
                                      }
                ,  "MinMass"      : { 'Hlt2RV2P' : 1.5*units.GeV
                                      , 'Hlt2SingleLonglived' : 6.*units.GeV
                                      , 'Hlt2SinglePSLonglived' : 4.5*units.GeV
                                      , 'Hlt2DoubleLonglived' : 1.5*units.GeV
                                      }
                ,  "MinSumpt"     :{ 'Hlt2RV2P' : 0*units.GeV
                                     , 'Hlt2SingleLonglived' : 6*units.GeV
                                     , 'Hlt2SinglePSLonglived' : 4.5*units.GeV
                                     , 'Hlt2DoubleLonglived' : 3.*units.GeV
                                      } 
                ,  "RemVtxFromDet" : { 'Hlt2RV2P' : 0
                                   , 'Hlt2SingleLonglived' : 0
                                   , 'Hlt2SinglePSLonglived' : 0  
                                   , 'Hlt2DoubleLonglived' : 0
                                      }
                , "DetDist"  : { 'Hlt2RV2P' : 1
                                 , 'Hlt2SingleLonglived' : 1
                                 , 'Hlt2SinglePSLonglived' : 1
                                 , 'Hlt2DoubleLonglived' : 1
                                      }
                , "PostScale" : { 'Hlt2RV2P' : 1
                                 , 'Hlt2SingleLonglived' : 1
                                 , 'Hlt2SinglePSLonglived' : 0.01
                                 , 'Hlt2DoubleLonglived' : 1
                                      }
                }
    
    
    def __apply_configuration__(self) :
        
        from HltLine.HltLine import Hlt2Line
        from Configurables import GaudiSequencer, HltANNSvc
        from Configurables import PatPV3D, PVOfflineTool, PVSeed3DTool, LSAdaptPV3DFitter
        #from Hlt2SharedParticles.BasicParticles import NoCutsPions
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions
        from HltTracking.HltPVs import PV3D
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        Hlt2BiKalmanFittedForwardTracking = Hlt2BiKalmanFittedForwardTracking()
        #######################################################################
        # Eventually get primary vertices
        DVSeq = []
        if( self.getProp('RCutMethod') == "FromUpstreamPV" ):
            DVSeq.append( PV3D() )
        
        ######################################################################
        # Run PatPV3D : reconstruction of displaced vertices
        Hlt2PatPV3D = PatPV3D("Hlt2DisplVerticesV3D")
        DVSeq.append( Hlt2PatPV3D )
        Hlt2PatPV3D.addTool(PVOfflineTool)
        Hlt2PatPV3D.PVOfflineTool.InputTracks = [(Hlt2BiKalmanFittedForwardTracking.hlt2VeloTracking()).outputSelection()]
        Hlt2PatPV3D.PVOfflineTool.PVFitterName = "LSAdaptPV3DFitter"
        Hlt2PatPV3D.PVOfflineTool.PVSeedingName = "PVSeed3DTool"
        Hlt2PatPV3D.PVOfflineTool.addTool(PVSeed3DTool)
        Hlt2PatPV3D.PVOfflineTool.PVSeed3DTool.TrackPairMaxDistance = 0.2*units.mm
        Hlt2PatPV3D.PVOfflineTool.PVSeed3DTool.zMaxSpread = 1*units.mm
        Hlt2PatPV3D.PVOfflineTool.PVSeed3DTool.MinCloseTracks = 3
        Hlt2PatPV3D.PVOfflineTool.addTool( LSAdaptPV3DFitter)
        Hlt2PatPV3D.PVOfflineTool.LSAdaptPV3DFitter.maxIP2PV = 2*units.mm
        Hlt2PatPV3D.PVOfflineTool.LSAdaptPV3DFitter.MinTracks = 4
        Hlt2PatPV3D.OutputVerticesName = "Rec/Vertices/Hlt2DisplVerticesV3D"

        #######################################################################
        #Get the pions
        #DVSeq.append( NoCutsPions )
        DVSeq.append( BiKalmanFittedPions )
        
        #######################################################################
        #Run Get the candidates
        from Configurables import Hlt2PreSelDV

        Hlt2RV2P = Hlt2PreSelDV("Hlt2RV2P")
        DVSeq.append( Hlt2RV2P )
        #Hlt2RV2P.InputLocations = [NoCutsPions.outputSelection()]
        Hlt2RV2P.InputLocations = [BiKalmanFittedPions.outputSelection()]
        Hlt2RV2P.RecVerticesLocation = [Hlt2PatPV3D.OutputVerticesName] 
        Hlt2RV2P.RCutMethod = self.getProp('RCutMethod')
        Hlt2RV2P.RMin = self.getProp('RMin')['Hlt2RV2P']
        Hlt2RV2P.PreyMinMass = self.getProp('MinMass')['Hlt2RV2P']
        Hlt2RV2P.RemVtxFromDet = self.getProp('RemVtxFromDet')['Hlt2RV2P']
        Hlt2RV2P.DetDist = self.getProp('DetDist')['Hlt2RV2P']
        Hlt2RV2P.UseMap = True

        #######################################################################
        #Run Single LLP selection
        from Configurables import Hlt2SelDV
        Hlt2SingleLonglived = Hlt2SelDV("Hlt2SingleLonglived")
        Hlt2SingleLonglived.InputLocations = [ Hlt2RV2P.getName() ]
        Hlt2SingleLonglived.MinNBCands = 1
        Hlt2SingleLonglived.RMin = self.getProp('RMin')['Hlt2SingleLonglived']
        Hlt2SingleLonglived.PreyMinMass = self.getProp('MinMass')['Hlt2SingleLonglived']
        Hlt2SingleLonglived.PreyMinSumpt = self.getProp('MinSumpt')['Hlt2SingleLonglived']
        Hlt2SingleLonglived.NbTracks = self.getProp('MinNbTracks')['Hlt2SingleLonglived']
        Hlt2SingleLonglived.SigmaR = 0.1*units.mm
        Hlt2SingleLonglived.SigmaZ = 1*units.mm
        Hlt2SingleLonglived.SaveOnTES = False

        line = Hlt2Line( 'DisplVerticesSingle'
                         #, VoidFilter = ''
                         #, VoidFilter = " CONTAINS( 'Hlt/Track/Velo') > 349"
                        , prescale = self.prescale
                        , algos = DVSeq + [ Hlt2SingleLonglived ]
                        , postscale = self.getProp('PostScale')['Hlt2SingleLonglived']
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DisplVerticesSingleDecision" : 50280 } )

        #######################################################################
        #Run Single LLP selection
        from Configurables import Hlt2SelDV
        Hlt2SinglePSLonglived = Hlt2SelDV("Hlt2SinglePSLonglived")
        Hlt2SinglePSLonglived.InputLocations = [ Hlt2RV2P.getName() ]
        Hlt2SinglePSLonglived.MinNBCands = 1
        Hlt2SinglePSLonglived.RMin = self.getProp('RMin')['Hlt2SinglePSLonglived']
        Hlt2SinglePSLonglived.PreyMinMass = self.getProp('MinMass')['Hlt2SinglePSLonglived']
        Hlt2SinglePSLonglived.PreyMinSumpt = self.getProp('MinSumpt')['Hlt2SinglePSLonglived']
        Hlt2SinglePSLonglived.NbTracks = self.getProp('MinNbTracks')['Hlt2SinglePSLonglived']
        Hlt2SinglePSLonglived.SaveOnTES = False

        line = Hlt2Line( 'DisplVerticesSinglePostScaled'
                        , prescale = self.prescale
                        , algos = DVSeq + [ Hlt2SingleLonglived ]
                        , postscale = self.getProp('PostScale')['Hlt2SinglePSLonglived']
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DisplVerticesSinglePostScaledDecision" : 50282 } )

        #######################################################################
        #Run Hlt2LonglivedParts : double LLP selection
        Hlt2DoubleLonglived = Hlt2SelDV("Hlt2DoubleLonglived")
        Hlt2DoubleLonglived.InputLocations = [ Hlt2RV2P.getName() ]
        Hlt2DoubleLonglived.MinNBCands = 2
        Hlt2DoubleLonglived.RMin = self.getProp('RMin')['Hlt2DoubleLonglived']
        Hlt2DoubleLonglived.PreyMinMass = self.getProp('MinMass')['Hlt2DoubleLonglived']
        Hlt2DoubleLonglived.PreyMinSumpt = self.getProp('MinSumpt')['Hlt2DoubleLonglived']
        Hlt2DoubleLonglived.NbTracks = self.getProp('MinNbTracks')['Hlt2SingleLonglived']
        Hlt2DoubleLonglived.SaveOnTES = False

        line = Hlt2Line('DisplVerticesDouble'
                         #, VoidFilter = ''
                         #, VoidFilter = " CONTAINS( 'Hlt/Track/Velo') > 349"
                        , prescale = self.prescale
                        , algos = DVSeq + [ Hlt2DoubleLonglived ]
                        , postscale = self.getProp('PostScale')['Hlt2SingleLonglived']
                        )
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DisplVerticesDoubleDecision" : 50281 } )

        #######################################################################
        
