### @file
#
#  Basic particles for fitted tracks
#
#  @author V. Gligorov vladimir.gligorov@cern.ch
#
#  Based on code by Patrick Koppenburg
#
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from HltLine.HltLine import bindMembers
from Configurables import NoPIDsParticleMaker, CombinedParticleMaker, TrackSelector
from Configurables import PhotonMaker, PhotonMakerAlg
from Configurables import ProtoParticleCALOFilter, ProtoParticleMUONFilter
from GaudiKernel.SystemOfUnits import MeV
#
# Forward fitted
#
from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
Hlt2BiKalmanFittedForwardTracking   = Hlt2BiKalmanFittedForwardTracking()
#
# Now the downstream tracking
#
from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedDownstreamTracking
Hlt2BiKalmanFittedDownstreamTracking 	= Hlt2BiKalmanFittedDownstreamTracking()
##########################################################################
#
# Now all PID
# 
##########################################################################
#
# Charged hadron protoparticles, no Rich
#
BiKalmanFittedChargedProtoMaker 		= Hlt2BiKalmanFittedForwardTracking.hlt2ChargedNoPIDsProtos()
#
# Downstream hadron protoparticles, no Rich
#
BiKalmanFittedChargedDownProtoMaker		= Hlt2BiKalmanFittedDownstreamTracking.hlt2ChargedNoPIDsProtos()
#
# hadrons with the Rich
#
BiKalmanFittedChargedRichHadronProtoMaker 	= Hlt2BiKalmanFittedForwardTracking.hlt2ChargedRichProtos()
#
# electrons
#
BiKalmanFittedChargedCaloProtoMaker 		= Hlt2BiKalmanFittedForwardTracking.hlt2ChargedCaloProtos()
#
# Muons  
#
BiKalmanFittedMuonProtoMaker 			= Hlt2BiKalmanFittedForwardTracking.hlt2ChargedMuonProtos()
##########################################################################
#
# Make the Muons
#
Hlt2BiKalmanFittedMuons 				= CombinedParticleMaker("Hlt2BiKalmanFittedMuons")
Hlt2BiKalmanFittedMuons.addTool(ProtoParticleMUONFilter('Muon'))
Hlt2BiKalmanFittedMuons.Input 				=  BiKalmanFittedMuonProtoMaker.outputSelection()
Hlt2BiKalmanFittedMuons.Particle 			= "muon"
Hlt2BiKalmanFittedMuons.Muon.Selection 			= ["RequiresDet='MUON' IsMuon=True"]
##########################################################################
#
# Make the pions
#
Hlt2BiKalmanFittedPions 				= NoPIDsParticleMaker("Hlt2BiKalmanFittedPions")
Hlt2BiKalmanFittedPions.Particle 			=  "pion"
Hlt2BiKalmanFittedPions.Input 				=  BiKalmanFittedChargedProtoMaker.outputSelection()
##########################################################################
#
# Make the downstream pions
#
Hlt2BiKalmanFittedDownPions 				= NoPIDsParticleMaker("Hlt2BiKalmanFittedDownPions")
Hlt2BiKalmanFittedDownPions.Particle 			=  "pion"
Hlt2BiKalmanFittedDownPions.Input 			=  BiKalmanFittedChargedDownProtoMaker.outputSelection()
##########################################################################
#
# Make the kaons
#
Hlt2BiKalmanFittedKaons 				= NoPIDsParticleMaker("Hlt2BiKalmanFittedKaons")
Hlt2BiKalmanFittedKaons.Particle 			=  "kaon"
Hlt2BiKalmanFittedKaons.Input 				=  BiKalmanFittedChargedProtoMaker.outputSelection()
##########################################################################
#
# Make the protons
#
Hlt2BiKalmanFittedProtons 				= NoPIDsParticleMaker("Hlt2BiKalmanFittedProtons")
Hlt2BiKalmanFittedProtons.Particle 			=  "proton"
Hlt2BiKalmanFittedProtons.Input 			=  BiKalmanFittedChargedProtoMaker.outputSelection()
##########################################################################
#
# Make the downstream protons
#
Hlt2BiKalmanFittedDownProtons 				= NoPIDsParticleMaker("Hlt2BiKalmanFittedDownProtons")
Hlt2BiKalmanFittedDownProtons.Particle 			=  "proton"
Hlt2BiKalmanFittedDownProtons.Input 			=  BiKalmanFittedChargedDownProtoMaker.outputSelection()
##########################################################################
#
# Make the Rich kaons 
#
Hlt2BiKalmanFittedRichKaons 				= CombinedParticleMaker("Hlt2BiKalmanFittedRichKaons")
Hlt2BiKalmanFittedRichKaons.Particle 			=  "kaon"
Hlt2BiKalmanFittedRichKaons.Input 			= BiKalmanFittedChargedRichHadronProtoMaker.outputSelection() 
##########################################################################
#
# Make the electrons
#
Hlt2BiKalmanFittedElectrons 				= CombinedParticleMaker("Hlt2BiKalmanFittedElectrons")
Hlt2BiKalmanFittedElectrons.addTool(ProtoParticleCALOFilter('Electron'))
Hlt2BiKalmanFittedElectrons.Particle 			=  "electron"
Hlt2BiKalmanFittedElectrons.Input 			=  BiKalmanFittedChargedCaloProtoMaker.outputSelection()
Hlt2BiKalmanFittedElectrons.Electron.Selection 		= ["RequiresDet='CALO' CombDLL(e-pi)>'-2.0'"]
##########################################################################
#
# define exported symbols -- these are for available
# for use in Hlt2 by adding:
#
# from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedMuons
#

__all__ = ( 	'BiKalmanFittedMuons', 
		'BiKalmanFittedElectrons', 
		'BiKalmanFittedKaons', 
		'BiKalmanFittedPions',
		'BiKalmanFittedDownPions',
                'BiKalmanFittedProtons',
                'BiKalmanFittedDownProtons',
		'BiKalmanFittedChargedProtos', 
		'BiKalmanFittedRichKaons' )

#
BiKalmanFittedKaons         = bindMembers( None, [ BiKalmanFittedChargedProtoMaker		, Hlt2BiKalmanFittedKaons 	] )
BiKalmanFittedPions         = bindMembers( None, [ BiKalmanFittedChargedProtoMaker		, Hlt2BiKalmanFittedPions 	] )
BiKalmanFittedDownPions     = bindMembers( None, [ BiKalmanFittedChargedDownProtoMaker          , Hlt2BiKalmanFittedDownPions   ] )
BiKalmanFittedProtons       = bindMembers( None, [ BiKalmanFittedChargedProtoMaker		, Hlt2BiKalmanFittedProtons 	] )
BiKalmanFittedDownProtons   = bindMembers( None, [ BiKalmanFittedChargedDownProtoMaker          , Hlt2BiKalmanFittedDownProtons ] )
BiKalmanFittedMuons         = bindMembers( None, [ BiKalmanFittedMuonProtoMaker			, Hlt2BiKalmanFittedMuons 	] )
BiKalmanFittedElectrons     = bindMembers( None, [ BiKalmanFittedChargedCaloProtoMaker		, Hlt2BiKalmanFittedElectrons 	] ) 
BiKalmanFittedRichKaons     = bindMembers( None, [ BiKalmanFittedChargedRichHadronProtoMaker	, Hlt2BiKalmanFittedRichKaons 	] ) 
