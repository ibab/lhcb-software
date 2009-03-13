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
SeqHlt2TFParticles = GaudiSequencer('SeqHlt2TFParticles')
#GaudiSequencer('Hlt2').Members += [ SeqHlt2TFParticles ]

SeqHlt2TFParticles.MeasureTime = 1
SeqHlt2TFParticles.IgnoreFilterPassed = 1  # do all

SeqHlt2TFCharged = GaudiSequencer('SeqHlt2TFCharged')
SeqHlt2TFParticles.Members += [ SeqHlt2TFCharged ]
SeqHlt2TFCharged.MeasureTime = 1

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
#SeqHlt2TFCharged.Members += [ NumberOfTracksFilter('NumberOfTracksFilter') ]

#---------------------------------------------------------------------
# MC truth associated tracks
#---------------------------------------------------------------------
#SeqTrueSignalTracks = GaudiSequencer('SeqTrueSignalTracks')
#SeqHlt2TFCharged.Members += [ SeqTrueSignalTracks ]     # debug

#---------------------------------------------------------------------
# TF tracks rather than hacking
#
# Modified so that the refit does not modify the existing tracks.
#---------------------------------------------------------------------
from Configurables import TrackEventFitter, TrackMasterFitter
from Configurables import TrackKalmanFilter, TrackMasterExtrapolator
Hlt2TFTrackFit = TrackEventFitter('Hlt2TFTrackFit')
SeqHlt2TFCharged.Members += [ Hlt2TFTrackFit ]

Hlt2TFTrackFit.TracksInContainer  = "Hlt/Track/Forward"
Hlt2TFTrackFit.TracksOutContainer = "Hlt/Track/TFForward"

Hlt2TFTrackFit.addTool(TrackMasterFitter, name = 'Fitter')
from TrackFitter.ConfiguredFitters import ConfiguredFastFitter
ConfiguredFastFitter( getattr(Hlt2TFTrackFit,'Fitter'))

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
#SeqHlt2TFParticles.Members += [ GaudiSequencer('HltRecoCALOSeq') ]


#// @todo temporary : redefine HLT track location to "Hlt/Track/ForwardCleaned"
#from Configurables import InSpdAcceptanceAlg, InPrsAcceptanceAlg
#from Configurables import InHcalAcceptanceAlg, InEcalAcceptanceAlg
#from Configurables import InBremAcceptanceAlg, PhotonMatchAlg, BremMatchAlg
#from Configurables import ElectronMatchAlg, Track2SpdEAlg, Track2PrsEAlg
#from Configurables import Track2EcalEAlg, Track2HcalEAlg, EcalChi22ID
#from Configurables import BremChi22ID, ClusChi22ID
#lclCaloTrackContainers = [ "Hlt/Track/Forward", "Hlt/Track/TFForward" ]
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
Hlt2TFChargedProtoPAlg = ChargedProtoPAlg('Hlt2TFChargedProtoPAlg')
Hlt2TFChargedProtoCombDLL = ChargedProtoCombineDLLsAlg('Hlt2TFChargedProtoCombDLL')
SeqHlt2TFParticles.Members += [ Hlt2TFChargedProtoPAlg
                                   , Hlt2TFChargedProtoCombDLL ]

Hlt2TFChargedProtoPAlg.InputTrackLocation = "Hlt/Track/TFForward"
Hlt2TFChargedProtoPAlg.OutputProtoParticleLocation = "Hlt/ProtoP/TFCharged"
Hlt2TFChargedProtoCombDLL.ProtoParticleLocation = "Hlt/ProtoP/TFCharged"


#---------------------------------------------------------------------
# ProtoParticles
#---------------------------------------------------------------------
#Hlt2TFChargedProtoPAlg.InputRichPIDLocation = "Rec/Rich/HltPIDs"
Hlt2TFChargedProtoPAlg.InputMuonPIDLocation = "Hlt/Muon/MuonPID"
#/ Calo PID
Hlt2TFChargedProtoPAlg.UseCaloSpdPID = 1
Hlt2TFChargedProtoPAlg.UseCaloPrsPID = 1
Hlt2TFChargedProtoPAlg.UseCaloEcalPID = 1
Hlt2TFChargedProtoPAlg.UseCaloHcalPID = 1
Hlt2TFChargedProtoPAlg.UseCaloBremPID = 1
#Hlt2TFChargedProtoPAlg.UseRichPID = 0 # Protos will NOT have any RICH information - HltRichPIDsKaons will not work
Hlt2TFChargedProtoPAlg.UseRichPID = 1  # Use this to add RICH info to the HLT protos, needed for HltRichPIDsKaons
Hlt2TFChargedProtoPAlg.UseMuonPID = 1
Hlt2TFChargedProtoPAlg.UseVeloPID = 0


#/---------------------------------------------------------------------
#/ Charged Particles - Here make all to Pi and K
#/---------------------------------------------------------------------
from Configurables import PreLoadParticles, NoPIDsParticleMaker
from Configurables import CombinedParticleMaker, TrackSelector
from Configurables import ProtoParticleCALOFilter, ProtoParticleMUONFilter
Hlt2TFPions = PreLoadParticles('Hlt2TFPions')
Hlt2TFKaons = PreLoadParticles('Hlt2TFKaons')
SeqHlt2TFParticles.Members += [ Hlt2TFPions, Hlt2TFKaons ]

Hlt2TFPions.addTool(PhysDesktop())
Hlt2TFPions.PhysDesktop.ParticleMakerType = "NoPIDsParticleMaker"
Hlt2TFPions.PhysDesktop.addTool(NoPIDsParticleMaker())
Hlt2TFPions.PhysDesktop.NoPIDsParticleMaker.Inputs = [ "Hlt/ProtoP/TFCharged" ]
Hlt2TFPions.PhysDesktop.NoPIDsParticleMaker.Particle = "pion"
Hlt2TFPions.DecayDescriptor = "Pion"

Hlt2TFKaons.addTool(PhysDesktop())
Hlt2TFKaons.PhysDesktop.ParticleMakerType = "NoPIDsParticleMaker"
Hlt2TFKaons.PhysDesktop.addTool(NoPIDsParticleMaker())
Hlt2TFKaons.PhysDesktop.NoPIDsParticleMaker.Inputs = [ "Hlt/ProtoP/TFCharged" ]
Hlt2TFKaons.PhysDesktop.NoPIDsParticleMaker.Particle = "kaon"
Hlt2TFKaons.DecayDescriptor = "Kaon"

#* To get the RICH kaons
#Hlt2TFKaons.addTool(PhysDesktop())
#Hlt2TFKaons.PhysDesktop.ParticleMakerType = "CombinedParticleMaker"
#Hlt2TFKaons.PhysDesktop.addTool(CombinedParticleMaker())
#Hlt2TFKaons.PhysDesktop.CombinedParticleMaker.ExclusiveSelection = 0
#Hlt2TFKaons.PhysDesktop.CombinedParticleMaker.InputProtoParticles = "Hlt/ProtoP/TFCharged"
#Hlt2TFKaons.PhysDesktop.CombinedParticleMaker.Particles = ["kaon"]
#Hlt2TFKaons.PhysDesktop.CombinedParticleMaker.addTool(TrackSelector())
#Hlt2TFKaons.PhysDesktop.CombinedParticleMaker.TrackSelector.TrackTypes = ["Long"]
#Hlt2TFKaons.PhysDesktop.CombinedParticleMaker.addTool(ProtoParticleCALOFilter('Kaon'))
#Hlt2TFKaons.PhysDesktop.CombinedParticleMaker.Kaon.Selection = [ "RequiresDet='RICH' CombDLL(k-pi)>'-5.0'" ]
#Hlt2TFKaons.DecayDescriptor = "Kaon"


#---------------------------------------------------------------------
# Kaons using RICH HLT reco results
#---------------------------------------------------------------------
HltRichPIDsKaons = PreLoadParticles('HltRichPIDsKaons')
SeqHlt2TFParticles.Members += [ HltRichPIDsKaons ]

HltRichPIDsKaons.addTool(PhysDesktop())
HltRichPIDsKaons.PhysDesktop.ParticleMakerType = "CombinedParticleMaker"
HltRichPIDsKaons.PhysDesktop.addTool(CombinedParticleMaker())
HltRichPIDsKaons.PhysDesktop.CombinedParticleMaker.ExclusiveSelection = 0
HltRichPIDsKaons.PhysDesktop.CombinedParticleMaker.InputProtoParticles = "Hlt/ProtoP/TFCharged"
HltRichPIDsKaons.PhysDesktop.CombinedParticleMaker.Particles = ["kaon"]
HltRichPIDsKaons.PhysDesktop.CombinedParticleMaker.addTool(TrackSelector())
HltRichPIDsKaons.PhysDesktop.CombinedParticleMaker.TrackSelector.TrackTypes = ["Long"]
HltRichPIDsKaons.PhysDesktop.CombinedParticleMaker.addTool(ProtoParticleCALOFilter('Kaon'))
HltRichPIDsKaons.PhysDesktop.CombinedParticleMaker.Kaon.Selection = ["RequiresDet='RICH' CombDLL(k-pi)>'-5.0'"]
HltRichPIDsKaons.DecayDescriptor = "Kaon"

#---------------------------------------------------------------------
# Muons sequence
#---------------------------------------------------------------------
SeqHlt2TFMuons = GaudiSequencer('SeqHlt2TFMuons')
SeqHlt2TFParticles.Members += [ SeqHlt2TFMuons ]
SeqHlt2TFMuons.IgnoreFilterPassed = 1

#---------------------------------------------------------------------
# Muons from Long tracks
#---------------------------------------------------------------------
Hlt2TFMuons = PreLoadParticles('Hlt2TFMuons')
SeqHlt2TFMuons.Members += [ Hlt2TFMuons ]

Hlt2TFMuons.addTool(PhysDesktop())
Hlt2TFMuons.PhysDesktop.ParticleMakerType = "CombinedParticleMaker" ;
Hlt2TFMuons.PhysDesktop.addTool(CombinedParticleMaker())
Hlt2TFMuons.PhysDesktop.CombinedParticleMaker.ExclusiveSelection = 0
Hlt2TFMuons.PhysDesktop.CombinedParticleMaker.Particles = ["muon"]
Hlt2TFMuons.PhysDesktop.CombinedParticleMaker.addTool(ProtoParticleMUONFilter('Muon'))
Hlt2TFMuons.PhysDesktop.CombinedParticleMaker.Muon.Selection = ["RequiresDet='MUON'"]
Hlt2TFMuons.PhysDesktop.CombinedParticleMaker.addTool(TrackSelector())
Hlt2TFMuons.PhysDesktop.CombinedParticleMaker.TrackSelector.TrackTypes = ["Long"]
Hlt2TFMuons.PhysDesktop.CombinedParticleMaker.InputProtoParticles =  "/Event/Hlt/ProtoP/TFCharged"
Hlt2TFMuons.DecayDescriptor = "Muon"

#---------------------------------------------------------------------
# Special case for electrons 
#---------------------------------------------------------------------
Hlt2TFElectrons = PreLoadParticles('Hlt2TFElectrons')
SeqHlt2TFParticles.Members += [ Hlt2TFElectrons ]

Hlt2TFElectrons.addTool(PhysDesktop())
Hlt2TFElectrons.PhysDesktop.ParticleMakerType = "CombinedParticleMaker"
Hlt2TFElectrons.PhysDesktop.addTool(CombinedParticleMaker())
Hlt2TFElectrons.PhysDesktop.CombinedParticleMaker.ExclusiveSelection = 0
Hlt2TFElectrons.PhysDesktop.CombinedParticleMaker.Particles =  ["electron"]
Hlt2TFElectrons.PhysDesktop.CombinedParticleMaker.InputProtoParticles =  "/Event/Hlt/ProtoP/TFCharged"
Hlt2TFElectrons.PhysDesktop.CombinedParticleMaker.addTool(ProtoParticleCALOFilter('Electron'))
Hlt2TFElectrons.PhysDesktop.CombinedParticleMaker.Electron.Selection = ["RequiresDet='CALO' CombDLL(e-pi)>'-2.0'"]
Hlt2TFElectrons.DecayDescriptor = "Electron"


