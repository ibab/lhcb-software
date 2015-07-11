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
from HltLine.HltLine import bindMembers, Hlt2Member
from Configurables import FilterDesktop
from Configurables import CombinedParticleMaker, BestPIDParticleMaker, NoPIDsParticleMaker, TrackSelector
from Configurables import ProtoParticleMUONFilter
#
# These are all based on fitted tracks
#
from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking, Hlt2BiKalmanFittedDownstreamTracking
Hlt2BiKalmanFittedForwardTracking   			= Hlt2BiKalmanFittedForwardTracking()
Hlt2BiKalmanFittedDownstreamTracking   			= Hlt2BiKalmanFittedDownstreamTracking()

# Tag and probe tracking configurations
from HltTracking.Hlt2ProbeTrackingConfigurations import (Hlt2MuonTTTracking,
                                                         Hlt2VeloMuonTracking,
                                                         Hlt2FullDownstreamTracking)

Hlt2MuonTTTracking = Hlt2MuonTTTracking()
Hlt2VeloMuonTracking = Hlt2VeloMuonTracking()
Hlt2FullDownstreamTracking = Hlt2FullDownstreamTracking()

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
# No PID muon particles, for association with probe track
#
##########################################################################
Hlt2LongAssocParts = NoPIDsParticleMaker("Hlt2LongAssocParts")
Hlt2LongAssocParts.Particle = 'pion' # call this a pion for CombinePart to distringuish from probe
Hlt2LongAssocParts.addTool( TrackSelector )
Hlt2LongAssocParts.TrackSelector.TrackTypes = [ "Long" ]
Hlt2LongAssocParts.Input =  muonWithCaloProtos.outputSelection()
Hlt2LongAssocParts.Output =  "Hlt2/Hlt2LongAssocMuons/Particles"

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



##########################################################################
#
# VeloMuon particles
#
##########################################################################
Hlt2VeloMuonParts = NoPIDsParticleMaker("Hlt2VeloMuonParts")
Hlt2VeloMuonParts.Particle = 'Muon'
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

##########################################################################
#
# Velo particles
#
##########################################################################

from HltTracking.HltTrackNames import HltSharedTrackLoc, HltDefaultFitSuffix, _baseProtoPLocation, TrackName, Hlt2TrackEffRoot
from HltLine.HltLine import bindMembers
from Configurables import CombinedParticleMaker, ChargedProtoParticleMaker, BestPIDParticleMaker, NoPIDsParticleMaker
from Configurables import DelegatingTrackSelector
from HltTracking.HltSharedTracking import RevivedVelo, FittedVelo

Hlt2VeloProtos = ChargedProtoParticleMaker('Hlt2VeloProtosForTrackEff')
Hlt2VeloProtos.Inputs = [ FittedVelo.outputSelection() ]
Hlt2VeloProtos.Output = _baseProtoPLocation("Hlt2", Hlt2TrackEffRoot+"/"+TrackName["Velo"])
Hlt2VeloProtos.addTool(DelegatingTrackSelector, name='delTrackSelVelo')
Hlt2VeloProtos.delTrackSelVelo.TrackTypes = ['Velo']

Hlt2VeloPionParts = NoPIDsParticleMaker("Hlt2VeloPionParts")
Hlt2VeloPionParts.Particle = 'pion'
Hlt2VeloPionParts.Input =  Hlt2VeloProtos.Output
Hlt2VeloPionParts.Output =  Hlt2TrackEffRoot+"/Hlt2VeloPions/Particles"

Hlt2VeloKaonParts = NoPIDsParticleMaker("Hlt2VeloKaonParts")
Hlt2VeloKaonParts.Particle = 'kaon'
Hlt2VeloKaonParts.Input =  Hlt2VeloProtos.Output
Hlt2VeloKaonParts.Output =  Hlt2TrackEffRoot+"/Hlt2VeloKaons/Particles"

Hlt2GoodVeloKaons = Hlt2Member( FilterDesktop,"VeloKaons",
                                Inputs = [ Hlt2VeloKaonParts.Output ],
                                Code = "(ETA > 1.9) & (ETA < 4.9) & (MIPCHI2DV(PRIMARY) > 4)")
Hlt2GoodVeloPions = Hlt2Member( FilterDesktop,"VeloPions",
                                Inputs = [ Hlt2VeloPionParts.Output ],
                                Code = "(ETA > 1.9) & (ETA < 4.9) & (MIPCHI2DV(PRIMARY) > 4)")


# define exported symbols -- these are for available
# for use in Hlt2 by adding:
#
# from Hlt2SharedParticles.TagAndProbeParticles import TagAndProbeMuons
#

__all__ = ( 'TagAndProbePions',
            'TagAndProbeMuons',
            'LongAssocMuons',
            'TagMuonTTMuons',
            'TagVeloMuons',
            'TagDownstreamMuons',
            'Hlt2ProbeVeloPions',
            'Hlt2ProbeVeloKaons',
            'Hlt2GoodProbeVeloPions',
            'Hlt2GoodProbeVeloKaons')

Hlt2ProbeVeloPions  = bindMembers( None, [ FittedVelo, Hlt2VeloProtos , Hlt2VeloPionParts ] )
Hlt2ProbeVeloKaons  = bindMembers( None, [ FittedVelo, Hlt2VeloProtos , Hlt2VeloKaonParts ] )
Hlt2GoodProbeVeloPions  = bindMembers( 'Good', [ FittedVelo, Hlt2VeloProtos, Hlt2ProbeVeloPions, Hlt2GoodVeloPions])
Hlt2GoodProbeVeloKaons  = bindMembers( 'Good', [ FittedVelo, Hlt2VeloProtos, Hlt2ProbeVeloKaons, Hlt2GoodVeloKaons])

TagAndProbePions   = bindMembers( None, [ caloProtos		,	Hlt2TagAndProbePions 	] )
TagAndProbeMuons   = bindMembers( None, [ muonWithCaloProtos	, 	Hlt2TagAndProbeMuons	] )
LongAssocMuons     = bindMembers( None, [ muonWithCaloProtos	, 	Hlt2LongAssocParts	] )
ProbeMuonTTMuons	= bindMembers( None, [ muonTTProtos	,	Hlt2MuonTTParts	] )
ProbeVeloMuons   	= bindMembers( None, [ velomuonProtos, Hlt2VeloMuonParts] )
ProbeDownstreamMuons	= bindMembers( None, [ fulldownProtos, Hlt2FullDownParts] )
