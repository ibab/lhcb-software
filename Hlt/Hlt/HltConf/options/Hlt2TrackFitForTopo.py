#** @file 
#
#  Redo Hlt exclusive selections particle making with refit tracks.
#  Based on HltConf/Hlt2Particles.opts
#
#---------------------------------------------------------------------
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import CombineParticles, PhysDesktop

#---------------------------------------------------------------------
# Charged ProtoParticles
#
# Do not run this as part of the Hlt2 by default.  Include it before
# algorithms that require the refit particles.
#---------------------------------------------------------------------
SeqHlt2TFParticlesForTopo = GaudiSequencer('SeqHlt2TFParticlesForTopo')
#GaudiSequencer('Hlt2').Members += [ SeqHlt2TFParticlesForTopo ]

SeqHlt2TFParticlesForTopo.MeasureTime = 1
SeqHlt2TFParticlesForTopo.IgnoreFilterPassed = 1  # do all

SeqHlt2TFChargedForTopo = GaudiSequencer('SeqHlt2TFChargedForTopo')
SeqHlt2TFParticlesForTopo.Members += [ SeqHlt2TFChargedForTopo ]
SeqHlt2TFChargedForTopo.MeasureTime = 1

#---------------------------------------------------------------------
# @todo TEMPORARY kill huge events
#
# This shouldn't need to be done in this seqence.  It is done in the main
# Hlt2 sequence.
#
# If it does need to be included in the sequence, it doesn't need to be
# configured---it is already configured in HltConf/Hlt2Particles.opts
#---------------------------------------------------------------------
#from Configurables import NumberOfTracksFilter
#SeqHlt2TFChargedForTopo.Members += [ NumberOfTracksFilter('NumberOfTracksFilter') ]

#---------------------------------------------------------------------
# MC truth associated tracks
#---------------------------------------------------------------------
#SeqTrueSignalTracks = GaudiSequencer('SeqTrueSignalTracks')
#SeqHlt2TFChargedForTopo.Members += [ SeqTrueSignalTracks ]     # debug

#---------------------------------------------------------------------
# TF tracks rather than hacking
#
# Modified so that the refit does not modify the existing tracks.
#---------------------------------------------------------------------
from Configurables import TrackEventFitter, TrackMasterFitter
from Configurables import TrackKalmanFilter, TrackMasterExtrapolator
Hlt2TFTrackFitForTopo = TrackEventFitter('Hlt2TFTrackFitForTopo')
SeqHlt2TFChargedForTopo.Members += [ Hlt2TFTrackFitForTopo ]

Hlt2TFTrackFitForTopo.TracksInContainer  = "Hlt/Track/Forward"
Hlt2TFTrackFitForTopo.TracksOutContainer = "Hlt/Track/TFForwardForTopo"

Hlt2TFTrackFitForTopo.addTool(TrackMasterFitter, name = 'Fitter')
from TrackFitter.ConfiguredFitters import ConfiguredFastFitter
ConfiguredFastFitter( getattr(Hlt2TFTrackFitForTopo,'Fitter'))

#---------------------------------------------------------------------
# Calo Reco & PIDs --------
#
# Calo matching
#
# This can't work as it is written here because the calo matching is
# run on the original tracks before the refit tracks are created.
# It should not rerun the calo matching for the refit tracks.
# If we need to redo the calo matching, we will need to either clone the
# results, or clone the sequence and all its algorithms under a different
# name.
#---------------------------------------------------------------------
#importOptions($CALORECOROOT/options/HltCaloSeq.opts)
#SeqHlt2TFParticlesForTopo.Members += [ GaudiSequencer('HltRecoCALOSeq') ]


#// @todo temporary : redefine HLT track location to "Hlt/Track/ForwardCleaned"
#from Configurables import InSpdAcceptanceAlg, InPrsAcceptanceAlg
#from Configurables import InHcalAcceptanceAlg, InEcalAcceptanceAlg
#from Configurables import InBremAcceptanceAlg, PhotonMatchAlg, BremMatchAlg
#from Configurables import ElectronMatchAlg, Track2SpdEAlg, Track2PrsEAlg
#from Configurables import Track2EcalEAlg, Track2HcalEAlg, EcalChi22ID
#from Configurables import BremChi22ID, ClusChi22ID
#lclCaloTrackContainers = [ "Hlt/Track/Forward", "Hlt/Track/TFForwardForTopo" ]
#InSpdAcceptanceAlg('HltInSPD').Inputs       = lclCaloTrackContainers
#InPrsAcceptanceAlg('HltInPRS').Inputs       = lclCaloTrackContainers
#InHcalAcceptanceAlg('HltInHCAL').Inputs     = lclCaloTrackContainers
#InEcalAcceptanceAlg('HltInECAL').Inputs     = lclCaloTrackContainers
#InBremAcceptanceAlg('HltInBREM').Inputs     = lclCaloTrackContainers
#PhotonMatchAlg('HltClusterMatch').Tracks    = lclCaloTrackContainers
#BremMatchAlg('HltBremMatch').Tracks         = lclCaloTrackContainers
#ElectronMatchAlg('HltElectronMatch').Tracks = lclCaloTrackContainers
#Track2SpdEAlg('HltSpdE').Inputs             = lclCaloTrackContainers
#Track2PrsEAlg('HltPrsE').Inputs             = lclCaloTrackContainers
#Track2EcalEAlg('HltEcalE').Inputs           = lclCaloTrackContainers
#Track2HcalEAlg('HltHcalE').Inputs           = lclCaloTrackContainers
#EcalChi22ID('HltEcalChi22ID').Tracks        = lclCaloTrackContainers
#ClusChi22ID('HltClusChi22ID').Tracks        = lclCaloTrackContainers
#BremChi22ID('HltBremChi22ID').Tracks        = lclCaloTrackContainers


#---------------------------------------------------------------------
# ChargedProtoPAlg
#---------------------------------------------------------------------
from Configurables import ChargedProtoPAlg, ChargedProtoCombineDLLsAlg
Hlt2TFChargedForTopoProtoPAlg = ChargedProtoPAlg('Hlt2TFChargedForTopoProtoPAlg')
Hlt2TFChargedForTopoProtoCombDLL = ChargedProtoCombineDLLsAlg('Hlt2TFChargedForTopoProtoCombDLL')
SeqHlt2TFParticlesForTopo.Members += [ Hlt2TFChargedForTopoProtoPAlg
                                   , Hlt2TFChargedForTopoProtoCombDLL ]

Hlt2TFChargedForTopoProtoPAlg.InputTrackLocation = "Hlt/Track/TFForwardForTopo"
Hlt2TFChargedForTopoProtoPAlg.OutputProtoParticleLocation = "Hlt/ProtoP/TFChargedForTopo"
Hlt2TFChargedForTopoProtoCombDLL.ProtoParticleLocation = "Hlt/ProtoP/TFChargedForTopo"


#---------------------------------------------------------------------
# ProtoParticles
#---------------------------------------------------------------------
#Hlt2TFChargedForTopoProtoPAlg.InputRichPIDLocation = "Rec/Rich/HltPIDs"
Hlt2TFChargedForTopoProtoPAlg.InputMuonPIDLocation = "Hlt/Muon/MuonPID"
#/ Calo PID
Hlt2TFChargedForTopoProtoPAlg.UseCaloSpdPID = 1
Hlt2TFChargedForTopoProtoPAlg.UseCaloPrsPID = 1
Hlt2TFChargedForTopoProtoPAlg.UseCaloEcalPID = 1
Hlt2TFChargedForTopoProtoPAlg.UseCaloHcalPID = 1
Hlt2TFChargedForTopoProtoPAlg.UseCaloBremPID = 1
#Hlt2TFChargedForTopoProtoPAlg.UseRichPID = 0 # Protos will NOT have any RICH information - HltRichPIDsTFKaonsForTopo will not work
Hlt2TFChargedForTopoProtoPAlg.UseRichPID = 1  # Use this to add RICH info to the HLT protos, needed for HltRichPIDsTFKaonsForTopo
Hlt2TFChargedForTopoProtoPAlg.UseMuonPID = 1
Hlt2TFChargedForTopoProtoPAlg.UseVeloPID = 0


#/---------------------------------------------------------------------
#/ Charged Particles - Here make all to Pi and K
#/---------------------------------------------------------------------
from Configurables import NoPIDsParticleMaker
from Configurables import CombinedParticleMaker, TrackSelector
from Configurables import ProtoParticleCALOFilter, ProtoParticleMUONFilter
Hlt2TFPionsForTopo = NoPIDsParticleMaker('Hlt2TFPionsForTopo')
Hlt2TFKaonsForTopo = NoPIDsParticleMaker('Hlt2TFKaonsForTopo')
SeqHlt2TFParticlesForTopo.Members += [ Hlt2TFPionsForTopo, Hlt2TFKaonsForTopo ]

Hlt2TFPionsForTopo.Inputs = [ "Hlt/ProtoP/TFChargedForTopo" ]
Hlt2TFPionsForTopo.Particle = "pion"

Hlt2TFKaonsForTopo.Inputs = [ "Hlt/ProtoP/TFChargedForTopo" ]
Hlt2TFKaonsForTopo.Particle = "kaon"

#* To get the RICH kaons
#Hlt2TFKaonsForTopo = CombinedParticleMaker("Hlt2TFKaonsForTopo")
#Hlt2TFKaonsForTopo.InputProtoParticles = "Hlt/ProtoP/TFChargedForTopo"
#Hlt2TFKaonsForTopo.Particles = ["kaon"]
#Hlt2TFKaonsForTopo.addTool(TrackSelector)
#Hlt2TFKaonsForTopo.TrackSelector.TrackTypes = ["Long"]
#Hlt2TFKaonsForTopo.addTool(ProtoParticleCALOFilter('Kaon'))
#Hlt2TFKaonsForTopo.Kaon.Selection = [ "RequiresDet='RICH' CombDLL(k-pi)>'-5.0'" ]
#Hlt2TFKaonsForTopo.DecayDescriptor = "Kaon"


#---------------------------------------------------------------------
# Kaons using RICH HLT reco results
#---------------------------------------------------------------------
HltRichPIDsTFKaonsForTopo = CombinedParticleMaker('HltRichPIDsTFKaonsForTopo')
SeqHlt2TFParticlesForTopo.Members += [ HltRichPIDsTFKaonsForTopo ]

HltRichPIDsTFKaonsForTopo.ExclusiveSelection = 0
HltRichPIDsTFKaonsForTopo.InputProtoParticles = "Hlt/ProtoP/TFChargedForTopo"
HltRichPIDsTFKaonsForTopo.Particles = ["kaon"]
HltRichPIDsTFKaonsForTopo.addTool(TrackSelector())
HltRichPIDsTFKaonsForTopo.TrackSelector.TrackTypes = ["Long"]
HltRichPIDsTFKaonsForTopo.addTool(ProtoParticleCALOFilter('Kaon'))
HltRichPIDsTFKaonsForTopo.Kaon.Selection = ["RequiresDet='RICH' CombDLL(k-pi)>'-5.0'"]

#---------------------------------------------------------------------
# Muons sequence
#---------------------------------------------------------------------
SeqHlt2TFMuonsForTopo = GaudiSequencer('SeqHlt2TFMuonsForTopo')
SeqHlt2TFParticlesForTopo.Members += [ SeqHlt2TFMuonsForTopo ]
SeqHlt2TFMuonsForTopo.IgnoreFilterPassed = 1

#---------------------------------------------------------------------
# Muons from Long tracks
#---------------------------------------------------------------------
Hlt2TFMuonsForTopo = CombinedParticleMaker('Hlt2TFMuonsForTopo')
SeqHlt2TFMuonsForTopo.Members += [ Hlt2TFMuonsForTopo ]

Hlt2TFMuonsForTopo.InputProtoParticles =  "/Event/Hlt/ProtoP/TFChargedForTopo"
Hlt2TFMuonsForTopo.Particles = ["muon"]
Hlt2TFMuonsForTopo.addTool(ProtoParticleMUONFilter('Muon'))
Hlt2TFMuonsForTopo.Muon.Selection = ["RequiresDet='MUON'"]
Hlt2TFMuonsForTopo.addTool(TrackSelector())
Hlt2TFMuonsForTopo.TrackSelector.TrackTypes = ["Long"]
Hlt2TFMuonsForTopo.DecayDescriptor = "Muon"

#---------------------------------------------------------------------
# Special case for electrons 
#---------------------------------------------------------------------
Hlt2TFElectronsForTopo = CombinedParticleMaker('Hlt2TFElectronsForTopo')
SeqHlt2TFParticlesForTopo.Members += [ Hlt2TFElectronsForTopo ]

Hlt2TFElectronsForTopo.Particles =  ["electron"]
Hlt2TFElectronsForTopo.InputProtoParticles =  "/Event/Hlt/ProtoP/TFChargedForTopo"
Hlt2TFElectronsForTopo.addTool(ProtoParticleCALOFilter('Electron'))
Hlt2TFElectronsForTopo.Electron.Selection = ["RequiresDet='CALO' CombDLL(e-pi)>'-2.0'"]


