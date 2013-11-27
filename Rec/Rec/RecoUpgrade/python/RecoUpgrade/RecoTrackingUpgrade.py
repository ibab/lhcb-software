from os import environ
from Gaudi.Configuration import *
import GaudiKernel.ProcessJobOptions
from TrackSys.Configuration import *
from GaudiKernel.SystemOfUnits import mm

from TrackFitter.ConfiguredFitters import *

from Configurables import TrackSys
from Configurables import RecSysConf


from Configurables import (  ProcessPhase,MuonIDAlg,
                             PatLHCbIDUp2MCParticle,PatLHCbIDUp2MCHit,
                             CheatedVeloPixPat,
                             VeloPixPatLinear,
                             Tf__Tsa__Seed,
                             TrackStateInitAlg,TrackStateInitTool,
                             TrackEventCloneKiller, TrackPrepareVelo, VeloPixLiteMeasurementProvider,
                             TrackAddLikelihood,PatForward,PatSeedFit,PatSeeding,PatVeloTT,PatVeloTTTool,
                             TrackMatchVeloSeed, PatDownstream,PatMatch,Tf__PatVeloPixFitLHCbIDs,Tf__PatVeloFitLHCbIDs,
			     PatCheckerUpgrade  )

def ConfiguredFitVeloPix( Name = "FitVeloPix",
                       TracksInContainer = "Rec/Track/PreparedVeloPix"):
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer,
                                        FieldOff = True,
                                        SimplifiedGeometry = False,
                                        NoDriftTimes       = None,
                                        KalmanSmoother     = True,
                                        LiteClusters = True,
                                        ApplyMaterialCorrections = True,
                                        StateAtBeamLine = True,
                                        MaxNumberOutliers = 3)
    
    eventfitter.Fitter.addTool( MeasurementProvider(), name = "MeasProvider")
    eventfitter.Fitter.MeasProvider.VeloPixProvider = VeloPixLiteMeasurementProvider()
    eventfitter.Fitter.MeasProvider.IgnoreVelo = True
    eventfitter.Fitter.MeasProvider.IgnoreVeloPix = False
    return eventfitter

def ConfiguredFitVeloPixTT( Name = "FitVeloPixTT",
                       TracksInContainer = "Rec/Track/VeloPixTT"):
    # note that we ignore curvatue in velo. in the end that seems the
    # most sensible thing to do.
    eventfitter = ConfiguredEventFitter(Name,TracksInContainer,
                                        FieldOff = True,
                                        SimplifiedGeometry = False,
                                        NoDriftTimes       = None,
                                        KalmanSmoother     = True,
                                        LiteClusters = True,
                                        ApplyMaterialCorrections = True,
                                        StateAtBeamLine = True,
                                        MaxNumberOutliers = 3)
    
    eventfitter.Fitter.addTool( MeasurementProvider(), name = "MeasProvider")
    eventfitter.Fitter.MeasProvider.VeloPixProvider = VeloPixLiteMeasurementProvider()
    eventfitter.Fitter.MeasProvider.IgnoreVelo = True
    eventfitter.Fitter.MeasProvider.IgnoreVeloPix = False
    return eventfitter


def ConfigVeloPixProvider(fitter):
    fitter.addTool( MeasurementProvider(), name = "MeasProvider")
    fitter.MeasProvider.VeloPixProvider = VeloPixLiteMeasurementProvider()
    fitter.MeasProvider.IgnoreVelo = True
    fitter.MeasProvider.IgnoreVeloPix = False

def ConfigureTrackStateInitTool(nameTSInit,thebeamState = False):
    myTool = TrackStateInitAlg(nameTSInit).StateInitTool
    myTool.VeloFitterName="Tf::PatVeloPixFitLHCbIDs/FitVeloPix"
    myTool.addTool(Tf__PatVeloPixFitLHCbIDs,name="FitVeloPix")



def RecoTrackingUpgrade(exclude=[]):
    
   ## --------------------------------------------------------------------
   ## Pattern Recognition and Fitting
   ## --------------------------------------------------------------------
   Tf__Tsa__Seed("TsaSeed").maxOTHits = 100000
   Tf__Tsa__Seed("TsaSeed").maxITHits = 30000
    
   
   # Which algs to run ?
   trackAlgs = TrackSys().getProp("TrackPatRecAlgorithms")
   cloneKiller = TrackEventCloneKiller()
   from DAQSys.Decoders import DecoderDB
   from DAQSys.DecoderClass import decodersForBank
   ########## Replace/insert decoding of velopart
   if "VeloPix" in trackAlgs :
       decs=decodersForBank(DecoderDB,"VP")
       from Configurables import DecodeVeloRawBuffer
       if DecodeVeloRawBuffer("DecodeVeloClusters") in GaudiSequencer("RecoDecodingSeq").Members :
           GaudiSequencer("RecoDecodingSeq").remove(DecodeVeloRawBuffer("DecodeVeloClusters"))
       seq=GaudiSequencer("RecoDecodingSeq")
       seq.Members=[d.setup() for d in decs]+seq.Members
   
   ## Do what is needed for Dst
   #from GaudiConf.DstConf import DummyWriter
   
   if "VeloPix" in trackAlgs :
       from Configurables import DigiConf
       DigiConf().Detectors.insert(DigiConf().Detectors.index("Velo"),"VeloPix")
       DigiConf().Detectors.remove("Velo")
       DigiConf().Detectors.remove("L0")
   

   ######### Definition of cheated Pattern Recognition
   if "VeloPix" in trackAlgs :
       if not TrackSys().getProp( "WithMC" ):
           raise RuntimeError("Only Cheated Pattern recognition is implemented for VeloPix, MC is needed")
       if "Velo" in trackAlgs :
           GaudiSequencer("RecoVELOSeq").removeAll() ## Is there a better way to remove it if it exists
       
       ## IF ASSOCIATION WITH MCHIT IS NEEDED, IS A DataOnDemandSvc    
       from Configurables import MCParticle2MCHitAlg, TrackerMCParticle2MCHitAlg, DataOnDemandSvc

       velopixAssoc = MCParticle2MCHitAlg( "MCP2VeloPixMCHitAlg", MCHitPath = "MC/VeloPix/Hits", OutputData = "/Event/MC/Particles2MCVeloPixHits" )
       trackerAssoc = TrackerMCParticle2MCHitAlg( "MCP2TrackerHitAlg", OutputData = "/Event/MC/Particles2MCTrackerHits" )
       
       ## Create the different linkers
       DataOnDemandSvc().AlgMap[ "/Event/Link/MC/Particles2MCVeloPixHits" ]    = velopixAssoc
       DataOnDemandSvc().AlgMap[ "/Event/Link/MC/Particles2MCTrackerHits" ] = trackerAssoc
       DataOnDemandSvc().NodeMap[ "/Event/Link" ]    = "DataObject"
       DataOnDemandSvc().NodeMap[ "/Event/Link/MC" ] = "DataObject"


       from Configurables import DataPacking__Unpack_LHCb__MCVeloPixHitPacker_      
       GaudiSequencer("RecoVELOPIXSeq").Members    += [ DataPacking__Unpack_LHCb__MCVeloPixHitPacker_("UnpackMCVeloPixHits")
                                                       ]#,unfitChecker, fitChecker,fitFwdChecker]

 
        ## Cheated Pattern recognition
       GaudiSequencer("RecoVELOPIXSeq").Members += [PatLHCbIDUp2MCParticle("PatLHCbID2MCParticleVeloPix"),
                                                    VeloPixPatLinear("VeloPixPatLinearing")]
#                                                    CheatedVeloPixPat("CheatedPatVeloPixTracking")]
       patLHCbID2MCP = PatLHCbIDUp2MCParticle("PatLHCbID2MCParticleVeloPix")
       patLHCbID2MCP.LinkVELO = False
       patLHCbID2MCP.LinkTT = False
       patLHCbID2MCP.LinkIT = False
       patLHCbID2MCP.LinkOT = False
       patLHCbID2MCP.LinkVELOPIX = True
       
 #      cheatPat = CheatedVeloPixPat("CheatedPatVeloPixTracking")
 #      cheatPat.MinimalMCHitForTrack = 2

   ######### Definition/Replacement of Tracking
   track = ProcessPhase("Track")
   GaudiSequencer("TrackAddExtraInfoSeq").Members.remove(TrackAddLikelihood())
   
   if "VeloPix" in trackAlgs :
       track.DetectorList += [ "VeloPixFit"]
       GaudiSequencer("TrackVeloPixFitSeq").Members = [ TrackPrepareVelo("TrackPrepareVeloPix")]
       TrackPrepareVelo("TrackPrepareVeloPix").inputLocation = "/Event/Rec/Track/VeloPix"
       TrackPrepareVelo("TrackPrepareVeloPix").outputLocation = "/Event/Rec/Track/PreparedVeloPix"
       
       ## Fit the velo tracks
       fitter = ConfiguredFitVeloPix("FitVeloPix","Rec/Track/PreparedVeloPix")
       GaudiSequencer("TrackVeloPixFitSeq").Members += [fitter]
       
       ## copy the velo tracks to the "best" container (except in RDST case)
       if TrackSys().getProp( "OutputType" ).upper() != "RDST":
           from Configurables import TrackContainerCopy
           copyVeloPix = TrackContainerCopy( "CopyVeloPix" )
           copyVeloPix.inputLocation = "Rec/Track/PreparedVeloPix";
           GaudiSequencer("TrackVeloPixFitSeq").Members += [ copyVeloPix ]


       ## Modify the Forward Pattern reco
       if "ForwardPat" in  track.DetectorList :
           PatForward("PatForward").InputTracksName = "/Event/Rec/Track/VeloPix"
#           PatForward.maxITHits = 30000
#           PatForward.maxOTHits = 100000
       ## Modify the Forward fit
       if "ForwardFit" in track.DetectorList :
           ConfigureTrackStateInitTool("InitForwardFit")          
           fitter = TrackEventFitter("FitForward") 
           ConfigVeloPixProvider(fitter.Fitter)
           GaudiSequencer("TrackForwardFitSeq").Members += [ fitter ]
           
       ## Modify the Seed fit    
       if "SeedFit" in track.DetectorList :
           ConfigureTrackStateInitTool("InitSeedFit")
           
           if "Match" not in trackAlgs :
               fitter = TrackEventFitter("FitSeed")
               ConfigVeloPixProvider(fitter.Fitter)
           else :
               fitter = TrackEventFitter("FitSeedForMatch")
               ConfigVeloPixProvider(fitter.Fitter)
                   
      ## Modify the MatchPat
       if "MatchPat" in track.DetectorList :
           trmatchVSeed = TrackMatchVeloSeed("TrackMatch")
           ConfigVeloPixProvider(trmatchVSeed)
           trmatchVSeed.InputVeloTracks =  "/Event/Rec/Track/VeloPix"
           trmatchVSeed.DiscardChi2  =  10          
      ## Modify the MatchFit
       if "MatchFit" in track.DetectorList :
           ConfigureTrackStateInitTool("InitMatchFit")
           fitter = TrackEventFitter("FitMatch")
           ConfigVeloPixProvider(fitter.Fitter)

       if "DownstreamFit"in track.DetectorList :
           ConfigureTrackStateInitTool("InitDownstreamFit")
           fitter = TrackEventFitter("FitDownstream")
           ConfigVeloPixProvider(fitter.Fitter)
           
       if "SeedRefit"in track.DetectorList : 
           ConfigureTrackStateInitTool("InitSeedRefit")
           fitter = TrackEventFitter("RefitSeed")
           ConfigVeloPixProvider(fitter.Fitter)  

       if "Fit"in track.DetectorList :
           ConfigureTrackStateInitTool("InitBestFit")
           fitter = TrackEventFitter("FitBest")
           ConfigVeloPixProvider(fitter.Fitter)
           
         
    ## Velo-TT pattern
   if "VeloPixTT" in trackAlgs :
       track.DetectorList += ["VeloPixTTPat"]
       patVPixTT = PatVeloTT("PatVeloPixTT")
       patVPixTT.InputTracksName  = "Rec/Track/VeloPix"
       patVPixTT.OutputTracksName = "Rec/Track/VeloPixTT"
       GaudiSequencer("TrackVeloPixTTPatSeq").Members += [ patVPixTT ]

       patVPixTT.InputUsedTracksNames = ["Rec/Track/Forward","Rec/Track/Match"]
       patVPixTT.addTool(ConfiguredMasterFitter( "Fitter",
                                                 FieldOff = None,
                                                 SimplifiedGeometry = False,
                                                 NoDriftTimes       = None,
                                                 KalmanSmoother     = None,
                                                 LiteClusters       = True,
                                                 ApplyMaterialCorrections = None,
                                                 StateAtBeamLine = True,
                                                 MaxNumberOutliers = 2 ))
       patVPixTT.Fitter.NumberFitIterations = 1
       patVPixTT.Fitter.MaxNumberOutliers = 1
       patVPixTT.Fitter.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector"
       patVPixTT.Fitter.Extrapolator.addTool(TrackSimpleExtraSelector, "ExtraSelector")
       patVPixTT.Fitter.NodeFitter.addTool(TrackMasterExtrapolator, "Extrapolator")
       patVPixTT.Fitter.NodeFitter.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector";
       patVPixTT.Fitter.StateAtBeamLine = False
       
       ConfigVeloPixProvider(patVPixTT.Fitter)
       
       patVPixTT.addTool(PatVeloTTTool, name="PatVeloTTTool")
       if TrackSys().fieldOff():
           patVPixTT.PatVeloTTTool.minMomentum = 5000.
           patVPixTT.PatVeloTTTool.maxPseudoChi2 = 256
           patVPixTT.maxChi2 = 256.
           patVPixTT.PatVeloTTTool.DxGroupFactor = 0.0
           patVPixTT.fitTracks = False
       else:
           patVPixTT.PatVeloTTTool.minMomentum = 800.
           patVPixTT.PatVeloTTTool.maxPseudoChi2 = 10000.
           patVPixTT.maxChi2 = 5.
       
       #from PatVeloTT import PatVeloConf
       #PatVeloConf.PatVeloTTConf().configureAlg(patVPixTT)
       ##cloneKiller.TracksInContainers += ["Rec/Track/VeloPixTT"]
       if "fastSequence" not in TrackSys().getProp("ExpertTracking") :
           track.DetectorList += ["VeloPixTTFit"]
           GaudiSequencer("TrackVeloPixTTFitSeq").Members += [TrackStateInitAlg("InitVeloPixTTFit")]
           ConfigureTrackStateInitTool("InitVeloPixTTFit")
           TrackStateInitAlg("InitVeloPixTTFit").TrackLocation = "Rec/Track/VeloPixTT"
           fitter = ConfiguredFitVeloPixTT(  "FitVeloPixTT", "Rec/Track/VeloPixTT")
           GaudiSequencer("TrackVeloPixTTFitSeq").Members += [ fitter ]
        
#       GaudiSequencer("TrackCheckSeq").Members += [PatCheckerUpgrade("checker")] 
       





