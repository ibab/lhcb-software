from os import environ
from Gaudi.Configuration import *
from GaudiConf.Configuration import *
import GaudiKernel.ProcessJobOptions
from TrackSys.Configuration import *
from GaudiKernel.SystemOfUnits import mm

from TrackFitter.ConfiguredFitters import *


from Configurables import ( ProcessPhase, MagneticFieldSvc,
                            DecodeVeloRawBuffer,
                            VeloPixRawBankToLiteCluster,VeloPixRawBankToPartialCluster,
                            Tf__PatVeloRTracking, Tf__PatVeloSpaceTool,
                            Tf__PatVeloSpaceTracking, Tf__PatVeloGeneralTracking,
                            Tf__PatVeloTrackTool, Tf__PatVeloGeneric,
                            PatLHCbID2MCParticle,
                            PatLHCbIDUp2MCParticle,PatLHCbIDUp2MCHit,CheatedVeloPixPat,
                            RawBankToSTClusterAlg, RawBankToSTLiteClusterAlg,
                            PatForward,
                            TrackEventFitter,
                            Tf__Tsa__Seed, Tf__Tsa__SeedTrackCnv,
                            PatSeeding,
                            TrackMatchVeloSeed, PatDownstream, PatVeloTT,
                            TrackStateInitAlg,TrackStateInitTool,
                            TrackEventCloneKiller, TrackPrepareVelo,
                            TrackAddLikelihood, TrackLikelihood, TrackAddNNGhostId, Tf__OTHitCreator,
                            TrackBuildCloneTable, TrackCloneCleaner, AlignMuonRec,
                            TrackEraseExtraInfo, PatMatch, VeloPixLiteMeasurementProvider
                           )
def ConfiguredFitVeloPix( Name = "FitVeloPix",
                       TracksInContainer = "Rec/Track/PreparedVeloPix"):
    # note that we ignore curvatue in velo. in the end that seems the
    # most sensible thing to do.
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer,
                                        FieldOff = True,
                                        SimplifiedGeometry = False,
                                        NoDriftTimes       = None,
                                        KalmanSmoother     = True,
                                        LiteClusters = True,
                                        ApplyMaterialCorrections = True,
                                        StateAtBeamLine = False,
                                        MaxNumberOutliers = 3)
    return eventfitter


## Start TransportSvc, needed by track fit
ApplicationMgr().ExtSvc.append("TransportSvc")

## --------------------------------------------------------------------
## Pattern Recognition and Fitting
## --------------------------------------------------------------------

# Which algs to run ?
trackAlgs = TrackSys().getProp("TrackPatRecAlgorithms")

if "Velo" in trackAlgs :
   GaudiSequencer("RecoDecodingSeq").Members += [ DecodeVeloRawBuffer("DecodeVeloClusters")]

   veloClusters = DecodeVeloRawBuffer("DecodeVeloClusters")
   veloClusters.DecodeToVeloLiteClusters = True;
   veloClusters.DecodeToVeloClusters     = True;

if "VeloPix" in trackAlgs or len(trackAlgs)==0:
   ## Decode Lite cluster
   GaudiSequencer("RecoDecodingSeq").Members += [ VeloPixRawBankToLiteCluster("VeloPixRawBankToLiteCluster") ]
   veloPixLiteClusters = VeloPixRawBankToLiteCluster("VeloPixRawBankToLiteCluster")
   veloPixLiteClusters.ClusterLocation = "Raw/VeloPix/LiteClusters"
   ## Partially decode full cluster (will only decode the LiteCluster componant and the central LHCbID of full Cluster)
   GaudiSequencer("RecoDecodingSeq").Members += [ VeloPixRawBankToPartialCluster("VeloPixRawBankToPartialCluster") ]
   veloPixClusters = VeloPixRawBankToPartialCluster("VeloPixRawBankToPartialCluster")
   veloPixClusters.ClusterLocation = "Raw/VeloPix/Clusters"

GaudiSequencer("RecoDecodingSeq").Members += [ RawBankToSTClusterAlg("CreateTTClusters"),
                                                RawBankToSTLiteClusterAlg("CreateTTLiteClusters")]

createITClusters = RawBankToSTClusterAlg("CreateITClusters")
createITLiteClusters = RawBankToSTLiteClusterAlg("CreateITLiteClusters")
createITClusters.DetType     = "IT"
createITLiteClusters.DetType = "IT"

GaudiSequencer("RecoDecodingSeq").Members += [ createITClusters, createITLiteClusters ]

importOptions( "$STTOOLSROOT/options/Brunel.opts" )


## Velo tracking
if "Velo" in trackAlgs :
   if TrackSys().veloOpen():
      if TrackSys().beamGas(): 
        GaudiSequencer("RecoVELOSeq").Members += [ Tf__PatVeloGeneric("PatVeloGeneric"),
                                                   Tf__PatVeloGeneralTracking("PatVeloGeneralTracking")]
      else:
        GaudiSequencer("RecoVELOSeq").Members += [ Tf__PatVeloGeneralTracking("PatVeloGeneralTracking")]
      Tf__PatVeloGeneralTracking("PatVeloGeneralTracking").PointErrorMin = 2*mm;
      Tf__PatVeloGeneralTracking("PatVeloGeneralTracking").addTool(Tf__PatVeloTrackTool("PatVeloTrackTool"))
      Tf__PatVeloTrackTool("PatVeloTrackTool").highChargeFract = 0.5;
   else:
      GaudiSequencer("RecoVELOSeq").Members += [ Tf__PatVeloRTracking("PatVeloRTracking"),
                                                 Tf__PatVeloSpaceTracking("PatVeloSpaceTracking"),
                                                 Tf__PatVeloGeneralTracking("PatVeloGeneralTracking")]
      Tf__PatVeloSpaceTracking("PatVeloSpaceTracking").addTool( Tf__PatVeloSpaceTool(), name="PatVeloSpaceTool" )
      Tf__PatVeloSpaceTracking("PatVeloSpaceTracking").PatVeloSpaceTool.MarkClustersUsed = True;

if "VeloPix" in trackAlgs :

   from Configurables import MCParticle2MCHitAlg, TrackerMCParticle2MCHitAlg, DataOnDemandSvc

   # Define the algorithms
   velopixAssoc = MCParticle2MCHitAlg( "MCP2VeloPixMCHitAlg", MCHitPath = "MC/VeloPix/Hits", OutputData = "/Event/MC/Particles2MCVeloPixHits" )
   trackerAssoc = TrackerMCParticle2MCHitAlg( "MCP2TrackerHitAlg", OutputData = "/Event/MC/Particles2MCTrackerHits" )
   DataOnDemandSvc().AlgMap[ "/Event/Link/MC/Particles2MCVeloPixHits" ]    = velopixAssoc
   DataOnDemandSvc().AlgMap[ "/Event/Link/MC/Particles2MCTrackerHits" ] = trackerAssoc
   DataOnDemandSvc().NodeMap[ "/Event/Link" ]    = "DataObject"
   DataOnDemandSvc().NodeMap[ "/Event/Link/MC" ] = "DataObject"


        
   from Configurables import DataPacking__Unpack_LHCb__MCVeloPixHitPacker_
   GaudiSequencer("RecoVELOPIXSeq").Members += [PatLHCbIDUp2MCParticle("PatLHCbID2MCParticleVeloPix"),
                                                DataPacking__Unpack_LHCb__MCVeloPixHitPacker_("UnpackMCVeloPixHits"),
                                                CheatedVeloPixPat("CheatedPatVeloPixTracking")]
   patLHCbID2MCP = PatLHCbIDUp2MCParticle("PatLHCbID2MCParticleVeloPix")
   patLHCbID2MCP.LinkVELO = False
   patLHCbID2MCP.LinkTT = False
   patLHCbID2MCP.LinkIT = False
   patLHCbID2MCP.LinkOT = False
   patLHCbID2MCP.LinkVELOPIX = True
   cheatPat = CheatedVeloPixPat("CheatedPatVeloPixTracking")
   cheatPat.MinimalMCHitForTrack = 2

## Special OT decoder for cosmics to merge spills.
if TrackSys().cosmics():
   from Configurables import (Tf__OTHitCreator)
   Tf__OTHitCreator('OTHitCreator').RawBankDecoder = 'OTMultiBXRawBankDecoder'
   ## note: this does not change the OTMeasurementProvider used in the fit.
   # also adapt the MasterExtrapolator in the TrackInterpolator
   from Configurables import TrackInterpolator
   TrackInterpolator().Extrapolator.ExtraSelector = 'TrackSimpleExtraSelector'
   


## Make sure the default extrapolator and interpolator use simplified material
if TrackSys().simplifiedGeometry() and not( "VeloPix" in trackAlgs):
   from Configurables import TrackMasterExtrapolator, TrackInterpolator
   TrackMasterExtrapolator().MaterialLocator = 'SimplifiedMaterialLocator'
   TrackInterpolator().addTool( TrackMasterExtrapolator( MaterialLocator = 'SimplifiedMaterialLocator' ), name='Extrapolator')
elif  "VeloPix" in trackAlgs:
   print "Uses the detailed material locator"
   from Configurables import TrackMasterExtrapolator, TrackInterpolator
   TrackMasterExtrapolator().MaterialLocator = 'DetailedMaterialLocator'
   TrackInterpolator().addTool( TrackMasterExtrapolator( MaterialLocator = 'DetailedMaterialLocator' ), name='Extrapolator')
   
## Tracking sequence
track = ProcessPhase("Track");
GaudiSequencer("RecoTrSeq").Members += [ track ]

if TrackSys().fieldOff() : MagneticFieldSvc().ScaleFactor = 0
               
if "noDrifttimes" in TrackSys().getProp("ExpertTracking"):
   Tf__OTHitCreator("OTHitCreator").NoDriftTimes = True

# Get the fitters
from TrackFitter.ConfiguredFitters import ConfiguredFit, ConfiguredFitSeed

# Clone killer
cloneKiller = TrackEventCloneKiller()
cloneKiller.TracksInContainers = []


# Is this standard sequence?
stdSeq = "fastSequence" not in TrackSys().getProp("ExpertTracking")
#print "*** Standard sequence: ", stdSeq

## Forward pattern
if "Forward" in trackAlgs :
   track.DetectorList += [ "ForwardPat" ]
   GaudiSequencer("TrackForwardPatSeq").Members +=  [ PatForward("PatForward") ]
   importOptions("$PATALGORITHMSROOT/options/PatForward.py")
   if "VeloPix" in trackAlgs :
      PatForward("PatForward").InputTracksName = "LHCb::TrackLocation::VeloPix" ;
   cloneKiller.TracksInContainers += ["Rec/Track/Forward"]
   if stdSeq :
      # Fit now
      track.DetectorList += [ "ForwardFit" ]
      GaudiSequencer("TrackForwardFitSeq").Members += [TrackStateInitAlg("InitForwardFit")]
      TrackStateInitAlg("InitForwardFit").TrackLocation = "Rec/Track/Forward"
      ## Forward fit
      GaudiSequencer("TrackForwardFitSeq").Members += [ConfiguredFit("FitForward","Rec/Track/Forward")]
      
## Seed pattern
if "TsaSeed" in trackAlgs and "PatSeed" in trackAlgs :
   raise RuntimeError("Cannot run both Tsa and Pat Seeding at once")
if "TsaSeed" in trackAlgs :
   track.DetectorList += [ "SeedPat" ]
   GaudiSequencer("TrackSeedPatSeq").Members += [Tf__Tsa__Seed("TsaSeed"),
                                                 Tf__Tsa__SeedTrackCnv( "TsaSeedTrackCnv" )]
   importOptions("$TSAALGORITHMSROOT/options/TsaSeeding.py")
if "PatSeed" in trackAlgs :
   track.DetectorList += [ "SeedPat" ]
   GaudiSequencer("TrackSeedPatSeq").Members += [PatSeeding("PatSeeding")]
   importOptions("$PATALGORITHMSROOT/options/PatSeeding.py")
   if TrackSys().cosmics() :
      importOptions("$PATALGORITHMSROOT/options/PatSeedingTool-Cosmics.opts")
if "TsaSeed" in trackAlgs or "PatSeed" in trackAlgs :
   cloneKiller.TracksInContainers += ["Rec/Track/Seed"]
   if stdSeq or "Match" in trackAlgs :
      # Fit now
      track.DetectorList += [ "SeedFit" ]
      ## Seed fit initialization
      GaudiSequencer("TrackSeedFitSeq").Members += [TrackStateInitAlg("InitSeedFit")]
      TrackStateInitAlg("InitSeedFit").TrackLocation = "Rec/Track/Seed"
      # Choose fitter configuration
      if stdSeq and "Match" not in trackAlgs :
         # Use standard fitter
         GaudiSequencer("TrackSeedFitSeq").Members += [ConfiguredFit("FitSeed", "Rec/Track/Seed")]
      if "Match" in trackAlgs :
         # Use small ErrorQoP fitter, needed for Match
         GaudiSequencer("TrackSeedFitSeq").Members += [ConfiguredFitSeed()]
       
## Match
if "Match" in trackAlgs and "PatMatch" in trackAlgs :
   raise RuntimeError("Cannot run both TrackMatching and PatMatch at once")
if "Match" in trackAlgs :
   track.DetectorList += [ "MatchPat" ]
   GaudiSequencer("TrackMatchPatSeq").Members += [ TrackMatchVeloSeed("TrackMatch") ]
   importOptions("$TRACKMATCHINGROOT/options/TrackMatch.py")
   TrackMatchVeloSeed("TrackMatch").LikCut = -99999.   
   if "VeloPix" in trackAlgs :
      TrackMatchVeloSeed("TrackMatch").InputVeloTracks =  "TrackLocation::VeloPix"
 
if "PatMatch" in trackAlgs :
   track.DetectorList += [ "MatchPat" ]
   GaudiSequencer("TrackMatchPatSeq").Members += [ PatMatch("PatMatch") ]
if "Match" in trackAlgs or "PatMatch" in trackAlgs :
   cloneKiller.TracksInContainers  += ["Rec/Track/Match"]
   if stdSeq :
      ## Match fit initialization
      track.DetectorList += [ "MatchFit" ]
      GaudiSequencer("TrackMatchFitSeq").Members += [TrackStateInitAlg("InitMatchFit")]
      TrackStateInitAlg("InitMatchFit").TrackLocation = "Rec/Track/Match"
      ## Match fit
      GaudiSequencer("TrackMatchFitSeq").Members += [ConfiguredFit("FitMatch","Rec/Track/Match")]
   
## Downstream
if "Downstream" in trackAlgs :
   track.DetectorList += [ "DownstreamPat" ]
   GaudiSequencer("TrackDownstreamPatSeq").Members += [ PatDownstream() ];
   cloneKiller.TracksInContainers += ["Rec/Track/Downstream"]
   importOptions("$PATALGORITHMSROOT/options/PatDownstream.py")
   if stdSeq :
      track.DetectorList += [ "DownstreamFit" ]
      GaudiSequencer("TrackDownstreamFitSeq").Members += [TrackStateInitAlg("InitDownstreamFit")]
      TrackStateInitAlg("InitDownstreamFit").TrackLocation = "Rec/Track/Downstream"
      ## Downstream fit
      GaudiSequencer("TrackDownstreamFitSeq").Members += [ConfiguredFit("FitDownstream","Rec/Track/Downstream")]
      
## Velo-TT pattern
if "VeloTT" in trackAlgs :
   track.DetectorList += ["VeloTTPat"]
   GaudiSequencer("TrackVeloTTPatSeq").Members += [ PatVeloTT("PatVeloTT")] 
   importOptions ("$PATVELOTTROOT/options/PatVeloTT.py")
   cloneKiller.TracksInContainers += ["Rec/Track/VeloTT"]
   if stdSeq :
      track.DetectorList += ["VeloTTFit"]
      GaudiSequencer("TrackVeloTTFitSeq").Members += [TrackStateInitAlg("InitVeloTTFit")]
      TrackStateInitAlg("InitVeloTTFit").TrackLocation = "Rec/Track/VeloTT"
      ## Velo-TT fit
      GaudiSequencer("TrackVeloTTFitSeq").Members += [ ConfiguredFit("FitVeloTT","Rec/Track/VeloTT") ]


## VeloPix-TT pattern
if "VeloPixTT" in trackAlgs :
   print "VeloPixTT pattern"
   track.DetectorList += ["VeloPixTTPat"]
   GaudiSequencer("TrackVeloPixTTPatSeq").Members += [ PatVeloTT("PatVeloPixTT")]
   PatVeloTT("PatVeloPixTT").InputTracksName="Rec/Track/VeloPix"
   PatVeloTT("PatVeloPixTT").OutputTracksName="Rec/Track/VeloPixTT"
   importOptions ("$PATVELOTTROOT/options/PatVeloPixTT.py")
   cloneKiller.TracksInContainers += ["Rec/Track/VeloPixTT"]
   if stdSeq :
      track.DetectorList += ["VeloPixTTFit"]
      GaudiSequencer("TrackVeloPixTTFitSeq").Members += [TrackStateInitAlg("InitVeloPixTTFit")]
      TrackStateInitAlg("InitVeloPixTTFit").TrackLocation = "Rec/Track/VeloPixTT"
      TrackStateInitAlg("InitVeloPixTTFit").StateInitTool = TrackStateInitTool("TrackStateInitToolForVeloPix")
      TrackStateInitTool("TrackStateInitToolForVeloPix").UseVeloPix = True
      ## Velo-TT fit
      GaudiSequencer("TrackVeloPixTTFitSeq").Members += [ ConfiguredFit("FitVeloPixTT","Rec/Track/VeloPixTT") ]
    

if stdSeq and "Match" in trackAlgs :
   ## Refit Seeds with standard fitter
   track.DetectorList += [ "SeedRefit" ]
   ## Seed refit initialization
   GaudiSequencer("TrackSeedRefitSeq").Members += [TrackStateInitAlg("InitSeedRefit")]
   TrackStateInitAlg("InitSeedRefit").TrackLocation = "Rec/Track/Seed"
   ## Seed refit
   GaudiSequencer("TrackSeedRefitSeq").Members += [ConfiguredFit("RefitSeed", "Rec/Track/Seed")]


if stdSeq :
   track.DetectorList += ["PostFit"]
   GaudiSequencer("TrackPostFitSeq").Members += [ cloneKiller ]
else :
   track.DetectorList += ["Fit"]
   GaudiSequencer("TrackFitSeq").Members += [ cloneKiller ]
   GaudiSequencer("TrackFitSeq").Members += [TrackStateInitAlg("InitBestFit")]
   TrackStateInitAlg("InitBestFit").TrackLocation = "Rec/Track/Best"
   GaudiSequencer("TrackFitSeq").Members += [ConfiguredFit("FitBest","Rec/Track/Best")]

## Velo fitting
if "Velo" in trackAlgs :
   ## Prepare the velo tracks for the fit
   track.DetectorList += [ "VeloFit"]
   GaudiSequencer("TrackVeloFitSeq").Members += [ TrackPrepareVelo()]
   ## Fit the velo tracks
   GaudiSequencer("TrackVeloFitSeq").Members += [ ConfiguredFit("FitVelo","Rec/Track/PreparedVelo") ]
   ## copy the velo tracks to the "best" container (except in RDST case)
   if TrackSys().getProp( "OutputType" ).upper() != "RDST":
      from Configurables import TrackContainerCopy
      copyVelo = TrackContainerCopy( "CopyVelo" )
      copyVelo.inputLocation = "Rec/Track/PreparedVelo";
      GaudiSequencer("TrackVeloFitSeq").Members += [ copyVelo ]


 ## VeloPix fitting
if "VeloPix" in trackAlgs :
   ## Prepare the velo tracks for the fit
   track.DetectorList += [ "VeloPixFit"]
   GaudiSequencer("TrackVeloPixFitSeq").Members += [ TrackPrepareVelo("TrackPrepareVeloPix")]
   TrackPrepareVelo("TrackPrepareVeloPix").inputLocation = "/Event/Rec/Track/VeloPix"
   TrackPrepareVelo("TrackPrepareVeloPix").outputLocation = "/Event/Rec/Track/PreparedVeloPix"

   ## Fit the velo tracks
   fitter = ConfiguredFitVeloPix("FitVeloPix","Rec/Track/PreparedVeloPix")
   fitter.Fitter.MeasProvider.VeloPixProvider = VeloPixLiteMeasurementProvider()
   fitter.Fitter.MeasProvider.IgnoreVelo = True
   fitter.Fitter.MeasProvider.IgnoreVeloPix = False
   GaudiSequencer("TrackVeloPixFitSeq").Members += [fitter]
   
   ##   ConfiguredFit("FitVeloPix","Rec/Track/PreparedVeloPix") ]
   ## copy the velo tracks to the "best" container (except in RDST case)
   if TrackSys().getProp( "OutputType" ).upper() != "RDST":
      from Configurables import TrackContainerCopy
      copyVeloPix = TrackContainerCopy( "CopyVeloPix" )
      copyVeloPix.inputLocation = "Rec/Track/PreparedVeloPix";
      GaudiSequencer("TrackVeloPixFitSeq").Members += [ copyVeloPix ]
   


## Extra track information sequence
extraInfos = TrackSys().getProp("TrackExtraInfoAlgorithms")
if len(extraInfos) > 0 :

   track.DetectorList += ["AddExtraInfo"]

   ## Clone finding and flagging
   if "CloneFlagging" in extraInfos :
      trackClones = GaudiSequencer("TrackClonesSeq")
      GaudiSequencer("TrackAddExtraInfoSeq").Members += [ trackClones ]
      trackClones.MeasureTime = True
      cloneTable = TrackBuildCloneTable("FindTrackClones")
      cloneTable.maxDz   = 500*mm
      cloneTable.zStates = [ 0*mm, 990*mm, 9450*mm ]
      cloneTable.klCut   = 5e3
      cloneCleaner = TrackCloneCleaner("FlagTrackClones")
      cloneCleaner.CloneCut = 5e3
      trackClones.Members += [ cloneTable, cloneCleaner ]

   ## Add expected hit information   
   #if "ExpectedHits" in extraInfos :
   #   GaudiSequencer("TrackAddExtraInfoSeq").Members += [ TrackComputeExpectedHits() ]

   ## Add the likelihood information
   if "TrackLikelihood" in extraInfos and not "VeloPix" in trackAlgs :
      trackAddLikelihood = TrackAddLikelihood()
      trackAddLikelihood.addTool( TrackLikelihood, name = "TrackMatching_likTool" )
      trackAddLikelihood.TrackMatching_likTool.otEff = 0.9
      GaudiSequencer("TrackAddExtraInfoSeq").Members += [ trackAddLikelihood ]

   ## ghost probability using a Neural Net
   if "GhostProbability" in extraInfos :
      GaudiSequencer("TrackAddExtraInfoSeq").Members += [ TrackAddNNGhostId() ]

track.DetectorList += ["EraseExtraInformation"]

GaudiSequencer("TrackEraseExtraInformationSeq").Members += [ TrackEraseExtraInfo() ]

      
## Muon alignment tracks
if "MuonAlign" in trackAlgs :
   track.DetectorList += ["MuonRec"]
   GaudiSequencer("TrackMuonRecSeq").Members += [ AlignMuonRec("AlignMuonRec"),
                                                  TrackEventFitter("MuonTrackFitter") ]
   importOptions("$TRACKSYSROOT/options/AlignMuonRec.opts")
   if TrackSys().fieldOff():
      AlignMuonRec("AlignMuonRec").BField = False;
      importOptions( "$STDOPTS/DstContentMuonAlign.opts" )

