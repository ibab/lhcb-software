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
        ,"MinPt"           : 1500.  # MeV
        ,"MinIP"           : 0.100  # mm
        ,"MaxIP"           : 3.000  # mm
        ,"MaxChi2Ndf"      : 3.0  # mm
        ,"MinVeloClusters" : 9
        ,"VeloType"        : "Velo"
        ,"TTType"          : "ValidateTT"
        ,"TStationType"    : "IT+OT"
        ,"FastDecoding"    : True
        ,"TrackFit"        : "HltFit"
        }

    def applyConf(self):
        GaudiSequencer( "RecoHLTSeq" ).Members = [ "FstSequencer/RecoFstSeq" ]
        FstSequencer( "RecoFstSeq" ).ForcePassOK = True
        ## Velo configuration
        if "Velo" == self.getProp( "VeloType" ):
            from Configurables import FastVeloTracking
            if self.getProp( "FastDecoding" ):
                FstSequencer( "RecoFstSeq" ).Members = [ "FastVeloDecoding"]
            else:
                FstSequencer( "RecoFstSeq" ).Members = [ "DecodeVeloRawBuffer/DecodeVeloClusters" ]
            FstSequencer( "RecoFstSeq" ).Members += [ "FastVeloTracking/FstVeloTracking" ]
            FastVeloTracking( "FstVeloTracking" ).OutputTracksName = self.getProp( "RootInTES") + "Track/Velo"
            FastVeloTracking( "FstVeloTracking" ).ResetUsedFlags = True
            FastVeloTracking( "FstVeloTracking" ).HLT1Only = True
        elif "VP" == self.getProp( "VeloType" ):
            FstSequencer( "RecoFstSeq" ).Members = [ "PatPixel/FstPixel" ]
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
            FstSequencer( "RecoFstSeq" ).Members += [ "RawBankToSTLiteClusterAlg/CreateTTLiteClusters" ]
        FstSequencer( "RecoFstSeq" ).Members += [ "FstSelectVeloTracks" ]
        FstSelectVeloTracks().InputTracksName  = self.getProp( "RootInTES") + "Track/Velo"
        FstSelectVeloTracks().OutputTracksName = self.getProp( "RootInTES") + "Track/VeloFst"
        FstSelectVeloTracks().PVName           = self.getProp( "RootInTES") + "Vertices/PV"
        FstSelectVeloTracks().MinIP            = self.getProp( "MinIP" )
        FstSelectVeloTracks().MaxIP            = self.getProp( "MaxIP" )
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
            from Configurables import PatForward, PatForwardTool
            FstSequencer( "RecoFstSeq" ).Members += [ "RawBankToSTLiteClusterAlg/CreateTTLiteClusters",
                                                      "RawBankToSTLiteClusterAlg/CreateITLiteClusters",
                                                      "PatForward/FstForward" ]
            
            PatForward("FstForward").InputTracksName  = self.getProp( "RootInTES") + "Track/VeloFst"
            PatForward("FstForward").OutputTracksName = self.getProp( "RootInTES") + "Track/Forward"
            PatForward("FstForward").addTool( PatForwardTool )
            PatForward("FstForward").PatForwardTool.MinPt = self.getProp( "MinPt" )
        elif "FT" == self.getProp( "TStationType" ):
            from Configurables import PrForwardTracking
            FstSequencer( "RecoFstSeq" ).Members += [ "FTRawBankDecoder",
                                                      "PrForwardTracking/FstForward" ]
            PrForwardTracking( "FstForward" ).InputName  = self.getProp( "RootInTES") + "Track/VeloFst"
            PrForwardTracking( "FstForward" ).OutputName = self.getProp( "RootInTES") + "Track/Forward"
        else:
            log.warning( "Unknown TStationType option '%s' !"%self.getProp( "TStationType" ) )
            exit(0)

        if "HltFit" == self.getProp ( "TrackFit" ):
            from Configurables import TrackEventFitter, TrackMasterFitter
            HltFastFit_name    = 'FastFit'
            HltFastFit           = TrackEventFitter(HltFastFit_name)
            HltFastFit.TracksInContainer    = self.getProp( "RootInTES") + "Track/Forward" 
            #HltFastFit.TracksOutContainer    = self.getProp( "RootInTES") + "Track/FittedForward"  
            HltFastFit.addTool(TrackMasterFitter, name = 'Fitter')
            from TrackFitter.ConfiguredFitters import ConfiguredHltFitter
            fitter = ConfiguredHltFitter( getattr(HltFastFit,'Fitter'))
            FstSequencer( "RecoFstSeq" ).Members +=[ HltFastFit ]

        ## Selection after measuring momentum
        FstSequencer( "RecoFstSeq" ).Members += [ "FstSelectForwardTracks" ]
        FstSelectForwardTracks().InputTracksName  = self.getProp( "RootInTES") + "Track/Forward"
        FstSelectForwardTracks().OutputTracksName = self.getProp( "RootInTES") + "Track/ForwardFst"
        FstSelectForwardTracks().PVName           = self.getProp( "RootInTES") + "Vertices/PV"
        FstSelectForwardTracks().MinIP            = self.getProp( "MinIP" )
        FstSelectForwardTracks().MaxIP            = self.getProp( "MaxIP" )
        FstSelectForwardTracks().MinPt            = self.getProp( "MinPt" )
        if "" == self.getProp ( "TrackFit" ):
            FstSelectForwardTracks().MaxChi2Ndf            = -1.
        else:
            FstSelectForwardTracks().MaxChi2Ndf            = self.getProp( "MaxChi2Ndf" )

