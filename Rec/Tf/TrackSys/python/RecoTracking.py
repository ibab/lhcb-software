from os import environ
from Gaudi.Configuration import *
from GaudiConf.Configuration import *
import GaudiKernel.ProcessJobOptions
from Brunel.Configuration import *
from GaudiKernel.SystemOfUnits import mm

from Configurables import ( ProcessPhase, CondDBAccessSvc, MagneticFieldSvc, ReadStripETC,
                            DecodeVeloRawBuffer, Tf__PatVeloRTracking, Tf__PatVeloSpaceTool,
                            Tf__PatVeloSpaceTracking, Tf__PatVeloGeneralTracking,
                            Tf__PatVeloTrackTool,
                            RawBankToSTClusterAlg, RawBankToSTLiteClusterAlg,
                            OTTimeCreator,
                            PatForward, 
                            TrackEventFitter, TrackMasterFitter, TrackMasterExtrapolator,
                            TrackKalmanFilter, TrackSimpleExtraSelector,
                            Tf__Tsa__Seed, Tf__Tsa__SeedTrackCnv,
                            PatSeeding,
                            TrackMatchVeloSeed, PatDownstream, PatVeloTT,
                            TrackEventCloneKiller, TrackPrepareVelo, TrackContainerCopy,
                            TrackAddLikelihood, TrackLikelihood, NeuralNetTmva)


## Start TransportSvc, needed by track fit
ApplicationMgr().ExtSvc.append("TransportSvc")

## --------------------------------------------------------------------
## Pattern Recognition and Fitting
## --------------------------------------------------------------------

## Velo tracking

patVeloSpaceTracking = Tf__PatVeloSpaceTracking("PatVeloSpaceTracking");



if Brunel().getProp( "veloOpen" ):
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

if Brunel().getProp("fieldOff"):
   track.DetectorList = [ "ForwardPat",   "ForwardPreFit",   "ForwardFit"
                      , "SeedPat",      "SeedPreFit",      "SeedFit"
                      , "MatchPat",     "MatchPreFit",     "MatchFit"
                      , "VeloTTPat",    "VeloTTPreFit",    "VeloTTFit"
                      , "PostFit"
                      , "VeloPreFit", "VeloFit"
                      , "AddExtraInfo"
                      ]
   MagneticFieldSvc.UseConstantField = True;
else: 
   track.DetectorList  = [ "ForwardPat",   "ForwardPreFit",   "ForwardFit"
                           , "SeedPat",      "SeedPreFit",      "SeedFit"
                           , "MatchPat",     "MatchPreFit",     "MatchFit"
                           , "DownstreamPat","DownstreamPreFit","DownstreamFit"
                           , "VeloTTPat",    "VeloTTPreFit",    "VeloTTFit"
                           , "PostFit"
                           , "VeloPreFit", "VeloFit"
                           , "AddExtraInfo"
                           ]
               
## Set of standard fitting options
importOptions( "$TRACKSYSROOT/python/Fitting.py" )

## Forward pattern
GaudiSequencer("TrackForwardPatSeq").Members +=  [ PatForward("PatForward") ]
importOptions("$PATALGORITHMSROOT/python/PatForward.py")

## Forward prefit
GaudiSequencer("TrackForwardPreFitSeq").Members += [TrackEventFitter("PreFitForward")] 

## Forward fit
GaudiSequencer("TrackForwardFitSeq").Members += [TrackEventFitter("FitForward" )]

## Seed pattern
  
if Brunel().getProp("usePatSeeding"):
   GaudiSequencer("TrackSeedPatSeq").Members += [PatSeeding("PatSeeding")]
   importOptions("$PATALGORITHMSROOT/python/PatSeeding.py")
else:     
   GaudiSequencer("TrackSeedPatSeq").Members += [Tf__Tsa__Seed("TsaSeed"),
                                                 Tf__Tsa__SeedTrackCnv( "TsaSeedTrackCnv" )]
   importOptions("$TSAALGORITHMSROOT/python/TsaSeeding.py")

##get parameters for fast momentum estimate
importOptions ("$TRACKTOOLSROOT/options/FastMomentumEstimate.opts")

## Seed fit
GaudiSequencer("TrackSeedFitSeq").Members += [ TrackEventFitter("FitSeed")]

## Match pattern
GaudiSequencer("TrackMatchPatSeq").Members += [ TrackMatchVeloSeed("TrackMatch")]

importOptions("$TRACKMATCHINGROOT/python/TrackMatch.py")

## Match prefit
GaudiSequencer("TrackMatchPreFitSeq").Members += [ TrackEventFitter("PreFitMatch")] 

## Match fit
GaudiSequencer("TrackMatchFitSeq").Members += [ TrackEventFitter("FitMatch")]

## Downstream pattern
GaudiSequencer("TrackDownstreamPatSeq").Members += [ PatDownstream() ];

## Downstream prefit
GaudiSequencer("TrackDownstreamPreFitSeq").Members +=[ TrackEventFitter("PreFitDownstream")]

## Downstream fit
GaudiSequencer("TrackDownstreamFitSeq").Members += [ TrackEventFitter("FitDownstream" )]

## Velo-TT pattern
GaudiSequencer("TrackVeloTTPatSeq").Members += [ PatVeloTT("PatVeloTT")] 
importOptions ("$PATVELOTTROOT/python/PatVeloTT.py")

## Velo-TT fit
GaudiSequencer("TrackVeloTTFitSeq").Members += [ TrackEventFitter("FitVeloTT") ]

## Clone tracks killer: output is "best" container
GaudiSequencer("TrackPostFitSeq").Members += [ TrackEventCloneKiller() ]

if Brunel().getProp("fieldOff"):
  TrackEventCloneKiller().TracksInContainers = ["Rec/Track/Forward", "Rec/Track/Match",
                                                "Rec/Track/VeloTT", "Rec/Track/Seed"]
else:
  TrackEventCloneKiller().TracksInContainers  = [ "Rec/Track/Forward", "Rec/Track/Match",
                                                  "Rec/Track/Downstream", "Rec/Track/VeloTT",
                                                  "Rec/Track/Seed"]
                                                    

## Prepare the velo tracks for the fit
GaudiSequencer("TrackVeloPreFitSeq").Members += [ TrackPrepareVelo() ] 

## Fit the velo tracks and copy them to the "best" container
GaudiSequencer("TrackVeloFitSeq").Members += [ TrackEventFitter("FitVelo") ]
copyVelo = TrackContainerCopy( "CopyVelo" )
copyVelo.inputLocation = "Rec/Track/PreparedVelo";
GaudiSequencer("TrackVeloFitSeq").Members += [ copyVelo ]

## Add the likelihood information & ghost probability using TMVA package
trackAddLikelihood = TrackAddLikelihood()
trackAddLikelihood.addTool( TrackLikelihood, name = "TrackMatching_likTool" )
trackAddLikelihood.TrackMatching_likTool.otEff = 0.9
GaudiSequencer("TrackAddExtraInfoSeq").Members += [ trackAddLikelihood] ## NeuralNetTmva() ]

## Clone finding and flagging
trackClones = GaudiSequencer("TrackClones")
GaudiSequencer("TrackAddExtraInfoSeq").Members += [ trackClones ];
trackClones.MeasureTime = True;
importOptions( "$TRACKSYSROOT/options/TrackClones.opts" )


