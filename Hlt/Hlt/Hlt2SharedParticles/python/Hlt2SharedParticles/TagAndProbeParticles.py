### @file
#
#  Charged particles for the tag-and-probe muon lines
#
#  @author V. Gligorov vladimir.gligorov@cern.ch
#  @date 2010-03-25
#
##
# =============================================================================
__author__  = "V. Gligorov vladimir.gligorov@cern.ch"
# =============================================================================
from Gaudi.Configuration import *
from HltLine.HltLine import bindMembers
from Configurables import CombinedParticleMaker, ChargedProtoParticleMaker, BestPIDParticleMaker, NoPIDsParticleMaker, TrackSelector
from Configurables import ProtoParticleMUONFilter
from Configurables import GaudiSequencer
from GaudiKernel.SystemOfUnits import MeV
#
# These are all based on unfitted tracks
#
from HltTracking.Hlt2ProbeTracking import Hlt2ProbeTracking
from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking, Hlt2BiKalmanFittedDownstreamTracking
Hlt2BiKalmanFittedForwardTracking   			= Hlt2BiKalmanFittedForwardTracking()
Hlt2BiKalmanFittedDownstreamTracking   			= Hlt2BiKalmanFittedDownstreamTracking()
Hlt2MuonTTTracking = Hlt2ProbeTracking('Hlt2MuonTTTracking',ProbeTrack='MuonTT')
Hlt2VeloMuonTracking = Hlt2ProbeTracking('Hlt2VeloMuonTracking',ProbeTrack='VeloMuon')
Hlt2FullDownstreamTracking = Hlt2ProbeTracking('Hlt2FullDownstreamTracking',ProbeTrack='FullDownstream')
##########################################################################
#
# Charged protoparticles -> pulls all the pid
#
caloProtos 		= Hlt2BiKalmanFittedForwardTracking.hlt2ChargedCaloProtos( )
muonWithCaloProtos 	= Hlt2BiKalmanFittedForwardTracking.hlt2ChargedMuonWithCaloProtos( )
muonTTProtos = Hlt2MuonTTTracking.hlt2ProbeMuonProtos()
velomuonProtos = Hlt2VeloMuonTracking.hlt2ProbeMuonProtos()
fulldownProtos = Hlt2FullDownstreamTracking.hlt2ProbeMuonProtos()
##########################################################################
# Make the pions
#
Hlt2TagAndProbePions = CombinedParticleMaker("Hlt2TagAndProbePions")
Hlt2TagAndProbePions.Input =  caloProtos.outputSelection()
Hlt2TagAndProbePions.Output = 'Hlt2/Hlt2TagAndProbePions/Particles'
Hlt2TagAndProbePions.Particle =  "pion" 
Hlt2TagAndProbePions.WriteP2PVRelations = False
##########################################################################
# Make the Muons
#
Hlt2TagAndProbeMuons = CombinedParticleMaker("Hlt2TagAndProbeMuons")
Hlt2TagAndProbeMuons.Particle = "muon" 
Hlt2TagAndProbeMuons.addTool(ProtoParticleMUONFilter('Muon'))
Hlt2TagAndProbeMuons.Muon.Selection = ["RequiresDet='MUON' IsMuon=True" ]
Hlt2TagAndProbeMuons.Input =  muonWithCaloProtos.outputSelection()
Hlt2TagAndProbeMuons.Output = 'Hlt2/Hlt2TagAndProbeMuons/Particles'
Hlt2TagAndProbeMuons.WriteP2PVRelations = False
##########################################################################
#
# MuonTT particles
#
##########################################################################
Hlt2MuonTTParts = NoPIDsParticleMaker("Hlt2MuonTTParts")
Hlt2MuonTTParts.Particle = 'muon'
Hlt2MuonTTParts.addTool( TrackSelector )
Hlt2MuonTTParts.TrackSelector.TrackTypes = [ "Long" ]
Hlt2MuonTTParts.Input =  muonTTProtos.outputSelection()
Hlt2MuonTTParts.Output =  "Hlt2/Hlt2MuonTTMuons/Particles"
Hlt2MuonTTParts.OutputLevel = 6 

##########################################################################
#
# VeloMuon particles
#
##########################################################################
Hlt2VeloMuonParts = NoPIDsParticleMaker("Hlt2VeloMuonParts")
Hlt2VeloMuonParts.Particle = 'Muon'
Hlt2VeloMuonParts.OutputLevel = 6
Hlt2VeloMuonParts.Input =  velomuonProtos.outputSelection()
Hlt2VeloMuonParts.Output =  "Hlt2/Hlt2VeloMuons/Particles"
	
##########################################################################
#
# FullDownstream particles
#
##########################################################################
Hlt2FullDownParts = BestPIDParticleMaker("DownParts" , Particle = "muon")
Hlt2FullDownParts.addTool(ProtoParticleMUONFilter,name="muon")
Hlt2FullDownParts.muon.Selection = ["RequiresDet='MUON' IsMuonLoose=True"]
Hlt2FullDownParts.Particles = [ "muon" ]
Hlt2FullDownParts.Input = fulldownProtos.outputSelection()
Hlt2FullDownParts.Output = "Hlt2/Hlt2DownstreamMuons/Particles"

#
# define exported symbols -- these are for available
# for use in Hlt2 by adding:
#
# from Hlt2SharedParticles.TagAndProbeParticles import TagAndProbeMuons
#

__all__ = ( 'TagAndProbePions', 'TagAndProbeMuons', 'TagMuonTTMuons', 'TagVeloMuons', 'TagDownstreamMuons' )

TagAndProbePions   = bindMembers( None, [ caloProtos		,	Hlt2TagAndProbePions 	] )
TagAndProbeMuons   = bindMembers( None, [ muonWithCaloProtos	, 	Hlt2TagAndProbeMuons	] )
TagMuonTTMuons		= bindMembers( None, [ muonTTProtos	,	Hlt2MuonTTParts	] )
TagVeloMuons   		= bindMembers( None, [ velomuonProtos, Hlt2VeloMuonParts] )
TagDownstreamMuons	= bindMembers( None, [ fulldownProtos, Hlt2FullDownParts] )
