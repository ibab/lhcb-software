## @package FstTools
#  Test configuration of a First Software Trigger fo rthe LHCb upgrade
#  @author Olivier Callot
#  @date   09/10/2012

__version__ = "0.1"
__author__  = "Olivier Callot"

from Gaudi.Configuration import *
from Configurables import LHCbConfigurableUser
from Configurables import GaudiSequencer, FstSequencer, FstSelectVeloTracks
from Configurables import FstSelectForwardTracksPartOne, FstSelectForwardTracksPartTwo

## @class FstConf
#  Configurable for the Fst simulation for the LHCb upgrade
#  @author Olivier Callot
class FstConf(LHCbConfigurableUser):
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
        ,"TStationHits"    : 16
        ,"ForwardMinPt"    : 1250.  # MeV, determines window size in forward tracking
                                    # or rejection cut in forward tracking if momentum
                                    # estimate is used
        ,"FastDecoding"    : True   # if false uses the old official decoding
        ,"Compare"         : False  # if using the old decoding, can run the new one and compare the containers
        ,"TrackFit"        : "HltFit"
        }

    def applyConf(self):
        GaudiSequencer( "RecoHLTSeq" ).Members = [ "FstSequencer/RecoFstSeq" ]
        FstSequencer( "RecoFstSeq" ).ForcePassOK = True

        ## Multiplicity cuts (Kevin Dungs, 2014-01-18)
        from Configurables import LLTCaloAlg
        FstSequencer("RecoFstSeq").Members = ["LLTCaloAlg", "FstSelectGEC"]

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
            from Configurables import PatPixelTracking, VPRawBankToLiteCluster
            FstSequencer("RecoFstSeq").Members += ["VPRawBankToLiteCluster/FstVPDecoding",
                                                   "PatPixelTracking/FstPixel"]
            # Centrally produced upgrade samples need this fix
            # see https://twiki.cern.ch/twiki/bin/viewauth/LHCbPhysics/UpgradeTrackingSequence
            VPRawBankToLiteCluster("FstVPDecoding").RawEventLocation = "/Event/Other/RawEvent"
            PatPixelTracking("FstPixel").OutputTracksName = self.getProp("RootInTES") + "Track/Velo"
            # Use Kalman fit to estimate track state at end of the Velo
            # this is the state used by the UT tracking
            PatPixelTracking("FstPixel").EndVeloStateKalmanFit = True
            PatPixelTracking("FstPixel").AddFirstLastMeasurementStatesKalmanFit = True
            
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
            from Configurables import PrForwardTracking, PrForwardTool
            from Configurables import PrVeloUT
            from Configurables import TrackMasterFitter
            from Configurables import RawBankToSTLiteClusterAlg
            from Configurables import PrVeloUTTool

            prVeloUT = PrVeloUT()
            prVeloUT.InputTracksName = self.getProp("RootInTES") + "Track/VeloFst"
            prVeloUT.OutputTracksName = self.getProp("RootInTES") + "Track/VeloUTFst"
            prVeloUT.TimingMeasurement = False
            prVeloUT.removeUsedTracks = False
            prVeloUT.InputUsedTracksNames = []
            prVeloUT.fitTracks = False
            prVeloUT.maxChi2 = 1280.
            prVeloUT.AddMomentumEstimate = True

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

            PrVeloUTTool("PrVeloUTTool").DxGroupFactor = 0.25
            PrVeloUTTool("PrVeloUTTool").maxPseudoChi2 = 1280.
            PrVeloUTTool("PrVeloUTTool").minMomentum = 2000.
            PrVeloUTTool("PrVeloUTTool").minPT = 200.
            PrVeloUTTool("PrVeloUTTool").PrintVariables = True
            
            createUTLiteClusters = RawBankToSTLiteClusterAlg("CreateUTLiteClusters")
            createUTLiteClusters.DetType = "UT"
            #createUTLiteClusters.clusterLocation = "Raw/UT/LiteClusters"
            
            FstSequencer("RecoFstSeq").Members += [#"UnpackMCParticle", "UnpackMCVertex",
                                                   createUTLiteClusters,
                                                   PrVeloUT("PrVeloUT"),
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
            print "XXX HltFit"
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

