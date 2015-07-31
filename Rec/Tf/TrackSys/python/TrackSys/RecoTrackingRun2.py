from os import environ
from Gaudi.Configuration import *
import GaudiKernel.ProcessJobOptions
from TrackSys.Configuration import *
from GaudiKernel.SystemOfUnits import mm


def DecodeTracking(trackAlgs):

   #configure the decoding
   from DAQSys.Decoders import DecoderDB
   from DAQSys.DecoderClass import decodersForBank
   decs=[]
   if "Velo" or "FastVelo" in trackAlgs :
      #clone an existing algorithm, in order to create both the full
      #and the partial clusters
      vdec=DecoderDB["DecodeVeloRawBuffer/createBothVeloClusters"]
      #set as active to make sure nobody tries to use the DoD service along side...
      vdec.Active=True
      #set the other algs to inactive, needed for re-running tracking in DaVinci... quite annoying really, see task #19106
      #there is another way to do this, set vdec.Active=False, but then someone might run a decoder before the createBoth alg, and screw everything up
      DecoderDB["DecodeVeloRawBuffer/createVeloClusters"].Active=False
      DecoderDB["DecodeVeloRawBuffer/createVeloLiteClusters"].Active=False
      
      globalCuts = TrackSys().getProp("GlobalCuts")
      if( "Velo" in globalCuts ) :
         vdec.Properties["MaxVeloClusters"] =  globalCuts["Velo"]
      decs=decs+[vdec]
      
   #from Configurables import RawBankToSTClusterAlg, RawBankToSTLiteClusterAlg
   decs=decs+decodersForBank(DecoderDB,"TT")
   decs=decs+decodersForBank(DecoderDB,"IT")
   GaudiSequencer("RecoDecodingSeq").Members += [d.setup() for d in decs ]
   
   ## Special OT decoder for cosmics to merge spills.
   if TrackSys().cosmics():
      from Configurables import (Tf__OTHitCreator)
      Tf__OTHitCreator('OTHitCreator').RawBankDecoder = 'OTMultiBXRawBankDecoder'
      ## note: this does not change the OTMeasurementProvider used in the fit.
      # also adapt the MasterExtrapolator in the TrackInterpolator
      from Configurables import TrackInterpolator
      TrackInterpolator().Extrapolator.ExtraSelector = 'TrackSimpleExtraSelector'
      
   d=DecoderDB["OTRawBankDecoder/ToolSvc.OTRawBankDecoder"]
   if "disableOTTimeWindow" in TrackSys().getProp("ExpertTracking"):
      from GaudiKernel.SystemOfUnits import ns
      d.Properties["TimeWindow"] = ( -999.0*ns, 999.0*ns )
   #ensure the public tool is configured and marked as used
   d.setup()
         

def RecoTrackingHLT1(exclude=[], simplifiedGeometryFit = True, liteClustersFit = True):
   '''Function that defines the pattern recognition algorithms for the HLT1 sequence of the Run 2 offline tracking'''
   ## Start TransportSvc, needed by track fit
   ApplicationMgr().ExtSvc.append("TransportSvc")
   
   ## --------------------------------------------------------------------
   ## Pattern Recognition and Fitting
   ## --------------------------------------------------------------------
   
   # Which algs to run ?
   trackAlgs = TrackSys().getProp("TrackPatRecAlgorithms")

   # Which data type is it?
   dataType = TrackSys().getProp("DataType")

   # Decode the RAW banks
   DecodeTracking(trackAlgs)
   
   from Configurables import STOfflinePosition
   IT = STOfflinePosition('ITClusterPosition')
   IT.DetType = "IT"
   TT = STOfflinePosition('TTClusterPosition')
   TT.DetType = "TT"
   
   from STTools import STOfflineConf
   STOfflineConf.DefaultConf().configureTools()

   ## Make sure the default extrapolator and interpolator use simplified material
   if TrackSys().simplifiedGeometry() and ('SimpleGeom' not in exclude):
      from Configurables import TrackMasterExtrapolator, TrackInterpolator
      TrackMasterExtrapolator().MaterialLocator = 'SimplifiedMaterialLocator'
      TrackInterpolator().addTool( TrackMasterExtrapolator( MaterialLocator = 'SimplifiedMaterialLocator' ), name='Extrapolator')
      

   ### This configures public tools to use the new multiple scattering description without the log term
   from Configurables import TrackMasterExtrapolator, DetailedMaterialLocator, StateThickMSCorrectionTool
   me = TrackMasterExtrapolator()
   me.addTool(DetailedMaterialLocator(), name="MaterialLocator")
   me.MaterialLocator.addTool( StateThickMSCorrectionTool, name= "StateMSCorrectionTool")
   me.MaterialLocator.StateMSCorrectionTool.UseRossiAndGreisen = True

   from Configurables import TrackInterpolator
   ti = TrackInterpolator()
   ti.addTool( me )

   from Configurables import TrackStateProvider
   tsp = TrackStateProvider()
   tsp.addTool( TrackInterpolator, name = "TrackInterpolator" )
   tsp.TrackInterpolator.addTool( TrackMasterExtrapolator, name='TrackMasterExtrapolator')
   tsp.TrackInterpolator.TrackMasterExtrapolator.addTool(DetailedMaterialLocator, name = "MaterialLocator" ) 
   tsp.TrackInterpolator.TrackMasterExtrapolator.MaterialLocator.addTool( StateThickMSCorrectionTool, name= "StateMSCorrectionTool")
   tsp.TrackInterpolator.TrackMasterExtrapolator.MaterialLocator.StateMSCorrectionTool.UseRossiAndGreisen = True
   ###
      




   
   ## Velo tracking
   ## Why is Velo not in the tracking sequence?
   if "FastVelo" in trackAlgs :
      from Configurables import FastVeloTracking
      GaudiSequencer("RecoVELOSeq").Members += [ FastVeloTracking("FastVeloTracking") ]
      if TrackSys().timing() :
         FastVeloTracking().TimingMeasurement = True; 

  
      
   ## Tracking sequence
   from Configurables import ProcessPhase
   track = ProcessPhase("TrackHLT1");
   GaudiSequencer("RecoTrHLT1Seq").Members += [ track ]

   from Configurables import MagneticFieldSvc
   if TrackSys().fieldOff() : MagneticFieldSvc().ForcedSignedCurrentScaling = 0.
   
   if "noDrifttimes" in TrackSys().getProp("ExpertTracking"):
      from Configurables import (Tf__OTHitCreator)
      Tf__OTHitCreator("OTHitCreator").NoDriftTimes = True
      
   # Get the fitters
   from TrackFitter.ConfiguredFitters import ConfiguredFit, ConfiguredFitSeed, ConfiguredMasterFitter

   # Clone killer
   tracklists = []
   
   # Is this standard sequence?
   stdSeq = "fastSequence" not in TrackSys().getProp("ExpertTracking")

   ## Velo-TT pattern
   if "VeloTT" in trackAlgs :
      track.DetectorList += ["VeloTTPat"]
      from Configurables import PatVeloTTHybrid
      GaudiSequencer("TrackHLT1VeloTTPatSeq").Members += [ PatVeloTTHybrid("PatVeloTTHybrid")]
      from PatVeloTT import PatVeloTTAlgConf
      PatVeloTTAlgConf.PatVeloTTConf().configureAlgRun2HLT1()
      if TrackSys().timing() :
         PatVeloTTHybrid("PatVeloTTHybrid").TimingMeasurement = True;
      tracklists += ["Rec/Track/VeloTT"]
      
   ## Forward pattern
   if "ForwardHLT1" in trackAlgs :
      if "VeloTT" not in trackAlgs:
         raise RuntimeError("Cannot run HLT1 forward without VeloTT")
      
      track.DetectorList += [ "ForwardPatHLT1" ]
      from Configurables import PatForward, PatForwardTool
      GaudiSequencer("TrackHLT1ForwardPatHLT1Seq").Members +=  [ PatForward("PatForwardHLT1") ]
      
      # should be replaced by more 'global' tracking configuration
      from PatAlgorithms import PatAlgConf
      PatAlgConf.ForwardConf().configureAlgRun2HLT1()
      if TrackSys().timing() :
         PatForward("PatForwardHLT1").TimingMeasurement = True;    
      tracklists += ["Rec/Track/ForwardHLT1"]
      
   ## Fitting all HLT1 tracks
   track.DetectorList += ["FitHLT1"]


   from Configurables import TrackEventFitter, TrackInitFit, TrackStateInitTool, TrackStateInitAlg, TrackMasterExtrapolator, TrackMasterFitter
   from Configurables import SimplifiedMaterialLocator, DetailedMaterialLocator
   from Configurables import TrackContainerCopy
   
   ######
   ### Fitter for Velo tracks
   ######
   if "FastVelo" in trackAlgs:
      if "VeloForwardKalmanHLT1" in TrackSys().getProp("ExpertTracking"):
         # This is the option for the 2015 early measurements
         veloFitter = TrackEventFitter('VeloOnlyFitterAlg')
         veloFitter.TracksInContainer = "Rec/Track/Velo"
         veloFitter.TracksOutContainer = "Rec/Track/FittedHLT1VeloTracks"
         veloFitter.Fitter = "TrackInitFit/Fit"
         veloFitter.addTool(TrackInitFit, "Fit")
         veloFitter.Fit.Init = "TrackStateInitTool/VeloOnlyStateInit"
         veloFitter.Fit.addTool(TrackStateInitTool, "VeloOnlyStateInit")
         veloFitter.Fit.VeloOnlyStateInit.VeloFitterName = "FastVeloFitLHCbIDs"
         veloFitter.Fit.VeloOnlyStateInit.addTool(TrackMasterExtrapolator, "Extrapolator")
         if( simplifiedGeometryFit ) :
            veloFitter.Fit.VeloOnlyStateInit.Extrapolator.addTool(SimplifiedMaterialLocator, name = "MaterialLocator")
         else:
            veloFitter.Fit.VeloOnlyStateInit.Extrapolator.addTool(DetailedMaterialLocator, name = "MaterialLocator")
            
         veloFitter.Fit.Fit = "TrackMasterFitter/Fit"
         veloFitter.Fit.addTool(TrackMasterFitter, name = "Fit")
            
         from TrackFitter.ConfiguredFitters import ConfiguredForwardFitter
         ConfiguredForwardFitter(veloFitter.Fit.Fit, LiteClusters = liteClustersFit)
         
         GaudiSequencer("TrackHLT1FitHLT1Seq").Members += [ veloFitter ]

      else:
         # and this is the option for after the early measurements
         # copy tracks from pat reco output container to a new one
         copyVeloTracks = TrackContainerCopy("CopyVeloTracks")
         copyVeloTracks.inputLocations = [ "Rec/Track/Velo" ]
         copyVeloTracks.outputLocation = "Rec/Track/FittedHLT1VeloTracks"
         
         from FastVelo import FastVeloAlgConf
         stateInit = TrackStateInitAlg('VeloOnlyInitAlg')
         FastVeloAlgConf.FastVeloKalmanConf().configureFastKalmanFit(init = stateInit)
         GaudiSequencer("TrackHLT1FitHLT1Seq").Members += [ copyVeloTracks, stateInit ]


   ######
   ### Fitter for Forward tracks
   ### Need to be careful: This is all a little fragile, so if you plan to change something, check that everything configures the way you expect
   ### The Extrapolator for the StateInitTool does not use material corrections, so don't need to explicitely add the StateThickMSCorrectionTool 
   ######
   if "ForwardHLT1" in trackAlgs:
      fwdFitter = TrackEventFitter('ForwardHLT1FitterAlg')
      fwdFitter.TracksInContainer = "Rec/Track/ForwardHLT1"
      fwdFitter.TracksOutContainer = "Rec/Track/FittedHLT1ForwardTracks"
      # Keep only good tracks, this cut should be aligned with the one in the TrackBestTrackCreator
      fwdFitter.MaxChi2DoF = 3.
      fwdFitter.Fitter = "TrackInitFit/Fit"
      fwdFitter.addTool(TrackInitFit, "Fit")
      fwdFitter.Fit.Init = "TrackStateInitTool/FwdStateInit"
      fwdFitter.Fit.addTool(TrackStateInitTool, "FwdStateInit")
      fwdFitter.Fit.FwdStateInit.addTool(TrackMasterExtrapolator, "Extrapolator")
      fwdFitter.Fit.FwdStateInit.UseFastMomentumEstimate = True 
      fwdFitter.Fit.FwdStateInit.VeloFitterName = "FastVeloFitLHCbIDs"
      if( simplifiedGeometryFit ) :
         fwdFitter.Fit.FwdStateInit.Extrapolator.addTool(SimplifiedMaterialLocator, name = "MaterialLocator")
      else:
         fwdFitter.Fit.FwdStateInit.Extrapolator.addTool(DetailedMaterialLocator, name = "MaterialLocator")
      
      fwdFitter.Fit.Fit = "TrackMasterFitter/Fit"
      fwdFitter.Fit.addTool(TrackMasterFitter, name = "Fit")
   
      from TrackFitter.ConfiguredFitters import ConfiguredMasterFitter
      ConfiguredMasterFitter(fwdFitter.Fit.Fit, SimplifiedGeometry = simplifiedGeometryFit, LiteClusters = liteClustersFit, MSRossiAndGreisen = True)
      
      GaudiSequencer("TrackHLT1FitHLT1Seq").Members += [ fwdFitter ]
  
  

   
      
def RecoTrackingHLT2(exclude=[], simplifiedGeometryFit = True, liteClustersFit = True):
   '''Function that defines the pattern recognition algorithms for the HLT2 sequence of the Run 2 offline tracking'''

   ## Tracking sequence
   from Configurables import ProcessPhase
   track = ProcessPhase("TrackHLT2");
   GaudiSequencer("RecoTrHLT2Seq").Members += [ track ]


   tracklists = []

   # Which algs to run ?
   trackAlgs = TrackSys().getProp("TrackPatRecAlgorithms")

   # Which data type is it?
   dataType = TrackSys().getProp("DataType")

   
   ## Forward pattern
   if "ForwardHLT2" in trackAlgs :
      track.DetectorList += [ "ForwardPatHLT2" ]
      # Need to filter out the Velo tracks of the fitted HLT1 tracks
      #from Configurables import TrackListRefiner, TrackSelector
      #refiner = TrackListRefiner("FilterForwardHLT1Tracks")
      #refiner.inputLocation = "Rec/Track/FittedHLT1Tracks"
      #refiner.outputLocation = "Rec/Track/FittedHLT1ForwardTracks"
      #refiner.Selector = "TrackSelector"
      #refiner.addTool(TrackSelector("TrackSelector"))
      #refiner.TrackSelector.TrackTypes = [ "Long" ]
      #GaudiSequencer("TrackHLT2ForwardPatHLT2Seq").Members +=  [ refiner ]

      from Configurables import PatForward
      GaudiSequencer("TrackHLT2ForwardPatHLT2Seq").Members +=  [ PatForward("PatForwardHLT2") ]
      from PatAlgorithms import PatAlgConf
      PatAlgConf.ForwardConf().configureAlgRun2HLT2()
      if TrackSys().timing() :
         PatForward("PatForwardHLT2").TimingMeasurement = True;    
         #tracklists += ["Rec/Track/ForwardHLT2"]

      #merge forward from HLT1 and HLT2
      from Configurables import TrackContainerCopy
      merger = TrackContainerCopy("MergeForwardHLT1HLT2")
      merger.inputLocations = [ "Rec/Track/FittedHLT1ForwardTracks", "Rec/Track/ForwardHLT2" ]
      merger.outputLocation =  "Rec/Track/Forward"
      merger.copyFailures = True
      GaudiSequencer("TrackHLT2ForwardPatHLT2Seq").Members +=  [ merger ]
      tracklists += ["Rec/Track/Forward"]

   ## Seed pattern
   if "PatSeed" in trackAlgs :
      track.DetectorList += [ "SeedPat" ]
      from Configurables import PatSeeding
      GaudiSequencer("TrackHLT2SeedPatSeq").Members += [PatSeeding("PatSeeding")]
      # should be replaced by more 'global' tracking configuration
      from PatAlgorithms import PatAlgConf
      PatAlgConf.SeedingConf().configureAlg()
      
      if TrackSys().timing() :
         PatSeeding("PatSeeding").TimingMeasurement = True;
       
      if TrackSys().cosmics() :
         from PatAlgorithms import PatAlgConf
         PatAlgConf.CosmicConf().configureAlg()
         
      tracklists += ["Rec/Track/Seed"]
     
   ## Matching 
   if "PatMatch" in trackAlgs :
      track.DetectorList += [ "MatchPat" ]
      from Configurables import PatMatch
      GaudiSequencer("TrackHLT2MatchPatSeq").Members += [ PatMatch("PatMatch") ]
      # timing?
      # global conf?
      tracklists  += ["Rec/Track/Match"]

   ## avoid running both downstream algos
   if "Downstream" in trackAlgs and "PatLongLivedTracking" in trackAlgs :
      raise RuntimeError("Cannot run both PatDownstream and PatLongLivedTracking at once")
   
   ## Downstream
   if "Downstream" in trackAlgs :
      track.DetectorList += [ "DownstreamPat" ]
      from Configurables import PatDownstream
      GaudiSequencer("TrackHLT2DownstreamPatSeq").Members += [ PatDownstream("PatDownstream") ];
      from PatAlgorithms import PatAlgConf
      # global conf?
      #PatAlgConf.DownstreamConf().configureAlg()
      if TrackSys().timing() :
         PatDownstream("PatDownstream").TimingMeasurement = True;
      tracklists += ["Rec/Track/Downstream"]

   ## PatLongLivedTracking (aka improved Downstream)
   if "PatLongLivedTracking" in trackAlgs :
      track.DetectorList += [ "DownstreamPat" ]
      from Configurables import PatLongLivedTracking
      GaudiSequencer("TrackHLT2DownstreamPatSeq").Members += [ PatLongLivedTracking("PatLongLivedTracking") ];
      if TrackSys().timing() :
         PatLongLivedTracking("PatLongLivedTracking").TimingMeasurement = True;
      tracklists += ["Rec/Track/Downstream"]


   fit = ProcessPhase("FitHLT2");
   GaudiSequencer("RecoTrHLT2Seq").Members += [ fit ]
   ### Clean clone and fit
   fit.DetectorList += ["Best"]
   
   # complete the list of track lists
   if "FastVelo" in trackAlgs :
      tracklists += ["Rec/Track/Velo"]

   if "VeloTT" in trackAlgs :
      tracklists += ["Rec/Track/VeloTT"]
      
   # create the best track creator
   # note the comment for the HLT1 forward fitter about configurations
   from TrackFitter.ConfiguredFitters import ConfiguredMasterFitter  
   from Configurables import TrackBestTrackCreator, TrackMasterFitter, TrackStateInitTool
   bestTrackCreator = TrackBestTrackCreator( TracksInContainers = tracklists )
   bestTrackCreator.addTool( TrackMasterFitter, name = "Fitter" )
   bestTrackCreator.DoNotRefit = True
   bestTrackCreator.addTool( TrackStateInitTool, name = "StateInitTool")
   bestTrackCreator.StateInitTool.UseFastMomentumEstimate = True
   # cut on ghost prob
   bestTrackCreator.AddGhostProb = True
   bestTrackCreator.GhostIdTool = "Run2GhostId"
   bestTrackCreator.MaxGhostProb = 0.5
   # configure its fitter and stateinittool
   ConfiguredMasterFitter( getattr(bestTrackCreator, "Fitter"), SimplifiedGeometry = simplifiedGeometryFit, LiteClusters = liteClustersFit, MSRossiAndGreisen = True )
   if "FastVelo" in trackAlgs :
      bestTrackCreator.StateInitTool.VeloFitterName = "FastVeloFitLHCbIDs"
   # add to the sequence
   GaudiSequencer("FitHLT2BestSeq").Members += [ bestTrackCreator ]

   ### Change dEdx correction for simulated data
   #if TrackSys().getProp("Simulation"):
   #   from Configurables import StateDetailedBetheBlochEnergyCorrectionTool,DetailedMaterialLocator
   #   from Configurables import TrackBestTrackCreator
   #   fitter = TrackBestTrackCreator().Fitter
   #   fitter.MaterialLocator.addTool(StateDetailedBetheBlochEnergyCorrectionTool("GeneralDedxTool"))
   #   fitter.MaterialLocator.GeneralDedxTool.EnergyLossFactor = 0.76

   # Make V0
   # needs to be done after fitting, but before the extra infos
   from Configurables import TrackV0Finder
   V0 = ProcessPhase("RecV0");
   GaudiSequencer("RecoTrHLT2Seq").Members += [ V0 ]
   V0.DetectorList += [ "MakeV0" ]
   ### Clean clone and fit
   trackV0Finder = TrackV0Finder("TrackV0Finder")
   GaudiSequencer("RecV0MakeV0Seq").Members += [ trackV0Finder ]


   #########################################################
   ##########################################################
   
   addExtraInfo = ProcessPhase("AddExtraInfo");
   GaudiSequencer("RecoTrHLT2Seq").Members += [ addExtraInfo ]
      
   ## Extra track information sequence
   extraInfos = TrackSys().getProp("TrackExtraInfoAlgorithms")
   if len(extraInfos) > 0 :
      
      
      
      ## Clone finding and flagging
      if "CloneFlagging" in extraInfos :

         addExtraInfo.DetectorList += ["Clones"]
         
         from Configurables import TrackBuildCloneTable, TrackCloneCleaner
         #trackClones = GaudiSequencer("TrackClonesSeq")
         if TrackSys().timing() :
            trackClones.MeasureTime = True
         cloneTable = TrackBuildCloneTable("FindTrackClones")
         cloneTable.maxDz   = 500*mm
         cloneTable.zStates = [ 0*mm, 990*mm, 9450*mm ]
         cloneTable.klCut   = 5e3
         cloneCleaner = TrackCloneCleaner("FlagTrackClones")
         cloneCleaner.CloneCut = 5e3
         #trackClones.Members += [ cloneTable, cloneCleaner ]
         GaudiSequencer("AddExtraInfoClonesSeq").Members += [ cloneTable, cloneCleaner ]

      ## Add the likelihood information
      if "TrackLikelihood" in extraInfos and ('TrackLikelihood' not in exclude):

         addExtraInfo.DetectorList += ["TrackLikelihood"]
         
         from Configurables import TrackAddLikelihood, TrackLikelihood 
         trackAddLikelihood = TrackAddLikelihood()
         trackAddLikelihood.addTool( TrackLikelihood, name = "TrackMatching_likTool" )
         trackAddLikelihood.TrackMatching_likTool.otEff = 0.9
         GaudiSequencer("AddExtraInfoTrackLikelihoodSeq").Members += [ trackAddLikelihood ]
         
      ## ghost probability using a Neural Net
      if "GhostProbability" in extraInfos :

         addExtraInfo.DetectorList += ["GhostProb"]
         
         from Configurables import TrackAddNNGhostId
         nn = TrackAddNNGhostId("TrackAddNNGhostdId")
         nn.GhostIdTool = "Run2GhostId" # to be decided
         GaudiSequencer("AddExtraInfoGhostProbSeq").Members += [ nn ]

   # this is very misleading (naming wise), but it will erase extra info that is put on the track by some algorithms
   # (not of the ones beforehand).
   addExtraInfo.DetectorList += ["EraseExtraInfo"]
   from Configurables import TrackEraseExtraInfo

   # erase extra info on Best tracks and on fitted Velo tracks (i.e. on everything that is saved on a DST)
   eraseExtraInfoBest = TrackEraseExtraInfo("TrackEraseExtraInfoBest")
   eraseExtraInfoFittedVelo = TrackEraseExtraInfo("TrackEraseExtraInfoFittedVelo")
   eraseExtraInfoFittedVelo.InputLocation = "Rec/Track/FittedHLT1VeloTracks"

   GaudiSequencer("AddExtraInfoEraseExtraInfoSeq").Members += [ eraseExtraInfoBest, eraseExtraInfoFittedVelo ]
      
   
   
   ## Muon alignment tracks
   ## is this still needed? Looks rather old
   if "MuonAlign" in trackAlgs :
      from Configurables import TrackEventFitter, AlignMuonRec
      track.DetectorList += ["MuonRec"]
      GaudiSequencer("TrackMuonRecSeq").Members += [ AlignMuonRec("AlignMuonRec"),
                                                     TrackEventFitter("MuonTrackFitter") ]
      importOptions("$TRACKSYSROOT/options/AlignMuonRec.opts")
      if TrackSys().fieldOff():
         AlignMuonRec("AlignMuonRec").BField = False;
         importOptions( "$STDOPTS/DstContentMuonAlign.opts" )


