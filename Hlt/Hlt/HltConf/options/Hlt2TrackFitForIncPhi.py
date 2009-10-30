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
fitter.AddDefaultReferenceNodes = True    # says Wouter


######################################################################
# Set up Kalman fitted ChargedProtoPAlg
######################################################################
Hlt2IncPhiTFMakeProtoSeq = GaudiSequencer("Hlt2IncPhiTFMakeProtoSeq")
Hlt2IncPhiTFParticlesSeq.Members += [ Hlt2IncPhiTFMakeProtoSeq ]

from Configurables import ChargedProtoPAlg, TrackSelector
Hlt2IncPhiTFChargedProtoPAlg = ChargedProtoPAlg('Hlt2IncPhiTFChargedProtoPAlg')
Hlt2IncPhiTFMakeProtoSeq.Members += [ Hlt2IncPhiTFChargedProtoPAlg ]

Hlt2IncPhiTFChargedProtoPAlg.InputTrackLocation = Hlt2IncPhiTFTrackFit.TracksOutContainer
Hlt2IncPhiTFChargedProtoPAlg.OutputProtoParticleLocation = "Hlt/ProtoP/TFChargedForIncPhi"
# Clones will not be accepted
Hlt2IncPhiTFChargedProtoPAlg.addTool(TrackSelector, name = 'TrackSelector')
Hlt2IncPhiTFChargedProtoPAlg.TrackSelector.AcceptClones = False


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
# Add RICH info to protoparticles
######################################################################
from Configurables import RichTrackCreatorConfig,ChargedProtoParticleAddRichInfo,ChargedProtoCombineDLLsAlg

# Set up RICH PID sequencer (HltRICHReco must be added at appropriate point)
importOptions('$HLTCONFROOT/options/Hlt2Rich.py')

# Create RICH PID using the TF tracks
RichTrackCreatorConfig().InputTracksLocation = Hlt2IncPhiTFTrackFit.TracksOutContainer

# Add RICH PID to Inc Phi TF protoparticles
Hlt2IncPhiAddRichInfo = ChargedProtoParticleAddRichInfo('Hlt2IncPhiAddRichInfo')
Hlt2IncPhiAddRichInfo.ProtoParticleLocation = Hlt2IncPhiTFChargedProtoPAlg.OutputProtoParticleLocation
Hlt2IncPhiAddCombInfo = ChargedProtoCombineDLLsAlg('Hlt2IncPhiAddCombInfo')
Hlt2IncPhiAddCombInfo.ProtoParticleLocation = Hlt2IncPhiTFChargedProtoPAlg.OutputProtoParticleLocation


