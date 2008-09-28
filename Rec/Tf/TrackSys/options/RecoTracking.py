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


## Velo tracking

patVeloSpaceTracking = Tf__PatVeloSpaceTracking("PatVeloSpaceTracking");

if TrackSys().getProp( "veloOpen" ):
   GaudiSequencer("RecoVELOSeq").Members += [ DecodeVeloRawBuffer(), Tf__PatVeloGeneralTracking("PatVeloGeneralTracking")]
   Tf__PatVeloGeneralTracking("PatVeloGeneralTracking").PointErrorMin = 2*mm;
   Tf__PatVeloGeneralTracking("PatVeloGeneralTracking").addTool(Tf__PatVeloTrackTool("PatVeloTrackTool"))
   Tf__PatVeloTrackTool("PatVeloTrackTool").highChargeFract = 0.5;
else:
   GaudiSequencer("RecoVELOSeq").Members += [ DecodeVeloRawBuffer(), Tf__PatVeloRTracking("PatVeloRTracking"),
                                              Tf__PatVeloSpaceTracking("PatVeloSpaceTracking"),
                                              Tf__PatVeloGeneralTracking("PatVeloGeneralTracking")]
   Tf__PatVeloSpaceTracking("PatVeloSpaceTracking").addTool( Tf__PatVeloSpaceTool(), name="PatVeloSpaceTool" )
   Tf__PatVeloSpaceTracking("PatVeloSpaceTracking").PatVeloSpaceTool.MarkClustersUsed = True;

GaudiSequencer("RecoVELOSeq").Members += [ DecodeVeloRawBuffer("DecodeVeloClusters") ]

DecodeVeloRawBuffer("DecodeVeloClusters").DecodeToVeloLiteClusters = False;
DecodeVeloRawBuffer("DecodeVeloClusters").DecodeToVeloClusters     = True;

## TT clusters for pattern recognition and track fit
GaudiSequencer("RecoTTSeq").Members += [ RawBankToSTClusterAlg("CreateTTClusters"),
                                         RawBankToSTLiteClusterAlg("CreateTTLiteClusters")]

## IT clusters for pattern recognition and track fit
createITClusters = RawBankToSTClusterAlg("CreateITClusters")
createITLiteClusters = RawBankToSTLiteClusterAlg("CreateITLiteClusters")
createITClusters.detType     = "IT";
createITLiteClusters.detType = "IT";

GaudiSequencer("RecoITSeq").Members += [ createITClusters, createITLiteClusters ]

importOptions( "$STTOOLSROOT/options/Brunel.opts" )

## OTTimes for pattern recognition and track fit
GaudiSequencer("RecoOTSeq").Members += [ OTTimeCreator() ]

## Tracking sequence
track = ProcessPhase("Track");
GaudiSequencer("RecoTrSeq").Members += [ track ]

if TrackSys().getProp("fieldOff"):
  track.DetectorList += [ "ForwardPat",   "ForwardPreFit",   "ForwardFit"
                        , "SeedPat",      "SeedPreFit",      "SeedFit"
                        , "MatchPat",     "MatchPreFit",     "MatchFit"] 

  MagneticFieldSvc().ScaleFactor = 0;
else:
  track.DetectorList += [ "ForwardPat",   "ForwardPreFit",   "ForwardFit"
                        , "SeedPat",      "SeedPreFit",      "SeedFit"
                        , "MatchPat",     "MatchPreFit",     "MatchFit"
                        ,  "DownstreamPat","DownstreamPreFit","DownstreamFit"]

track.DetectorList += [ "VeloTTPat",    "VeloTTPreFit",    "VeloTTFit"
                        , "PostFit"
                        , "VeloPreFit", "VeloFit"
                        , "AddExtraInfo", "MuonRec"
                        ]
               
## get all the trackfitters
from TrackFitter.ConfiguredFitters import *

if "noDrifttimes" in TrackSys().getProp("expertTracking"):
   Tf__OTHitCreator("OTHitCreator").NoDriftTimes = True

## Forward pattern
GaudiSequencer("TrackForwardPatSeq").Members +=  [ PatForward("PatForward") ]
importOptions("$PATALGORITHMSROOT/options/PatForward.py")

## Forward prefit
GaudiSequencer("TrackForwardPreFitSeq").Members += [ConfiguredPreFitForward()]

## Forward fit
GaudiSequencer("TrackForwardFitSeq").Members += [ConfiguredFitForward()]

## Seed pattern
  
if "usePatSeeding" in TrackSys().getProp("expertTracking"):
   GaudiSequencer("TrackSeedPatSeq").Members += [PatSeeding("PatSeeding")]
   importOptions("$PATALGORITHMSROOT/options/PatSeeding.py")
else:     
   GaudiSequencer("TrackSeedPatSeq").Members += [Tf__Tsa__Seed("TsaSeed"),
                                                 Tf__Tsa__SeedTrackCnv( "TsaSeedTrackCnv" )]
   importOptions("$TSAALGORITHMSROOT/options/TsaSeeding.py")

##get parameters for fast momentum estimate
importOptions ("$TRACKTOOLSROOT/options/FastMomentumEstimate.opts")

## Seed fit
GaudiSequencer("TrackSeedFitSeq").Members += [ConfiguredFitSeed()]

## Match pattern
GaudiSequencer("TrackMatchPatSeq").Members += [ TrackMatchVeloSeed("TrackMatch")]

importOptions("$TRACKMATCHINGROOT/options/TrackMatch.py")

## Match prefit
GaudiSequencer("TrackMatchPreFitSeq").Members += [ConfiguredPreFitMatch()]

## Match fit
GaudiSequencer("TrackMatchFitSeq").Members += [ConfiguredFitMatch()]

## Downstream pattern
GaudiSequencer("TrackDownstreamPatSeq").Members += [ PatDownstream() ];

## Downstream prefit
GaudiSequencer("TrackDownstreamPreFitSeq").Members += [ConfiguredPreFitDownstream()]

## Downstream fit
GaudiSequencer("TrackDownstreamFitSeq").Members += [ConfiguredFitDownstream()]

## Velo-TT pattern
GaudiSequencer("TrackVeloTTPatSeq").Members += [ PatVeloTT("PatVeloTT")] 
importOptions ("$PATVELOTTROOT/options/PatVeloTT.py")

## Velo-TT fit
GaudiSequencer("TrackVeloTTFitSeq").Members += [ ConfiguredFitVeloTT() ]

## Clone tracks killer: output is "best" container
GaudiSequencer("TrackPostFitSeq").Members += [ TrackEventCloneKiller() ]

if TrackSys().getProp("fieldOff"):
  TrackEventCloneKiller().TracksInContainers = ["Rec/Track/Forward", "Rec/Track/Match",
                                                "Rec/Track/VeloTT", "Rec/Track/Seed"]
else:
  TrackEventCloneKiller().TracksInContainers  = [ "Rec/Track/Forward", "Rec/Track/Match",
                                                  "Rec/Track/Downstream", "Rec/Track/VeloTT",
                                                  "Rec/Track/Seed"]
                                                    
## Prepare the velo tracks for the fit
GaudiSequencer("TrackVeloPreFitSeq").Members += [ TrackPrepareVelo() ] 

## Fit the velo tracks and copy them to the "best" container
GaudiSequencer("TrackVeloFitSeq").Members += [ ConfiguredFitVelo() ]
copyVelo = TrackContainerCopy( "CopyVelo" )
copyVelo.inputLocation = "Rec/Track/PreparedVelo";
GaudiSequencer("TrackVeloFitSeq").Members += [ copyVelo ]

## Clone finding and flagging
trackClones = GaudiSequencer("TrackClonesSeq")
GaudiSequencer("TrackAddExtraInfoSeq").Members += [ trackClones ];
trackClones.MeasureTime = True;
cloneTable = TrackBuildCloneTable("FindTrackClones")
cloneTable.maxDz   = 500*mm
cloneTable.zStates = [ 0*mm, 990*mm, 9450*mm ]
cloneTable.klCut   = 5e3
cloneCleaner = TrackCloneCleaner("FlagTrackClones")
cloneCleaner.CloneCut = 5e3
trackClones.Members += [ cloneTable, cloneCleaner ]

## Add the likelihood information & ghost probability using TMVA package
trackAddLikelihood = TrackAddLikelihood()
trackAddLikelihood.addTool( TrackLikelihood, name = "TrackMatching_likTool" )
trackAddLikelihood.TrackMatching_likTool.otEff = 0.9
GaudiSequencer("TrackAddExtraInfoSeq").Members += [ trackAddLikelihood, NeuralNetTmva() ]
importOptions ("$NNTOOLSROOT/options/NeuralNetTmva.opts")

if "MuonAlignTracks" in TrackSys().getProp("expertTracking"):
  GaudiSequencer("TrackMuonRecSeq").Members += [ AlignMuonRec(), TrackEventFitter("MuonTrackFitter") ];
  importOptions("$TRACKSYSROOT/options/AlignMuonRec.opts")
