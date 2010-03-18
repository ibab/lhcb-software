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
from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedRICHForwardTracking
Hlt2BiKalmanFittedRICHForwardTracking 	= Hlt2BiKalmanFittedRICHForwardTracking()
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
# Charged protoparticles, no RICH
#
BiKalmanFittedChargedProtoMaker 		= Hlt2BiKalmanFittedRICHForwardTracking.hlt2ChargedProtos()
#
# Downstream protoparticles, no RICH
#
BiKalmanFittedChargedDownProtoMaker		= Hlt2BiKalmanFittedDownstreamTracking.hlt2ChargedProtos()
#
# hadrons with the RICH
#
BiKalmanFittedChargedRICHHadronProtoMaker 	= Hlt2BiKalmanFittedRICHForwardTracking.hlt2ChargedHadronProtos()
#
# electrons
#
BiKalmanFittedChargedCaloProtoMaker 		= Hlt2BiKalmanFittedRICHForwardTracking.hlt2ChargedCaloProtos()
#
# Muons  
#
BiKalmanFittedMuonProtoMaker 			= Hlt2BiKalmanFittedRICHForwardTracking.hlt2MuonProtos()
##########################################################################
#
# Make the Muons
#
Hlt2BiKalmanFittedMuons 				= CombinedParticleMaker("Hlt2BiKalmanFittedMuons")
Hlt2BiKalmanFittedMuons.addTool(ProtoParticleMUONFilter('Muon'))
Hlt2BiKalmanFittedMuons.addTool(TrackSelector)
Hlt2BiKalmanFittedMuons.Input 				=  BiKalmanFittedMuonProtoMaker.outputSelection()
Hlt2BiKalmanFittedMuons.Particle 			= "muon"
Hlt2BiKalmanFittedMuons.Muon.Selection 			= ["RequiresDet='MUON'"]
Hlt2BiKalmanFittedMuons.TrackSelector.TrackTypes 	= ["Long"]
##########################################################################
#
# Make the pions
#
Hlt2BiKalmanFittedPions 				= NoPIDsParticleMaker("Hlt2BiKalmanFittedPions")
Hlt2BiKalmanFittedPions.addTool(TrackSelector)
Hlt2BiKalmanFittedPions.Particle 			=  "pion"
Hlt2BiKalmanFittedPions.Input 				=  BiKalmanFittedChargedProtoMaker.outputSelection()
Hlt2BiKalmanFittedPions.TrackSelector.TrackTypes 	= ["Long"]
##########################################################################
#
# Make the downstream pions
#
Hlt2BiKalmanFittedDownPions 				= NoPIDsParticleMaker("Hlt2BiKalmanFittedDownPions")
Hlt2BiKalmanFittedDownPions.addTool(TrackSelector)
Hlt2BiKalmanFittedDownPions.Particle 			=  "pion"
Hlt2BiKalmanFittedDownPions.Input 			=  BiKalmanFittedChargedDownProtoMaker.outputSelection()
Hlt2BiKalmanFittedDownPions.TrackSelector.TrackTypes 	= ["Downstream"]
##########################################################################
#
# Make the kaons
#
Hlt2BiKalmanFittedKaons 				= NoPIDsParticleMaker("Hlt2BiKalmanFittedKaons")
Hlt2BiKalmanFittedKaons.addTool(TrackSelector)
Hlt2BiKalmanFittedKaons.Particle 			=  "kaon"
Hlt2BiKalmanFittedKaons.Input 				=  BiKalmanFittedChargedProtoMaker.outputSelection()
Hlt2BiKalmanFittedKaons.TrackSelector.TrackTypes 	= ["Long"]
##########################################################################
#
# Make the protons
#
Hlt2BiKalmanFittedProtons 				= NoPIDsParticleMaker("Hlt2BiKalmanFittedProtons")
Hlt2BiKalmanFittedProtons.addTool(TrackSelector)
Hlt2BiKalmanFittedProtons.Particle 			=  "protons"
Hlt2BiKalmanFittedProtons.Input 				=  BiKalmanFittedChargedProtoMaker.outputSelection()
Hlt2BiKalmanFittedProtons.TrackSelector.TrackTypes 	= ["Long"]
##########################################################################
#
# Make the downstream protons
#
Hlt2BiKalmanFittedDownProtons 				= NoPIDsParticleMaker("Hlt2BiKalmanFittedDownProtons")
Hlt2BiKalmanFittedDownProtons.addTool(TrackSelector)
Hlt2BiKalmanFittedDownProtons.Particle 			=  "proton"
Hlt2BiKalmanFittedDownProtons.Input 			=  BiKalmanFittedChargedDownProtoMaker.outputSelection()
Hlt2BiKalmanFittedDownProtons.TrackSelector.TrackTypes 	= ["Downstream"]
##########################################################################
#
# Make the Rich kaons 
#
Hlt2BiKalmanFittedRichKaons 				= CombinedParticleMaker("Hlt2BiKalmanFittedRichKaons")
Hlt2BiKalmanFittedRichKaons.addTool(TrackSelector)
Hlt2BiKalmanFittedRichKaons.Particle 			=  "kaon"
Hlt2BiKalmanFittedRichKaons.Input 			= BiKalmanFittedChargedRICHHadronProtoMaker.outputSelection() 
Hlt2BiKalmanFittedRichKaons.TrackSelector.TrackTypes 	= ["Long"]
##########################################################################
#
# Make the electrons
#
Hlt2BiKalmanFittedElectrons 				= CombinedParticleMaker("Hlt2BiKalmanFittedElectrons")
Hlt2BiKalmanFittedElectrons.addTool(ProtoParticleCALOFilter('Electron'))
Hlt2BiKalmanFittedElectrons.addTool(TrackSelector)
Hlt2BiKalmanFittedElectrons.Particle 			=  "electron"
Hlt2BiKalmanFittedElectrons.Input 			=  BiKalmanFittedChargedCaloProtoMaker.outputSelection()
Hlt2BiKalmanFittedElectrons.Electron.Selection 		= ["RequiresDet='CALO' CombDLL(e-pi)>'-2.0'"]
Hlt2BiKalmanFittedElectrons.TrackSelector.TrackTypes 	= ["Long"]
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
BiKalmanFittedRichKaons     = bindMembers( None, [ BiKalmanFittedChargedRICHHadronProtoMaker	, Hlt2BiKalmanFittedRichKaons 	] ) 
