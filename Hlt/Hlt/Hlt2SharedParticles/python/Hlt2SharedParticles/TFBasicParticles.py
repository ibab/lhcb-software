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
from Configurables import Hlt2PID
from GaudiKernel.SystemOfUnits import MeV
#
prefix = "Hlt"
suffix = "Fitted" 
tracks = "Long"   
protoloc = "/Event/"+prefix+"/ProtoP/" + suffix + "/Charged"
##########################################################################
#
# Make the Muons
#
Hlt2TFMuons = CombinedParticleMaker("Hlt2TFMuons")
Hlt2TFMuons.Input =  protoloc
Hlt2TFMuons.Particle = "muon"
Hlt2TFMuons.addTool(ProtoParticleMUONFilter('Muon'))
Hlt2TFMuons.Muon.Selection = ["RequiresDet='MUON'"]
Hlt2TFMuons.addTool(TrackSelector())
Hlt2TFMuons.TrackSelector.TrackTypes = ["Long"]
##########################################################################
#
# Make the pions
#
Hlt2TFPions = NoPIDsParticleMaker("Hlt2TFPions")
Hlt2TFPions.Particle =  "pion"
Hlt2TFPions.Input =  protoloc
Hlt2TFPions.addTool(TrackSelector())
Hlt2TFPions.TrackSelector.TrackTypes = ["Long"]
##########################################################################
#
# Make the kaons
#
Hlt2TFKaons = NoPIDsParticleMaker("Hlt2TFKaons")
Hlt2TFKaons.Particle =  "kaon"
Hlt2TFKaons.Input =  protoloc
Hlt2TFKaons.addTool(TrackSelector())
Hlt2TFKaons.TrackSelector.TrackTypes = ["Long"]
##########################################################################
#
# Make the Rich kaons 
#
Hlt2TFRichKaons = CombinedParticleMaker("Hlt2TFRichKaons")
Hlt2TFRichKaons.Particle =  "kaon"
Hlt2TFRichKaons.Input = protoloc 
Hlt2TFRichKaons.addTool(TrackSelector()) 
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
Hlt2TFElectrons.addTool(TrackSelector())
Hlt2TFElectrons.TrackSelector.TrackTypes = ["Long"]
##########################################################################
#
# Now all PID
# 
##########################################################################
# Need another instance of Hlt2PID
class Hlt2TFPID(Hlt2PID) :
    __used_configurables__ = []
    __slots__ = []

Hlt2TFPID = Hlt2TFPID()
Hlt2TFPID.Prefix = prefix
Hlt2TFPID.Suffix = suffix
Hlt2TFPID.Hlt2Tracks = tracks
Hlt2TFPID.UseRICH = True
Hlt2TFPID.UseCALO = False
Hlt2TFPID.DataType = Hlt2PID().getProp("DataType")
#
# Charged protoparticles
#
TFChargedProtoMaker = Hlt2TFPID.hlt2ChargedProtos( )
#
# hadrons with the RICH
#
TFChargedHadronProtoMaker = Hlt2TFPID.hlt2ChargedHadronProtos( )
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
TFMuonProtoMaker = Hlt2TFPID.hlt2MuonProtos()  
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
