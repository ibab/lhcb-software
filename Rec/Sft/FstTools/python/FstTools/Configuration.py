## @package FstTools
#  Test configuration of a First Software Trigger for the LHCb upgrade
#  @author Tim Head
#  @date   10/02/2014

__version__ = "0.1"
__author__  = "Tim Head"

from Gaudi.Configuration import *
from Configurables import RichRecSysConf
from Configurables import LHCbConfigurableUser
from Configurables import GaudiSequencer, FstSequencer, FstSelectVeloTracks
from Configurables import FstSelectForwardTracksPartOne, FstSelectForwardTracksPartTwo


## @class FstConf
#  Configurable for the Fst simulation for the LHCb upgrade
#  @author Olivier Callot
class FstConf(LHCbConfigurableUser):
    __used_configurables__ = [(RichRecSysConf, "RichReco"),]
    __slots__ = {
        "RootInTES" : "Fst/"
        ,"MinPt"           : 1200.  # MeV
        ,"MinP"            : 10000.  # MeV
        ,"MinIP"           : 0.100  # mm
        ,"MaxIP"           : 3.000  # mm
        ,"MinIPChi2"       : 9.0
        ,"MaxChi2Ndf"      : 3.0    
        ,"MinVeloClusters" : 9
        ,"MinVPClusters"   : 3
        ,"VeloType"        : "Velo"
        ,"TTType"          : "ValidateTT"
        ,"TStationType"    : "IT+OT"
        ,"RICHType"        : "" #"HLT2015"
        ,"TrackFit"        : "HltFit"
        ,"MuonID"          : False 
        ,"TStationHits"    : 16
        ,"ForwardMinPt"    : 1250.  # MeV, determines window size in forward tracking
                                    # or rejection cut in forward tracking if momentum
                                    # estimate is used
        ,"FastDecoding"    : True   # if false uses the old official decoding
        ,"Compare"         : False  # if using the old decoding, can run the new one and compare the containers
        ,"OutputFile" : ""
        }

    def applyConf(self):
        GaudiSequencer( "RecoHLTSeq" ).Members = [ "FstSequencer/RecoFstSeq" ]
        FstSequencer( "RecoFstSeq" ).ForcePassOK = True

        ## Multiplicity cuts (Kevin Dungs, 2014-01-18)
        from Configurables import LLTCaloAlg
        FstSequencer("RecoFstSeq").Members = ["LLTCaloAlg", "FstSelectGEC"]

        ## DST writer
        from Configurables import InputCopyStream
        writer = InputCopyStream('DstWriter3')
        writer.ItemList += [#"Fst#999",
                            "Link#999",
                            ]
        #FstSequencer("RecoFstSeq").Members += [writer]

        ## Velo configuration
        if "Velo" == self.getProp("VeloType"):
            from Configurables import FastVeloTracking, FastVeloDecoding, DecodeVeloRawBuffer
            if self.getProp("FastDecoding"):
                FstSequencer("RecoFstSeq").Members += ["FastVeloDecoding"]
            else:
                # TDH XXX had to rename it to DecodeVeloClusters2 to get my config changes to take effect?
                FstSequencer("RecoFstSeq").Members += ["DecodeVeloRawBuffer/DecodeVeloClusters2"]
                DecodeVeloRawBuffer("DecodeVeloClusters2").MaxVeloClusters = 10000
                DecodeVeloRawBuffer("DecodeVeloClusters2").DecodeToVeloClusters = False
                
                if self.getProp("Compare"):
                    FstSequencer("RecoFstSeq").Members += ["FastVeloDecoding"]
                    FastVeloDecoding().CompareResult = True
                    FastVeloDecoding().OutputLocation = "Raw/VeloLite"
                    
            FstSequencer("RecoFstSeq").Members += ["FastVeloTracking/FstVeloTracking"]
            FastVeloTracking("FstVeloTracking").OutputTracksName = self.getProp("RootInTES") + "Track/Velo"
            FastVeloTracking("FstVeloTracking").ResetUsedFlags = True
            FastVeloTracking("FstVeloTracking").HLT1Only = True
            
        elif "VP" == self.getProp("VeloType"):
            from Configurables import PrPixelTracking, VPRawBankToLiteCluster
            FstSequencer("RecoFstSeq").Members += ["VPRawBankToLiteCluster/FstVPDecoding",
                                                   "PrPixelTracking/FstPixel",
                                                   #"PrPixelStoreClusters"
                                                   ]
            # Centrally produced upgrade samples need this fix
            # see https://twiki.cern.ch/twiki/bin/viewauth/LHCbPhysics/UpgradeTrackingSequence
            #VPRawBankToLiteCluster("FstVPDecoding").RawEventLocation = "/Event/Other/RawEvent"
            
            PrPixelTracking("FstPixel").OutputTracksName = self.getProp("RootInTES") + "Track/Velo"
            # Use Kalman fit to estimate track state at end of the Velo
            # this is the state used by the UT tracking
            PrPixelTracking("FstPixel").EndVeloStateKalmanFit = True
            PrPixelTracking("FstPixel").AddFirstLastMeasurementStatesKalmanFit = True
            
        elif "VL" == self.getProp("VeloType"):
            from Configurables import PrVLTracking
            from Configurables import VLRawBankDecoder
            FstSequencer("RecoFstSeq").Members += ["VLRawBankDecoder/FstVLDecoding",
                                                   "PrVLTracking/FstVLTracking"]
            # Centrally produced upgrade samples need this fix
            # see https://twiki.cern.ch/twiki/bin/viewauth/LHCbPhysics/UpgradeTrackingSequence
            VLRawBankDecoder('FstVLDecoding').RawEventLocation = "/Event/Other/RawEvent"
            PrVLTracking("FstVLTracking").TrackLocation = self.getProp("RootInTES") + "Track/Velo"
            
        else:
            log.warning( "Unknown VeloType option '%s' !"%self.getProp("VeloType"))
            exit(0)

        ## PV
        # Originally the FastPV algorithm was used
        if False:
            from Configurables import FastPVFinder
            FstSequencer("RecoFstSeq").Members += ["FastPVFinder/FstPV"]
            FastPVFinder("FstPV").InputLocation  = self.getProp("RootInTES") + "Track/Velo"
            FastPVFinder("FstPV").OutputLocation = self.getProp("RootInTES") + "Vertices/PV"


        # This is the HLT PatPV3D algorithm
        else:
            from Configurables import PatPV3D, PVOfflineTool, LSAdaptPV3DFitter
            FstSequencer("RecoFstSeq").Members += ["PatPV3D/HltPVsPV3D"]
            recoPV3D = PatPV3D('HltPVsPV3D' )
            recoPV3D.addTool(PVOfflineTool,"PVOfflineTool")
            recoPV3D.PVOfflineTool.UseBeamSpotRCut = True
            recoPV3D.PVOfflineTool.BeamSpotRCut = 0.3
            recoPV3D.PVOfflineTool.PVSeedingName = "PVSeedTool"
            recoPV3D.PVOfflineTool.PVFitterName = "LSAdaptPV3DFitter"
            recoPV3D.PVOfflineTool.addTool(LSAdaptPV3DFitter, "LSAdaptPV3DFitter")
            recoPV3D.PVOfflineTool.LSAdaptPV3DFitter.TrackErrorScaleFactor = 2.
            recoPV3D.PVOfflineTool.LSAdaptPV3DFitter.maxIP2PV = 0.3
            recoPV3D.PVOfflineTool.InputTracks = [self.getProp("RootInTES") + "Track/Velo"]
            recoPV3D.OutputVerticesName = self.getProp("RootInTES") + "Vertices/PV"

        # Selection of Velo tracks to extend
        if "ValidateTT" == self.getProp("TTType"):
            if self.getProp("FastDecoding"):
                from Configurables import FastSTDecoding
                FstSequencer("RecoFstSeq").Members += ["FastSTDecoding/FastTTDecoding"]
                FastSTDecoding("FastTTDecoding").DetectorName = "TT"
            else:
                FstSequencer("RecoFstSeq").Members += ["RawBankToSTLiteClusterAlg/CreateTTLiteClusters"]

        FstSequencer("RecoFstSeq").Members += ["FstSelectVeloTracks"]
        FstSelectVeloTracks().InputTracksName = self.getProp("RootInTES") + "Track/Velo"
        FstSelectVeloTracks().OutputTracksName = self.getProp("RootInTES") + "Track/VeloFst"
        FstSelectVeloTracks().PVName = self.getProp("RootInTES") + "Vertices/PV"
        FstSelectVeloTracks().MinIP = self.getProp("MinIP")
        FstSelectVeloTracks().MaxIP = self.getProp("MaxIP")
        if "VP" == self.getProp("VeloType"):
            FstSelectVeloTracks().MinVeloClusters  = self.getProp( "MinVPClusters" )
        else:
            FstSelectVeloTracks().MinVeloClusters  = self.getProp("MinVeloClusters")
        
        ## Handle TT/UT
        if "ValidateTT" == self.getProp("TTType"):
            FstSelectVeloTracks().ValidateWithTT = True
        elif "none" == self.getProp("TTType"):
            FstSelectVeloTracks().ValidateWithTT = False
        else:
            log.warning( "Unknown TTType option '%s' !"%self.getProp("TTType"))
            exit(0)

        ## Forward tracking on selected tracks
        if "IT+OT" == self.getProp("TStationType"):
            from Configurables import PatForward, PatForwardTool, FastSTDecoding, PatTStationHitManager
            from Configurables import STOnlinePosition

            # Configure same errors as used in the HLT
            ITOnl = STOnlinePosition("ITLiteClusterPosition")
            ITOnl.ErrorVec = [0.253, 0.236, 0.273]
            ITOnl.APE = 0.0758
            
            if self.getProp("FastDecoding"):
                FstSequencer("RecoFstSeq").Members += ["FastSTDecoding/FastTTDecoding",
                                                       "FastSTDecoding/FastITDecoding"]
                FastSTDecoding("FastTTDecoding").DetectorName = "TT"
                FastSTDecoding("FastITDecoding").DetectorName = "IT"
                
            else:
                FstSequencer("RecoFstSeq").Members += ["RawBankToSTLiteClusterAlg/CreateTTLiteClusters",
                                                       "RawBankToSTLiteClusterAlg/CreateITLiteClusters"]
                
                if self.getProp("Compare"):
                    FstSequencer("RecoFstSeq").Members += ["FastSTDecoding/FastTTDecoding",
                                                           "FastSTDecoding/FastITDecoding"]
                    FastSTDecoding("FastTTDecoding").DetectorName = "TT"
                    FastSTDecoding("FastTTDecoding").CompareResult = True
                    FastSTDecoding("FastITDecoding").DetectorName = "IT"
                    FastSTDecoding("FastITDecoding").CompareResult = True
            
            FstSequencer("RecoFstSeq").Members += ["PatForward/FstForward"]
            PatForward("FstForward").InputTracksName  = self.getProp("RootInTES") + "Track/VeloFst"
            PatForward("FstForward").OutputTracksName = self.getProp("RootInTES") + "Track/Forward"
            PatForward("FstForward").maxOTHits = 1E8
            PatForward("FstForward").DeltaNumberInT = 1E8
            PatForward("FstForward").DeltaNumberInTT = 1E8
            PatForward("FstForward").MaxNVelo = 1E8
            PatForward("FstForward").addTool(PatForwardTool)
            PatForward("FstForward").PatForwardTool.MinPt = 1250 #self.getProp( "MinPt" )
            PatForward("FstForward").PatForwardTool.MinMomentum = 3000
            PatForward("FstForward").PatForwardTool.SecondLoop = False
            
        elif "FT" == self.getProp("TStationType"):
            from Configurables import PrForwardTracking, PrForwardTool
            FstSequencer("RecoFstSeq").Members += ["FTRawBankDecoder",
                                                   "PrForwardTracking/FstForward"]            
            FstForward = PrForwardTracking("FstForward")
            FstForward.InputName = self.getProp("RootInTES") + "Track/VeloFst"
            FstForward.OutputName = self.getProp("RootInTES") + "Track/Forward"
            FstForward.addTool(PrForwardTool)
            FstForward.PrForwardTool.MinPt = self.getProp("ForwardMinPt")
            FstForward.PrForwardTool.AddUTHitsToolName = ""

        # Forward tracking using the VeloUT to estimate
        # a tracks momentum
        elif "FT+VeloUT" == self.getProp("TStationType"):
            from Configurables import PrForwardTracking, PrForwardTool, PrFTHitManager
            from Configurables import PrVeloUT
            from Configurables import TrackMasterFitter
            from Configurables import RawBankToSTLiteClusterAlg
            from Configurables import PrVeloUTTool

            prVeloUT = PrVeloUT()
            prVeloUT.InputTracksName = self.getProp("RootInTES") + "Track/VeloFst"
            prVeloUT.OutputTracksName = self.getProp("RootInTES") + "Track/VeloUTFst"
            
            prVeloUT.addTool(TrackMasterFitter, "Fitter")
            prVeloUT.Fitter.MeasProvider.IgnoreVelo = True
            #prVeloUT.Fitter.MeasProvider.IgnoreVL = True
            prVeloUT.Fitter.MeasProvider.IgnoreVP = True
            prVeloUT.Fitter.MeasProvider.IgnoreTT = True
            prVeloUT.Fitter.MeasProvider.IgnoreIT = True
            prVeloUT.Fitter.MeasProvider.IgnoreOT = True
            prVeloUT.Fitter.MeasProvider.IgnoreUT = False
            if self.getProp("VeloType") == "VP":
                prVeloUT.Fitter.MeasProvider.IgnoreVP = False
            elif self.getProp("VeloType") == "VL":
                prVeloUT.Fitter.MeasProvider.IgnoreVL = False
            elif self.getProp("VeloType") == "Velo":
                prVeloUT.Fitter.MeasProvider.IgnoreVelo = False

            PrVeloUTTool("PrVeloUT.PrVeloUTTool").minMomentum = 2000.
            PrVeloUTTool("PrVeloUT.PrVeloUTTool").minPT = 200.
            PrVeloUTTool("PrVeloUT.PrVeloUTTool").PrintVariables = True

            #createUTLiteClusters = RawBankToSTLiteClusterAlg("CreateUTLiteClusters")
            #createUTLiteClusters.DetType = "UT"
            #createUTLiteClusters.clusterLocation = "Raw/UT/LiteClusters"
            # This create Lite clusters as well as the full ones
            # lite is all that is needed for tracking, but to
            # evaluate the efficiencies in simulation we also
            # need the full clusters
            from DAQSys.Decoders import DecoderDB
            from DAQSys.DecoderClass import decodersForBank

            createUTLiteClusters = decodersForBank(DecoderDB, "UT")
            FstSequencer("RecoFstSeq").Members += [d.setup() for d in createUTLiteClusters]
            
            FstSequencer("RecoFstSeq").Members += [PrVeloUT("PrVeloUT"),
                                                   "FTRawBankDecoder",
                                                   "PrForwardTracking/FstForward"]
            
            FstForward = PrForwardTracking("FstForward")
            FstForward.InputName = self.getProp("RootInTES") + "Track/VeloUTFst"
            FstForward.OutputName = self.getProp("RootInTES") + "Track/Forward"
            FstForward.addTool(PrForwardTool)
            FstForward.PrForwardTool.AddUTHitsToolName = ""
            # Now with the momentum estimate
            FstForward.PrForwardTool.UseMomentumEstimate = True
            # MinPt determines the size of the search window, set
            # this to the lowest pT you want to be able to find
            FstForward.PrForwardTool.MinPt = self.getProp("ForwardMinPt")
            # Remove tracks with an UT estimated pT below this value,
            # because the UT resolution is not perfect set this to a
            # value less than the lowest pT you want to be able to
            # find, eg: MinPt = 500 and PreselectionPT = 400MeV
            FstForward.PrForwardTool.PreselectionPT = self.getProp("ForwardMinPt") * 0.8
            FstForward.PrForwardTool.Preselection = True
            FstForward.PrForwardTool.UseWrongSignWindow = True

            #FstForward.PrForwardTool.HitManagerName = "PrFTHitManager/TimsHitManager"
            #ft_hitmanager = PrFTHitManager("TimsHitManager")
            #ft_hitmanager.OutputLevel = 1
            #ft_hitmanager.XSmearing = 0.002#2
                        
        else:
            log.warning("Unknown TStationType option '%s' !"%self.getProp("TStationType"))
            exit(0)

        FstSequencer("RecoFstSeq").Members += ["FstSelectForwardTracksPartOne/SelectFwd1"]
        fwd1 = FstSelectForwardTracksPartOne("SelectFwd1")
        fwd1.InputTracksName = self.getProp("RootInTES") + "Track/Forward"
        fwd1.OutputTracksName = self.getProp("RootInTES") + "Track/ForwardFstIntermediate"
        fwd1.PVName = self.getProp("RootInTES") + "Vertices/PV"
        fwd1.MinIP = 10e-6
        fwd1.MaxIP = 10e6
        fwd1.MinPt = self.getProp("MinPt")
        fwd1.MinP = self.getProp("MinP")
        fwd1.MinTHits = self.getProp("TStationHits")

        
        if "HltFit" == self.getProp("TrackFit"):
            from Configurables import TrackEventFitter, TrackMasterFitter, MeasurementProvider
            from TrackFitter.ConfiguredFitters import ConfiguredHltFitter
            HltFastFit_name = 'FastFit'
            HltFastFit = TrackEventFitter(HltFastFit_name)
            FstSequencer("RecoFstSeq").Members += [HltFastFit]
            
            HltFastFit.TracksInContainer = self.getProp("RootInTES") + "Track/ForwardFstIntermediate"
            
            HltFastFit.addTool(ConfiguredHltFitter("Fitter"), name='Fitter')
            
            HltFastFit.Fitter.addTool(MeasurementProvider)
            if "VP" == self.getProp("VeloType"):
                HltFastFit.Fitter.MeasurementProvider.IgnoreVelo = True
                HltFastFit.Fitter.MeasurementProvider.IgnoreVP   = False
            elif "VL" == self.getProp("VeloType"):
               HltFastFit.Fitter.MeasurementProvider.IgnoreVelo = True
               HltFastFit.Fitter.MeasurementProvider.IgnoreVL   = False
                
            if "ValidateTT" != self.getProp("TTType"):
                HltFastFit.Fitter.MeasurementProvider.IgnoreTT = True

            # ignore IT+OT as it cannot initialize
            if "FT" in self.getProp("TStationType"):
                HltFastFit.Fitter.MeasurementProvider.IgnoreIT = True
                HltFastFit.Fitter.MeasurementProvider.IgnoreOT = True
                HltFastFit.Fitter.MeasurementProvider.IgnoreFT = False

        ## Selection after measuring momentum
        FstSequencer("RecoFstSeq").Members += ["FstSelectForwardTracksPartTwo/SelectFwd2"]
        fwd2 = FstSelectForwardTracksPartTwo("SelectFwd2")
        fwd2.InputTracksName  = self.getProp("RootInTES") + "Track/ForwardFstIntermediate"
        fwd2.OutputTracksName = self.getProp("RootInTES") + "Track/ForwardFst"
        fwd2.PVName = self.getProp("RootInTES") + "Vertices/PV"
        fwd2.MinIP = 1E-7 #self.getProp( "MinIP" )
        fwd2.MaxIP = 1E7 #self.getProp( "MaxIP" )
        fwd2.MinIPChi2 = self.getProp("MinIPChi2")
        if "HltFit" in self.getProp("TrackFit"):
            fwd2.MaxChi2Ndf = self.getProp("MaxChi2Ndf")
        else:
            fwd2.MaxChi2Ndf = -1.

        # RICH related things
        if "HLT2015" == self.getProp("RICHType"):
            richRecConfName = "RichReco"
            rich_seq = GaudiSequencer("RecoRICHSeq")
            FstSequencer("RecoFstSeq").Members += [rich_seq]
        
            # Create the top level Conf object and set some general options
            richConf = RichRecSysConf(richRecConfName)
            #richConf.OutputLevel = 1
            # Set the sequencer the RICH reco algs should be added to
            richConf.RecoSequencer = rich_seq
            richConf.Context = "HLT"
            #richConf.Context = "Offline"
            richConf.Simulation = True
            richConf.DataType = "Upgrade"
            
            richConf.Radiators = ["Rich1Gas", "Rich2Gas"]
            richConf.PidConfig = "FullGlobal"
            #richConf.Particles = ["electron", "muon", "pion",
            #                      "kaon", "proton", "belowThreshold"]
            richConf.Particles = ["kaon", "pion"]
            richConf.TracklessRingAlgs = []
            richConf.pixelConfig().FindClusters = False
            richConf.CheckProcStatus = False
            richConf.InitPixels = True
            richConf.InitTracks = True
            richConf.InitPhotons = True
            richConf.pixelConfig().FindClusters = False
            # Set cuts on which tracks enter the RICH reco
            # XXX do we need this? we already only have tracks
            # XXX above 500MeV
            #richConf.trackConfig().TrackCuts = self.getProp("RichTrackCuts")
            #richConf.gpidConfig().TrackCuts = self.getProp("RichTrackCuts")
        
            # Input tracks and RICH output locations
            richConf.trackConfig().InputTracksLocation = self.getProp("RootInTES") + "Track/ForwardFst"
            richConf.RichPIDLocation = self.getProp("RootInTES") + "Rich/PIDs"
            
        
        if self.getProp("MuonID"):    
            print "run muon ID. CAUTION: muon ID run on all forward tracks, no selection applied"
            from MuonID import ConfiguredMuonIDs
            from Configurables import RawBankReadoutStatusConverter, RawBankReadoutStatusFilter
            cm = ConfiguredMuonIDs.ConfiguredMuonIDs(data="Upgrade")#self.getProp("DataType"),specialData=self.getProp("SpecialData"))
            MuonIDSeq = cm.getMuonIDSeq()
            RawBankReadoutStatusConverter("MuonProcStatus").System = "Muon"
            RawBankReadoutStatusConverter("MuonProcStatus").BankTypes = ["Muon"]
            RawBankReadoutStatusFilter("MuonROFilter").BankType = 13
            RawBankReadoutStatusFilter("MuonROFilter").RejectionMask = 2067
            
            from Configurables import MuonRec, MuonIDAlg
            # XXX should we get the tracks from Track/ForwardFst? Those are after Kfit
            MuonIDAlg().TrackLocation = self.getProp("RootInTES") + "Track/Forward"
            MuonIDAlg().FindQuality = False# speed up option - poinless w/o M1
            
            FstSequencer("RecoFstSeq").Members += ["MuonRec",
                                                   "RawBankReadoutStatusConverter/MuonProcStatus",
                                                   "RawBankReadoutStatusFilter/MuonROFilter",
                                                   MuonIDSeq]

        if self.getProp("OutputFile"):
            from Configurables import InputCopyStream
            writer = InputCopyStream('FstDstWriter')
            writer.ItemList += ["Fst#999",
                                "Link/Pr#999",
                                "Link/Fst#999",
                                ]
            writer.Output = "DATAFILE='PFN:%s'"%(self.getProp("OutputFile"))
            

from Configurables import FastVeloTracking, FastVeloDecoding, DecodeVeloRawBuffer
from Configurables import PatForward, PatForwardTool, TrackUsedLHCbID
from Configurables import PatVeloTTHybrid, PatVeloTTHybridTool
from Configurables import FastSTDecoding, PatTStationHitManager
from Configurables import CreateFastTrackCollection
from Configurables import PatSeeding, PatSeedingTool
from Configurables import PatMatch, PatMatchTool
from Configurables import PatDownstream
from Configurables import STOnlinePosition
from Configurables import TrackEventCloneKiller, TrackCloneFinder
from Configurables import TrackEventFitter, TrackMasterFitter
from Configurables import TrackContainerCopy, TrackSelector
from Configurables import RecycleGhostHits
from TrackFitter.ConfiguredFitters import ConfiguredHltFitter, ConfiguredMasterFitter


class StagedRecoConf(LHCbConfigurableUser):
    __slots__ = {"RootInTES" : "Fst/",
                 # which long tracks to merge into the 'best' container
                 "AddToBest": ["Fwd1", "Fwd2", "Match"],
                 # pT threshold of VeloTT tracking
                 "VeloTTMinPT": 200, # MeV
                 # pT threshold of first Forward tracking
                 "Fwd1MinPT": 500, #MeV
                 # pT threshold of second Forward tracking
                 "Fwd2MinPT": 200, #MeV
                 # skip already extended Velo seeds for the offline forward
                 "SkipUsedVeloSeeds": True,
                 # use only so far unused hits in the offline forward
                 "UnUsedHitsOnly": True,
                 }
    
    def applyConf(self):
        max_OT_hits = 1E8
        
        GaudiSequencer("RecoHLTSeq").Members = ["FstSequencer/RecoFstSeq"]
        FstSequencer("RecoFstSeq").ForcePassOK = True

        fst_seq = FstSequencer("RecoFstSeq")

        killer = EventNodeKiller()
        killer.Nodes += ["pRec", "Rec", "Raw", "Link/Rec"]
        fst_seq.Members += [killer]

        # All the decoding
        from DAQSys.Decoders import DecoderDB
        from DAQSys.DecoderClass import decodersForBank

        velo_decoder = DecodeVeloRawBuffer("DecodeVeloClusters2")
        velo_decoder.MaxVeloClusters = 10000
        velo_decoder.DecodeToVeloClusters = True
        createTTClusters = decodersForBank(DecoderDB, "TT")
        createITClusters = decodersForBank(DecoderDB, "IT")
        fst_seq.Members += [velo_decoder]
        fst_seq.Members += [d.setup() for d in createTTClusters]
        fst_seq.Members += [d.setup() for d in createITClusters]

        # Velo tracking as offline
        velo_tracking = FastVeloTracking("FstVeloTracking")
        velo_tracking.OutputTracksName = self.getProp("RootInTES") + "Track/Velo"
        fst_seq.Members += [velo_tracking]

        # VeloTT hybrid-nut
        veloTT = PatVeloTTHybrid("FstVeloTT")
        veloTT.InputTracksName = velo_tracking.getProp("OutputTracksName")
        veloTT.OutputTracksName = self.getProp("RootInTES") + "Track/VeloTT"
        veloTT.removeUsedTracks = False
        veloTT.InputUsedTracksNames = []
        veloTT.maxChi2 = 1280
        veloTT.fitTracks = False

        veloTT_tool = PatVeloTTHybridTool("PatVeloTTHybridTool")
        veloTT_tool.minMomentum = self.getProp("VeloTTMinPT")*10#2000
        veloTT_tool.minPT = self.getProp("VeloTTMinPT")
        veloTT_tool.maxPseudoChi2 = 1280
        veloTT_tool.PrintVariables = True
        veloTT_tool.PassTracks = True
        # Defines the size of the hole in the center (the tracks there are excluded)
        veloTT_tool.centralHoleSize = 33
        # Defines the size of the hole in the center where the tracks
        # are not used by veloTT but passed to PatForward directly
        # except for the tracks in the central hole
        veloTT_tool.PassHoleSize = 45

        fst_seq.Members += [veloTT]

        # Configure same errors as used in the HLT
        ITOnl = STOnlinePosition("ITLiteClusterPosition")
        ITOnl.ErrorVec = [0.253, 0.236, 0.273]
        ITOnl.APE = 0.0758

        # Forward tracking a la HLT1
        fwd_hlt1 = PatForward("FstFwdHlt1")
        fwd_hlt1.InputTracksName = veloTT.getProp("OutputTracksName")
        fwd_hlt1.OutputTracksName = self.getProp("RootInTES") + "Track/FwdHLT1"
        fwd_hlt1.maxOTHits = 1E8
        fwd_hlt1.DeltaNumberInT = 1E8
        fwd_hlt1.DeltaNumberInTT = 1E8
        fwd_hlt1.MaxNVelo = 1E8
        
        fwd_hlt1.addTool(PatForwardTool)
        fwd_hlt1_tool = fwd_hlt1.PatForwardTool
        fwd_hlt1_tool.SecondLoop = False
        fwd_hlt1_tool.MinPt = self.getProp("Fwd1MinPT")
        # Mark velo tracks which could be extended as "extended"/"used"
        fwd_hlt1_tool.FlagUsedSeeds = True
        fwd_hlt1_tool.MinMomentum = 3000
        fwd_hlt1_tool.UseMomentumEstimate = True
        fwd_hlt1_tool.UseWrongSignWindow = True
        fwd_hlt1_tool.WrongSignPT = 2000
        fwd_hlt1_tool.PreselectionPT = 0.8 * self.getProp("Fwd1MinPT")
        fwd_hlt1_tool.Preselection = True
        
        fst_seq.Members += [fwd_hlt1]

        
        # Forward tracking a la HLT2
        # basically try and recover some low momentum tracks
        # XXX should this run on VeloTT or just Velo tracks?
        fwd_hlt2 = PatForward("FstFwdHlt2")
        fwd_hlt2.InputTracksName = veloTT.getProp("OutputTracksName")
        #fwd_hlt2.InputTracksName = velo_tracking.getProp("OutputTracksName")
        fwd_hlt2.OutputTracksName = self.getProp("RootInTES") + "Track/FwdHLT2"
        fwd_hlt2.maxOTHits = max_OT_hits
        fwd_hlt2.DeltaNumberInT = 1E8
        fwd_hlt2.DeltaNumberInTT = 1E8
        fwd_hlt2.MaxNVelo = 1E8
        
        fwd_hlt2.addTool(PatForwardTool)
        fwd_hlt2_tool = fwd_hlt2.PatForwardTool
        fwd_hlt2_tool.AddTTClusterName = "PatAddTTCoord"
        fwd_hlt2_tool.SecondLoop = True
        fwd_hlt2_tool.MinPt = self.getProp("Fwd2MinPT")
        # Skip seeds we marked as used in HLT1
        fwd_hlt2_tool.SkipUsedSeeds = True
        fwd_hlt2_tool.FlagUsedSeeds = True
        fwd_hlt2_tool.MinMomentum = 1000
        fwd_hlt2_tool.UseMomentumEstimate = True
        fwd_hlt2_tool.UseWrongSignWindow = True
        fwd_hlt2_tool.WrongSignPT = 2000
        fwd_hlt2_tool.PreselectionPT = 0.8 * self.getProp("Fwd2MinPT")
        fwd_hlt2_tool.Preselection = True
        
        fst_seq.Members += [fwd_hlt2]

        
        # Merge the two types of Forward tracks into one container
        # PatForward could already do this for us, but we keep
        # the output of first and second loop seperate in case
        # we want to study efficiencies for each one
        merge_fwd = CreateFastTrackCollection("FstMergeForward")
        merge_fwd.SlowContainer = True
        merge_fwd.InputLocations = [fwd_hlt1.getProp("OutputTracksName"),
                                    fwd_hlt2.getProp("OutputTracksName"),
                                    ]
        merge_fwd.OutputLocation = self.getProp("RootInTES") + "Track/TmpForward"

        fst_seq.Members += [merge_fwd]
        
        # fit tracks then release hits used by "ghost" tracks so they
        # can be reused by the seeding
        fast_fit = TrackEventFitter("FstFastForwardFit")
        # Fit all long tracks which were merged by the clone killer
        fast_fit.TracksInContainer = merge_fwd.getProp("OutputLocation")
        fast_fit.TracksOutContainer = self.getProp("RootInTES") + "Track/ForwardFitted"
        fast_fit.addTool(TrackMasterFitter, name='Fitter')
        fitter = ConfiguredHltFitter(getattr(fast_fit, 'Fitter'))

        recycle_ghosts = RecycleGhostHits("FstRecycler")
        recycle_ghosts.inputLocation = fast_fit.getProp("TracksOutContainer")
        recycle_ghosts.outputLocation = self.getProp("RootInTES") + "Track/Forward"
        max_chi2 = TrackSelector("MaxChi2Selector")
        max_chi2.MaxChi2Cut = 5
        recycle_ghosts.Selector = max_chi2
        fst_seq.Members += [fast_fit, recycle_ghosts]

        
        # Seeding
        seeding = PatSeeding("FstSeeding")
        seeding.OutputTracksName = self.getProp("RootInTES") + "Track/Seed"
        
        seeding.addTool(PatSeedingTool, name="PatSeedingTool")
        seeding.PatSeedingTool.UseForward = True
        seeding.PatSeedingTool.ForwardCloneMergeSeg = True
        seeding.PatSeedingTool.NDblOTHitsInXSearch = 2
        # tracks from the two Forward instances are used to flag hits as used
        #seeding.PatSeedingTool.InputTracksName = merge_fwd.getProp("OutputLocation")
        seeding.PatSeedingTool.InputTracksName = recycle_ghosts.getProp("outputLocation")
        seeding.PatSeedingTool.MinMomentum = 1000
        seeding.PatSeedingTool.MaxOTHits = max_OT_hits

        # Matching
        matching = PatMatch("FstMatching")
        matching.VeloInput = velo_tracking.getProp("OutputTracksName")
        matching.SeedInput = seeding.getProp("OutputTracksName")
        matching.MatchOutput = self.getProp("RootInTES") + "Track/Match"

        matching.addTool(PatMatchTool, name="PatMatchTool")
        matching.PatMatchTool.MinMomentum = 1000
        matching.PatMatchTool.MinPt = self.getProp("Fwd2MinPT")

        fst_seq.Members += [seeding, matching]

        
        # Downstream tracking
        downstream = PatDownstream("FstDownstream")
        downstream.InputLocation = seeding.getProp("OutputTracksName")
        downstream.ForwardLocation = merge_fwd.getProp("OutputLocation")
        downstream.MatchLocation = matching.getProp("MatchOutput")
        downstream.OutputLocation = self.getProp("RootInTES") + "Track/Downstream"
        downstream.RemoveUsed = False
        downstream.RemoveAll = True
        downstream.MinMomentum = 0
        downstream.MinPt = 0

        fst_seq.Members += [downstream]


        # Hyperloop, third loop, or running the forward tracking
        # as is done offline. This could be the first step of the
        # new offline track reconstruction
        # Somehow PatForward interacts with seed+match because
        # of this we run after seed+match, as all these algos
        # share a HitManager
        offline_fwd = PatForward("FstOfflineForward")
        offline_fwd.InputTracksName = velo_tracking.getProp("OutputTracksName")
        offline_fwd.OutputTracksName = self.getProp("RootInTES") + "Track/OfflineFwd"
        offline_fwd.maxOTHits = max_OT_hits
        offline_fwd.DeltaNumberInT = 1E8
        offline_fwd.DeltaNumberInTT = 1E8
        offline_fwd.MaxNVelo = 1E8

        # With this enabled we achieve lower ghostrate but also lose efficiency
        if self.getProp("UnUsedHitsOnly"):
            offline_fwd.addTool(TrackUsedLHCbID, name='TrackUsedLHCbID')
            offline_fwd.UsedLHCbIDToolName = "TrackUsedLHCbID"
            #offline_fwd.TrackUsedLHCbID.inputContainers = [merge_fwd.getProp("OutputLocation")]
            offline_fwd.TrackUsedLHCbID.inputContainers = [recycle_ghosts.getProp("outputLocation"),
                                                           #matching.getProp("MatchOutput"),
                                                           #seeding.getProp("OutputTracksName"),
                                                           #downstream.getProp("OutputLocation"),
                                                           ]
            offline_fwd.TrackUsedLHCbID.selectorNames = ['ForwardSelector']
        
        offline_fwd.addTool(PatForwardTool)
        offline_fwd_tool = offline_fwd.PatForwardTool
        offline_fwd_tool.SecondLoop = True
        #offline_fwd_tool.MinPt = 70#self.getProp("Fwd2MinPT")
        # Skip seeds we marked as used in HLT
        offline_fwd_tool.SkipUsedSeeds = self.getProp("SkipUsedVeloSeeds")
        offline_fwd_tool.AddTTClusterName = "PatAddTTCoord"
        #offline_fwd_tool.addTool(PatFwdTool("PatFwdTool"))

        fst_seq.Members += [offline_fwd]

        
        # Merge the various containers with long tracks
        clone_killer = TrackEventCloneKiller('FstCloneKiller')
        # It makes little difference if you directly use the two Forwards
        # as input here or merge them first as done in `merge_fwd`
        merge_to_best = []
        if "Fwd1" in self.getProp("AddToBest"):
            merge_to_best.append(fwd_hlt1.getProp("OutputTracksName"))
        if "Fwd2" in self.getProp("AddToBest"):
            merge_to_best.append(fwd_hlt2.getProp("OutputTracksName"))
        if "OfflineFwd" in self.getProp("AddToBest"):
            merge_to_best.append(offline_fwd.getProp("OutputTracksName"))
        if "MergedFwd" in self.getProp("AddToBest"):
            merge_to_best.append(merge_fwd.getProp("OutputLocation"))
        if "GhostBustedFwd" in self.getProp("AddToBest"):
            merge_to_best.append(recycle_ghosts.getProp("outputLocation"))
        if "Match" in self.getProp("AddToBest"):
            merge_to_best.append(matching.getProp("MatchOutput"))
        if "Downstream" in self.getProp("AddToBest"):
            merge_to_best.append(downstream.getProp("OutputLocation"))
        
        clone_killer.TracksInContainers = merge_to_best
        clone_killer.TracksOutContainer = self.getProp("RootInTES") + "Track/UnfittedBest"
        clone_killer.SkipSameContainerTracks = False
        clone_killer.CopyTracks = True

        clone_killer.addTool(TrackCloneFinder, name='CloneFinderTool')
        clone_killer.CloneFinderTool.RestrictedSearch = True
        
        fst_seq.Members += [clone_killer]


        # Fitting
        fast_fit = TrackEventFitter("FstFastFit")
        # Fit all long tracks which were merged by the clone killer
        fast_fit.TracksInContainer = clone_killer.getProp("TracksOutContainer")
        fast_fit.TracksOutContainer = self.getProp("RootInTES") + "Track/FittedBest"
        fast_fit.addTool(TrackMasterFitter, name='Fitter')
        #fitter = ConfiguredHltFitter(getattr(fast_fit, 'Fitter'))
        fitter = ConfiguredMasterFitter(getattr(fast_fit, 'Fitter'),
                                        FieldOff=None,
                                        SimplifiedGeometry=False,
                                        LiteClusters=True,
                                        NoDriftTimes=False,
                                        KalmanSmoother=False,
                                        ApplyMaterialCorrections=True,
                                        )
        from Configurables import StateDetailedBetheBlochEnergyCorrectionTool
        state = StateDetailedBetheBlochEnergyCorrectionTool
        fitter.MaterialLocator.addTool(state("GeneralDedxTool"))
        fitter.MaterialLocator.GeneralDedxTool.EnergyLossFactor = 0.76

        fst_seq.Members += [fast_fit]
        
        
        # Remove ghosts by requiring chi2/ndof < 5, similar to TrackBestTrackCreator
        ghost_killer = TrackContainerCopy("FstGhostKiller")
        ghost_killer.inputLocation = fast_fit.getProp("TracksOutContainer")
        ghost_killer.outputLocation = self.getProp("RootInTES") + "Track/Best"
        max_chi2 = TrackSelector("MaxChi2Selector")
        max_chi2.MaxChi2Cut = 5
        ghost_killer.Selector = max_chi2
        
        fst_seq.Members += [ghost_killer]
