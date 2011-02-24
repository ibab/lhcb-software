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

class Hlt2DisplVerticesLinesConf(HltLinesConfigurableUser) :
    
    __slots__ = { 
            "MinNbTracks"  : { 'Hlt2RV2P' : 4
                               , 'Hlt2SingleDown' : 4
                               , 'Hlt2SingleLonglivedHighFD' : 5
                               , 'Hlt2SingleLonglivedLowMass' : 6
                               , 'Hlt2SingleLonglivedHighMass' : 6
                               , 'Hlt2DoubleLonglived' : 4
                               , 'Hlt2SinglePSLonglived' : 4
                               }
            ,  "RCutMethod"   : "FromUpstreamPV"
            ,  "RMin"         : {   'Hlt2RV2P' : 0.3
                                  , 'Hlt2SingleDown' : 0.5
                                  , 'Hlt2SingleLonglivedHighFD' : 0.7
                                  , 'Hlt2SingleLonglivedLowMass' : 0.3
                                  , 'Hlt2SingleLonglivedHighMass' : 0.3
                                  , 'Hlt2DoubleLonglived' : 0.3
                                  , 'Hlt2SinglePSLonglived' : 0.3
                                  }
            ,  "MinMass"      : {   'Hlt2RV2P' : 2000.
                                  , 'Hlt2SingleDown' : 3000.
                                  , 'Hlt2SingleLonglivedHighFD' : 4500.
                                  , 'Hlt2SingleLonglivedLowMass' : 4500.
                                  , 'Hlt2SingleLonglivedHighMass' : 9000.
                                  , 'Hlt2DoubleLonglived' : 2000.
                                  , 'Hlt2SinglePSLonglived' : 2000.
                                  }
            ,  "MinSumpt"     :{ 'Hlt2RV2P' : 3000.
                                 , 'Hlt2SingleDown' : 3000.
                                 , 'Hlt2SingleLonglivedHighFD' : 4500.
                                 , 'Hlt2SingleLonglivedLowMass' : 4500.
                                 , 'Hlt2SingleLonglivedHighMass' : 10000.
                                 , 'Hlt2DoubleLonglived' : 3000.
                                 , 'Hlt2SinglePSLonglived' : 3000.
                                 } 
            ,  "RemVtxFromDet" : { 'Hlt2RV2P' : 0
                                 , 'Hlt2SingleLonglivedHighFD' : 5
                                 , 'Hlt2SingleLonglivedLowMass' : 5
                                 , 'Hlt2SingleLonglivedHighMass' : 5
                                 , 'Hlt2DoubleLonglived' : 0
                                 , 'Hlt2SinglePSLonglived' : 0
                                   }
            ,  "MinAngle" : { 'Hlt2RV2P' : 0.
                                 , 'Hlt2SingleLonglivedHighFD' : 0.035
                                 , 'Hlt2SingleLonglivedLowMass' : 0.0
                                 , 'Hlt2SingleLonglivedHighMass' : 0.
                                 , 'Hlt2DoubleLonglived' : 0.0
                                 , 'Hlt2SinglePSLonglived' : 0.0
                                   }
            ,  "MinFD" : {  'Hlt2SingleLonglivedHighFD' : 15.
                                   }
            , "PostScale" : { 'Hlt2RV2P' : 1.
                              , 'Hlt2SingleDown' : 1.
                              , 'Hlt2SingleLonglivedHighFD' : 1.
                              , 'Hlt2SingleLonglivedLowMass' : 1.
                              , 'Hlt2SingleLonglivedHighMass' : 1.
                              , 'Hlt2DoubleLonglived' : 1.
                              , 'Hlt2SinglePSLonglived' : 0.0006
                              }
            }
    
    ##
    ## Tighter solution is:
    ##    for Hlt2SingleLonglivedLowMass:  MinAngle can go until 0.035
    ##    for Hlt2SingleLonglivedHighMass:  RMin can go until 0.5
    ##    for Hlt2DoubleLonglived:  MinAngle can go until 0.02
    ##    for Hlt2SingleLonglivedHighFD:  MinFD can go until 20.
    
    def __apply_configuration__(self) :
        
        from HltLine.HltLine import Hlt2Line
        from Configurables import GaudiSequencer, HltANNSvc
        from Configurables import PatPV3D, PVOfflineTool, PVSeed3DTool, LSAdaptPV3DFitter
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions
        from HltTracking.HltPVs import PV3D
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        Hlt2BiKalmanFittedForwardTracking = Hlt2BiKalmanFittedForwardTracking()
        
        #######################################################################
        # Eventually get primary vertices
        DVSeq = []
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
        DVSeq.append( BiKalmanFittedPions )
        
        #######################################################################
        #Run Get the candidates
        from Configurables import Hlt2PreSelDV

        Hlt2RV2P = Hlt2PreSelDV("Hlt2RV2P")
        DVSeq.append( Hlt2RV2P )
        Hlt2RV2P.Inputs = [BiKalmanFittedPions.outputSelection()]
        Hlt2RV2P.Output = 'Hlt2/Hlt2RV2P/Particles'
        Hlt2RV2P.RecVerticesLocation = [Hlt2PatPV3D.OutputVerticesName] 
        Hlt2RV2P.RCutMethod = self.getProp('RCutMethod')
        Hlt2RV2P.RMin = self.getProp('RMin')['Hlt2RV2P']
        Hlt2RV2P.PreyMinMass = self.getProp('MinMass')['Hlt2RV2P']
        Hlt2RV2P.RemVtxFromDet = self.getProp('RemVtxFromDet')['Hlt2RV2P']
        Hlt2RV2P.UseMap = True

        #######################################################################
        #Run Single LLP selection
        from Configurables import Hlt2SelDV
        Hlt2SingleLonglivedLowMass = Hlt2SelDV("Hlt2SingleLonglivedLowMass")
        Hlt2SingleLonglivedLowMass.Inputs = [ Hlt2RV2P.Output ]
        Hlt2SingleLonglivedLowMass.Output = 'Hlt2/Hlt2SingleLonglivedLowMass/Particles'
        Hlt2SingleLonglivedLowMass.MinNBCands = 1
        Hlt2SingleLonglivedLowMass.RMin = self.getProp('RMin')['Hlt2SingleLonglivedLowMass']
        Hlt2SingleLonglivedLowMass.PreyMinMass = self.getProp('MinMass')['Hlt2SingleLonglivedLowMass']
        Hlt2SingleLonglivedLowMass.PreyMinSumpt = self.getProp('MinSumpt')['Hlt2SingleLonglivedLowMass']
        Hlt2SingleLonglivedLowMass.PreyMaxMass = self.getProp('MinMass')['Hlt2SingleLonglivedHighMass']
        Hlt2SingleLonglivedLowMass.PreyMaxSumpt = self.getProp('MinSumpt')['Hlt2SingleLonglivedHighMass']
        Hlt2SingleLonglivedLowMass.NbTracks = self.getProp('MinNbTracks')['Hlt2SingleLonglivedLowMass']
        Hlt2SingleLonglivedLowMass.PreyMinAngle = self.getProp('MinAngle')['Hlt2SingleLonglivedLowMass']
        Hlt2SingleLonglivedLowMass.SaveOnTES = False
        Hlt2SingleLonglivedLowMass.RemVtxFromDet = self.getProp('RemVtxFromDet')['Hlt2SingleLonglivedLowMass']
        line = Hlt2Line( 'DisplVerticesLowMassSingle'
                        , prescale = self.prescale
                        , algos = DVSeq + [ Hlt2SingleLonglivedLowMass ]
                        , postscale = self.getProp('PostScale')['Hlt2SingleLonglivedLowMass']
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DisplVerticesLowMassSingleDecision" : 50280 } )

        from Configurables import Hlt2SelDV
        Hlt2SingleLonglivedHighMass = Hlt2SelDV("Hlt2SingleLonglivedHighMass")
        Hlt2SingleLonglivedHighMass.Inputs = [ Hlt2RV2P.Output ]
        Hlt2SingleLonglivedHighMass.Output = 'Hlt2/Hlt2SingleLonglivedHighMass/Particles'
        Hlt2SingleLonglivedHighMass.MinNBCands = 1
        Hlt2SingleLonglivedHighMass.RMin = self.getProp('RMin')['Hlt2SingleLonglivedHighMass']
        Hlt2SingleLonglivedHighMass.PreyMinMass = self.getProp('MinMass')['Hlt2SingleLonglivedHighMass']
        Hlt2SingleLonglivedHighMass.PreyMinSumpt = self.getProp('MinSumpt')['Hlt2SingleLonglivedHighMass']
        Hlt2SingleLonglivedHighMass.NbTracks = self.getProp('MinNbTracks')['Hlt2SingleLonglivedHighMass']
        Hlt2SingleLonglivedHighMass.PreyMinAngle = self.getProp('MinAngle')['Hlt2SingleLonglivedHighMass']
        Hlt2SingleLonglivedHighMass.RemVtxFromDet = self.getProp('RemVtxFromDet')['Hlt2SingleLonglivedHighMass']
        Hlt2SingleLonglivedHighMass.SaveOnTES = False
                      
        line = Hlt2Line( 'DisplVerticesHighMassSingle'
                        , prescale = self.prescale
                        , algos = DVSeq + [ Hlt2SingleLonglivedHighMass ]
                        , postscale = self.getProp('PostScale')['Hlt2SingleLonglivedHighMass']
                        )
                                               
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DisplVerticesHighMassSingleDecision" : 50281 } )

        
        from Configurables import Hlt2SelDV
        #######################################################################
        #Run Hlt2LonglivedParts : double LLP selection
        Hlt2DoubleLonglived = Hlt2SelDV("Hlt2DoubleLonglived")
        Hlt2DoubleLonglived.Inputs = [ Hlt2RV2P.Output ]
        Hlt2DoubleLonglived.Output = 'Hlt2/Hlt2DoubleLonglived/Particles'
        Hlt2DoubleLonglived.RMin = self.getProp('RMin')['Hlt2DoubleLonglived']
        Hlt2DoubleLonglived.MinNBCands = 2
        Hlt2DoubleLonglived.PreyMinMass = self.getProp('MinMass')['Hlt2DoubleLonglived']
        Hlt2DoubleLonglived.PreyMinSumpt = self.getProp('MinSumpt')['Hlt2DoubleLonglived']
        Hlt2DoubleLonglived.NbTracks = self.getProp('MinNbTracks')['Hlt2DoubleLonglived']
        Hlt2DoubleLonglived.PreyMinAngle = self.getProp('MinAngle')['Hlt2DoubleLonglived']
        Hlt2DoubleLonglived.SaveOnTES = False

        line = Hlt2Line('DisplVerticesDouble'
                        , prescale = self.prescale
                        , algos = DVSeq + [ Hlt2DoubleLonglived ]
                        , postscale = self.getProp('PostScale')['Hlt2DoubleLonglived']
                        )
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DisplVerticesDoubleDecision" : 50282 } )
        
        #######################################################################
        #Run Single LLP selection
        from Configurables import Hlt2SelDV
        Hlt2SinglePSLonglived = Hlt2SelDV("Hlt2SinglePSLonglived")
        Hlt2SinglePSLonglived.Inputs = [ Hlt2RV2P.Output ]
        Hlt2SinglePSLonglived.Output = 'Hlt2/Hlt2SinglePSLonglived/Particles'
        Hlt2SinglePSLonglived.MinNBCands = 1
        Hlt2SinglePSLonglived.RMin = self.getProp('RMin')['Hlt2SinglePSLonglived']
        Hlt2SinglePSLonglived.PreyMinMass = self.getProp('MinMass')['Hlt2SinglePSLonglived']
        Hlt2SinglePSLonglived.PreyMinSumpt = self.getProp('MinSumpt')['Hlt2SinglePSLonglived']
        Hlt2SinglePSLonglived.NbTracks = self.getProp('MinNbTracks')['Hlt2SinglePSLonglived']
        Hlt2SinglePSLonglived.SaveOnTES = False

        line = Hlt2Line( 'DisplVerticesSinglePostScaled'
                        , prescale = self.prescale
                        , algos = DVSeq + [ Hlt2SinglePSLonglived ]
                        , postscale = self.getProp('PostScale')['Hlt2SinglePSLonglived']
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DisplVerticesSinglePostScaledDecision" : 50283 } )
        
        from Configurables import Hlt2SelDV
        Hlt2SingleLonglivedHighFD = Hlt2SelDV("Hlt2SingleLonglivedHighFD")
        Hlt2SingleLonglivedHighFD.Inputs = [ Hlt2RV2P.Output ]
        Hlt2SingleLonglivedHighFD.Output = 'Hlt2/Hlt2SingleLonglivedHighFD/Particles'
        Hlt2SingleLonglivedHighFD.MinNBCands = 1
        Hlt2SingleLonglivedHighFD.RMin = self.getProp('RMin')['Hlt2SingleLonglivedHighFD']
        Hlt2SingleLonglivedHighFD.PreyMinMass = self.getProp('MinMass')['Hlt2SingleLonglivedHighFD']
        Hlt2SingleLonglivedHighFD.PreyMinFD = self.getProp('MinFD')['Hlt2SingleLonglivedHighFD']
        Hlt2SingleLonglivedHighFD.PreyMinAngle = self.getProp('MinAngle')['Hlt2SingleLonglivedHighFD']
        Hlt2SingleLonglivedHighFD.PreyMinSumpt = self.getProp('MinSumpt')['Hlt2SingleLonglivedHighFD']
        Hlt2SingleLonglivedHighFD.NbTracks = self.getProp('MinNbTracks')['Hlt2SingleLonglivedHighFD']
        Hlt2SingleLonglivedHighFD.RemVtxFromDet = self.getProp('RemVtxFromDet')['Hlt2SingleLonglivedHighFD']
        Hlt2SingleLonglivedHighFD.SaveOnTES = False

        line = Hlt2Line( 'DisplVerticesHighFDSingle'
                        , prescale = self.prescale
                        , algos = DVSeq + [ Hlt2SingleLonglivedHighFD ]
                        , postscale = self.getProp('PostScale')['Hlt2SingleLonglivedHighFD']
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DisplVerticesHighFDSingleDecision" : 50284 } )


        #######################################################################
        ## Downstream Line
        ## The downstream track sequence is done here but should probably go to
        ## Hlt2Tracking
        ##

        ## Decoding of the ST
        from HltLine.HltDecodeRaw     import DecodeTT, DecodeIT
        from HltLine.HltLine        import bindMembers
        decode_members =  bindMembers("DVDecodeSTSeq", DecodeTT.members() + DecodeIT.members())

        ## Seeding
        fwdtracks = Hlt2BiKalmanFittedForwardTracking.hlt2ForwardTracking()
        seedTrackOutputLocation    = 'Hlt2/Track/Seeding'
    
        from Configurables    import PatSeeding,PatSeedingTool
        recoSeeding = PatSeeding('DVSeeding', OutputTracksName = seedTrackOutputLocation)
        recoSeeding.addTool(PatSeedingTool, name="PatSeedingTool")
        recoSeeding.PatSeedingTool.UseForward        = True
        recoSeeding.PatSeedingTool.UseForwardTracks  = False
        recoSeeding.PatSeedingTool.ForwardCloneMergeSeg = True
        recoSeeding.PatSeedingTool.MinMomentum = 2000.
        recoSeeding.PatSeedingTool.InputTracksName    = fwdtracks.outputSelection()   
        seedbm_name         = "DVSeedTracking" 
        seedbm_members      = decode_members.members() + [recoSeeding]
        seedbm_output       = seedTrackOutputLocation
        seedbm = bindMembers(seedbm_name, seedbm_members).setOutputSelection(seedbm_output) 

        ## Downstream tracking
        from Configurables    import PatDownstream
        PatDownstream            = PatDownstream('DVPatDownstream')
        PatDownstream.InputLocation    = 'Hlt2/Track/Seeding'
        PatDownstream.OutputLocation    = 'Hlt2/Track/SeedTT'
        PatDownstream.RemoveUsed    = True
        PatDownstream.RemoveAll        = True
        downbm_name         = "DVDownstreamTracking"
        downbm_members      = seedbm.members() + [PatDownstream]
        downbm_output       = 'Hlt2/Track/SeedTT'

        downbm = bindMembers(downbm_name, downbm_members).setOutputSelection(downbm_output)
  
        ### Downstream fitting
        from Configurables import TrackEventFitter, TrackMasterFitter
        downstreamFit_name      = 'DownStagedFastFit'
        downstreamFit           = TrackEventFitter(downstreamFit_name)
        downstreamFit.TracksInContainer    = 'Hlt2/Track/SeedTT'
        downstreamFit.TracksOutContainer    =   'Hlt2/Track/BiKalmanFitted/Downstream'
        downstreamFit.addTool(TrackMasterFitter, name = 'Fitter')
        from TrackFitter.ConfiguredFitters import ConfiguredFastFitter
        fitter = ConfiguredFastFitter( getattr(downstreamFit,'Fitter'))
        fitter.NodeFitter.BiDirectionalFit    = True
        fitter.NodeFitter.Smooth        = True
        fitter.AddDefaultReferenceNodes = True 
        fitter.NumberFitIterations = 3
        fitter.MaxNumberOutliers = 2 
        fitter.UpdateTransport = True
        fitbm_name         = "DVDownstreamFastFitSeq"
        fitbm_members      = downbm.members()+[downstreamFit]
        fitbm_output       = 'Hlt2/Track/BiKalmanFitted/Downstream'
        fitbm = bindMembers(fitbm_name, fitbm_members).setOutputSelection(fitbm_output)

        ## Downstream Vertexing
        from Configurables import LSAdaptPVFitter
        Hlt2PatPV3DDown = PatPV3D("Hlt2DisplVerticesV3DDown")
        Hlt2PatPV3DDown.addTool(PVOfflineTool)
        Hlt2PatPV3DDown.PVOfflineTool.InputTracks = [ 'Hlt2/Track/BiKalmanFitted/Downstream']
        Hlt2PatPV3DDown.OutputVerticesName = "Rec/Vertices/Hlt2DisplVerticesV3DDown"
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
        
        ## Create downstream protoparticle
        from Configurables import NoPIDsParticleMaker,ChargedProtoParticleMaker
        from HltLine.HltLine import bindMembers
        chargedDOWNProtosOutputLocation = "Hlt2/ProtoP/Fitted/Downstream/Charged"
        charged_name = 'Hlt2ProtoPFittedDownstreamChargedProtoPAlg'
        charged      = ChargedProtoParticleMaker(charged_name)
        charged.InputTrackLocation  = ['Hlt2/Track/BiKalmanFitted/Downstream']
        charged.OutputProtoParticleLocation = chargedDOWNProtosOutputLocation
        protobm_name         = "Hlt2ProtoPFittedDownstreamChargedProtosSeq" 
        protobm_members      = fitbm.members()+[ charged ]
        protobm_output       = chargedDOWNProtosOutputLocation
        ChargedProtoNoPIDsMAKER = bindMembers(protobm_name, protobm_members).setOutputSelection(protobm_output)
        BiKalmanFittedChargedDownstreamProtoMaker  = ChargedProtoNoPIDsMAKER
        Hlt2BiKalmanFittedDownstreamPions                                 = NoPIDsParticleMaker("Hlt2BiKalmanFittedDownstreamPions")
        Hlt2BiKalmanFittedDownstreamPions.Particle                        =  "pion"
        Hlt2BiKalmanFittedDownstreamPions.Input                           =  protobm_output
        Hlt2BiKalmanFittedDownstreamPions.WriteP2PVRelations              =  False
        BiKalmanFittedDownstreamPions  = bindMembers( None, [ BiKalmanFittedChargedDownstreamProtoMaker   , Hlt2BiKalmanFittedDownstreamPions   ] )

        
        ## Associated Velo candidate selection
        from Configurables import Hlt2SelDV
        Hlt2SingleDownVelo = Hlt2SelDV("Hlt2SingleDownVelo")
        Hlt2SingleDownVelo.Inputs = [ Hlt2RV2P.Output ]
        Hlt2SingleDownVelo.Output = 'Hlt2/Hlt2SingleDownVelo/Particles'
        Hlt2SingleDownVelo.MinNBCands = 1
        Hlt2SingleDownVelo.MinNBCandsExclusive = True
        Hlt2SingleDownVelo.RMin = self.getProp('RMin')['Hlt2RV2P']
        Hlt2SingleDownVelo.PreyMinMass = self.getProp('MinMass')['Hlt2RV2P']
        Hlt2SingleDownVelo.PreyMinSumpt = self.getProp('MinSumpt')['Hlt2RV2P']
        Hlt2SingleDownVelo.NbTracks = 5
        Hlt2SingleDownVelo.RemVtxFromDet = 5
        Hlt2SingleDownVelo.SaveOnTES = False

        ## Downstream candidate preselection
        from Configurables import Hlt2PreSelDV
        Hlt2RV2PDown = Hlt2PreSelDV("Hlt2RV2PDown")
        Hlt2RV2PDown.Inputs = [BiKalmanFittedDownstreamPions.outputSelection()]
        Hlt2RV2PDown.Output = 'Hlt2/Hlt2RV2PDown/Particles'
        Hlt2RV2PDown.RecVerticesLocation = [Hlt2PatPV3DDown.OutputVerticesName] 
        Hlt2RV2PDown.RCutMethod = self.getProp('RCutMethod')
        Hlt2RV2PDown.RMin = self.getProp('RMin')['Hlt2SingleDown']
        Hlt2RV2PDown.PreyMinMass = self.getProp('MinMass')['Hlt2SingleDown']
        Hlt2RV2PDown.RemVtxFromDet = 0
        Hlt2RV2PDown.UseMap = False
        Hlt2RV2PDown.KeepLowestZ = True
        Hlt2RV2PDown.NbTracks = self.getProp('MinNbTracks')['Hlt2SingleDown']


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
        Hlt2SingleDown.SaveOnTES = False

        line = Hlt2Line( 'DisplVerticesSingleDown'
                        , prescale = self.prescale
                        , algos = DVSeq + [Hlt2SingleDownVelo,fitbm ,Hlt2PatPV3DDown ,BiKalmanFittedDownstreamPions,Hlt2RV2PDown,Hlt2SingleDown ]
                        , postscale = 1.
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DisplVerticesSingleDownDecision" : 50285 } )

