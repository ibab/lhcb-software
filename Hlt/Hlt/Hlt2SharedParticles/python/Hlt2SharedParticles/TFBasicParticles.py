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
from Configurables import Hlt2PID, Hlt2CaloReco
from GaudiKernel.SystemOfUnits import MeV
#
prefix = "HltTF"
TFTracks = "Hlt/Track/TFForwardForTopo"   # @todo Temporary
##########################################################################
#
# Make the Muons
#
Hlt2TFMuons = CombinedParticleMaker("Hlt2TFMuons")
Hlt2TFMuons.Input =  "/Event/"+prefix+"/ProtoP/Charged"
Hlt2TFMuons.Particle = "muon"
Hlt2TFMuons.addTool(ProtoParticleMUONFilter('Muon'))
Hlt2TFMuons.Muon.Selection = ["RequiresDet='MUON'"]
Hlt2TFMuons.addTool(TrackSelector())
Hlt2TFMuons.TrackSelector.TrackTypes = ["Long"]
##########################################################################
#
# Make the electrons
#
Hlt2TFElectrons = CombinedParticleMaker("Hlt2TFElectrons")
Hlt2TFElectrons.Particle =  "electron"
Hlt2TFElectrons.Input =  "/Event/"+prefix+"/ProtoP/Charged"
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
Hlt2TFPID = Hlt2PID('Hlt2TFPID')
Hlt2TFPID.Prefix = prefix
Hlt2TFPID.Hlt2Tracks = TFTracks
#
# Charged protoparticles
#
TFChargedProtoMaker = Hlt2TFPID.hlt2ChargedProtos( )
##########################################################################
#
# Calo reco
# Need another instance of Hlt2CaloReco
#
Hlt2TFCaloReco = Hlt2CaloReco('Hlt2TFCaloReco')
Hlt2TFCaloReco.Prefix = prefix
Hlt2TFCaloReco.Hlt2Tracks = TFTracks
#
Hlt2TFCaloRecoSeq = Hlt2TFCaloReco.hlt2Calo()   # todo can I get that from Hlt2.py ?
##########################################################################
#
# Muon reco 
#
Hlt2TFMuonIDSeq = Hlt2TFPID.hlt2Muon()  
###################################################################
# TF
#
importOptions("$HLTCONFROOT/options/Hlt2TrackFitForTopo.py")
SeqHlt2TFParticlesForTopo = GaudiSequencer('SeqHlt2TFParticlesForTopo') # the sequencer that does the track fit
##########################################################################
#
# define exported symbols -- these are for available
# for use in Hlt2 by adding:
#
# from Hlt2SharedParticles.TFBasicParticles import TFMuons
#

__all__ = ( 'TFMuons', 'TFElectrons', 'TFChargedProtos' )

TFChargedProtos = bindMembers( None, [ SeqHlt2TFParticlesForTopo, Hlt2TFCaloRecoSeq,
                                       Hlt2TFMuonIDSeq, TFChargedProtoMaker ] )

TFMuons         = bindMembers( None, [ TFChargedProtos, Hlt2TFMuons ] )
TFElectrons     = bindMembers( None, [ TFChargedProtos, Hlt2TFElectrons ] )
