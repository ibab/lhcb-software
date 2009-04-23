###################################################################### 
#     HLT Inc Phi Kalman fitted and RICH particles
######################################################################

from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import CombineParticles, PhysDesktop

######################################################################
# Kalman fitted particles sequencer
######################################################################
Hlt2IncPhiTFParticlesSeq = GaudiSequencer('Hlt2IncPhiTFParticlesSeq')
Hlt2IncPhiTFParticlesSeq.MeasureTime = 1


######################################################################
# Do kalman fitting
######################################################################
from Configurables import TrackEventFitter, TrackMasterFitter
from Configurables import TrackKalmanFilter, TrackMasterExtrapolator
Hlt2IncPhiTFTrackFitSeq = GaudiSequencer( "Hlt2IncPhiTFTrackFitSeq" )
Hlt2IncPhiTFParticlesSeq.Members += [ Hlt2IncPhiTFTrackFitSeq ]

Hlt2IncPhiTFTrackFit = TrackEventFitter('Hlt2IncPhiTFTrackFit')
Hlt2IncPhiTFTrackFitSeq.Members += [ Hlt2IncPhiTFTrackFit ]

Hlt2IncPhiTFTrackFit.TracksInContainer  = "Hlt/Track/Forward"
Hlt2IncPhiTFTrackFit.TracksOutContainer = "Hlt/Track/TFForwardForIncPhi"

Hlt2IncPhiTFTrackFit.addTool(TrackMasterFitter, name = 'Fitter')
from TrackFitter.ConfiguredFitters import ConfiguredFastFitter
fitter = ConfiguredFastFitter( getattr(Hlt2IncPhiTFTrackFit,'Fitter'))

fitter.NodeFitter.BiDirectionalFit = True
fitter.NodeFitter.Smooth = True
fitter.ZPositions = [ 990., 9450.]

######################################################################
# Set up Kalman fitted ChargedProtoPAlg
######################################################################
Hlt2IncPhiTFMakeProtoSeq = GaudiSequencer("Hlt2IncPhiTFMakeProtoSeq")
Hlt2IncPhiTFParticlesSeq.Members += [ Hlt2IncPhiTFMakeProtoSeq ]

from Configurables import ChargedProtoPAlg #, ChargedProtoCombineDLLsAlg
Hlt2IncPhiTFChargedProtoPAlg = ChargedProtoPAlg('Hlt2IncPhiTFChargedProtoPAlg')
#Hlt2IncPhiTFChargedProtoCombDLL = ChargedProtoCombineDLLsAlg('Hlt2IncPhiTFChargedProtoCombDLL')
Hlt2IncPhiTFMakeProtoSeq.Members += [ Hlt2IncPhiTFChargedProtoPAlg ]
#                                   , Hlt2IncPhiTFChargedProtoCombDLL ]

Hlt2IncPhiTFChargedProtoPAlg.InputTrackLocation = "Hlt/Track/TFForwardForIncPhi"
Hlt2IncPhiTFChargedProtoPAlg.OutputProtoParticleLocation = "Hlt/ProtoP/TFChargedForIncPhi"
#Hlt2IncPhiTFChargedProtoCombDLL.ProtoParticleLocation = "Hlt/ProtoP/IncPhiTFCharged"

######################################################################
# Kalman fitted ProtoParticles
######################################################################
Hlt2IncPhiTFChargedProtoPAlg.UseCaloSpdPID = 0
Hlt2IncPhiTFChargedProtoPAlg.UseCaloPrsPID = 0
Hlt2IncPhiTFChargedProtoPAlg.UseCaloEcalPID = 0
Hlt2IncPhiTFChargedProtoPAlg.UseCaloHcalPID = 0
Hlt2IncPhiTFChargedProtoPAlg.UseCaloBremPID = 0
Hlt2IncPhiTFChargedProtoPAlg.UseRichPID = 0
Hlt2IncPhiTFChargedProtoPAlg.UseMuonPID = 0
Hlt2IncPhiTFChargedProtoPAlg.UseVeloPID = 0

######################################################################
# Kalman fitted Charged Particles
######################################################################
from Configurables import NoPIDsParticleMaker
from Configurables import CombinedParticleMaker, TrackSelector
from Configurables import ProtoParticleCALOFilter
Hlt2IncPhiTFKaons = NoPIDsParticleMaker('Hlt2IncPhiTFKaons')
Hlt2IncPhiTFMakeProtoSeq.Members += [ Hlt2IncPhiTFKaons ]

Hlt2IncPhiTFKaons.Input =  "Hlt/ProtoP/TFChargedForIncPhi" 
Hlt2IncPhiTFKaons.Particle = "kaon"

######################################################################
# Kaons using RICH HLT reco results
######################################################################

######################################################################
# Rich particles sequencer
######################################################################


Hlt2IncPhiRichParticlesSeq = GaudiSequencer('Hlt2IncPhiRichParticlesSeq')

######################################################################
# Set up Rich ChargedProtoPAlg
######################################################################
from Configurables import RichTrackCreatorConfig
RichTrackCreatorConfig().InputTracksLocation = "Hlt/Track/TFForwardForIncPhi"


from Configurables import ChargedProtoPAlg, ChargedProtoCombineDLLsAlg
importOptions("$HLTCONFROOT/options/Hlt2Rich.py")


Hlt2IncPhiRichChargedProtoPAlg = ChargedProtoPAlg('Hlt2IncPhiRichChargedProtoPAlg')
Hlt2IncPhiRichChargedProtoCombDLL = ChargedProtoCombineDLLsAlg('Hlt2IncPhiRichChargedProtoCombDLL')
Hlt2IncPhiRichParticlesSeq.Members += [ GaudiSequencer("HltRICHReco"), Hlt2IncPhiRichChargedProtoPAlg
                                   , Hlt2IncPhiRichChargedProtoCombDLL ]

Hlt2IncPhiRichChargedProtoPAlg.InputTrackLocation = "Hlt/Track/TFForwardForIncPhi"
Hlt2IncPhiRichChargedProtoPAlg.OutputProtoParticleLocation = "Hlt/ProtoP/RichChargedForIncPhi"
Hlt2IncPhiRichChargedProtoCombDLL.ProtoParticleLocation = "Hlt/ProtoP/RichChargedForIncPhi"

######################################################################
# Rich ProtoParticles
######################################################################
Hlt2IncPhiRichChargedProtoPAlg.InputRichPIDLocation = "Rec/Rich/HltPIDs"
Hlt2IncPhiRichChargedProtoPAlg.UseCaloSpdPID = 0
Hlt2IncPhiRichChargedProtoPAlg.UseCaloPrsPID = 0
Hlt2IncPhiRichChargedProtoPAlg.UseCaloEcalPID = 0
Hlt2IncPhiRichChargedProtoPAlg.UseCaloHcalPID = 0
Hlt2IncPhiRichChargedProtoPAlg.UseCaloBremPID = 0
Hlt2IncPhiRichChargedProtoPAlg.UseRichPID = 1
Hlt2IncPhiRichChargedProtoPAlg.UseMuonPID = 0
Hlt2IncPhiRichChargedProtoPAlg.UseVeloPID = 0

######################################################################
# Rich Charged Particles
######################################################################
HltIncPhiRichPIDsKaons = CombinedParticleMaker('HltIncPhiRichPIDsKaons')
Hlt2IncPhiRichParticlesSeq.Members += [ HltIncPhiRichPIDsKaons ]

HltIncPhiRichPIDsKaons.Input = "Hlt/ProtoP/RichChargedForIncPhi"
HltIncPhiRichPIDsKaons.Particle = "kaon"
HltIncPhiRichPIDsKaons.addTool(TrackSelector())
HltIncPhiRichPIDsKaons.TrackSelector.TrackTypes = ["Long"]
HltIncPhiRichPIDsKaons.addTool(ProtoParticleCALOFilter('Kaon'))
HltIncPhiRichPIDsKaons.Kaon.Selection = ["RequiresDet='RICH' CombDLL(k-pi)>'-1.0'"]


