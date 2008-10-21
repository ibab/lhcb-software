from os import environ
from Gaudi.Configuration import *
from GaudiConf.Configuration import *
import GaudiKernel.ProcessJobOptions
from TrackSys.Configuration import *
from GaudiKernel.SystemOfUnits import mm

from Configurables import ( ProcessPhase, MagneticFieldSvc,
                            DecodeVeloRawBuffer,
                            Tf__PatVeloRTracking, Tf__PatVeloSpaceTool,
                            Tf__PatVeloSpaceTracking, Tf__PatVeloGeneralTracking,
                            Tf__PatVeloTrackTool,
                            RawBankToSTClusterAlg, RawBankToSTLiteClusterAlg,
                            OTTimeCreator,
                            PatForward,
                            TrackEventFitter,
                            Tf__Tsa__Seed, Tf__Tsa__SeedTrackCnv,
                            PatSeeding,
                            TrackMatchVeloSeed, PatDownstream, PatVeloTT,
                            TrackEventCloneKiller, TrackPrepareVelo, TrackContainerCopy,
                            TrackAddLikelihood, TrackLikelihood, NeuralNetTmva, Tf__OTHitCreator,
                            TrackBuildCloneTable, TrackCloneCleaner, AlignMuonRec
                           )

## Start TransportSvc, needed by track fit
ApplicationMgr().ExtSvc.append("TransportSvc")

## --------------------------------------------------------------------
## Pattern Recognition and Fitting
## --------------------------------------------------------------------

# Which algs to run ?
trackAlgs = TrackSys().getProp("trackPatRecAlgorithms")

## Velo tracking
if "Velo" in trackAlgs :
   if TrackSys().veloOpen():
      GaudiSequencer("RecoVELOSeq").Members += [ DecodeVeloRawBuffer(),
                                                 Tf__PatVeloGeneralTracking("PatVeloGeneralTracking")]
      Tf__PatVeloGeneralTracking("PatVeloGeneralTracking").PointErrorMin = 2*mm;
      Tf__PatVeloGeneralTracking("PatVeloGeneralTracking").addTool(Tf__PatVeloTrackTool("PatVeloTrackTool"))
      Tf__PatVeloTrackTool("PatVeloTrackTool").highChargeFract = 0.5;
   else:
      GaudiSequencer("RecoVELOSeq").Members += [ DecodeVeloRawBuffer(), Tf__PatVeloRTracking("PatVeloRTracking"),
                                                 Tf__PatVeloSpaceTracking("PatVeloSpaceTracking"),
                                                 Tf__PatVeloGeneralTracking("PatVeloGeneralTracking")]
      Tf__PatVeloSpaceTracking("PatVeloSpaceTracking").addTool( Tf__PatVeloSpaceTool(), name="PatVeloSpaceTool" )
      Tf__PatVeloSpaceTracking("PatVeloSpaceTracking").PatVeloSpaceTool.MarkClustersUsed = True;
   veloClusters = DecodeVeloRawBuffer("DecodeVeloClusters")
   veloClusters.DecodeToVeloLiteClusters = False;
   veloClusters.DecodeToVeloClusters     = True;
   GaudiSequencer("RecoVELOSeq").Members += [ veloClusters ]
   
## TT clusters for pattern recognition and track fit
GaudiSequencer("RecoTTSeq").Members += [ RawBankToSTClusterAlg("CreateTTClusters"),
                                         RawBankToSTLiteClusterAlg("CreateTTLiteClusters")]

## IT clusters for pattern recognition and track fit
createITClusters = RawBankToSTClusterAlg("CreateITClusters")
createITLiteClusters = RawBankToSTLiteClusterAlg("CreateITLiteClusters")
createITClusters.DetType     = "IT";
createITLiteClusters.DetType = "IT";

GaudiSequencer("RecoITSeq").Members += [ createITClusters, createITLiteClusters ]

importOptions( "$STTOOLSROOT/options/Brunel.opts" )

## OTTimes for pattern recognition and track fit
GaudiSequencer("RecoOTSeq").Members += [ OTTimeCreator() ]

## Tracking sequence
track = ProcessPhase("Track");
GaudiSequencer("RecoTrSeq").Members += [ track ]

if TrackSys().fieldOff() : MagneticFieldSvc().ScaleFactor = 0
               
## get all the trackfitters
from TrackFitter.ConfiguredFitters import *

##get parameters for fast momentum estimate
importOptions ("$TRACKTOOLSROOT/options/FastMomentumEstimate.opts")

if "noDrifttimes" in TrackSys().getProp("expertTracking"):
   Tf__OTHitCreator("OTHitCreator").NoDriftTimes = True

# Clone killer
cloneKiller = TrackEventCloneKiller()
cloneKiller.TracksInContainers = []

## Forward pattern
if "Forward" in trackAlgs :
   track.DetectorList += [ "ForwardPat", "ForwardPreFit", "ForwardFit" ]
   GaudiSequencer("TrackForwardPatSeq").Members +=  [ PatForward("PatForward") ]
   importOptions("$PATALGORITHMSROOT/options/PatForward.py")
   ## Forward prefit
   GaudiSequencer("TrackForwardPreFitSeq").Members += [ConfiguredPreFitForward()]
   ## Forward fit
   GaudiSequencer("TrackForwardFitSeq").Members += [ConfiguredFitForward()]
   ## Add to output best tracks
   cloneKiller.TracksInContainers += ["Rec/Track/Forward"]

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
   ## Seed fit
   track.DetectorList += [ "SeedPreFit","SeedFit" ]
   GaudiSequencer("TrackSeedFitSeq").Members += [ConfiguredFitSeed()]
   ## Add to output best tracks
   cloneKiller.TracksInContainers += ["Rec/Track/Seed"]

## Match
if "Match" in trackAlgs :
   track.DetectorList += [ "MatchPat", "MatchPreFit", "MatchFit" ]
   GaudiSequencer("TrackMatchPatSeq").Members += [ TrackMatchVeloSeed("TrackMatch") ]
   importOptions("$TRACKMATCHINGROOT/options/TrackMatch.py")
   ## Match prefit
   GaudiSequencer("TrackMatchPreFitSeq").Members += [ConfiguredPreFitMatch()]
   ## Match fit
   GaudiSequencer("TrackMatchFitSeq").Members += [ConfiguredFitMatch()]
   ## Add to output best tracks
   cloneKiller.TracksInContainers += ["Rec/Track/Match"]

## Downstream
if "Downstream" in trackAlgs and TrackSys().fieldOff() == False :
   track.DetectorList += [ "DownstreamPat","DownstreamPreFit","DownstreamFit" ]
   GaudiSequencer("TrackDownstreamPatSeq").Members += [ PatDownstream() ];
   ## Downstream prefit
   GaudiSequencer("TrackDownstreamPreFitSeq").Members += [ConfiguredPreFitDownstream()]
   ## Downstream fit
   GaudiSequencer("TrackDownstreamFitSeq").Members += [ConfiguredFitDownstream()]
   ## Add to output best tracks
   cloneKiller.TracksInContainers += ["Rec/Track/Downstream"]
   
## Velo-TT pattern
if "VeloTT" in trackAlgs :
   track.DetectorList += ["VeloTTPat", "VeloTTPreFit", "VeloTTFit"]
   GaudiSequencer("TrackVeloTTPatSeq").Members += [ PatVeloTT("PatVeloTT")] 
   importOptions ("$PATVELOTTROOT/options/PatVeloTT.py")
   ## Velo-TT fit
   GaudiSequencer("TrackVeloTTFitSeq").Members += [ ConfiguredFitVeloTT() ]
   ## Add to output best tracks
   cloneKiller.TracksInContainers += ["Rec/Track/VeloTT"]

## Clone tracks killer: output is "best" container
track.DetectorList += ["PostFit"]
GaudiSequencer("TrackPostFitSeq").Members += [ cloneKiller ]

## Velo fitting
if "Velo" in trackAlgs :
   ## Prepare the velo tracks for the fit
   track.DetectorList += ["VeloPreFit", "VeloFit"]
   GaudiSequencer("TrackVeloPreFitSeq").Members += [ TrackPrepareVelo() ] 
   ## Fit the velo tracks and copy them to the "best" container
   GaudiSequencer("TrackVeloFitSeq").Members += [ ConfiguredFitVelo() ]
   copyVelo = TrackContainerCopy( "CopyVelo" )
   copyVelo.inputLocation = "Rec/Track/PreparedVelo";
   GaudiSequencer("TrackVeloFitSeq").Members += [ copyVelo ]

## Extra track information sequence
extraInfos = TrackSys().getProp("trackExtraInfoAlgorithms")
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

   ## Add the likelihood information
   if "TrackLikelihood" in extraInfos :
      trackAddLikelihood = TrackAddLikelihood()
      trackAddLikelihood.addTool( TrackLikelihood, name = "TrackMatching_likTool" )
      trackAddLikelihood.TrackMatching_likTool.otEff = 0.9
      GaudiSequencer("TrackAddExtraInfoSeq").Members += [ trackAddLikelihood ]

   ## ghost probability using TMVA package
   if "GhostProbability" in extraInfos :
      GaudiSequencer("TrackAddExtraInfoSeq").Members += [ NeuralNetTmva() ]
      importOptions ("$NNTOOLSROOT/options/NeuralNetTmva.opts")

## Muon alignment tracks
if "MuonAlign" in trackAlgs :
   track.DetectorList += ["MuonRec"]
   GaudiSequencer("TrackMuonRecSeq").Members += [ AlignMuonRec("AlignMuonRec"),
                                                  TrackEventFitter("MuonTrackFitter") ]
   importOptions("$TRACKSYSROOT/options/AlignMuonRec.opts")
   if TrackSys().fieldOff():
      AlignMuonRec("AlignMuonRec").BField = False;
      importOptions( "$STDOPTS/DstContentMuonAlign.opts" )
