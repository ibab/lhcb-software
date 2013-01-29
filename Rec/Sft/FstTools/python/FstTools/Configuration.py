## @package FstTools
#  Test configuration of a First Software Trigger fo rthe LHCb upgrade
#  @author Olivier Callot
#  @date   09/10/2012

__version__ = "0.1"
__author__  = "Olivier Callot"

from Gaudi.Configuration import *
from Configurables import LHCbConfigurableUser
from Configurables import GaudiSequencer, FstSequencer, FstSelectVeloTracks, FstSelectForwardTracks

## @class FstConf
#  Configurable for the Fst simulation for the LHCb upgrade
#  @author Olivier Callot
class FstConf(LHCbConfigurableUser):
    __slots__ = {
        "RootInTES" : "Fst/"
        ,"MinPt"           : 1200.  # MeV
        ,"MinIP"           : 0.100  # mm
        ,"MaxIP"           : 3.000  # mm
        ,"MinIPChi2"       : 9.0
        ,"MaxChi2Ndf"      : 3.0    
        ,"MinVeloClusters" : 9
        ,"MinVPClusters"   : 3
        ,"VeloType"        : "Velo"
        ,"TTType"          : "ValidateTT"
        ,"TStationType"    : "IT+OT"
        ,"FastDecoding"    : True   # if false uses the old official decoding
        ,"Compare"         : False  # if using the old decoding, can run the new one and compare the containers
        ,"TrackFit"        : "HltFit"
        }

    def applyConf(self):
        GaudiSequencer( "RecoHLTSeq" ).Members = [ "FstSequencer/RecoFstSeq" ]
        FstSequencer( "RecoFstSeq" ).ForcePassOK = True
        ## Velo configuration
        if "Velo" == self.getProp( "VeloType" ):
            from Configurables import FastVeloTracking, FastVeloDecoding
            if self.getProp( "FastDecoding" ):
                FstSequencer( "RecoFstSeq" ).Members = [ "FastVeloDecoding"]
            else:
                FstSequencer( "RecoFstSeq" ).Members = [ "DecodeVeloRawBuffer/DecodeVeloClusters" ]
                if self.getProp( "Compare" ):
                    FstSequencer( "RecoFstSeq" ).Members += [ "FastVeloDecoding"]
                    FastVeloDecoding().CompareResult = True
                    FastVeloDecoding().OutputLocation = "Raw/VeloLite"
            FstSequencer( "RecoFstSeq" ).Members += [ "FastVeloTracking/FstVeloTracking" ]
            FastVeloTracking( "FstVeloTracking" ).OutputTracksName = self.getProp( "RootInTES") + "Track/Velo"
            FastVeloTracking( "FstVeloTracking" ).ResetUsedFlags = True
            FastVeloTracking( "FstVeloTracking" ).HLT1Only = True
        elif "VP" == self.getProp( "VeloType" ):
            from Configurables import PatPixelTracking
            FstSequencer( "RecoFstSeq" ).Members = [ "VPRawBankToLiteCluster/FstVPDecoding",
                                                     "PatPixelTracking/FstPixel" ]
            PatPixelTracking( "FstPixel" ).OutputTracksName = self.getProp( "RootInTES") + "Track/Velo"
        else:
            log.warning( "Unknown VeloType option '%s' !"%self.getProp( "VeloType" ) )
            exit(0)

        ## PV
        from Configurables import FastPVFinder
        FstSequencer( "RecoFstSeq" ).Members += [ "FastPVFinder/FstPV" ]
        FastPVFinder( "FstPV" ).InputLocation  = self.getProp( "RootInTES") + "Track/Velo"
        FastPVFinder( "FstPV" ).OutputLocation = self.getProp( "RootInTES") + "Vertices/PV"

        ## Selection of Velo tracks to extend
        if "ValidateTT" == self.getProp( "TTType" ):
            if self.getProp( "FastDecoding" ):
                from Configurables import FastSTDecoding
                FstSequencer( "RecoFstSeq" ).Members += [ "FastSTDecoding/FastTTDecoding" ]
                FastSTDecoding( "FastTTDecoding" ).DetectorName = "TT"
            else:
                FstSequencer( "RecoFstSeq" ).Members += [ "RawBankToSTLiteClusterAlg/CreateTTLiteClusters" ]
        FstSequencer( "RecoFstSeq" ).Members += [ "FstSelectVeloTracks" ]
        FstSelectVeloTracks().InputTracksName  = self.getProp( "RootInTES") + "Track/Velo"
        FstSelectVeloTracks().OutputTracksName = self.getProp( "RootInTES") + "Track/VeloFst"
        FstSelectVeloTracks().PVName           = self.getProp( "RootInTES") + "Vertices/PV"
        FstSelectVeloTracks().MinIP            = self.getProp( "MinIP" )
        FstSelectVeloTracks().MaxIP            = self.getProp( "MaxIP" )
        if "VP" == self.getProp( "VeloType" ):
            FstSelectVeloTracks().MinVeloClusters  = self.getProp( "MinVPClusters" )
        else:
            FstSelectVeloTracks().MinVeloClusters  = self.getProp( "MinVeloClusters" )
        
        ## Handle TT/UT
        if "ValidateTT" == self.getProp( "TTType" ):
            FstSelectVeloTracks().ValidateWithTT = True
        elif "none" == self.getProp( "TTType" ):
            FstSelectVeloTracks().ValidateWithTT = False
        else:
            log.warning( "Unknown TTType option '%s' !"%self.getProp( "TTType" ))
            exit(0)

        ## Forward tracking on selected tracks
        if "IT+OT" == self.getProp( "TStationType" ):
            from Configurables import PatForward, PatForwardTool, FastSTDecoding
            if self.getProp( "FastDecoding" ):
                FstSequencer( "RecoFstSeq" ).Members += [ "FastSTDecoding/FastTTDecoding",
                                                          "FastSTDecoding/FastITDecoding"]
                FastSTDecoding( "FastTTDecoding" ).DetectorName = "TT"
                FastSTDecoding( "FastITDecoding" ).DetectorName = "IT"
            else:
                FstSequencer( "RecoFstSeq" ).Members += [ "RawBankToSTLiteClusterAlg/CreateTTLiteClusters",
                                                          "RawBankToSTLiteClusterAlg/CreateITLiteClusters" ]
                if self.getProp( "Compare" ):
                    FstSequencer( "RecoFstSeq" ).Members += [ "FastSTDecoding/FastTTDecoding",
                                                              "FastSTDecoding/FastITDecoding"]
                    if self.getProp( "Compare" ):
                        FastSTDecoding( "FastTTDecoding" ).DetectorName = "TT"
                        FastSTDecoding( "FastTTDecoding" ).CompareResult = True
                        FastSTDecoding( "FastITDecoding" ).DetectorName = "IT"
                        FastSTDecoding( "FastITDecoding" ).CompareResult = True
            FstSequencer( "RecoFstSeq" ).Members += [ "PatForward/FstForward" ]

            PatForward("FstForward").InputTracksName  = self.getProp( "RootInTES") + "Track/VeloFst"
            PatForward("FstForward").OutputTracksName = self.getProp( "RootInTES") + "Track/Forward"
            PatForward("FstForward").addTool( PatForwardTool )
            PatForward("FstForward").PatForwardTool.MinPt = self.getProp( "MinPt" )
        elif "FT" == self.getProp( "TStationType" ):
            from Configurables import PrForwardTracking, PrForwardTool
            FstSequencer( "RecoFstSeq" ).Members += [ "FTRawBankDecoder",
                                                      "PrForwardTracking/FstForward" ]
            PrForwardTracking( "FstForward" ).InputName  = self.getProp( "RootInTES") + "Track/VeloFst"
            PrForwardTracking( "FstForward" ).OutputName = self.getProp( "RootInTES") + "Track/Forward"
            PrForwardTracking( "FstForward" ).addTool( PrForwardTool )
            PrForwardTracking( "FstForward" ).PrForwardTool.MinPt = self.getProp( "MinPt" )
        else:
            log.warning( "Unknown TStationType option '%s' !"%self.getProp( "TStationType" ) )
            exit(0)

        if "HltFit" == self.getProp ( "TrackFit" ):
            from Configurables import TrackEventFitter, TrackMasterFitter, MeasurementProvider
            HltFastFit_name    = 'FastFit'
            HltFastFit           = TrackEventFitter(HltFastFit_name)
            HltFastFit.TracksInContainer    = self.getProp( "RootInTES") + "Track/Forward" 
            HltFastFit.addTool(TrackMasterFitter, name = 'Fitter')
            from TrackFitter.ConfiguredFitters import ConfiguredHltFitter
            fitter = ConfiguredHltFitter( getattr(HltFastFit,'Fitter'))
            FstSequencer( "RecoFstSeq" ).Members +=[ HltFastFit ]
            HltFastFit.Fitter.addTool( MeasurementProvider )
            if "FT" == self.getProp( "TStationType" ):                       #ignore IT+OT as it cannot initialize
                HltFastFit.Fitter.MeasurementProvider.IgnoreIT = True
                HltFastFit.Fitter.MeasurementProvider.IgnoreOT = True
                HltFastFit.Fitter.MeasurementProvider.IgnoreFT = False
            if "VP" == self.getProp( "VeloType" ):
                HltFastFit.Fitter.MeasurementProvider.IgnoreVelo = True
                HltFastFit.Fitter.MeasurementProvider.IgnoreVP   = False
            if "ValidateTT" != self.getProp( "TTType" ):
                HltFastFit.Fitter.MeasurementProvider.IgnoreTT = True

        ## Selection after measuring momentum
        FstSequencer( "RecoFstSeq" ).Members += [ "FstSelectForwardTracks" ]
        FstSelectForwardTracks().InputTracksName  = self.getProp( "RootInTES") + "Track/Forward"
        FstSelectForwardTracks().OutputTracksName = self.getProp( "RootInTES") + "Track/ForwardFst"
        FstSelectForwardTracks().PVName           = self.getProp( "RootInTES") + "Vertices/PV"
        FstSelectForwardTracks().MinIP            = self.getProp( "MinIP" )
        FstSelectForwardTracks().MaxIP            = self.getProp( "MaxIP" )
        FstSelectForwardTracks().MinPt            = self.getProp( "MinPt" )
        FstSelectForwardTracks().MinIPChi2        = self.getProp( "MinIPChi2" )
        if "HltFit" == self.getProp ( "TrackFit" ):
            FstSelectForwardTracks().MaxChi2Ndf            = self.getProp( "MaxChi2Ndf" )
        else:
            FstSelectForwardTracks().MaxChi2Ndf            = -1.

