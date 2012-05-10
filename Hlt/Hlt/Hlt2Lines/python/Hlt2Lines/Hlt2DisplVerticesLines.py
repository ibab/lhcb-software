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

__author__ = ['Neal Gauvin','Victor Coco']
__date__ = '12/01/2009'
__version__ = '$Revision: 1.22 $'
__all__ = ('Hlt2DisplVerticesLinesConf')

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
import GaudiKernel.SystemOfUnits as units

from HltLine.HltLine import bindMembers

class Hlt2DisplVerticesLinesConf(HltLinesConfigurableUser) :
    
    __slots__ = {
        "FilterVelo" : True ,
        "VeloIP" : -0.05 ,
        "UseVeloTracks": False,
        "FracE1Track": 0.85,
        "FracTrackBefV": 0.49,
        "MaxChi2Long": 5.,
        "MaxChi2Down": 10.,
        "MinNbTracks"  : { 'Hlt2RV2P' : 4
                           , 'Hlt2SingleLonglived' : 4
                           , 'Hlt2SingleDown' : 4
                           , 'Hlt2PreSelSingleDown' : 5
                           , 'Hlt2SingleLonglivedHighFD' :  5
                           , 'Hlt2SingleLonglivedHighMass' : 6
                           , 'Hlt2DoubleLonglived' : 4
                           , 'Hlt2SinglePSLonglived' : 4
                           , 'Hlt2DoublePSLonglived' : 4
                           , 'Hlt2SinglePSLonglivedHighMass' : 5
                           , 'Hlt2SinglePSLonglivedHighFD' : 5
                           , 'Hlt2SinglePSLonglivedMV' : 4
                           }
        ,  "RMin"         : {   'Hlt2RV2P' : 0.4
                                , 'Hlt2SingleLonglived' : 2.0
                                , 'Hlt2SingleDown' : 2.0 ##2.5 dans '3'
                                , 'Hlt2PreSelSingleDown' : 0.4
                                , 'Hlt2SingleLonglivedHighFD' : 4.
                                , 'Hlt2SingleLonglivedHighMass' : 0.7
                                , 'Hlt2DoubleLonglived' : 0.4 
                                , 'Hlt2SinglePSLonglived' : 0.4
                                , 'Hlt2DoublePSLonglived' : 0.4
                                , 'Hlt2SinglePSLonglivedHighMass' : 0.65
                                , 'Hlt2SinglePSLonglivedHighFD' : 2.5
                                , 'Hlt2SinglePSLonglivedMV' : 0.4  
                                }
        ,  "MinMass"      : {   'Hlt2RV2P' : 2800.
                                , 'Hlt2SingleLonglived' : 4000.
                                , 'Hlt2SingleDown' : 3000
                                , 'Hlt2PreSelSingleDown' : 4500.
                                , 'Hlt2SingleLonglivedHighFD' : 3000.
                                , 'Hlt2SingleLonglivedHighMass' : 8700.
                                , 'Hlt2DoubleLonglived' : 2800.
                                , 'Hlt2SinglePSLonglived' : 2800.
                                , 'Hlt2DoublePSLonglived' : 2800.
                                , 'Hlt2SinglePSLonglivedHighMass' : 8700.
                                , 'Hlt2SinglePSLonglivedHighFD' : 3000.
                                , 'Hlt2SinglePSLonglivedMV' :3000.
                                
                                }
        ,  "HighestMass"  : {   'Hlt2DoubleLonglived' : 3300.
                                ,'Hlt2DoublePSLonglived' : 2800.
                                }
        ,  "MinSumpt"     :{ 'Hlt2RV2P' : 0.
                             , 'Hlt2SingleLonglived' : 2600.
                             , 'Hlt2SingleDown' : 2600.
                             , 'Hlt2PreSelSingleDown' : 2600.
                             , 'Hlt2SingleLonglivedHighFD' : 2600.
                             , 'Hlt2SingleLonglivedHighMass' : 2600
                             , 'Hlt2DoubleLonglived' : 2600.
                             , 'Hlt2SinglePSLonglived' : 0.
                             , 'Hlt2DoublePSLonglived' : 2600.
                             , 'Hlt2SinglePSLonglivedHighMass' : 2600.
                             , 'Hlt2SinglePSLonglivedHighFD' : 2600.
                             , 'Hlt2SinglePSLonglivedMV' :0.
                             }
        
        ,  "ComputeMatterVeto" : { 'Hlt2RV2P' : True
                                   , 'Hlt2SingleLonglived' : 5
                                   , 'Hlt2SingleLonglivedHighFD' : 5
                                   , 'Hlt2SingleLonglivedHighMass' : 5
                                   , 'Hlt2DoubleLonglived' : 5
                                   , 'Hlt2SingleDown' : 0
                                   , 'Hlt2SinglePSLonglived' : 0
                                   , 'Hlt2DoublePSLonglived' : 0
                                   , 'Hlt2SinglePSLonglivedHighMass' : 5
                                   , 'Hlt2SinglePSLonglivedHighFD' : 5
                                   , 'Hlt2SinglePSLonglivedMV' : 5
                                   }
        ,  "AllOutDet"  : {  'Hlt2DoubleLonglived' : False
                             ,'Hlt2DoublePSLonglived' : False
                             }
        , "PostScale" : { 'Hlt2RV2P' : 1.
                          , 'Hlt2SingleLonglived' : 1.
                          , 'Hlt2SingleDown' : 1.
                          , 'Hlt2SingleLonglivedHighFD' : 1.
                          , 'Hlt2SingleLonglivedHighMass' : 1.
                          , 'Hlt2DoubleLonglived' : 1.
                          , 'Hlt2SinglePSLonglived' : 0.0005
                          , 'Hlt2DoublePSLonglived' : 0.01
                          , 'Hlt2SinglePSLonglivedHighMass' : 1.
                          , 'Hlt2SinglePSLonglivedHighFD' : 1.
                          , 'Hlt2SinglePSLonglivedMV' : 0.0005
                          }  
        }                 
    
    ##
    ## Tighter solution is prescaling the PS lines:
    ## Hlt2SinglePSLonglived
    ## Hlt2DoublePSLonglived
    ## Hlt2SinglePSLonglivedHighMass
    ## Hlt2SinglePSLonglivedHighFD
    ## Hlt2SinglePSLonglivedMV

    

    
    def __apply_configuration__(self) :

        from HltLine.HltLine import Hlt2Line
        from Configurables import GaudiSequencer, HltANNSvc
        from Configurables import PatPV3D, PVOfflineTool, PVSeed3DTool, LSAdaptPV3DFitter
        
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedDownPions
        from HltTracking.HltPVs import PV3D
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedDownstreamTracking

        Hlt2BiKalmanFittedDownstreamTracking = Hlt2BiKalmanFittedDownstreamTracking()
        Hlt2BiKalmanFittedForwardTracking = Hlt2BiKalmanFittedForwardTracking()
        PV3D = PV3D()
        
        print 'PV3DSelection',PV3D.outputSelection()

        from HltLine.HltLine import bindMembers
        
        #######################################################################
        # Get the primary vertices and run the Hlt2 Velo Tracking
        hlt2VeloTracking = Hlt2BiKalmanFittedForwardTracking.hlt2VeloTracking()
        from Configurables import VeloWithIP
        vwithIP = VeloWithIP('VeloWithIP')
        vwithIP.Inputs = [hlt2VeloTracking.outputSelection()]
        vwithIP.Output = 'Hlt2/Hlt2DVTrack/VeloWithIP'
        vwithIP.PVLocation = 'Hlt/Vertex/PV3D'
        if self.getProp("VeloIP")>0.:
            vwithIP.MinIP = self.getProp("VeloIP")
            vwithIP.DiscardFromPV = False
        else :
            vwithIP.DiscardFromPV = True
        
        from Configurables import LoKi__VoidFilter
        FilterNumDisplVeloTracks = LoKi__VoidFilter('Hlt2FilterNumDisplVeloTracks', Code="CONTAINS('Hlt2/Hlt2DVTrack/VeloWithIP')>3")
        
        ######################################################################
        # Run PatPV3D : reconstruction of displaced vertices
        Hlt2PatPV3D = PatPV3D("Hlt2DisplVertices3D")
        Hlt2PatPV3D.addTool(PVOfflineTool)
        if self.getProp('FilterVelo'):
            Hlt2PatPV3D.PVOfflineTool.InputTracks = [ vwithIP.Output ]
        else:
            Hlt2PatPV3D.PVOfflineTool.InputTracks = [ hlt2VeloTracking.outputSelection()  ]
        Hlt2PatPV3D.PVOfflineTool.PVFitterName = "LSAdaptPV3DFitter"
        Hlt2PatPV3D.PVOfflineTool.PVSeedingName = "PVSeed3DTool"
        Hlt2PatPV3D.PVOfflineTool.addTool(PVSeed3DTool)
        Hlt2PatPV3D.PVOfflineTool.PVSeed3DTool.TrackPairMaxDistance = 0.2*units.mm
        Hlt2PatPV3D.PVOfflineTool.PVSeed3DTool.zMaxSpread = 1*units.mm
        Hlt2PatPV3D.PVOfflineTool.PVSeed3DTool.MinCloseTracks = 3
        Hlt2PatPV3D.PVOfflineTool.addTool( LSAdaptPV3DFitter)
        Hlt2PatPV3D.PVOfflineTool.LSAdaptPV3DFitter.maxIP2PV = 2*units.mm
        Hlt2PatPV3D.PVOfflineTool.LSAdaptPV3DFitter.MinTracks = 4
        Hlt2PatPV3D.OutputVerticesName = "Hlt2/Hlt2VeloVertices/Vertices"
        
        veloV = None
        if self.getProp('FilterVelo'):
            veloV = bindMembers('VeloVForDV',  [ hlt2VeloTracking, PV3D ,vwithIP,FilterNumDisplVeloTracks,Hlt2PatPV3D ]).setOutputSelection( Hlt2PatPV3D.OutputVerticesName )
        else :
            veloV = bindMembers('VeloVForDV',  [ hlt2VeloTracking, PV3D ,Hlt2PatPV3D ]).setOutputSelection( Hlt2PatPV3D.OutputVerticesName )
        #######################################################################
        #Get the pions
        
        #######################################################################
        #Run Get the candidates
        from Configurables import Hlt2PreSelDV
        Hlt2RV2P = Hlt2PreSelDV("Hlt2RV2P")
        Hlt2RV2P.Inputs = [BiKalmanFittedPions.outputSelection()]
        Hlt2RV2P.Output = 'Hlt2/Hlt2RV2P/Particles'
        Hlt2RV2P.RecVerticesLocation = [Hlt2PatPV3D.OutputVerticesName] 
        Hlt2RV2P.RMin = self.getProp('RMin')['Hlt2RV2P']
        Hlt2RV2P.PreyMinMass = self.getProp('MinMass')['Hlt2RV2P']
        Hlt2RV2P.ComputeMatterVeto = self.getProp('ComputeMatterVeto')['Hlt2RV2P']
        Hlt2RV2P.UseMap = True
        Hlt2RV2P.UseVeloTracks = self.getProp("UseVeloTracks")
        Hlt2RV2P.FractionEFrom1Track = 1000.
        Hlt2RV2P.FractionTrackUpstreamV  = 1000.
        Hlt2RV2P.MaxChi2Long  = self.getProp("MaxChi2Long")
        
        veloCand = bindMembers('VeloV2PForDV', [  veloV, BiKalmanFittedPions,BiKalmanFittedDownPions ,Hlt2RV2P ]).setOutputSelection( Hlt2RV2P.Output  )

        #######################################################################
        ## Single LLP Selection
        #High Mass
        from Configurables import Hlt2SelDV
        Hlt2SingleLonglivedHighMass = Hlt2SelDV("Hlt2SingleLonglivedHighMass")
        Hlt2SingleLonglivedHighMass.Inputs = [ Hlt2RV2P.Output ]
        Hlt2SingleLonglivedHighMass.Output = 'Hlt2/Hlt2SingleLonglivedHighMass/Particles'
        Hlt2SingleLonglivedHighMass.MinNBCands = 1
        Hlt2SingleLonglivedHighMass.RMin = self.getProp('RMin')['Hlt2SingleLonglivedHighMass']
        Hlt2SingleLonglivedHighMass.PreyMinMass = self.getProp('MinMass')['Hlt2SingleLonglivedHighMass']
        Hlt2SingleLonglivedHighMass.PreyMinSumpt = self.getProp('MinSumpt')['Hlt2SingleLonglivedHighMass']
        Hlt2SingleLonglivedHighMass.NbTracks = self.getProp('MinNbTracks')['Hlt2SingleLonglivedHighMass']
        #Hlt2SingleLonglivedHighMass.PreyMinAngle = self.getProp('MinAngle')['Hlt2SingleLonglivedHighMass']
        Hlt2SingleLonglivedHighMass.RemVtxFromDet = self.getProp('ComputeMatterVeto')['Hlt2SingleLonglivedHighMass']
        Hlt2SingleLonglivedHighMass.UseVelo = self.getProp('UseVeloTracks')
        Hlt2SingleLonglivedHighMass.FractionEFrom1Track = self.getProp("FracE1Track")
        Hlt2SingleLonglivedHighMass.FractionTrackUpstreamV  = self.getProp("FracTrackBefV")

        line = Hlt2Line( 'DisplVerticesHighMassSingle'
                        , prescale = self.prescale
                        , algos = veloCand.members() + [ Hlt2SingleLonglivedHighMass ]
                        , postscale = self.getProp('PostScale')['Hlt2SingleLonglivedHighMass']
                        )
                                               
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DisplVerticesHighMassSingleDecision" : 50281 } )


        #######################################################################
        #Run Hlt2LonglivedParts : double LLP selection
        from Configurables import Hlt2SelDV
        Hlt2DoubleLonglived = Hlt2SelDV("Hlt2DoubleLonglived")
        Hlt2DoubleLonglived.Inputs = [ Hlt2RV2P.Output ]
        Hlt2DoubleLonglived.Output = 'Hlt2/Hlt2DoubleLonglived/Particles'
        Hlt2DoubleLonglived.RMin = self.getProp('RMin')['Hlt2DoubleLonglived']
        Hlt2DoubleLonglived.MinNBCands = 2
        Hlt2DoubleLonglived.AllOutDet = self.getProp('AllOutDet')['Hlt2DoubleLonglived']
        Hlt2DoubleLonglived.PreyMinHighestMass = self.getProp('HighestMass')['Hlt2DoubleLonglived']
        Hlt2DoubleLonglived.PreyMinMass = self.getProp('MinMass')['Hlt2DoubleLonglived']
        Hlt2DoubleLonglived.PreyMinSumpt = self.getProp('MinSumpt')['Hlt2DoubleLonglived']
        Hlt2DoubleLonglived.NbTracks = self.getProp('MinNbTracks')['Hlt2DoubleLonglived']
        #Hlt2DoubleLonglived.PreyMinAngle = self.getProp('MinAngle')['Hlt2DoubleLonglived']
        Hlt2DoubleLonglived.RemVtxFromDet = self.getProp('ComputeMatterVeto')['Hlt2DoubleLonglived']
        Hlt2DoubleLonglived.UseVelo = self.getProp('UseVeloTracks')
        Hlt2DoubleLonglived.FractionEFrom1Track = self.getProp("FracE1Track")
        Hlt2DoubleLonglived.FractionTrackUpstreamV  = self.getProp("FracTrackBefV")

        line = Hlt2Line('DisplVerticesDouble'
                        , prescale = self.prescale
                        , algos = veloCand.members() + [ Hlt2DoubleLonglived ]
                        , postscale = self.getProp('PostScale')['Hlt2DoubleLonglived']
                        )
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DisplVerticesDoubleDecision" : 50282 } )
        

        ## Single LLP Selection
        #High FD:
        from Configurables import Hlt2SelDV
        from Configurables import Hlt2SelDV
        Hlt2SingleLonglivedHighFD = Hlt2SelDV("Hlt2SingleLonglivedHighFD")
        Hlt2SingleLonglivedHighFD.Inputs = [ Hlt2RV2P.Output ]
        Hlt2SingleLonglivedHighFD.Output = 'Hlt2/Hlt2SingleLonglivedHighFD/Particles'
        Hlt2SingleLonglivedHighFD.MinNBCands = 1
        Hlt2SingleLonglivedHighFD.RMin = self.getProp('RMin')['Hlt2SingleLonglivedHighFD']
        Hlt2SingleLonglivedHighFD.PreyMinMass = self.getProp('MinMass')['Hlt2SingleLonglivedHighFD']
        #Hlt2SingleLonglivedHighFD.PreyMinFD = self.getProp('MinFD')['Hlt2SingleLonglivedHighFD']
        #Hlt2SingleLonglivedHighFD.PreyMinAngle = self.getProp('MinAngle')['Hlt2SingleLonglivedHighFD']
        Hlt2SingleLonglivedHighFD.PreyMinSumpt = self.getProp('MinSumpt')['Hlt2SingleLonglivedHighFD']
        Hlt2SingleLonglivedHighFD.NbTracks = self.getProp('MinNbTracks')['Hlt2SingleLonglivedHighFD']
        Hlt2SingleLonglivedHighFD.RemVtxFromDet = self.getProp('ComputeMatterVeto')['Hlt2SingleLonglivedHighFD']
        Hlt2SingleLonglivedHighFD.UseVelo = self.getProp('UseVeloTracks')
        Hlt2SingleLonglivedHighFD.FractionEFrom1Track = self.getProp("FracE1Track")
        Hlt2SingleLonglivedHighFD.FractionTrackUpstreamV  = self.getProp("FracTrackBefV")
        line = Hlt2Line( 'DisplVerticesHighFDSingle'
                        , prescale = self.prescale
                        , algos = veloCand.members() + [ Hlt2SingleLonglivedHighFD ]
                        , postscale = self.getProp('PostScale')['Hlt2SingleLonglivedHighFD']
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DisplVerticesHighFDSingleDecision" : 50284 } )


        #Intermediate Mass and FD:
        from Configurables import Hlt2SelDV
        Hlt2SingleLonglived = Hlt2SelDV("Hlt2SingleLonglived")
        Hlt2SingleLonglived.Inputs = [ Hlt2RV2P.Output ]
        Hlt2SingleLonglived.Output = 'Hlt2/Hlt2SingleLonglived/Particles'
        Hlt2SingleLonglived.MinNBCands = 1
        Hlt2SingleLonglived.RMin = self.getProp('RMin')['Hlt2SingleLonglived']
        Hlt2SingleLonglived.PreyMinMass = self.getProp('MinMass')['Hlt2SingleLonglived']
        Hlt2SingleLonglived.PreyMinSumpt = self.getProp('MinSumpt')['Hlt2SingleLonglived']
        Hlt2SingleLonglived.NbTracks = self.getProp('MinNbTracks')['Hlt2SingleLonglived']
        Hlt2SingleLonglived.RemVtxFromDet = self.getProp('ComputeMatterVeto')['Hlt2SingleLonglived']
        Hlt2SingleLonglived.UseVelo = self.getProp('UseVeloTracks')
        Hlt2SingleLonglived.FractionEFrom1Track = self.getProp("FracE1Track")
        Hlt2SingleLonglived.FractionTrackUpstreamV  = self.getProp("FracTrackBefV")
        line = Hlt2Line( 'DisplVerticesSingle'
                        , prescale = self.prescale
                        , algos = veloCand.members() + [ Hlt2SingleLonglived ]
                        , postscale = self.getProp('PostScale')['Hlt2SingleLonglived']
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DisplVerticesSingleDecision" : 50290 } )
        
        #######################################################################
        # Prescaled Lines
        #######################################################################
        from Configurables import Hlt2SelDV
        Hlt2SinglePSLonglived = Hlt2SelDV("Hlt2SinglePSLonglived")
        Hlt2SinglePSLonglived.Inputs = [ Hlt2RV2P.Output ]
        Hlt2SinglePSLonglived.Output = 'Hlt2/Hlt2SinglePSLonglived/Particles'
        Hlt2SinglePSLonglived.MinNBCands = 1
        Hlt2SinglePSLonglived.RMin = self.getProp('RMin')['Hlt2SinglePSLonglived']
        Hlt2SinglePSLonglived.PreyMinMass = self.getProp('MinMass')['Hlt2SinglePSLonglived']
        Hlt2SinglePSLonglived.PreyMinSumpt = self.getProp('MinSumpt')['Hlt2SinglePSLonglived']
        Hlt2SinglePSLonglived.NbTracks = self.getProp('MinNbTracks')['Hlt2SinglePSLonglived']
        Hlt2SinglePSLonglived.RemVtxFromDet = self.getProp('ComputeMatterVeto')['Hlt2SinglePSLonglived']
        Hlt2SinglePSLonglived.UseVelo = self.getProp('UseVeloTracks')
        Hlt2SinglePSLonglived.FractionEFrom1Track = 1000.
        Hlt2SinglePSLonglived.FractionTrackUpstreamV  = 1000.
        line = Hlt2Line( 'DisplVerticesSinglePostScaled'
                        , prescale = self.prescale
                        , algos = veloCand.members() + [ Hlt2SinglePSLonglived ]
                        , postscale = self.getProp('PostScale')['Hlt2SinglePSLonglived']
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DisplVerticesSinglePostScaledDecision" : 50283 } )
        
        from Configurables import Hlt2SelDV
        Hlt2DoublePSLonglived = Hlt2SelDV("Hlt2DoublePSLonglived")
        Hlt2DoublePSLonglived.Inputs = [ Hlt2RV2P.Output ]
        Hlt2DoublePSLonglived.Output = 'Hlt2/Hlt2DoublePSLonglived/Particles'
        Hlt2DoublePSLonglived.MinNBCands = 2
        Hlt2DoublePSLonglived.RMin = self.getProp('RMin')['Hlt2DoublePSLonglived']
        Hlt2DoublePSLonglived.PreyMinMass = self.getProp('MinMass')['Hlt2DoublePSLonglived']
        Hlt2DoublePSLonglived.NbTracks = self.getProp('MinNbTracks')['Hlt2DoublePSLonglived']
        Hlt2DoublePSLonglived.RemVtxFromDet = self.getProp('ComputeMatterVeto')['Hlt2DoublePSLonglived']
        Hlt2DoublePSLonglived.UseVelo = self.getProp('UseVeloTracks')  
        Hlt2DoublePSLonglived.FractionEFrom1Track = 1000.
        Hlt2DoublePSLonglived.FractionTrackUpstreamV  = 1000.          
        line = Hlt2Line( 'DisplVerticesDoublePostScaled'
                        , prescale = self.prescale
                        , algos = veloCand.members() + [ Hlt2DoublePSLonglived ]
                        , postscale = self.getProp('PostScale')['Hlt2DoublePSLonglived']
                        )                                               
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DisplVerticesDoublePostScaledDecision" : 50286 } )
        
        from Configurables import Hlt2SelDV
        Hlt2SinglePSLonglivedHighMass = Hlt2SelDV("Hlt2SinglePSLonglivedHighMass")
        Hlt2SinglePSLonglivedHighMass.Inputs = [ Hlt2RV2P.Output ]
        Hlt2SinglePSLonglivedHighMass.Output = 'Hlt2/Hlt2SinglePSLonglivedHighMass/Particles'
        Hlt2SinglePSLonglivedHighMass.MinNBCands = 1
        Hlt2SinglePSLonglivedHighMass.RMin = self.getProp('RMin')['Hlt2SinglePSLonglivedHighMass']
        Hlt2SinglePSLonglivedHighMass.PreyMinMass = self.getProp('MinMass')['Hlt2SinglePSLonglivedHighMass']
        Hlt2SinglePSLonglivedHighMass.NbTracks = self.getProp('MinNbTracks')['Hlt2SinglePSLonglivedHighMass']
        Hlt2SinglePSLonglivedHighMass.RemVtxFromDet = self.getProp('ComputeMatterVeto')['Hlt2SinglePSLonglivedHighMass']
        Hlt2SinglePSLonglivedHighMass.UseVelo = self.getProp('UseVeloTracks')
        Hlt2SinglePSLonglivedHighMass.FractionEFrom1Track = self.getProp("FracE1Track")
        Hlt2SinglePSLonglivedHighMass.FractionTrackUpstreamV  = self.getProp("FracTrackBefV")
        line = Hlt2Line( 'DisplVerticesSingleHighMassPostScaled'
                        , prescale = self.prescale
                        , algos = veloCand.members() + [ Hlt2SinglePSLonglivedHighMass ]
                        , postscale = self.getProp('PostScale')['Hlt2SinglePSLonglivedHighMass']
                        )
                                               
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DisplVerticesSingleHighMassPostScaledDecision" : 50287 } )

        from Configurables import Hlt2SelDV
        Hlt2SinglePSLonglivedHighFD = Hlt2SelDV("Hlt2SinglePSLonglivedHighFD")
        Hlt2SinglePSLonglivedHighFD.Inputs = [ Hlt2RV2P.Output ]
        Hlt2SinglePSLonglivedHighFD.Output = 'Hlt2/Hlt2SinglePSLonglivedHighFD/Particles'
        Hlt2SinglePSLonglivedHighFD.MinNBCands = 1
        Hlt2SinglePSLonglivedHighFD.RMin = self.getProp('RMin')['Hlt2SinglePSLonglivedHighFD']
        Hlt2SinglePSLonglivedHighFD.PreyMinMass = self.getProp('MinMass')['Hlt2SinglePSLonglivedHighFD']
        Hlt2SinglePSLonglivedHighFD.NbTracks = self.getProp('MinNbTracks')['Hlt2SinglePSLonglivedHighFD']
        Hlt2SinglePSLonglivedHighFD.RemVtxFromDet = self.getProp('ComputeMatterVeto')['Hlt2SinglePSLonglivedHighFD']
        Hlt2SinglePSLonglivedHighFD.UseVelo = self.getProp('UseVeloTracks')  
        Hlt2SinglePSLonglivedHighFD.FractionEFrom1Track = self.getProp("FracE1Track")
        Hlt2SinglePSLonglivedHighFD.FractionTrackUpstreamV  = self.getProp("FracTrackBefV")           
        line = Hlt2Line( 'DisplVerticesSingleHighFDPostScaled'
                        , prescale = self.prescale
                        , algos = veloCand.members() + [ Hlt2SinglePSLonglivedHighFD ]
                        , postscale = self.getProp('PostScale')['Hlt2SinglePSLonglivedHighFD']
                        )
                                               
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DisplVerticesSingleHighFDPostScaledDecision" : 50288 } )

        from Configurables import Hlt2SelDV
        Hlt2SinglePSLonglivedMV = Hlt2SelDV("Hlt2SinglePSLonglivedMV")
        Hlt2SinglePSLonglivedMV.Inputs = [ Hlt2RV2P.Output ]
        Hlt2SinglePSLonglivedMV.Output = 'Hlt2/Hlt2SinglePSLonglivedMV/Particles'
        Hlt2SinglePSLonglivedMV.MinNBCands = 1
        Hlt2SinglePSLonglivedMV.RMin = self.getProp('RMin')['Hlt2SinglePSLonglivedMV']
        Hlt2SinglePSLonglivedMV.PreyMinMass = self.getProp('MinMass')['Hlt2SinglePSLonglivedMV']
        Hlt2SinglePSLonglivedMV.NbTracks = self.getProp('MinNbTracks')['Hlt2SinglePSLonglivedMV']
        Hlt2SinglePSLonglivedMV.RemVtxFromDet = self.getProp('ComputeMatterVeto')['Hlt2SinglePSLonglivedMV']
        Hlt2SinglePSLonglivedMV.UseVelo = self.getProp('UseVeloTracks')   
        Hlt2SinglePSLonglivedMV.FractionEFrom1Track = 1000.
        Hlt2SinglePSLonglivedMV.FractionTrackUpstreamV  = 1000.       
        line = Hlt2Line( 'DisplVerticesSingleMVPostScaled'
                        , prescale = self.prescale
                        , algos = veloCand.members() + [ Hlt2SinglePSLonglivedMV ]
                        , postscale = self.getProp('PostScale')['Hlt2SinglePSLonglivedMV']
                        )
                                               
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DisplVerticesSingleMVPostScaledDecision" : 50289 } )

        #######################################################################
        ## Downstream Line
        ## The downstream track sequence is done here but should probably go to
        ## Hlt2Tracking
        ##
        #######################################################################

        hlt2DownstreamTracking = Hlt2BiKalmanFittedDownstreamTracking.hlt2DownstreamTracking()
        
        ## Downstream Vertexing
        from Configurables import LSAdaptPVFitter
        Hlt2PatPV3DDown = PatPV3D("Hlt2DisplVerticesV3DDown")
        Hlt2PatPV3DDown.addTool(PVOfflineTool)
        Hlt2PatPV3DDown.PVOfflineTool.InputTracks = [ hlt2DownstreamTracking.outputSelection() ]
        Hlt2PatPV3DDown.OutputVerticesName = "Hlt2/Hlt2DownVertices/Vertices"
        Hlt2PatPV3DDown.PVOfflineTool.addTool( LSAdaptPVFitter())
        Hlt2PatPV3DDown.PVOfflineTool.PVFitterName = "LSAdaptPVFitter"
        Hlt2PatPV3DDown.PVOfflineTool.PVSeedingName = "PVSeed3DTool"
        Hlt2PatPV3DDown.PVOfflineTool.addTool(PVSeed3DTool)
        Hlt2PatPV3DDown.PVOfflineTool.RequireVelo = False
        Hlt2PatPV3DDown.PVOfflineTool.PVSeed3DTool.MinCloseTracks = 3 
        Hlt2PatPV3DDown.PVOfflineTool.PVSeed3DTool.TrackPairMaxDistance = 2.*units.mm 
        Hlt2PatPV3DDown.PVOfflineTool.PVSeed3DTool.zMaxSpread = 20.*units.mm
        Hlt2PatPV3DDown.PVOfflineTool.LSAdaptPVFitter.MinTracks = 3
        Hlt2PatPV3DDown.PVOfflineTool.LSAdaptPVFitter.maxChi2 = 400.0
        Hlt2PatPV3DDown.PVOfflineTool.LSAdaptPVFitter.maxDeltaZ = 0.0005 *units.mm 
        Hlt2PatPV3DDown.PVOfflineTool.LSAdaptPVFitter.maxDeltaChi2NDoF = 0.002
        Hlt2PatPV3DDown.PVOfflineTool.LSAdaptPVFitter.acceptTrack = 0.00000001
        Hlt2PatPV3DDown.PVOfflineTool.LSAdaptPVFitter.trackMaxChi2 = 9 
        Hlt2PatPV3DDown.PVOfflineTool.LSAdaptPVFitter.trackMaxChi2Remove = 64
        
        ## Associated Velo candidate selection
        from Configurables import Hlt2SelDV
        Hlt2SingleDownVelo = Hlt2SelDV("Hlt2SingleDownVelo")
        Hlt2SingleDownVelo.Inputs = [ Hlt2RV2P.Output ]
        Hlt2SingleDownVelo.Output = 'Hlt2/Hlt2SingleDownVelo/Particles'
        Hlt2SingleDownVelo.MinNBCands = 1
        Hlt2SingleDownVelo.MinNBCandsExclusive = True
        Hlt2SingleDownVelo.RMin = self.getProp('RMin')['Hlt2PreSelSingleDown']
        Hlt2SingleDownVelo.PreyMinMass = self.getProp('MinMass')['Hlt2PreSelSingleDown']
        Hlt2SingleDownVelo.PreyMinSumpt = self.getProp('MinSumpt')['Hlt2PreSelSingleDown']
        Hlt2SingleDownVelo.NbTracks = self.getProp('MinNbTracks')['Hlt2PreSelSingleDown']
        Hlt2SingleDownVelo.RemVtxFromDet = 5

        
        ## Downstream candidate preselection
        from Configurables import Hlt2PreSelDV
        Hlt2RV2PDown = Hlt2PreSelDV("Hlt2RV2PDown")
        Hlt2RV2PDown.Inputs = [BiKalmanFittedDownPions.outputSelection()]
        Hlt2RV2PDown.Output = 'Hlt2/Hlt2RV2PDown/Particles'
        Hlt2RV2PDown.RecVerticesLocation = [Hlt2PatPV3DDown.OutputVerticesName] 
        Hlt2RV2PDown.RMin = self.getProp('RMin')['Hlt2SingleDown']
        Hlt2RV2PDown.PreyMinMass = self.getProp('MinMass')['Hlt2SingleDown']
        Hlt2RV2PDown.ComputeMatterVeto = False
        Hlt2RV2PDown.UseMap = False
        Hlt2RV2PDown.KeepLowestZ = True
        Hlt2RV2PDown.NbTracks = self.getProp('MinNbTracks')['Hlt2SingleDown']
        Hlt2RV2PDown.MaxChi2Down =  self.getProp("MaxChi2Down")

        ## Downstream candidate selection
        from Configurables import Hlt2SelDV
        Hlt2SingleDown = Hlt2SelDV("Hlt2SingleDown")
        Hlt2SingleDown.Inputs = [ Hlt2RV2PDown.Output ]
        Hlt2SingleDown.Output = 'Hlt2/Hlt2SingleDown/Particles'
        Hlt2SingleDown.MinNBCands = 1
        Hlt2SingleDown.RMin = self.getProp('RMin')['Hlt2SingleDown']
        Hlt2SingleDown.PreyMinMass = self.getProp('MinMass')['Hlt2SingleDown']
        Hlt2SingleDown.PreyMinSumpt = self.getProp('MinSumpt')['Hlt2SingleDown']
        Hlt2SingleDown.MinZ = 200*units.mm
        Hlt2SingleDown.NbTracks = self.getProp('MinNbTracks')['Hlt2SingleDown']
        Hlt2SingleDown.FractionEFrom1Track = 1000.
        Hlt2SingleDown.FractionTrackUpstreamV  = 1000.

        line = Hlt2Line( 'DisplVerticesSingleDown'
                        , prescale = self.prescale
                        , algos = veloCand.members()+ [ Hlt2SingleDownVelo,hlt2DownstreamTracking ,Hlt2PatPV3DDown ,BiKalmanFittedDownPions,Hlt2RV2PDown,Hlt2SingleDown ]
                        , postscale = 1.
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DisplVerticesSingleDownDecision" : 50285 } )

