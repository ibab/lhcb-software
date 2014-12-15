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
from Configurables import CombinedParticleMaker, ChargedProtoParticleMaker, BestPIDParticleMaker
from Configurables import ProtoParticleMUONFilter
from Configurables import GaudiSequencer
from GaudiKernel.SystemOfUnits import MeV
#
# These are all based on unfitted tracks
#
from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking, Hlt2BiKalmanFittedDownstreamTracking
from HltTracking.HltTrackNames import _baseProtoPLocation, Hlt2TrackLoc, TrackName
from HltTracking.HltTrackNames import HltSharedTrackLoc, Hlt2TrackRoot, HltBiDirectionalKalmanFitSuffix, HltSharedPIDPrefix, HltMuonIDSuffix, HltMuonTracksName, Hlt2ChargedProtoParticleSuffix, HltMuonProtosSuffix
Hlt2BiKalmanFittedForwardTracking   			= Hlt2BiKalmanFittedForwardTracking()
Hlt2BiKalmanFittedDownstreamTracking   			= Hlt2BiKalmanFittedDownstreamTracking()
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
# Make the Downstream muons
#
from Configurables import (PatSeeding,
			   PatSeedingTool,
			   PatDownstream,
			   TrackStateInitAlg,
			   TrackToDST,
			   MuonCombRec,
                           MuonTTTrack,
			   MuonHitDecode,
                           PatAddTTCoord,
                           TrackMasterFitter,
                           TrackMasterExtrapolator,
                           TrackSelector,
                           NoPIDsParticleMaker,
                           TisTosParticleTagger,
			   StandaloneMuonRec,
			   MuonIDAlg,
			   ChargedProtoParticleAddMuonInfo,
			   ChargedProtoCombineDLLsAlg,
			   ProtoParticleMUONFilter
			   )
from PatAlgorithms import PatAlgConf
from TrackFitter.ConfiguredFitters import ConfiguredFitDownstream, ConfiguredHltFitter
from MuonID import ConfiguredMuonIDs
from Configurables import FastVeloTracking, TrackPrepareVelo, DecodeVeloRawBuffer, DelegatingTrackSelector, VeloMuonBuilder, TrackEventFitter
from Configurables import (MeasurementProviderT_MeasurementProviderTypes__VeloLiteR_,
                           MeasurementProviderT_MeasurementProviderTypes__VeloLitePhi_,
                           MeasurementProviderT_MeasurementProviderTypes__TTLite_,
                           MeasurementProviderT_MeasurementProviderTypes__ITLite_,
                           MeasurementProviderT_MeasurementProviderTypes__UTLite_)
#	Hlt2VeloMuonBuild.Fitter.MeasProvider.VeloRProvider = MeasurementProviderT_MeasurementProviderTypes__VeloLiteR_()
#	Hlt2VeloMuonBuild.Fitter.MeasProvider.VeloPhiProvider = MeasurementProviderT_MeasurementProviderTypes__VeloLitePhi_()
#
# MuonTT particles
#######################################################################
# create the tracks
Hlt2MuonTTTrack = MuonTTTrack("Hlt2MuonTTTrack")
Hlt2MuonTTTrack.AddTTHits = True
Hlt2MuonTTTrack.FillMuonStubInfo = False
Hlt2MuonTTTrack.ToolName = "MuonCombRec"
Hlt2MuonTTTrack.OutputLevel = 6 
Hlt2MuonTTTrack.MC = False
Hlt2MuonTTTrack.addTool( MuonCombRec )
Hlt2MuonTTTrack.MuonCombRec.ClusterTool = "MuonFakeClustering" # to enable: "MuonClusterRec"
Hlt2MuonTTTrack.MuonCombRec.CloneKiller = False
Hlt2MuonTTTrack.MuonCombRec.StrongCloneKiller = False
Hlt2MuonTTTrack.MuonCombRec.SeedStation = 2 # try setting a different seed station
Hlt2MuonTTTrack.MuonCombRec.DecodingTool = "MuonHitDecode"
Hlt2MuonTTTrack.MuonCombRec.PadRecTool = "MuonPadRec"
Hlt2MuonTTTrack.MuonCombRec.AssumePhysics = True
Hlt2MuonTTTrack.MuonCombRec.MeasureTime = True
Hlt2MuonTTTrack.MuonCombRec.addTool(MuonHitDecode("MuonHitDecode"))
Hlt2MuonTTTrack.MuonCombRec.MuonHitDecode.SkipHWNumber = True
Hlt2MuonTTTrack.addTool( PatAddTTCoord )
Hlt2MuonTTTrack.PatAddTTCoord.YTolSlope = 400000.0
Hlt2MuonTTTrack.PatAddTTCoord.XTol = 25.0 # was 12.0
Hlt2MuonTTTrack.PatAddTTCoord.XTolSlope = 400000.0
Hlt2MuonTTTrack.PatAddTTCoord.MaxChi2Tol = 7.0 # was not included
Hlt2MuonTTTrack.PatAddTTCoord.MinAxProj = 5.5 # was 2.5
Hlt2MuonTTTrack.PatAddTTCoord.MajAxProj = 25.0 # was 22.0
Hlt2MuonTTTrack.addTool( TrackMasterFitter )
Hlt2MuonTTTrack.TrackMasterFitter.OutputLevel = 6 
Hlt2MuonTTTrack.TrackMasterFitter.MaterialLocator = "SimplifiedMaterialLocator"
Hlt2MuonTTTrack.TrackMasterFitter.MeasProvider.TTProvider = MeasurementProviderT_MeasurementProviderTypes__TTLite_()
Hlt2MuonTTTrack.addTool( TrackMasterExtrapolator )
Hlt2MuonTTTrack.TrackMasterExtrapolator.MaterialLocator = "SimplifiedMaterialLocator"
Hlt2MuonTTTrack.Output = Hlt2TrackRoot+HltBiDirectionalKalmanFitSuffix+"/"+TrackName["MuonTT"]
###################################################################################
# create the protos
Hlt2MuonTTPParts = ChargedProtoParticleMaker("Hlt2MuonTTPParts")
Hlt2MuonTTPParts.addTool( TrackSelector )
Hlt2MuonTTPParts.TrackSelector.TrackTypes = [ "Long" ]
Hlt2MuonTTPParts.Inputs = [Hlt2MuonTTTrack.Output]
Hlt2MuonTTPParts.Output = _baseProtoPLocation("Hlt2", HltBiDirectionalKalmanFitSuffix+"/"+TrackName["MuonTT"])
Hlt2MuonTTPParts.OutputLevel = 6 
##################################################################################
# create the particles
Hlt2MuonTTParts = NoPIDsParticleMaker("Hlt2MuonTTParts")
Hlt2MuonTTParts.Particle = 'muon'
Hlt2MuonTTParts.addTool( TrackSelector )
Hlt2MuonTTParts.TrackSelector.TrackTypes = [ "Long" ]
Hlt2MuonTTParts.Input =  Hlt2MuonTTPParts.Output
Hlt2MuonTTParts.Output =  "Hlt2/Hlt2MuonTTMuons/Particles"
Hlt2MuonTTParts.OutputLevel = 6 

######## VeloMuon Particles for trackeff TT method ###################
############### Build the MuonVelo tracks #########################

#get Velo/Muon tracks
DecodeVelo = DecodeVeloRawBuffer('DecodeVelo') #has to be executed for VeloMuonBuilder to work, why?
DecodeVelo.DecodeToVeloLiteClusters = False
DecodeVelo.DecodeToVeloClusters = True
StandaloneMuonRec("MyMuonStandalone").OutputLevel = 4

#build VeloMuon track
Hlt2VeloMuonBuild = VeloMuonBuilder('Hlt2VeloMuonBuild')
Hlt2VeloMuonBuild.MuonLocation = 'Hlt1/Track/MuonSeg'
Hlt2VeloMuonBuild.VeloLocation = HltSharedTrackLoc["Velo"] #Velo track location in Hlt
Hlt2VeloMuonBuild.lhcbids = 4
Hlt2VeloMuonBuild.OutputLocation = Hlt2TrackRoot+HltBiDirectionalKalmanFitSuffix+"/"+TrackName["VeloMuon"]
#build protos out of tracks
Hlt2VeloMuonProtos = ChargedProtoParticleMaker('Hlt2VeloMuonProtos')
Hlt2VeloMuonProtos.Inputs = [Hlt2VeloMuonBuild.OutputLocation]
Hlt2VeloMuonProtos.Output = _baseProtoPLocation("Hlt2", HltBiDirectionalKalmanFitSuffix+"/"+TrackName["VeloMuon"])
Hlt2VeloMuonProtos.addTool(DelegatingTrackSelector, name='delTrackSel')
Hlt2VeloMuonProtos.delTrackSel.TrackTypes = ['Long']
Hlt2VeloMuonProtos.OutputLevel = 6
#build particles out of protos
Hlt2VeloMuonParts = NoPIDsParticleMaker("Hlt2VeloMuonParts")
Hlt2VeloMuonParts.Particle = 'Muon'
Hlt2VeloMuonParts.OutputLevel = 6
Hlt2VeloMuonParts.Input =  Hlt2VeloMuonProtos.Output
Hlt2VeloMuonParts.Output =  "Hlt2/Hlt2VeloMuons/Particles"
	
# Downstream Muons
##########################################################################
# new seeding required, as in Hlt2 seeding hits from forward tracks are removed
DownSeed = PatSeeding("DownSeed")
DownSeed.addTool(PatSeedingTool, name='PatSeedingTool')
DownSeed.PatSeedingTool.MinMomentum = 1500 #momentum cut to reduce timing, softer than cut used in the line
DownSeed.PatSeedingTool.NDblOTHitsInXSearch = 2
DownstreamTracking = PatDownstream()
DownstreamTracking.OutputLocation = Hlt2TrackLoc["FullDownstream"]
DownstreamFit = TrackEventFitter('DownstreamFitter')
DownstreamFit.TracksInContainer = DownstreamTracking.OutputLocation
DownstreamFit.TracksOutContainer = Hlt2TrackRoot+HltBiDirectionalKalmanFitSuffix+"/"+TrackName["FullDownstream"]
DownstreamFit.addTool(TrackMasterFitter, name = 'Fitter')
fitter = ConfiguredHltFitter(getattr(DownstreamFit,'Fitter')) #use HltFitter for timing reasons
# add muon ID
idalg = MuonIDAlg("IDalg")
cm=ConfiguredMuonIDs.ConfiguredMuonIDs( "2012" ) #TODO: hardcoded right now, find better solution
cm.configureMuonIDAlg(idalg)
idalg.TrackLocation = DownstreamFit.TracksOutContainer
idalg.MuonIDLocation = Hlt2TrackRoot+HltBiDirectionalKalmanFitSuffix+"/"+HltSharedPIDPrefix+"/"+HltMuonIDSuffix
idalg.MuonTrackLocation = Hlt2TrackRoot+HltBiDirectionalKalmanFitSuffix+"/"+HltSharedPIDPrefix+"/"+HltMuonTracksName
# make protos
DownProtos = ChargedProtoParticleMaker("DownProtos")
DownProtos.Inputs = [DownstreamFit.TracksOutContainer]
DownProtos.Output = _baseProtoPLocation("Hlt2", HltBiDirectionalKalmanFitSuffix+"/"+TrackName["FullDownstream"]+"/"+Hlt2ChargedProtoParticleSuffix+"/"+HltMuonProtosSuffix)
DownProtos.addTool( DelegatingTrackSelector, name="TrackSelector" )
tracktypes = ["Downstream"]
DownProtos.TrackSelector.TrackTypes = tracktypes
addmuonpid = ChargedProtoParticleAddMuonInfo("addmuonpid")
addmuonpid.InputMuonPIDLocation = idalg.MuonIDLocation
addmuonpid.ProtoParticleLocation = DownProtos.Output
combinedll = ChargedProtoCombineDLLsAlg("combineDLL")
combinedll.ProtoParticleLocation = DownProtos.Output
# make particles
DownParts = BestPIDParticleMaker("DownParts" , Particle = "muon")
DownParts.addTool(ProtoParticleMUONFilter,name="muon")
DownParts.muon.Selection = ["RequiresDet='MUON' IsMuonLoose=True"]
DownParts.Particles = [ "muon" ]
DownParts.Input = DownProtos.Output
DownParts.Output = "Hlt2/Hlt2DownstreamMuons/Particles"

#
# define exported symbols -- these are for available
# for use in Hlt2 by adding:
#
# from Hlt2SharedParticles.TagAndProbeParticles import TagAndProbeMuons
#

__all__ = ( 'TagAndProbePions', 'TagAndProbeMuons', 'TagMuonTTMuons', 'TagVeloMuons', 'TagDownstreamMuons' )

TagAndProbePions   = bindMembers( None, [ caloProtos		,	Hlt2TagAndProbePions 	] )
TagAndProbeMuons   = bindMembers( None, [ muonWithCaloProtos	, 	Hlt2TagAndProbeMuons	] )
TagMuonTTMuons		= bindMembers( None, [ Hlt2MuonTTTrack	, 	Hlt2MuonTTPParts	,	Hlt2MuonTTParts	] )
TagVeloMuons   		= bindMembers( None, [ DecodeVelo, StandaloneMuonRec('MyMuonStandalone'), Hlt2VeloMuonBuild, Hlt2VeloMuonProtos, Hlt2VeloMuonParts] )
TagDownstreamMuons	= bindMembers( None, [ DownSeed, DownstreamTracking, DownstreamFit, idalg, DownProtos, addmuonpid, combinedll, DownParts] )
