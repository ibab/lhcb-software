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

#
# Forward fitted for Rich-ID of LowPT protons
#
from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedRichForLowPTParticlesForwardTracking
Hlt2BiKalmanFittedRichForLowPTParticlesForwardTracking 	= Hlt2BiKalmanFittedRichForLowPTParticlesForwardTracking()

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
# hadrons with the Rich configured to ID LowPT protons
#
BiKalmanFittedChargedRichForLowPTParticlesHadronProtoMaker = Hlt2BiKalmanFittedRichForLowPTParticlesForwardTracking.hlt2ChargedRichProtos()
#
# electrons
#
BiKalmanFittedChargedCaloProtoMaker 			= Hlt2BiKalmanFittedForwardTracking.hlt2ChargedCaloProtos()
BiKalmanFittedElectronFromL0Maker         = Hlt2BiKalmanFittedForwardTracking.hlt2ElectronsFromL0()
#
# Muons  
#
BiKalmanFittedMuonProtoMaker 				= Hlt2BiKalmanFittedForwardTracking.hlt2ChargedMuonProtos()
#
# Second Loop Muons  
#
BiKalmanFittedMuonSecondLoopProtoMaker 		 = Hlt2BiKalmanFittedForwardTracking.hlt2ChargedMuonProtos(secondLoop=True)

##########################################################################
#
# Neutral protoparticles
#
BiKalmanFittedNeutralProtoMaker      = Hlt2BiKalmanFittedForwardTracking.hlt2NeutralProtos()
BiKalmanFittedPhotonFromL0Maker      = Hlt2BiKalmanFittedForwardTracking.hlt2PhotonsFromL0()
BiKalmanFittedPhotonFromL0LowMaker   = Hlt2BiKalmanFittedForwardTracking.hlt2Pi0FromL0()
##########################################################################
#
# Make the Muons
#
Hlt2BiKalmanFittedMuons = CombinedParticleMaker("Hlt2BiKalmanFittedMuons"
                                            , Particle 			    = "muon"
                                            , Input 				=  BiKalmanFittedMuonProtoMaker.outputSelection()
                                            , Output                = 'Hlt2/Hlt2BiKalmanFittedMuons/Particles'
                                            , WriteP2PVRelations 	=  False
                                            )
Hlt2BiKalmanFittedMuons.addTool(ProtoParticleMUONFilter('Muon'))
Hlt2BiKalmanFittedMuons.Muon.Selection 			= ["RequiresDet='MUON' IsMuon=True"]
##########################################################################
#
# Make the pions
#
Hlt2BiKalmanFittedPions = NoPIDsParticleMaker("Hlt2BiKalmanFittedPions"
                                             , Particle 			=  "pion"
                                             , Input 				=  BiKalmanFittedChargedProtoMaker.outputSelection()
                                             , Output               = 'Hlt2/Hlt2BiKalmanFittedPions/Particles'
                                             , WriteP2PVRelations 	=  False
                                             )
##########################################################################
#
# Make the downstream pions
#
Hlt2BiKalmanFittedDownPions = NoPIDsParticleMaker("Hlt2BiKalmanFittedDownPions"
                                                 , Particle 			=  "pion"
                                                 , Input 			=  BiKalmanFittedChargedDownProtoMaker.outputSelection()
                                                 , Output                   = 'Hlt2/Hlt2BiKalmanFittedDownPions/Particles'
                                                 , WriteP2PVRelations 		=  False
                                                 )
##########################################################################
#
# Make the kaons
#
Hlt2BiKalmanFittedKaons = NoPIDsParticleMaker("Hlt2BiKalmanFittedKaons"
                                            , Particle           =  "kaon"
                                            , Input              =  BiKalmanFittedChargedProtoMaker.outputSelection()
                                            , Output             = 'Hlt2/Hlt2BiKalmanFittedKaons/Particles'
                                            , WriteP2PVRelations =  False
                                            )
##########################################################################
#
# Make the kaons w/ muon ID available
#
Hlt2BiKalmanFittedKaonsWithMuonID = NoPIDsParticleMaker("Hlt2BiKalmanFittedKaonsWithMuonID"
                                            , Particle 		= "kaon"
                                            , Input 		= BiKalmanFittedMuonProtoMaker.outputSelection()
                                            , Output        = 'Hlt2/Hlt2BiKalmanFittedKaonsWithMuonID/Particles'
                                            , WriteP2PVRelations	= False
                                            )
Hlt2BiKalmanFittedKaonsWithMuonID.addTool(ProtoParticleMUONFilter('Muon'))
Hlt2BiKalmanFittedKaonsWithMuonID.Muon.Selection        = [""]
##########################################################################
#
# Make the kaons w/ e ID available
#
Hlt2BiKalmanFittedKaonsWithEID = NoPIDsParticleMaker("Hlt2BiKalmanFittedKaonsWithEID"
                                            , Particle 		= "kaon"
                                            , Input 		= BiKalmanFittedChargedCaloProtoMaker.outputSelection()
                                            , Output        = 'Hlt2/Hlt2BiKalmanFittedKaonsWithEID/Particles'
                                            , WriteP2PVRelations	= False
                                            )
Hlt2BiKalmanFittedKaonsWithEID.addTool(ProtoParticleCALOFilter('Electron'))
Hlt2BiKalmanFittedKaonsWithEID.Electron.Selection = [""]
##########################################################################
#
# Make the protons
#
Hlt2BiKalmanFittedProtons = NoPIDsParticleMaker("Hlt2BiKalmanFittedProtons"
                                            , Particle 			    =  "proton"
                                            , Input 			    =  BiKalmanFittedChargedProtoMaker.outputSelection()
                                            , Output                = 'Hlt2/Hlt2BiKalmanFittedProtons/Particles'
                                            , WriteP2PVRelations 	=  False
                                            )
##########################################################################
#
# Make the downstream protons
#
Hlt2BiKalmanFittedDownProtons = NoPIDsParticleMaker("Hlt2BiKalmanFittedDownProtons"
                                            , Particle 			    =  "proton"
                                            , Input 			    =  BiKalmanFittedChargedDownProtoMaker.outputSelection()
                                            , Output                =  'Hlt2/Hlt2BiKalmanFittedDownProtons/Particles'
                                            , WriteP2PVRelations 	=  False
                                            )
##########################################################################
#
# Make the Rich kaons 
# Note: we set the DLL cut manually to -99999 to avoid the default of 2 specified in the ProtoP filter base class 
#
Hlt2BiKalmanFittedRichKaons = CombinedParticleMaker("Hlt2BiKalmanFittedRichKaons"
                                            , Particle 			=  "kaon"
                                            , Input 			=  BiKalmanFittedChargedRichHadronProtoMaker.outputSelection()
                                            , Output                = 'Hlt2/Hlt2BiKalmanFittedRichKaons/Particles'
                                            , WriteP2PVRelations  	=  False
                                            )
Hlt2BiKalmanFittedRichKaons.addTool(ProtoParticleCALOFilter('Kaon'))
Hlt2BiKalmanFittedRichKaons.Kaon.Selection		=  ["RequiresDet='RICH'"]
##########################################################################
#
# Make the Rich protons 
# Note: we set the DLL cut manually to -99999 to avoid the default of 2 specified in the ProtoP filter base class
#
Hlt2BiKalmanFittedRichProtons = CombinedParticleMaker("Hlt2BiKalmanFittedRichProtons"
                                            , Particle              =  "proton"
                                            , Input                 = BiKalmanFittedChargedRichForProtonsHadronProtoMaker.outputSelection()
                                            , Output                = 'Hlt2/Hlt2BiKalmanFittedRichProtons/Particles'
                                            , WriteP2PVRelations 	=  False
                                            )
Hlt2BiKalmanFittedRichProtons.addTool(ProtoParticleCALOFilter('Proton'))
Hlt2BiKalmanFittedRichProtons.Proton.Selection		= ["RequiresDet='RICH'"]
##########################################################################
#
# Make the Rich LowPT kaons 
# Note: we set the DLL cut manually to -99999 to avoid the default of 2 specified in the ProtoP filter base class
#
Hlt2BiKalmanFittedRichLowPTKaons = CombinedParticleMaker("Hlt2BiKalmanFittedRichLowPTKaons"
                                            , Particle              =  "kaon"
                                            , Input                 = BiKalmanFittedChargedRichForLowPTParticlesHadronProtoMaker.outputSelection()
                                            , Output                = 'Hlt2/Hlt2BiKalmanFittedRichLowPTKaons/Particles'
                                            , WriteP2PVRelations    =  False
                                            )   
Hlt2BiKalmanFittedRichLowPTKaons.addTool(ProtoParticleCALOFilter('Kaon'))
Hlt2BiKalmanFittedRichLowPTKaons.Kaon.Selection     = ["RequiresDet='RICH'"]
##########################################################################
#
# Make the Rich LowPT protons 
# Note: we set the DLL cut manually to -99999 to avoid the default of 2 specified in the ProtoP filter base class
#
Hlt2BiKalmanFittedRichLowPTProtons = CombinedParticleMaker("Hlt2BiKalmanFittedRichLowPTProtons"
                                            , Particle              =  "proton"
                                            , Input                 = BiKalmanFittedChargedRichForLowPTParticlesHadronProtoMaker.outputSelection()
                                            , Output                = 'Hlt2/Hlt2BiKalmanFittedRichLowPTProtons/Particles'
                                            , WriteP2PVRelations    =  False
                                            )
Hlt2BiKalmanFittedRichLowPTProtons.addTool(ProtoParticleCALOFilter('Proton'))
Hlt2BiKalmanFittedRichLowPTProtons.Proton.Selection		= ["RequiresDet='RICH'"]
########################################################################
#
# Make the electrons
#
Hlt2BiKalmanFittedElectrons	= CombinedParticleMaker("Hlt2BiKalmanFittedElectrons"
                                            , Particle 			    =  "electron"
                                            , Input 			    =  BiKalmanFittedChargedCaloProtoMaker.outputSelection()
                                            , Output                = 'Hlt2/Hlt2BiKalmanFittedElectrons/Particles'
                                            , WriteP2PVRelations 	=  False
                                            )
Hlt2BiKalmanFittedElectrons.addTool(ProtoParticleCALOFilter('Electron'))
Hlt2BiKalmanFittedElectrons.Electron.Selection 		= ["RequiresDet='CALO' CombDLL(e-pi)>'-2.0'"]
########################################################################
#
# Make the electrons from L0
#
Hlt2BiKalmanFittedElectronsFromL0	= CombinedParticleMaker("Hlt2BiKalmanFittedElectronsFromL0"
                                            , Particle 			    =  "electron"
                                            , Input 			    =  BiKalmanFittedElectronFromL0Maker.outputSelection()
                                            , Output                = 'Hlt2/Hlt2BiKalmanFittedElectronsFromL0/Particles'
                                            , WriteP2PVRelations 	=  False
                                            )
Hlt2BiKalmanFittedElectronsFromL0.addTool(ProtoParticleCALOFilter('Electron'))
Hlt2BiKalmanFittedElectronsFromL0.Electron.Selection 		= ["RequiresDet='CALO' CombDLL(e-pi)>'-2.0'"]
##########################################################################
#
# Make the second loop pions
#
Hlt2BiKalmanFittedSecondLoopPions = NoPIDsParticleMaker("Hlt2BiKalmanFittedSecondLoopPions"
                                            , Particle              =  "pion"
                                            , Input                 =  BiKalmanFittedChargedSecondLoopProtoMaker.outputSelection()
                                            , Output                = 'Hlt2/Hlt2BiKalmanFittedSecondLoopPions/Particles'
                                            , WriteP2PVRelations    =  False
                                            )
##########################################################################
#
# Make the second loop protons
#
Hlt2BiKalmanFittedSecondLoopProtons = NoPIDsParticleMaker("Hlt2BiKalmanFittedSecondLoopProtons"
                                            , Particle              =  "proton"
                                            , Input                 =  BiKalmanFittedChargedSecondLoopProtoMaker.outputSelection()
                                            , Output                = 'Hlt2/Hlt2BiKalmanFittedSecondLoopProtons/Particles'
                                            , WriteP2PVRelations    =  False
                                            ) 
##########################################################################
#
# Make the second loop kaons
#
Hlt2BiKalmanFittedSecondLoopKaons = NoPIDsParticleMaker("Hlt2BiKalmanFittedSecondLoopKaons"
                                            , Particle              =  "kaon"
                                            , Input                 =  BiKalmanFittedChargedSecondLoopProtoMaker.outputSelection()
                                            , Output                = 'Hlt2/Hlt2BiKalmanFittedSecondLoopKaons/Particles'
                                            , WriteP2PVRelations    =  False
                                            )
##########################################################################
#
# Make the second loop Muons
#
Hlt2BiKalmanFittedSecondLoopMuons = CombinedParticleMaker("Hlt2BiKalmanFittedSecondLoopMuons"
                                            , Particle 			    = "muon"
                                            , Input 				=  BiKalmanFittedMuonSecondLoopProtoMaker.outputSelection()
                                            , Output                = 'Hlt2/Hlt2BiKalmanFittedSecondLoopMuons/Particles'
                                            , WriteP2PVRelations 	=  False
                                            )
Hlt2BiKalmanFittedSecondLoopMuons.addTool(ProtoParticleMUONFilter('Muon'))
Hlt2BiKalmanFittedSecondLoopMuons.Muon.Selection 			= ["RequiresDet='MUON' IsMuon=True"]
##########################################################################
# Make the photons
#
Hlt2BiKalmanFittedPhotons = PhotonMakerAlg("Hlt2BiKalmanFittedPhotons")
Hlt2BiKalmanFittedPhotons.Input = BiKalmanFittedNeutralProtoMaker.outputSelection()
Hlt2BiKalmanFittedPhotons.Output = 'Hlt2/Hlt2BiKalmanFittedPhotons/Particles'
Hlt2BiKalmanFittedPhotons.addTool(PhotonMaker)
Hlt2BiKalmanFittedPhotons.PhotonMaker.Input = BiKalmanFittedNeutralProtoMaker.outputSelection()
Hlt2BiKalmanFittedPhotons.PhotonMaker.ConvertedPhotons = True  
Hlt2BiKalmanFittedPhotons.PhotonMaker.UnconvertedPhotons = True  
Hlt2BiKalmanFittedPhotons.PhotonMaker.PtCut = 200.* MeV 
Hlt2BiKalmanFittedPhotons.WriteP2PVRelations = False
##########################################################################
# Make the photons from L0: high and low pt
#
# High pt
Hlt2BiKalmanFittedPhotonsFromL0 = PhotonMakerAlg("Hlt2BiKalmanFittedPhotonsFromL0")
Hlt2BiKalmanFittedPhotonsFromL0.Input = BiKalmanFittedPhotonFromL0Maker.outputSelection()
Hlt2BiKalmanFittedPhotonsFromL0.Output = 'Hlt2/Hlt2BiKalmanFittedPhotonsFromL0/Particles'
Hlt2BiKalmanFittedPhotonsFromL0.addTool(PhotonMaker)
Hlt2BiKalmanFittedPhotonsFromL0.PhotonMaker.Input = BiKalmanFittedPhotonFromL0Maker.outputSelection()
Hlt2BiKalmanFittedPhotonsFromL0.PhotonMaker.ConvertedPhotons = True  
Hlt2BiKalmanFittedPhotonsFromL0.PhotonMaker.UnconvertedPhotons = True  
Hlt2BiKalmanFittedPhotonsFromL0.PhotonMaker.PtCut = 200.* MeV 
# Low pt
Hlt2BiKalmanFittedPhotonsFromL0Low = PhotonMakerAlg("Hlt2BiKalmanFittedPhotonsFromL0Low")
Hlt2BiKalmanFittedPhotonsFromL0Low.Input = BiKalmanFittedPhotonFromL0LowMaker.outputSelection()
Hlt2BiKalmanFittedPhotonsFromL0Low.Output = 'Hlt2/Hlt2BiKalmanFittedPhotonsFromL0Low/Particles'
Hlt2BiKalmanFittedPhotonsFromL0Low.addTool(PhotonMaker)
Hlt2BiKalmanFittedPhotonsFromL0Low.PhotonMaker.Input = BiKalmanFittedPhotonFromL0LowMaker.outputSelection()
Hlt2BiKalmanFittedPhotonsFromL0Low.PhotonMaker.ConvertedPhotons = True  
Hlt2BiKalmanFittedPhotonsFromL0Low.PhotonMaker.UnconvertedPhotons = True  
Hlt2BiKalmanFittedPhotonsFromL0Low.PhotonMaker.PtCut = 200.* MeV 
###############################Low###########################################

#
# define exported symbols -- these are for available
# for use in Hlt2 by adding:
#
# from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedMuons
#

__all__ = ( 	'BiKalmanFittedMuons', 
              'BiKalmanFittedElectrons', 
              'BiKalmanFittedElectronsFromL0', 
              'BiKalmanFittedKaons', 
              'BiKalmanFittedKaons', 
              'BiKalmanFittedPions',
              'BiKalmanFittedSecondLoopKaons',
              'BiKalmanFittedSecondLoopPions',
              'BiKalmanFittedSecondLoopProtons',
              'BiKalmanFittedSecondLoopMuons',
              'BiKalmanFittedDownPions',
              'BiKalmanFittedProtons',
              'BiKalmanFittedDownProtons',
              'BiKalmanFittedRichKaons',
              'BiKalmanFittedRichProtons',
              'BiKalmanFittedRichLowPTProtons',
              'BiKalmanFittedPhotons',
              'BiKalmanFittedPhotonsFromL0',
              'BiKalmanFittedPhotonsFromL0Low' )

#
BiKalmanFittedKaons         = bindMembers( None, [ BiKalmanFittedChargedProtoMaker			, Hlt2BiKalmanFittedKaons 	] )
BiKalmanFittedPions         = bindMembers( None, [ BiKalmanFittedChargedProtoMaker			, Hlt2BiKalmanFittedPions 	] )

BiKalmanFittedSecondLoopKaons = bindMembers( None, [ BiKalmanFittedChargedSecondLoopProtoMaker , Hlt2BiKalmanFittedSecondLoopKaons   ] ) 
BiKalmanFittedSecondLoopPions = bindMembers( None, [ BiKalmanFittedChargedSecondLoopProtoMaker , Hlt2BiKalmanFittedSecondLoopPions   ] )
BiKalmanFittedSecondLoopProtons = bindMembers( None, [ BiKalmanFittedChargedSecondLoopProtoMaker , Hlt2BiKalmanFittedSecondLoopProtons   ] )
BiKalmanFittedSecondLoopMuons = bindMembers( None, [ BiKalmanFittedMuonSecondLoopProtoMaker, Hlt2BiKalmanFittedSecondLoopMuons])

BiKalmanFittedDownPions     = bindMembers( None, [ BiKalmanFittedChargedDownProtoMaker      , Hlt2BiKalmanFittedDownPions   ] )
BiKalmanFittedProtons       = bindMembers( None, [ BiKalmanFittedChargedProtoMaker			, Hlt2BiKalmanFittedProtons 	] )
BiKalmanFittedDownProtons   = bindMembers( None, [ BiKalmanFittedChargedDownProtoMaker      , Hlt2BiKalmanFittedDownProtons ] )
BiKalmanFittedMuons         = bindMembers( None, [ BiKalmanFittedMuonProtoMaker				, Hlt2BiKalmanFittedMuons 	] )
BiKalmanFittedElectrons           = bindMembers( None, [ BiKalmanFittedChargedCaloProtoMaker		, Hlt2BiKalmanFittedElectrons 	] ) 
BiKalmanFittedElectronsFromL0     = bindMembers( None, [ BiKalmanFittedElectronFromL0Maker	   	, Hlt2BiKalmanFittedElectronsFromL0 	] ) 
BiKalmanFittedRichKaons     = bindMembers( None, [ BiKalmanFittedChargedRichHadronProtoMaker, Hlt2BiKalmanFittedRichKaons 	] )
BiKalmanFittedRichLowPTKaons = bindMembers( None, [ BiKalmanFittedChargedRichForLowPTParticlesHadronProtoMaker, Hlt2BiKalmanFittedRichLowPTKaons])
BiKalmanFittedRichProtons   = bindMembers( None, [ BiKalmanFittedChargedRichForProtonsHadronProtoMaker , Hlt2BiKalmanFittedRichProtons ] )
BiKalmanFittedRichLowPTProtons = bindMembers( None, [ BiKalmanFittedChargedRichForLowPTParticlesHadronProtoMaker , Hlt2BiKalmanFittedRichLowPTProtons ] )
BiKalmanFittedPhotons          = bindMembers( None, [ BiKalmanFittedNeutralProtoMaker  ,   Hlt2BiKalmanFittedPhotons         ] )
BiKalmanFittedPhotonsFromL0    = bindMembers( None, [ BiKalmanFittedPhotonFromL0Maker  ,   Hlt2BiKalmanFittedPhotonsFromL0   ] ) 
BiKalmanFittedPhotonsFromL0Low = bindMembers( None, [ BiKalmanFittedPhotonFromL0LowMaker  ,   Hlt2BiKalmanFittedPhotonsFromL0Low   ] ) 
BiKalmanFittedKaonsWithMuonID = bindMembers( None, [ BiKalmanFittedMuonProtoMaker, Hlt2BiKalmanFittedKaonsWithMuonID] )
BiKalmanFittedKaonsWithEID = bindMembers( None, [ BiKalmanFittedChargedCaloProtoMaker, Hlt2BiKalmanFittedKaonsWithEID] )
