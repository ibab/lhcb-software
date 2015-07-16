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

##########################################################################
#
# Now all PID
# 
##########################################################################
#
# Charged hadron protoparticles, no Rich
#
#BiKalmanFittedChargedProtoMaker 			= Hlt2BiKalmanFittedForwardTracking.hlt2ChargedNoPIDsProtos()
BiKalmanFittedChargedProtoMaker 			= Hlt2BiKalmanFittedForwardTracking.hlt2ChargedAllPIDsProtos()
#
# Downstream hadron protoparticles, no Rich
#
BiKalmanFittedChargedDownProtoMaker			= Hlt2BiKalmanFittedDownstreamTracking.hlt2ChargedNoPIDsProtos()
#BiKalmanFittedChargedDownProtoMaker			= Hlt2BiKalmanFittedDownstreamTracking.hlt2ChargedAllPIDsProtos()
#
# hadrons with the Rich
#
#BiKalmanFittedChargedRichHadronProtoMaker 		= Hlt2BiKalmanFittedForwardTracking.hlt2ChargedRichProtos()
BiKalmanFittedChargedRichHadronProtoMaker 		= Hlt2BiKalmanFittedForwardTracking.hlt2ChargedAllPIDsProtos()
#
# electrons
#
#BiKalmanFittedChargedCaloProtoMaker 			= Hlt2BiKalmanFittedForwardTracking.hlt2ChargedCaloProtos()
BiKalmanFittedChargedCaloProtoMaker 			= Hlt2BiKalmanFittedForwardTracking.hlt2ChargedAllPIDsProtos()
BiKalmanFittedElectronFromL0Maker         = Hlt2BiKalmanFittedForwardTracking.hlt2ElectronsFromL0()
BiKalmanFittedChargedCaloDownProtoMaker			= Hlt2BiKalmanFittedDownstreamTracking.hlt2ChargedCaloProtos()
#BiKalmanFittedChargedCaloDownProtoMaker			= Hlt2BiKalmanFittedDownstreamTracking.hlt2ChargedAllPIDsProtos()
#
# Muons  
#
#BiKalmanFittedMuonProtoMaker 				= Hlt2BiKalmanFittedForwardTracking.hlt2ChargedMuonProtos()
BiKalmanFittedMuonProtoMaker 				= Hlt2BiKalmanFittedForwardTracking.hlt2ChargedAllPIDsProtos()

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
# Make muons without PID requirements (e.g. for PID calibration lines)
#
Hlt2BiKalmanFittedNoPIDsMuons = NoPIDsParticleMaker("Hlt2BiKalmanFittedNoPIDsMuons"
                                            , Particle                      = "muon"
                                            , Input                         = BiKalmanFittedMuonProtoMaker.outputSelection()
                                            , Output                        = "Hlt2/Hlt2BiKalmanFittedNoPIDsMuons/Particles"
                                            , WriteP2PVRelations            = False
                                            )

##########################################################################
#
# Make the pions
#
Hlt2BiKalmanFittedPions = NoPIDsParticleMaker("Hlt2BiKalmanFittedPions"
                                             , Particle 			=  "pion"
                                             , Input 				=  BiKalmanFittedChargedRichHadronProtoMaker.outputSelection()
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
                                            , Input              =  BiKalmanFittedChargedRichHadronProtoMaker.outputSelection()
                                            , Output             = 'Hlt2/Hlt2BiKalmanFittedKaons/Particles'
                                            , WriteP2PVRelations =  False
                                            )
##########################################################################
#
# Make the downstream kaons
#
Hlt2BiKalmanFittedDownKaons = NoPIDsParticleMaker("Hlt2BiKalmanFittedDownKaons"
                                                 , Particle 			=  "pion"
                                                 , Input 			=  BiKalmanFittedChargedDownProtoMaker.outputSelection()
                                                 , Output                   = 'Hlt2/Hlt2BiKalmanFittedDownKaons/Particles'
                                                 , WriteP2PVRelations 		=  False
                                                 )
##########################################################################
#
# Make the pions w/ muon ID available
#
Hlt2BiKalmanFittedPionsWithMuonID = NoPIDsParticleMaker("Hlt2BiKalmanFittedPionsWithMuonID"
                                            , Particle 		= "pion"
                                            , Input 		= BiKalmanFittedMuonProtoMaker.outputSelection()
                                            , Output        = 'Hlt2/Hlt2BiKalmanFittedPionsWithMuonID/Particles'
                                            , WriteP2PVRelations	= False
                                            )
Hlt2BiKalmanFittedPionsWithMuonID.addTool(ProtoParticleMUONFilter('Muon'))
Hlt2BiKalmanFittedPionsWithMuonID.Muon.Selection        = [""]
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
                                            , Input 			    =  BiKalmanFittedChargedRichHadronProtoMaker.outputSelection()
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
# Make the Rich pions 
# Note: we set the DLL cut manually to -99999 to avoid the default of 2 specified in the ProtoP filter base class 
#
Hlt2BiKalmanFittedRichPions = CombinedParticleMaker("Hlt2BiKalmanFittedRichPions"
                                            , Particle 			=  "pion"
                                            , Input 			=  BiKalmanFittedChargedRichHadronProtoMaker.outputSelection()
                                            , Output                = 'Hlt2/Hlt2BiKalmanFittedRichPions/Particles'
                                            , WriteP2PVRelations  	=  False
                                            )
Hlt2BiKalmanFittedRichPions.addTool(ProtoParticleCALOFilter('Pion'))
Hlt2BiKalmanFittedRichPions.Pion.Selection		=  ["RequiresDet='RICH'"]
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
                                            , Input                 = BiKalmanFittedChargedRichHadronProtoMaker.outputSelection()
                                            , Output                = 'Hlt2/Hlt2BiKalmanFittedRichProtons/Particles'
                                            , WriteP2PVRelations 	=  False
                                            )
Hlt2BiKalmanFittedRichProtons.addTool(ProtoParticleCALOFilter('Proton'))
Hlt2BiKalmanFittedRichProtons.Proton.Selection		= ["RequiresDet='RICH'"]
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

##########################################################################
#
# Make electrons without PID requirements (e.g. for PID calibration lines)
#
Hlt2BiKalmanFittedNoPIDsElectrons = NoPIDsParticleMaker("Hlt2BiKalmanFittedNoPIDsElectrons"
                                            , Particle                      = "electron"
                                            , Input                         = BiKalmanFittedChargedCaloProtoMaker.outputSelection()
                                            , Output                        = "Hlt2/Hlt2BiKalmanFittedNoPIDsElectrons/Particles"
                                            , WriteP2PVRelations            = False
                                            )

##########################################################################
#
# Make the downstream electrons
#
Hlt2BiKalmanFittedDownElectrons = CombinedParticleMaker("Hlt2BiKalmanFittedDownElectrons"
                                            , Particle 			    =  "electron"
                                            , Input 			    =  BiKalmanFittedChargedCaloDownProtoMaker.outputSelection()
                                            , Output                =  'Hlt2/Hlt2BiKalmanFittedDownElectrons/Particles'
                                            , WriteP2PVRelations 	=  False
                                            )
Hlt2BiKalmanFittedDownElectrons.addTool(ProtoParticleCALOFilter('Electron'))
Hlt2BiKalmanFittedDownElectrons.Electron.Selection 		= ["RequiresDet='CALO' CombDLL(e-pi)>'-2.0'"]

##########################################################################
#
# Make downstream electrons without PID requirements (e.g. for PID calibration lines)
#
Hlt2BiKalmanFittedNoPIDsDownElectrons = NoPIDsParticleMaker("Hlt2BiKalmanFittedNoPIDsDownElectrons"
                                            , Particle                      = "electron"
                                            , Input                         = BiKalmanFittedChargedCaloDownProtoMaker.outputSelection()
                                            , Output                        = "Hlt2/Hlt2BiKalmanFittedNoPIDsDownElectrons/Particles"
                                            , WriteP2PVRelations            = False
                                            )

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
# Make electrons from L0 without a DLL requirements (e.g. for PID calibration lines)
#
Hlt2BiKalmanFittedNoDLLCutElectronsFromL0 = NoPIDsParticleMaker("Hlt2BiKalmanFittedNoDLLCutElectronsFromL0"
                                            , Particle                      = "electron"
                                            , Input                         = BiKalmanFittedElectronFromL0Maker.outputSelection()
                                            , Output                        = "Hlt2/Hlt2BiKalmanFittedNoDLLCutElectronsFromL0/Particles"
                                            , WriteP2PVRelations            = False
                                            )

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

__all__ = (   'BiKalmanFittedMuons',
              'BiKalmanFittedNoPIDsMuons',
              'BiKalmanFittedElectrons',
              'BiKalmanFittedNoPIDsElectrons',
              'BiKalmanFittedElectronsFromL0',
              'BiKalmanFittedNoDLLCutElectronsFromL0', 
              'BiKalmanFittedDownElectrons', 
              'BiKalmanFittedNoPIDsDownElectrons',
              'BiKalmanFittedKaons', 
              'BiKalmanFittedPions',
              'BiKalmanFittedDownPions',
              'BiKalmanFittedProtons',
              'BiKalmanFittedDownProtons',
              'BiKalmanFittedRichPions',
              'BiKalmanFittedRichKaons',
              'BiKalmanFittedRichProtons',
              'BiKalmanFittedPhotons',
              'BiKalmanFittedPionsWithMuonID',
              'BiKalmanFittedKaonsWithMuonID',
              'BiKalmanFittedPhotonsFromL0',
              'BiKalmanFittedPhotonsFromL0Low' )

#
BiKalmanFittedKaons         = bindMembers( None, [ BiKalmanFittedChargedRichHadronProtoMaker, Hlt2BiKalmanFittedKaons 	] )
BiKalmanFittedPions         = bindMembers( None, [ BiKalmanFittedChargedRichHadronProtoMaker, Hlt2BiKalmanFittedPions 	] )

BiKalmanFittedDownPions     = bindMembers( None, [ BiKalmanFittedChargedDownProtoMaker      , Hlt2BiKalmanFittedDownPions   ] )
BiKalmanFittedDownKaons     = bindMembers( None, [ BiKalmanFittedChargedDownProtoMaker      , Hlt2BiKalmanFittedDownKaons   ] )
BiKalmanFittedProtons       = bindMembers( None, [ BiKalmanFittedChargedRichHadronProtoMaker, Hlt2BiKalmanFittedProtons 	] )
BiKalmanFittedDownProtons   = bindMembers( None, [ BiKalmanFittedChargedDownProtoMaker      , Hlt2BiKalmanFittedDownProtons ] )
BiKalmanFittedMuons         = bindMembers( None, [ BiKalmanFittedMuonProtoMaker				, Hlt2BiKalmanFittedMuons 	] )
BiKalmanFittedNoPIDsMuons   = bindMembers( None, [ BiKalmanFittedMuonProtoMaker                         , Hlt2BiKalmanFittedNoPIDsMuons ] )
BiKalmanFittedElectrons           = bindMembers( None, [ BiKalmanFittedChargedCaloProtoMaker		, Hlt2BiKalmanFittedElectrons 	] )
BiKalmanFittedNoPIDsElectrons     = bindMembers( None, [ BiKalmanFittedChargedCaloProtoMaker            , Hlt2BiKalmanFittedNoPIDsElectrons ] )
BiKalmanFittedElectronsFromL0     = bindMembers( None, [ BiKalmanFittedElectronFromL0Maker	   	, Hlt2BiKalmanFittedElectronsFromL0 	] ) 
BiKalmanFittedNoDLLCutElectronsFromL0 = bindMembers( None, [ BiKalmanFittedElectronFromL0Maker                  , Hlt2BiKalmanFittedNoDLLCutElectronsFromL0 ] )
BiKalmanFittedDownElectrons           = bindMembers( None, [ BiKalmanFittedChargedCaloDownProtoMaker		, Hlt2BiKalmanFittedDownElectrons 	] ) 
BiKalmanFittedNoPIDsDownElectrons     = bindMembers( None, [ BiKalmanFittedChargedCaloDownProtoMaker            , Hlt2BiKalmanFittedNoPIDsDownElectrons ] )
BiKalmanFittedRichPions     = bindMembers( None, [ BiKalmanFittedChargedRichHadronProtoMaker, Hlt2BiKalmanFittedRichPions 	] )
BiKalmanFittedRichKaons     = bindMembers( None, [ BiKalmanFittedChargedRichHadronProtoMaker, Hlt2BiKalmanFittedRichKaons 	] )
BiKalmanFittedRichProtons   = bindMembers( None, [ BiKalmanFittedChargedRichHadronProtoMaker , Hlt2BiKalmanFittedRichProtons ] )
BiKalmanFittedPhotons          = bindMembers( None, [ BiKalmanFittedNeutralProtoMaker  ,   Hlt2BiKalmanFittedPhotons         ] )
BiKalmanFittedPhotonsFromL0    = bindMembers( None, [ BiKalmanFittedPhotonFromL0Maker  ,   Hlt2BiKalmanFittedPhotonsFromL0   ] ) 
BiKalmanFittedPhotonsFromL0Low = bindMembers( None, [ BiKalmanFittedPhotonFromL0LowMaker  ,   Hlt2BiKalmanFittedPhotonsFromL0Low   ] ) 
BiKalmanFittedKaonsWithMuonID = bindMembers( None, [ BiKalmanFittedMuonProtoMaker, Hlt2BiKalmanFittedKaonsWithMuonID] )
BiKalmanFittedPionsWithMuonID = bindMembers( None, [ BiKalmanFittedMuonProtoMaker, Hlt2BiKalmanFittedPionsWithMuonID] )
BiKalmanFittedKaonsWithEID = bindMembers( None, [ BiKalmanFittedChargedCaloProtoMaker, Hlt2BiKalmanFittedKaonsWithEID] )
