from os import environ
from Gaudi.Configuration import *
import GaudiKernel.ProcessJobOptions
from TrackSys.Configuration import *
from GaudiKernel.SystemOfUnits import mm

from Configurables import ( ProcessPhase, MagneticFieldSvc,
                            DecodeVeloRawBuffer,
                            Tf__PatVeloRTracking, Tf__PatVeloSpaceTool,
                            Tf__PatVeloSpaceTracking, Tf__PatVeloGeneralTracking,
                            Tf__PatVeloTrackTool, Tf__PatVeloGeneric,
                            FastVeloTracking,
                            RawBankToSTClusterAlg, RawBankToSTLiteClusterAlg,
                            PatForward,
                            TrackEventFitter,
                            Tf__Tsa__Seed, Tf__Tsa__SeedTrackCnv,
                            PatSeeding,
                            TrackMatchVeloSeed, PatDownstream, PatVeloTT,
                            TrackStateInitAlg, TrackStateInitTool,
                            FilterMatchTracks, FilterDownstreamTracks, FilterSeedTracks,
                            TrackEventCloneKiller, TrackPrepareVelo,
                            TrackAddLikelihood, TrackLikelihood, TrackAddNNGhostId, Tf__OTHitCreator,
                            TrackBuildCloneTable, TrackCloneCleaner, AlignMuonRec,
                            TrackEraseExtraInfo, PatMatch
                           )

def RecoTracking(exclude=[]):
   '''What used to be in the options file, moved here'''
   ## Start TransportSvc, needed by track fit
   ApplicationMgr().ExtSvc.append("TransportSvc")
   
   ## --------------------------------------------------------------------
   ## Pattern Recognition and Fitting
   ## --------------------------------------------------------------------
   
   # Which algs to run ?
   trackAlgs = TrackSys().getProp("TrackPatRecAlgorithms")

   if "Velo" or "FastVelo" in trackAlgs :
      GaudiSequencer("RecoDecodingSeq").Members += [ DecodeVeloRawBuffer("DecodeVeloClusters")]

      veloClusters = DecodeVeloRawBuffer("DecodeVeloClusters")
      veloClusters.DecodeToVeloLiteClusters = True;
      veloClusters.DecodeToVeloClusters     = True;
      globalCuts = TrackSys().getProp("GlobalCuts")
      if( "Velo" in globalCuts ) :
          veloClusters.MaxVeloClusters =  globalCuts["Velo"]
      
   GaudiSequencer("RecoDecodingSeq").Members += [ RawBankToSTClusterAlg("CreateTTClusters"),
                                                   RawBankToSTLiteClusterAlg("CreateTTLiteClusters")]
   
   createITClusters = RawBankToSTClusterAlg("CreateITClusters")
   createITLiteClusters = RawBankToSTLiteClusterAlg("CreateITLiteClusters")
   createITClusters.DetType     = "IT"
   createITLiteClusters.DetType = "IT"
   
   GaudiSequencer("RecoDecodingSeq").Members += [ createITClusters, createITLiteClusters ]
   

   from STTools import STOfflineConf
   if TrackSys().MC09() :
      STOfflineConf.DefaultConfMC09().configureTools()
   else :
      STOfflineConf.DefaultConf().configureTools()

   #importOptions( "$STTOOLSROOT/options/Brunel.opts" )
   
   ## Velo tracking
   if "FastVelo" in trackAlgs :
      GaudiSequencer("RecoVELOSeq").Members += [ FastVeloTracking() ]
      if TrackSys().timing() :
         FastVeloTracking().TimingMeasurement = True; 

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
            if TrackSys().timing() :
               Tf__PatVeloGeneralTracking("PatVeloGeneralTracking").TimingMeasurement = True;
      else:
         GaudiSequencer("RecoVELOSeq").Members += [ Tf__PatVeloRTracking("PatVeloRTracking"),
                                                    Tf__PatVeloSpaceTracking("PatVeloSpaceTracking"),
                                                    Tf__PatVeloGeneralTracking("PatVeloGeneralTracking")
                                                    ]
         Tf__PatVeloSpaceTracking("PatVeloSpaceTracking").addTool( Tf__PatVeloSpaceTool(), name="PatVeloSpaceTool" )
         Tf__PatVeloSpaceTracking("PatVeloSpaceTracking").PatVeloSpaceTool.MarkClustersUsed = True;
         if TrackSys().timing() :
            Tf__PatVeloSpaceTracking("PatVeloSpaceTracking").TimingMeasurement = True;
            Tf__PatVeloRTracking("PatVeloRTracking").TimingMeasurement = True;
            Tf__PatVeloGeneralTracking("PatVeloGeneralTracking").TimingMeasurement = True;
            
         
   ## Special OT decoder for cosmics to merge spills.
   if TrackSys().cosmics():
      from Configurables import (Tf__OTHitCreator)
      Tf__OTHitCreator('OTHitCreator').RawBankDecoder = 'OTMultiBXRawBankDecoder'
      ## note: this does not change the OTMeasurementProvider used in the fit.
      # also adapt the MasterExtrapolator in the TrackInterpolator
      from Configurables import TrackInterpolator
      TrackInterpolator().Extrapolator.ExtraSelector = 'TrackSimpleExtraSelector'
      
      
      
   ## Make sure the default extrapolator and interpolator use simplified material
   if TrackSys().simplifiedGeometry() and ('SimpleGeom' not in exclude):
      from Configurables import TrackMasterExtrapolator, TrackInterpolator
      TrackMasterExtrapolator().MaterialLocator = 'SimplifiedMaterialLocator'
      TrackInterpolator().addTool( TrackMasterExtrapolator( MaterialLocator = 'SimplifiedMaterialLocator' ), name='Extrapolator')
      
   ## Tracking sequence
   track = ProcessPhase("Track");
   GaudiSequencer("RecoTrSeq").Members += [ track ]
   
   if TrackSys().fieldOff() : MagneticFieldSvc().ForcedSignedCurrentScaling = 0.
   
   if "noDrifttimes" in TrackSys().getProp("ExpertTracking"):
      from Configurables import (Tf__OTHitCreator)
      Tf__OTHitCreator("OTHitCreator").NoDriftTimes = True

   if "disableOTTimeWindow" not in TrackSys().getProp("ExpertTracking"):
      from Configurables import OTRawBankDecoder
      from GaudiKernel.SystemOfUnits import ns
      OTRawBankDecoder().TimeWindow = ( -8.0*ns, 56.0*ns )                     
      
   # Get the fitters
   from TrackFitter.ConfiguredFitters import ConfiguredFit, ConfiguredFitSeed
   
   # Clone killer
   cloneKiller = TrackEventCloneKiller()
   cloneKiller.TracksInContainers = []   
   
   # Is this standard sequence?
   stdSeq = "fastSequence" not in TrackSys().getProp("ExpertTracking")
   
   ## Forward pattern
   if "Forward" in trackAlgs :
      track.DetectorList += [ "ForwardPat" ]
      GaudiSequencer("TrackForwardPatSeq").Members +=  [ PatForward("PatForward") ]
      from PatAlgorithms import PatAlgConf
      PatAlgConf.ForwardConf().configureAlg()
      if TrackSys().timing() :
         PatForward("PatForward").TimingMeasurement = True;    
      cloneKiller.TracksInContainers += ["Rec/Track/Forward"]
   
   ## Seed pattern
   if "TsaSeed" in trackAlgs and "PatSeed" in trackAlgs :
      raise RuntimeError("Cannot run both Tsa and Pat Seeding at once")
   if "TsaSeed" in trackAlgs :
      track.DetectorList += [ "SeedPat" ]
      GaudiSequencer("TrackSeedPatSeq").Members += [Tf__Tsa__Seed("TsaSeed"),
                                                    Tf__Tsa__SeedTrackCnv( "TsaSeedTrackCnv" )]
      from TsaAlgorithms import TsaAlgConf
      TsaAlgConf.TsaSeedConf().configureAlg()
      if TrackSys().timing() :
         Tf__Tsa__Seed("TsaSeed").TimingMeasurement = True;
      
   if "PatSeed" in trackAlgs :
      track.DetectorList += [ "SeedPat" ]
      GaudiSequencer("TrackSeedPatSeq").Members += [PatSeeding("PatSeeding")]
      from PatAlgorithms import PatAlgConf
      PatAlgConf.SeedingConf().configureAlg()
      
      if TrackSys().timing() :
         PatSeeding("PatSeeding").TimingMeasurement = True;
      

      if TrackSys().cosmics() :
         from PatAlgorithms import PatAlgConf
         PatAlgConf.CosmicConf().configureAlg()
         
   if "TsaSeed" in trackAlgs or "PatSeed" in trackAlgs :
      cloneKiller.TracksInContainers += ["Rec/Track/Seed"]
      if "Match" in trackAlgs :
         # Fit now
         track.DetectorList += [ "SeedFit" ]
         ## Seed fit initialization
         GaudiSequencer("TrackSeedFitSeq").Members += [TrackStateInitAlg("InitSeedFit")]
         TrackStateInitAlg("InitSeedFit").TrackLocation = "Rec/Track/Seed"
         if "FastVelo" in trackAlgs :
            TrackStateInitAlg("InitSeedFit").addTool( TrackStateInitTool("TrackStateInitTool" ) )
            TrackStateInitTool( "TrackStateInitTool" ).VeloFitterName = "FastVeloFitLHCbIDs"
         # Use small ErrorQoP fitter, needed for Match
         GaudiSequencer("TrackSeedFitSeq").Members += [ConfiguredFitSeed()]
            
   ## Match
   if "Match" in trackAlgs and "PatMatch" in trackAlgs :
      raise RuntimeError("Cannot run both TrackMatching and PatMatch at once")
   if "Match" in trackAlgs :
      track.DetectorList += [ "MatchPat" ]
      GaudiSequencer("TrackMatchPatSeq").Members += [ TrackMatchVeloSeed("TrackMatch") ]
      from TrackMatching import TrackMatchConf
      TrackMatchConf.MatchingConf().configureAlg()      
      TrackMatchVeloSeed("TrackMatch").LikCut = -99999.
      if TrackSys().timing() :
         TrackMatchVeloSeed("TrackMatch").TimingMeasurement = True;

   if "PatMatch" in trackAlgs :
      track.DetectorList += [ "MatchPat" ]
      GaudiSequencer("TrackMatchPatSeq").Members += [ PatMatch("PatMatch") ]
   if "Match" in trackAlgs or "PatMatch" in trackAlgs :
      cloneKiller.TracksInContainers  += ["Rec/Track/Match"]
         
   ## Downstream
   if "Downstream" in trackAlgs :
      track.DetectorList += [ "DownstreamPat" ]
      GaudiSequencer("TrackDownstreamPatSeq").Members += [ PatDownstream() ];
      cloneKiller.TracksInContainers += ["Rec/Track/Downstream"]
      from PatAlgorithms import PatAlgConf
      #PatAlgConf.DownstreamConf().configureAlg()
      if TrackSys().timing() :
         PatDownstream("PatDownstream").TimingMeasurement = True;
      
      
   ## Velo-TT pattern
   if "VeloTT" in trackAlgs :
      track.DetectorList += ["VeloTTPat"]
      GaudiSequencer("TrackVeloTTPatSeq").Members += [ PatVeloTT("PatVeloTT")] 
      from PatVeloTT import PatVeloConf
      PatVeloConf.PatVeloTTConf().configureAlg()      
      cloneKiller.TracksInContainers += ["Rec/Track/VeloTT"]
      if TrackSys().timing() :
         PatVeloTT("PatVeloTT").TimingMeasurement = True;
         

   ### Clean clone and fit
   track.DetectorList += ["Fit"]
   cloneKiller.TracksOutContainer = "Rec/Track/AllBest"
   GaudiSequencer("TrackFitSeq").Members += [ cloneKiller ]
   GaudiSequencer("TrackFitSeq").Members += [TrackStateInitAlg("InitBestFit")]
   TrackStateInitAlg("InitBestFit").TrackLocation = "Rec/Track/AllBest"
   if "FastVelo" in trackAlgs :
      TrackStateInitAlg("InitBestFit").addTool( TrackStateInitTool("TrackStateInitTool" ) )
      TrackStateInitTool( "TrackStateInitTool" ).VeloFitterName = "FastVeloFitLHCbIDs"

   GaudiSequencer("TrackFitSeq").Members += [ConfiguredFit("FitBest","Rec/Track/AllBest")]      
   from Configurables import TrackContainerCopy
   copyBest = TrackContainerCopy( "CopyBest" )
   copyBest.inputLocation = "Rec/Track/AllBest";
   GaudiSequencer("TrackFitSeq").Members += [ copyBest ]

                                 
   ## Velo fitting
   if "Velo" in trackAlgs or "FastVelo" in trackAlgs :
      ## Prepare the velo tracks for the fit
      track.DetectorList += [ "VeloFit"]
      GaudiSequencer("TrackVeloFitSeq").Members += [ TrackPrepareVelo()]
      ## Fit the velo tracks
      GaudiSequencer("TrackVeloFitSeq").Members += [ ConfiguredFit("FitVelo","Rec/Track/PreparedVelo") ]
      ## copy the velo tracks to the "best" container (except in RDST case)
      if TrackSys().getProp( "OutputType" ).upper() != "RDST":
         copyVelo = TrackContainerCopy( "CopyVelo" )
         copyVelo.inputLocation = "Rec/Track/PreparedVelo";
         GaudiSequencer("TrackVeloFitSeq").Members += [ copyVelo ]
         
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
      if "TrackLikelihood" in extraInfos and ('TrackLikelihood' not in exclude):
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

