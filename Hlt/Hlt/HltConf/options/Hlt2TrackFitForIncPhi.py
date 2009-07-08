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
Hlt2IncPhiTFTrackFitSeq = GaudiSequencer( "Hlt2IncPhiTFTrackFitSeq" )
Hlt2IncPhiTFParticlesSeq.Members += [ Hlt2IncPhiTFTrackFitSeq ]

Hlt2IncPhiTFTrackFit = TrackEventFitter('Hlt2IncPhiTFTrackFit')
Hlt2IncPhiTFTrackFitSeq.Members += [ Hlt2IncPhiTFTrackFit ]

Hlt2IncPhiTFTrackFit.TracksInContainer  = "Hlt/Track/Long"
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

from Configurables import ChargedProtoPAlg, TrackSelector, DelegatingTrackSelector #, ChargedProtoCombineDLLsAlg
Hlt2IncPhiTFChargedProtoPAlg = ChargedProtoPAlg('Hlt2IncPhiTFChargedProtoPAlg')
#Hlt2IncPhiTFChargedProtoCombDLL = ChargedProtoCombineDLLsAlg('Hlt2IncPhiTFChargedProtoCombDLL')
Hlt2IncPhiTFMakeProtoSeq.Members += [ Hlt2IncPhiTFChargedProtoPAlg ]
#                                   , Hlt2IncPhiTFChargedProtoCombDLL ]


Hlt2IncPhiTFChargedProtoPAlg.InputTrackLocation = Hlt2IncPhiTFTrackFit.TracksOutContainer
Hlt2IncPhiTFChargedProtoPAlg.OutputProtoParticleLocation = "Hlt/ProtoP/TFChargedForIncPhi"
# Clones will not be accepted
Hlt2IncPhiTFChargedProtoPAlg.addTool(DelegatingTrackSelector, name = 'TrackSelector')
Hlt2IncPhiTFChargedProtoPAlg.TrackSelector.TrackTypes = ["Long"]
Hlt2IncPhiTFChargedProtoPAlg.TrackSelector.addTool( TrackSelector, name="Long")
Hlt2IncPhiTFChargedProtoPAlg.TrackSelector.Long.AcceptClones = False

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


Hlt2IncPhiTFKaons.Input = Hlt2IncPhiTFChargedProtoPAlg.OutputProtoParticleLocation 
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
RichTrackCreatorConfig().InputTracksLocation = Hlt2IncPhiTFTrackFit.TracksOutContainer


from Configurables import ChargedProtoPAlg, ChargedProtoCombineDLLsAlg, TrackSelector
importOptions("$HLTCONFROOT/options/Hlt2Rich.py")


Hlt2IncPhiRichChargedProtoPAlg = ChargedProtoPAlg('Hlt2IncPhiRichChargedProtoPAlg')
Hlt2IncPhiRichChargedProtoCombDLL = ChargedProtoCombineDLLsAlg('Hlt2IncPhiRichChargedProtoCombDLL')
Hlt2IncPhiRichParticlesSeq.Members += [ GaudiSequencer("HltRICHReco"), Hlt2IncPhiRichChargedProtoPAlg
                                   , Hlt2IncPhiRichChargedProtoCombDLL ]

Hlt2IncPhiRichChargedProtoPAlg.InputTrackLocation = Hlt2IncPhiTFTrackFit.TracksOutContainer
Hlt2IncPhiRichChargedProtoPAlg.OutputProtoParticleLocation = "Hlt/ProtoP/RichChargedForIncPhi"
# Clones will not be accepted
Hlt2IncPhiRichChargedProtoPAlg.addTool(DelegatingTrackSelector, name = 'TrackSelector')
Hlt2IncPhiRichChargedProtoPAlg.TrackSelector.TrackTypes = ["Long"]
Hlt2IncPhiRichChargedProtoPAlg.TrackSelector.addTool( TrackSelector, name="Long")
Hlt2IncPhiRichChargedProtoPAlg.TrackSelector.Long.AcceptClones = False

Hlt2IncPhiRichChargedProtoCombDLL.ProtoParticleLocation = Hlt2IncPhiRichChargedProtoPAlg.OutputProtoParticleLocation 

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
Hlt2IncPhiRichPIDsKaons = CombinedParticleMaker('Hlt2IncPhiRichPIDsKaons')
Hlt2IncPhiRichParticlesSeq.Members += [ Hlt2IncPhiRichPIDsKaons ]

Hlt2IncPhiRichPIDsKaons.Input = Hlt2IncPhiRichChargedProtoPAlg.OutputProtoParticleLocation 
Hlt2IncPhiRichPIDsKaons.Particle = "kaon"
Hlt2IncPhiRichPIDsKaons.addTool(TrackSelector())
Hlt2IncPhiRichPIDsKaons.TrackSelector.TrackTypes = ["Long"]
Hlt2IncPhiRichPIDsKaons.addTool(ProtoParticleCALOFilter('Kaon'))
Hlt2IncPhiRichPIDsKaons.Kaon.Selection = ["RequiresDet='RICH' CombDLL(k-pi)>'-10.0'"]
