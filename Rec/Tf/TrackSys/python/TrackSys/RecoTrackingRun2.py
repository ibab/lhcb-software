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
         

def RecoTrackingHLT1(exclude=[]):
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
   
   ## Velo tracking
   ## Why is Velo not in the tracking sequence?
   if "FastVelo" in trackAlgs :
      from Configurables import FastVeloTracking
      GaudiSequencer("RecoVELOSeq").Members += [ FastVeloTracking("FastVeloTracking") ]
      if TrackSys().timing() :
         FastVeloTracking().TimingMeasurement = True; 

  
      
   ## Tracking sequence
   from Configurables import ProcessPhase
   track = ProcessPhase("Track");
   GaudiSequencer("RecoTrSeq").Members += [ track ]

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
      GaudiSequencer("TrackVeloTTPatSeq").Members += [ PatVeloTTHybrid("PatVeloTTHybrid")]
      from PatVeloTT import PatVeloTTAlgConf
      PatVeloTTAlgConf.PatVeloTTConf().configureAlgRun2HLT1()
      if TrackSys().timing() :
         PatVeloTTHybrid("PatVeloTTHybrid").TimingMeasurement = True;
      tracklists += ["Rec/Track/VeloTTHybrid"]
      
   ## Forward pattern
   if "ForwardHLT1" in trackAlgs :
      track.DetectorList += [ "ForwardPatHLT1" ]
      from Configurables import PatForward
      GaudiSequencer("TrackForwardPatHLT1Seq").Members +=  [ PatForward("PatForwardHLT1") ]
      # should be replaced by more 'global' tracking configuration
      from PatAlgorithms import PatAlgConf
      PatAlgConf.ForwardConf().configureAlgRun2HLT1()
      if TrackSys().timing() :
         PatForward("PatForwardHLT1").TimingMeasurement = True;    
      tracklists += ["Rec/Track/ForwardHLT1"]

      
def RecoTrackingHLT2(exclude=[]):
   '''Function that defines the pattern recognition algorithms for the HLT2 sequence of the Run 2 offline tracking'''

   tracklists = []
   
   ## Forward pattern
   if "ForwardHLT2" in trackAlgs :
      track.DetectorList += [ "ForwardPatHLT2" ]
      from Configurables import PatForward
      GaudiSequencer("TrackForwardPatHLT2Seq").Members +=  [ PatForward("PatForwardHLT2") ]
      from PatAlgorithms import PatAlgConf
      PatAlgConf.ForwardConf().configureAlgRun2HLT2()
      if TrackSys().timing() :
         PatForward("PatForwardHLT2").TimingMeasurement = True;    
      tracklists += ["Rec/Track/ForwardHLT2"]

      #merge forward from HLT1 and HLT2
      from Configurables import TrackListMerger
      merger = TrackListMerger("MergeForwardHLT1HLT2")
      merger.inputLocations = [ "Rec/Track/ForwardHLT1", "Rec/Track/ForwardHLT2" ]
      merger.outputLocation =  "Rec/Track/Forward"
      GaudiSequencer("TrackForwardPatHLT2Seq").Members +=  [ merger ]

   ## Seed pattern
   if "PatSeed" in trackAlgs :
      track.DetectorList += [ "SeedPat" ]
      from Configurables import PatSeeding
      GaudiSequencer("TrackSeedPatSeq").Members += [PatSeeding("PatSeeding")]
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
      GaudiSequencer("TrackMatchPatSeq").Members += [ PatMatch("PatMatch") ]
      # timing?
      # global conf?
      tracklists  += ["Rec/Track/Match"]
         
   ## Downstream
   if "Downstream" in trackAlgs :
      track.DetectorList += [ "DownstreamPat" ]
      from Configurables import PatDownstream
      GaudiSequencer("TrackDownstreamPatSeq").Members += [ PatDownstream("PatDownstream") ];
      from PatAlgorithms import PatAlgConf
      # global conf?
      #PatAlgConf.DownstreamConf().configureAlg()
      if TrackSys().timing() :
         PatDownstream("PatDownstream").TimingMeasurement = True;
      tracklists += ["Rec/Track/Downstream"]
      
   ### Clean clone and fit
   track.DetectorList += ["Fit"]
   
   # complete the list of track lists
   if "FastVelo" in trackAlgs :
      tracklists += ["Rec/Track/Velo"]
   # create the best track creator
   from Configurables import TrackBestTrackCreator
   bestTrackCreator = TrackBestTrackCreator( TracksInContainers = tracklists )
   # configure its fitter and stateinittool
   ConfiguredMasterFitter( bestTrackCreator.Fitter )
   if "FastVelo" in trackAlgs :
      bestTrackCreator.StateInitTool.VeloFitterName = "FastVeloFitLHCbIDs"
   # add to the sequence
   GaudiSequencer("TrackFitSeq").Members.append( bestTrackCreator )

   ### Change dEdx correction for simulated data
   if TrackSys().getProp("Simulation"):
      from Configurables import StateDetailedBetheBlochEnergyCorrectionTool,DetailedMaterialLocator
      from Configurables import TrackBestTrackCreator
      fitter = TrackBestTrackCreator().Fitter
      fitter.MaterialLocator.addTool(StateDetailedBetheBlochEnergyCorrectionTool("GeneralDedxTool"))
      fitter.MaterialLocator.GeneralDedxTool.EnergyLossFactor = 0.76
      
   ## Extra track information sequence
   extraInfos = TrackSys().getProp("TrackExtraInfoAlgorithms")
   if len(extraInfos) > 0 :
      
      track.DetectorList += ["AddExtraInfo"]
      
      ## Clone finding and flagging
      if "CloneFlagging" in extraInfos :
         from Configurables import TrackBuildCloneTable, TrackCloneCleaner
         trackClones = GaudiSequencer("TrackClonesSeq")
         GaudiSequencer("TrackAddExtraInfoSeq").Members += [ trackClones ]
         if TrackSys().timing() :
            trackClones.MeasureTime = True
         cloneTable = TrackBuildCloneTable("FindTrackClones")
         cloneTable.maxDz   = 500*mm
         cloneTable.zStates = [ 0*mm, 990*mm, 9450*mm ]
         cloneTable.klCut   = 5e3
         cloneCleaner = TrackCloneCleaner("FlagTrackClones")
         cloneCleaner.CloneCut = 5e3
         trackClones.Members += [ cloneTable, cloneCleaner ]
         
      ## ghost probability using a Neural Net
      if "GhostProbability" in extraInfos :
         from Configurables import TrackAddNNGhostId
         nn = TrackAddNNGhostId("TrackAddNNGhostdId")
         nn.GhostIdTool = "Run2GhostId" # to be decided
         GaudiSequencer("TrackAddExtraInfoSeq").Members += [ nn ]
         
   track.DetectorList += ["EraseExtraInformation"]
   from Configurables import TrackEraseExtraInfo
   GaudiSequencer("TrackEraseExtraInformationSeq").Members += [ TrackEraseExtraInfo("TrackEraseExtraInfo") ]
   
   
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


