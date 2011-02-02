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
Hlt2BiKalmanFittedForwardTracking   			= Hlt2BiKalmanFittedForwardTracking()
#
# Now the downstream tracking
#
from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedDownstreamTracking
Hlt2BiKalmanFittedDownstreamTracking 			= Hlt2BiKalmanFittedDownstreamTracking()
#
# Forward fitted for Rich-ID of protons 
#
from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedRichForProtonsForwardTracking
Hlt2BiKalmanFittedRichForProtonsForwardTracking 	= Hlt2BiKalmanFittedRichForProtonsForwardTracking() 
##########################################################################
#
# Now all PID
# 
##########################################################################
#
# Charged hadron protoparticles, no Rich
#
BiKalmanFittedChargedProtoMaker 			= Hlt2BiKalmanFittedForwardTracking.hlt2ChargedNoPIDsProtos()
#
# Charged hadron protoparticles, second loop forward trackign, no Rich
#
BiKalmanFittedChargedSecondLoopProtoMaker   = Hlt2BiKalmanFittedForwardTracking.hlt2ChargedNoPIDsProtos(secondLoop=True)
#
# Downstream hadron protoparticles, no Rich
#
BiKalmanFittedChargedDownProtoMaker			= Hlt2BiKalmanFittedDownstreamTracking.hlt2ChargedNoPIDsProtos()
#
# hadrons with the Rich
#
BiKalmanFittedChargedRichHadronProtoMaker 		= Hlt2BiKalmanFittedForwardTracking.hlt2ChargedRichProtos()
#
# hadrons with the Rich configured to ID protons
#
BiKalmanFittedChargedRichForProtonsHadronProtoMaker	= Hlt2BiKalmanFittedRichForProtonsForwardTracking.hlt2ChargedRichProtos()
#
# electrons
#
BiKalmanFittedChargedCaloProtoMaker 			= Hlt2BiKalmanFittedForwardTracking.hlt2ChargedCaloProtos()
#
# Muons  
#
BiKalmanFittedMuonProtoMaker 				= Hlt2BiKalmanFittedForwardTracking.hlt2ChargedMuonProtos()
##########################################################################
#
# Neutral protoparticles
#
BiKalmanFittedNeutralProtoMaker   = Hlt2BiKalmanFittedForwardTracking.hlt2NeutralProtos()
##########################################################################
#
# Make the Muons
#
Hlt2BiKalmanFittedMuons 				= CombinedParticleMaker("Hlt2BiKalmanFittedMuons")
Hlt2BiKalmanFittedMuons.addTool(ProtoParticleMUONFilter('Muon'))
Hlt2BiKalmanFittedMuons.Input 				=  BiKalmanFittedMuonProtoMaker.outputSelection()
Hlt2BiKalmanFittedMuons.Particle 			= "muon"
Hlt2BiKalmanFittedMuons.Muon.Selection 			= ["RequiresDet='MUON' IsMuon=True"]
Hlt2BiKalmanFittedMuons.WriteP2PVRelations 		=  False
##########################################################################
#
# Make the pions
#
Hlt2BiKalmanFittedPions 				= NoPIDsParticleMaker("Hlt2BiKalmanFittedPions")
Hlt2BiKalmanFittedPions.Particle 			=  "pion"
Hlt2BiKalmanFittedPions.Input 				=  BiKalmanFittedChargedProtoMaker.outputSelection()
Hlt2BiKalmanFittedPions.WriteP2PVRelations 		=  False
##########################################################################
#
# Make the downstream pions
#
Hlt2BiKalmanFittedDownPions 				= NoPIDsParticleMaker("Hlt2BiKalmanFittedDownPions")
Hlt2BiKalmanFittedDownPions.Particle 			=  "pion"
Hlt2BiKalmanFittedDownPions.Input 			=  BiKalmanFittedChargedDownProtoMaker.outputSelection()
Hlt2BiKalmanFittedDownPions.WriteP2PVRelations 		=  False
##########################################################################
#
# Make the kaons
#
Hlt2BiKalmanFittedKaons 				= NoPIDsParticleMaker("Hlt2BiKalmanFittedKaons")
Hlt2BiKalmanFittedKaons.Particle 			=  "kaon"
Hlt2BiKalmanFittedKaons.Input 				=  BiKalmanFittedChargedProtoMaker.outputSelection()
Hlt2BiKalmanFittedKaons.WriteP2PVRelations 		=  False
##########################################################################
#
# Make the kaons w/ muon ID available
#
Hlt2BiKalmanFittedKaonsWithMuonID			= NoPIDsParticleMaker("Hlt2BiKalmanFittedKaonsWithMuonID")
Hlt2BiKalmanFittedKaonsWithMuonID.addTool(ProtoParticleMUONFilter('Muon'))
Hlt2BiKalmanFittedKaonsWithMuonID.Particle 		= "kaon"
Hlt2BiKalmanFittedKaonsWithMuonID.Input 		= BiKalmanFittedMuonProtoMaker.outputSelection()
Hlt2BiKalmanFittedKaonsWithMuonID.Muon.Selection        = [""]
Hlt2BiKalmanFittedKaonsWithMuonID.WriteP2PVRelations	= False
##########################################################################
#
# Make the protons
#
Hlt2BiKalmanFittedProtons 				= NoPIDsParticleMaker("Hlt2BiKalmanFittedProtons")
Hlt2BiKalmanFittedProtons.Particle 			=  "proton"
Hlt2BiKalmanFittedProtons.Input 			=  BiKalmanFittedChargedProtoMaker.outputSelection()
Hlt2BiKalmanFittedProtons.WriteP2PVRelations 		=  False
##########################################################################
#
# Make the downstream protons
#
Hlt2BiKalmanFittedDownProtons 				= NoPIDsParticleMaker("Hlt2BiKalmanFittedDownProtons")
Hlt2BiKalmanFittedDownProtons.Particle 			=  "proton"
Hlt2BiKalmanFittedDownProtons.Input 			=  BiKalmanFittedChargedDownProtoMaker.outputSelection()
Hlt2BiKalmanFittedDownProtons.WriteP2PVRelations 	=  False
##########################################################################
#
# Make the Rich kaons 
# Note: we set the DLL cut manually to -99999 to avoid the default of 2 specified in the ProtoP filter base class 
#
Hlt2BiKalmanFittedRichKaons 				=  CombinedParticleMaker("Hlt2BiKalmanFittedRichKaons")
Hlt2BiKalmanFittedRichKaons.addTool(ProtoParticleCALOFilter('Kaon'))
Hlt2BiKalmanFittedRichKaons.Particle 			=  "kaon"
Hlt2BiKalmanFittedRichKaons.Input 			=  BiKalmanFittedChargedRichHadronProtoMaker.outputSelection()
Hlt2BiKalmanFittedRichKaons.WriteP2PVRelations  	=  False
Hlt2BiKalmanFittedRichKaons.Kaon.Selection		=  ["RequiresDet='RICH'"]
##########################################################################
#
# Make the Rich protons 
# Note: we set the DLL cut manually to -99999 to avoid the default of 2 specified in the ProtoP filter base class
#
Hlt2BiKalmanFittedRichProtons                           = CombinedParticleMaker("Hlt2BiKalmanFittedRichProtons")
Hlt2BiKalmanFittedRichProtons.addTool(ProtoParticleCALOFilter('Proton'))
Hlt2BiKalmanFittedRichProtons.Particle                  =  "proton"
Hlt2BiKalmanFittedRichProtons.Input                     = BiKalmanFittedChargedRichForProtonsHadronProtoMaker.outputSelection()
Hlt2BiKalmanFittedRichProtons.WriteP2PVRelations 	=  False
Hlt2BiKalmanFittedRichProtons.Proton.Selection		= ["RequiresDet='RICH'"]
########################################################################
#
# Make the electrons
#
Hlt2BiKalmanFittedElectrons 				= CombinedParticleMaker("Hlt2BiKalmanFittedElectrons")
Hlt2BiKalmanFittedElectrons.addTool(ProtoParticleCALOFilter('Electron'))
Hlt2BiKalmanFittedElectrons.Particle 			=  "electron"
Hlt2BiKalmanFittedElectrons.Input 			=  BiKalmanFittedChargedCaloProtoMaker.outputSelection()
Hlt2BiKalmanFittedElectrons.Electron.Selection 		= ["RequiresDet='CALO' CombDLL(e-pi)>'-2.0'"]
Hlt2BiKalmanFittedElectrons.WriteP2PVRelations 		=  False
##########################################################################
#
# Make the second loop pions
#
Hlt2BiKalmanFittedSecondLoopPions                 = NoPIDsParticleMaker("Hlt2BiKalmanFittedSecondLoopPions")
Hlt2BiKalmanFittedSecondLoopPions.Particle            =  "pion"
Hlt2BiKalmanFittedSecondLoopPions.Input               =  BiKalmanFittedChargedSecondLoopProtoMaker.outputSelection()
Hlt2BiKalmanFittedSecondLoopPions.WriteP2PVRelations      =  False
##########################################################################
#
# Make the second loop kaons
#
Hlt2BiKalmanFittedSecondLoopKaons                 = NoPIDsParticleMaker("Hlt2BiKalmanFittedSecondLoopKaons")
Hlt2BiKalmanFittedSecondLoopKaons.Particle            =  "kaon"
Hlt2BiKalmanFittedSecondLoopKaons.Input               =  BiKalmanFittedChargedSecondLoopProtoMaker.outputSelection()
Hlt2BiKalmanFittedSecondLoopKaons.WriteP2PVRelations      =  False
##########################################################################
# Make the photons
#
Hlt2BiKalmanFittedPhotons = PhotonMakerAlg("Hlt2BiKalmanFittedPhotons")
Hlt2BiKalmanFittedPhotons.addTool(PhotonMaker)
Hlt2BiKalmanFittedPhotons.PhotonMaker.Input = BiKalmanFittedNeutralProtoMaker.outputSelection()
Hlt2BiKalmanFittedPhotons.PhotonMaker.ConvertedPhotons = True  
Hlt2BiKalmanFittedPhotons.PhotonMaker.UnconvertedPhotons = True  
Hlt2BiKalmanFittedPhotons.PhotonMaker.PtCut = 200.* MeV 
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
		        'BiKalmanFittedSecondLoopKaons',
                'BiKalmanFittedSecondLoopPions',
                'BiKalmanFittedDownPions',
                'BiKalmanFittedProtons',
                'BiKalmanFittedDownProtons',
		        'BiKalmanFittedRichKaons',
		        'BiKalmanFittedRichProtons',
                'BiKalmanFittedPhotons' )

#
BiKalmanFittedKaons         = bindMembers( None, [ BiKalmanFittedChargedProtoMaker			, Hlt2BiKalmanFittedKaons 	] )
BiKalmanFittedPions         = bindMembers( None, [ BiKalmanFittedChargedProtoMaker			, Hlt2BiKalmanFittedPions 	] )

BiKalmanFittedSecondLoopKaons = bindMembers( None, [ BiKalmanFittedChargedSecondLoopProtoMaker , Hlt2BiKalmanFittedSecondLoopKaons   ] ) 
BiKalmanFittedSecondLoopPions = bindMembers( None, [ BiKalmanFittedChargedSecondLoopProtoMaker , Hlt2BiKalmanFittedSecondLoopPions   ] )

BiKalmanFittedDownPions     = bindMembers( None, [ BiKalmanFittedChargedDownProtoMaker      , Hlt2BiKalmanFittedDownPions   ] )
BiKalmanFittedProtons       = bindMembers( None, [ BiKalmanFittedChargedProtoMaker			, Hlt2BiKalmanFittedProtons 	] )
BiKalmanFittedDownProtons   = bindMembers( None, [ BiKalmanFittedChargedDownProtoMaker      , Hlt2BiKalmanFittedDownProtons ] )
BiKalmanFittedMuons         = bindMembers( None, [ BiKalmanFittedMuonProtoMaker				, Hlt2BiKalmanFittedMuons 	] )
BiKalmanFittedElectrons     = bindMembers( None, [ BiKalmanFittedChargedCaloProtoMaker		, Hlt2BiKalmanFittedElectrons 	] ) 
BiKalmanFittedRichKaons     = bindMembers( None, [ BiKalmanFittedChargedRichHadronProtoMaker, Hlt2BiKalmanFittedRichKaons 	] )
BiKalmanFittedRichProtons   = bindMembers( None, [ BiKalmanFittedChargedRichForProtonsHadronProtoMaker , Hlt2BiKalmanFittedRichProtons ] )
BiKalmanFittedPhotons       = bindMembers( None, [ BiKalmanFittedNeutralProtoMaker  ,   Hlt2BiKalmanFittedPhotons         ] ) 
BiKalmanFittedKaonsWithMuonID = bindMembers( None, [ BiKalmanFittedMuonProtoMaker, Hlt2BiKalmanFittedKaonsWithMuonID] )
