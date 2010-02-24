### @file
#
#  Basic particles for Track fitted tracks
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from HltLine.HltLine import bindMembers
from Configurables import NoPIDsParticleMaker, CombinedParticleMaker, TrackSelector
from Configurables import PhotonMaker, PhotonMakerAlg
from Configurables import ProtoParticleCALOFilter, ProtoParticleMUONFilter
from Configurables import Hlt2Tracking
from GaudiKernel.SystemOfUnits import MeV
#
from HltLine.HltTrackNames import HltBiDirectionalKalmanFitSuffix

# Need another instance of Hlt2Tracking
class Hlt2TFTracking(Hlt2Tracking) :
    __used_configurables__ = []
    __slots__ = []

Hlt2TFTracking = Hlt2TFTracking()
Hlt2TFTracking.Prefix = Hlt2Tracking().getProp("Prefix")
Hlt2TFTracking.Suffix = HltBiDirectionalKalmanFitSuffix
Hlt2TFTracking.Hlt2Tracks = Hlt2Tracking().getProp("Hlt2Tracks")
Hlt2TFTracking.UseRICH = True
Hlt2TFTracking.UseCALO = False
Hlt2TFTracking.DataType = Hlt2Tracking().getProp("DataType")

protoloc = (Hlt2TFTracking.hlt2ChargedProtos()).outputSelection()
##########################################################################
#
# Make the Muons
#
Hlt2TFMuons = CombinedParticleMaker("Hlt2TFMuons")
Hlt2TFMuons.Input =  protoloc
Hlt2TFMuons.Particle = "muon"
Hlt2TFMuons.addTool(ProtoParticleMUONFilter('Muon'))
Hlt2TFMuons.Muon.Selection = ["RequiresDet='MUON'"]
Hlt2TFMuons.addTool(TrackSelector)
Hlt2TFMuons.TrackSelector.TrackTypes = ["Long"]
##########################################################################
#
# Make the pions
#
Hlt2TFPions = NoPIDsParticleMaker("Hlt2TFPions")
Hlt2TFPions.Particle =  "pion"
Hlt2TFPions.Input =  protoloc
Hlt2TFPions.addTool(TrackSelector)
Hlt2TFPions.TrackSelector.TrackTypes = ["Long"]
##########################################################################
#
# Make the kaons
#
Hlt2TFKaons = NoPIDsParticleMaker("Hlt2TFKaons")
Hlt2TFKaons.Particle =  "kaon"
Hlt2TFKaons.Input =  protoloc
Hlt2TFKaons.addTool(TrackSelector)
Hlt2TFKaons.TrackSelector.TrackTypes = ["Long"]
##########################################################################
#
# Make the Rich kaons 
#
Hlt2TFRichKaons = CombinedParticleMaker("Hlt2TFRichKaons")
Hlt2TFRichKaons.Particle =  "kaon"
Hlt2TFRichKaons.Input = protoloc 
Hlt2TFRichKaons.addTool(TrackSelector) 
Hlt2TFRichKaons.TrackSelector.TrackTypes = ["Long"]
##########################################################################
#
# Make the electrons
#
Hlt2TFElectrons = CombinedParticleMaker("Hlt2TFElectrons")
Hlt2TFElectrons.Particle =  "electron"
Hlt2TFElectrons.Input =  protoloc
Hlt2TFElectrons.addTool(ProtoParticleCALOFilter('Electron'))
Hlt2TFElectrons.Electron.Selection = ["RequiresDet='CALO' CombDLL(e-pi)>'-2.0'"]
Hlt2TFElectrons.addTool(TrackSelector)
Hlt2TFElectrons.TrackSelector.TrackTypes = ["Long"]
##########################################################################
#
# Now all PID
# 
##########################################################################
#
# Charged protoparticles
#
TFChargedProtoMaker = Hlt2TFTracking.hlt2ChargedProtos()
#
# hadrons with the RICH
#
TFChargedHadronProtoMaker = Hlt2TFTracking.hlt2ChargedHadronProtos( )
##########################################################################
#
# Calo reco
# @todo need CaloReco to be able to deal with other type of tracks
#
# Hlt2TFCaloReco = Hlt2CaloReco('Hlt2TFCaloReco')
# Hlt2TFCaloReco.Prefix = prefix
# Hlt2TFCaloReco.Hlt2Tracks = tracks
##
# Hlt2TFCaloRecoSeq = Hlt2TFCaloReco.hlt2Calo()   # todo can I get that from Hlt2.py ?
##########################################################################
#
# Muon reco 
#
TFMuonProtoMaker = Hlt2TFTracking.hlt2MuonProtos()  
##########################################################################
#
# define exported symbols -- these are for available
# for use in Hlt2 by adding:
#
# from Hlt2SharedParticles.TFBasicParticles import TFMuons
#

__all__ = ( 'TFMuons', 'TFElectrons', 'TFKaons', 'TFPions', 'TFChargedProtos', 'TFRichKaons' )

#
TFKaons         = bindMembers( None, [ TFChargedProtoMaker, Hlt2TFKaons ] )
TFPions         = bindMembers( None, [ TFChargedProtoMaker, Hlt2TFPions ] )
TFMuons         = bindMembers( None, [ TFMuonProtoMaker, Hlt2TFMuons ] )
TFElectrons     = bindMembers( None, [ TFChargedProtoMaker, Hlt2TFElectrons ] ) #this is buggy for now as no CALO ID yet 
TFRichKaons     = bindMembers( None, [ TFChargedHadronProtoMaker, Hlt2TFRichKaons ] ) 
