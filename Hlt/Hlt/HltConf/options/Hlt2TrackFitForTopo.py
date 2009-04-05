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
SeqHlt2TFParticlesForTopo = GaudiSequencer('SeqHlt2TFParticles')
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
Hlt2TFTrackFitForTopo = TrackEventFitter('Hlt2TFTrackFit')
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
from Configurables import PreLoadParticles, NoPIDsParticleMaker
from Configurables import CombinedParticleMaker, TrackSelector
from Configurables import ProtoParticleCALOFilter, ProtoParticleMUONFilter
Hlt2TFPionsForTopo = PreLoadParticles('Hlt2TFPionsForTopo')
Hlt2TFKaonsForTopo = PreLoadParticles('Hlt2TFKaonsForTopo')
SeqHlt2TFParticlesForTopo.Members += [ Hlt2TFPionsForTopo, Hlt2TFKaonsForTopo ]

Hlt2TFPionsForTopo.addTool(PhysDesktop)
Hlt2TFPionsForTopo.PhysDesktop.ParticleMakerType = "NoPIDsParticleMaker"
Hlt2TFPionsForTopo.PhysDesktop.addTool(NoPIDsParticleMaker)
Hlt2TFPionsForTopo.PhysDesktop.NoPIDsParticleMaker.Inputs = [ "Hlt/ProtoP/TFChargedForTopo" ]
Hlt2TFPionsForTopo.PhysDesktop.NoPIDsParticleMaker.Particle = "pion"
Hlt2TFPionsForTopo.DecayDescriptor = "Pion"

Hlt2TFKaonsForTopo.addTool(PhysDesktop)
Hlt2TFKaonsForTopo.PhysDesktop.ParticleMakerType = "NoPIDsParticleMaker"
Hlt2TFKaonsForTopo.PhysDesktop.addTool(NoPIDsParticleMaker)
Hlt2TFKaonsForTopo.PhysDesktop.NoPIDsParticleMaker.Inputs = [ "Hlt/ProtoP/TFChargedForTopo" ]
Hlt2TFKaonsForTopo.PhysDesktop.NoPIDsParticleMaker.Particle = "kaon"
Hlt2TFKaonsForTopo.DecayDescriptor = "Kaon"

#* To get the RICH kaons
#Hlt2TFKaonsForTopo.addTool(PhysDesktop)
#Hlt2TFKaonsForTopo.PhysDesktop.ParticleMakerType = "CombinedParticleMaker"
#Hlt2TFKaonsForTopo.PhysDesktop.addTool(CombinedParticleMaker)
#Hlt2TFKaonsForTopo.PhysDesktop.CombinedParticleMaker.ExclusiveSelection = 0
#Hlt2TFKaonsForTopo.PhysDesktop.CombinedParticleMaker.InputProtoParticles = "Hlt/ProtoP/TFChargedForTopo"
#Hlt2TFKaonsForTopo.PhysDesktop.CombinedParticleMaker.Particles = ["kaon"]
#Hlt2TFKaonsForTopo.PhysDesktop.CombinedParticleMaker.addTool(TrackSelector)
#Hlt2TFKaonsForTopo.PhysDesktop.CombinedParticleMaker.TrackSelector.TrackTypes = ["Long"]
#Hlt2TFKaonsForTopo.PhysDesktop.CombinedParticleMaker.addTool(ProtoParticleCALOFilter('Kaon'))
#Hlt2TFKaonsForTopo.PhysDesktop.CombinedParticleMaker.Kaon.Selection = [ "RequiresDet='RICH' CombDLL(k-pi)>'-5.0'" ]
#Hlt2TFKaonsForTopo.DecayDescriptor = "Kaon"


#---------------------------------------------------------------------
# Kaons using RICH HLT reco results
#---------------------------------------------------------------------
HltRichPIDsTFKaonsForTopo = PreLoadParticles('HltRichPIDsTFKaonsForTopo')
SeqHlt2TFParticlesForTopo.Members += [ HltRichPIDsTFKaonsForTopo ]

HltRichPIDsTFKaonsForTopo.addTool(PhysDesktop)
HltRichPIDsTFKaonsForTopo.PhysDesktop.ParticleMakerType = "CombinedParticleMaker"
HltRichPIDsTFKaonsForTopo.PhysDesktop.addTool(CombinedParticleMaker)
HltRichPIDsTFKaonsForTopo.PhysDesktop.CombinedParticleMaker.ExclusiveSelection = 0
HltRichPIDsTFKaonsForTopo.PhysDesktop.CombinedParticleMaker.InputProtoParticles = "Hlt/ProtoP/TFChargedForTopo"
HltRichPIDsTFKaonsForTopo.PhysDesktop.CombinedParticleMaker.Particles = ["kaon"]
HltRichPIDsTFKaonsForTopo.PhysDesktop.CombinedParticleMaker.addTool(TrackSelector())
HltRichPIDsTFKaonsForTopo.PhysDesktop.CombinedParticleMaker.TrackSelector.TrackTypes = ["Long"]
HltRichPIDsTFKaonsForTopo.PhysDesktop.CombinedParticleMaker.addTool(ProtoParticleCALOFilter('Kaon'))
HltRichPIDsTFKaonsForTopo.PhysDesktop.CombinedParticleMaker.Kaon.Selection = ["RequiresDet='RICH' CombDLL(k-pi)>'-5.0'"]
HltRichPIDsTFKaonsForTopo.DecayDescriptor = "Kaon"

#---------------------------------------------------------------------
# Muons sequence
#---------------------------------------------------------------------
SeqHlt2TFMuonsForTopo = GaudiSequencer('SeqHlt2TFMuons')
SeqHlt2TFParticlesForTopo.Members += [ SeqHlt2TFMuonsForTopo ]
SeqHlt2TFMuonsForTopo.IgnoreFilterPassed = 1

#---------------------------------------------------------------------
# Muons from Long tracks
#---------------------------------------------------------------------
Hlt2TFMuonsForTopo = PreLoadParticles('Hlt2TFMuons')
SeqHlt2TFMuonsForTopo.Members += [ Hlt2TFMuons ]

Hlt2TFMuonsForTopo.addTool(PhysDesktop)
Hlt2TFMuonsForTopo.PhysDesktop.ParticleMakerType = "CombinedParticleMaker" ;
Hlt2TFMuonsForTopo.PhysDesktop.addTool(CombinedParticleMaker)
Hlt2TFMuonsForTopo.PhysDesktop.CombinedParticleMaker.ExclusiveSelection = 0
Hlt2TFMuonsForTopo.PhysDesktop.CombinedParticleMaker.InputProtoParticles =  "/Event/Hlt/ProtoP/TFChargedForTopo"
Hlt2TFMuonsForTopo.PhysDesktop.CombinedParticleMaker.Particles = ["muon"]
Hlt2TFMuonsForTopo.PhysDesktop.CombinedParticleMaker.addTool(ProtoParticleMUONFilter('Muon'))
Hlt2TFMuonsForTopo.PhysDesktop.CombinedParticleMaker.Muon.Selection = ["RequiresDet='MUON'"]
Hlt2TFMuonsForTopo.PhysDesktop.CombinedParticleMaker.addTool(TrackSelector())
Hlt2TFMuonsForTopo.PhysDesktop.CombinedParticleMaker.TrackSelector.TrackTypes = ["Long"]
Hlt2TFMuonsForTopo.DecayDescriptor = "Muon"

#---------------------------------------------------------------------
# Special case for electrons 
#---------------------------------------------------------------------
Hlt2TFElectronsForTopo = PreLoadParticles('Hlt2TFElectrons')
SeqHlt2TFParticlesForTopo.Members += [ Hlt2TFElectronsForTopo ]

Hlt2TFElectronsForTopo.addTool(PhysDesktop)
Hlt2TFElectronsForTopo.PhysDesktop.ParticleMakerType = "CombinedParticleMaker"
Hlt2TFElectronsForTopo.PhysDesktop.addTool(CombinedParticleMaker)
Hlt2TFElectronsForTopo.PhysDesktop.CombinedParticleMaker.ExclusiveSelection = 0
Hlt2TFElectronsForTopo.PhysDesktop.CombinedParticleMaker.Particles =  ["electron"]
Hlt2TFElectronsForTopo.PhysDesktop.CombinedParticleMaker.InputProtoParticles =  "/Event/Hlt/ProtoP/TFChargedForTopo"
Hlt2TFElectronsForTopo.PhysDesktop.CombinedParticleMaker.addTool(ProtoParticleCALOFilter('Electron'))
Hlt2TFElectronsForTopo.PhysDesktop.CombinedParticleMaker.Electron.Selection = ["RequiresDet='CALO' CombDLL(e-pi)>'-2.0'"]
Hlt2TFElectronsForTopo.DecayDescriptor = "Electron"


