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
from Configurables import CombinedParticleMaker
from Configurables import ProtoParticleMUONFilter
from Configurables import GaudiSequencer
from GaudiKernel.SystemOfUnits import MeV
#
# These are all based on unfitted tracks
#
from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
Hlt2BiKalmanFittedForwardTracking   			= Hlt2BiKalmanFittedForwardTracking()
##########################################################################
#
# Charged protoparticles -> pulls all the pid
#
caloProtos 		= Hlt2BiKalmanFittedForwardTracking.hlt2ChargedCaloProtos( )
muonWithCaloProtos 	= Hlt2BiKalmanFittedForwardTracking.hlt2ChargedMuonWithCaloProtos( )
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
# define exported symbols -- these are for available
# for use in Hlt2 by adding:
#
# from Hlt2SharedParticles.TagAndProbeParticles import TagAndProbeMuons
#

__all__ = ( 'TagAndProbePions', 'TagAndProbeMuons' )

TagAndProbePions   = bindMembers( None, [ caloProtos		,	Hlt2TagAndProbePions 	] )
TagAndProbeMuons   = bindMembers( None, [ muonWithCaloProtos	, 	Hlt2TagAndProbeMuons	] )
