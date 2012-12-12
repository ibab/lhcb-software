from os import environ
from Gaudi.Configuration import *
import GaudiKernel.ProcessJobOptions
from TrackSys.Configuration import *
from GaudiKernel.SystemOfUnits import mm


def RecoUpgradeTracking(exclude=[]):
    print "Yes we run the upgrade sequence"
    ## Start TransportSvc, needed by track fit  ???
    ApplicationMgr().ExtSvc.append("TransportSvc")

    subDets = TrackSys().getProp("UpgradeDetectors")
    trackTypes = TrackSys().getProp("TrackTypes")


    ### Sanity checks
    if ("VL" in subDets) and ("VP" in subDets):
        raise RuntimeError("There is no place for two Velos, choose one.")
    if ("FT" in subDets) and ("IT+OT" in subDets):
        raise RuntimeError("Currently either FT or IT+OT are supported.")

    if "Forward" in trackTypes:
        if not ("Velo" in trackTypes):
            log.warning("Velo tracks added to tracking sequence.")
            trackTypes += ["Velo"]
        if not (("FT" in subDets) or ("IT+OT" in subDets)):
            raise RuntimeError("Specify T-Stations. FT or IT+OT.")
            
    if "Velo" in trackTypes:
        if not (("VL" in subDets) or ("VP" in subDets)):
            raise RuntimeError("Specify a Velo. VL or VP.")
    
    if "Seeding" in trackTypes:
        if not (("FT" in subDets) or ("IT+OT" in subDets)):
            raise RuntimeError("Specify T-Stations. FT or IT+OT.")

    ### Do the decoding of the detectors
   
    print subDets

    decodingSeq = GaudiSequencer("RecoDecodingSeq")

    # Are these the right decoders?
    if "VL" in subDets:
        from Configurables import VLRawBankDecoder
        VLRawBankDecoder().DecodeToClusters = True
        decodingSeq.Members += [ VLRawBankDecoder() ]
    if "VP" in subDets:
        from Configurables import VPRawBankToLiteCluster,VPRawBankToPartialCluster
        decodingSeq.Members += [ VPRawBankToLiteCluster() ]
        #decodingSeq.Members += [ VPRawBankToPartialCluster() ]
    if "UT" in subDets:
        print "UT not yet implemented"
    if "FT" in subDets:
        from Configurables import FTRawBankDecoder
        decodingSeq.Members += [ FTRawBankDecoder() ]
    if "IT+OT" in subDets:
        print "IT+OT, Not yet implemented"
        from Configurables import RawBankToSTClusterAlg, RawBankToSTLiteClusterAlg
        createITClusters = RawBankToSTClusterAlg("CreateITClusters")
        createITLiteClusters = RawBankToSTLiteClusterAlg("CreateITLiteClusters")
        createITClusters.DetType     = "IT"
        createITLiteClusters.DetType = "IT"
        decodingSeq.Members += [ createITClusters, createITLiteClusters ]
 

    ### Define the pattern recognition

    print trackTypes
    if "Velo" in trackTypes:
        veloSeq = GaudiSequencer("TrVeloSeq")
        GaudiSequencer("RecoTrSeq").Members += [ veloSeq ]
        if "VL" in subDets:
            from Configurables import PrVLTracking
            veloSeq.Members += [ PrVLTracking() ]
        if "VP" in subDets:
            from Configurables import  PatPixelTracking
            veloSeq.Members += [ PatPixelTracking() ]


    if "Forward" in trackTypes:
        forwardSeq = GaudiSequencer("TrForwardSeq")
        GaudiSequencer("RecoTrSeq").Members += [ forwardSeq ]
        if "FT" in subDets:
            from Configurables import PrForwardTracking
            forwardSeq.Members += [ PrForwardTracking() ]
        if "IT+OT" in subDets:
            from Configurables import PatForward
            forwardSeq.Members += [ PatForward() ]
    if "Seeding" in trackTypes:
        seedingSeq = GaudiSequencer("TrSeedingSeq")
        GaudiSequencer("RecoTrSeq").Members += [ seedingSeq ]
        if "FT" in subDets:
            from Configurables import PrSeedingAlgorithm
            seedingSeq.Members += [ PrSeedingAlgorithm() ]
        if "IT+OT" in subDets:
            from Configurables import PatSeeding
            seedingSeq.Members += [ PatSeeding() ]


    ### Do the track fit and Clone killing
    ### For now Dummy fitter for Long tracks, needs to be checked by experts
    if "Forward" in trackTypes:
        fitSeq = GaudiSequencer("TrFitSeq")
        GaudiSequencer("RecoTrSeq").Members += [ fitSeq ]
        from Configurables import TrackEventFitter, TrackMasterFitter, MeasurementProvider
        from TrackFitter.ConfiguredFitters import ConfiguredEventFitter, ConfiguredMasterFitter
        eventfitter = TrackEventFitter("FitForward")
        eventfitter.TracksInContainer = "Rec/Track/Forward"
        eventfitter.addTool( TrackMasterFitter, name="Fitter")
        ConfiguredMasterFitter(eventfitter.Fitter,
                               FieldOff = False,
                               SimplifiedGeometry = False,
                               NoDriftTimes       = None,
                               KalmanSmoother     = None,
                               LiteClusters = True,
                               ApplyMaterialCorrections = None,
                               StateAtBeamLine = True,
                               MaxNumberOutliers = 2)
        eventfitter.Fitter.MeasProvider.IgnoreVelo = True
        eventfitter.Fitter.MeasProvider.IgnoreIT = True
        eventfitter.Fitter.MeasProvider.IgnoreTT = True
        eventfitter.Fitter.MeasProvider.IgnoreOT = True
        eventfitter.Fitter.MeasProvider.IgnoreVP = True
        eventfitter.Fitter.MeasProvider.IgnoreVL = True
        eventfitter.Fitter.MeasProvider.IgnoreFT = True

                
        if ("VP" in subDets):
            eventfitter.Fitter.MeasProvider.IgnoreVP = False
        if ("VL" in subDets):
            eventfitter.Fitter.MeasProvider.IgnoreVL = False
        if ("FT" in subDets):
            eventfitter.Fitter.MeasProvider.IgnoreFT = False

        ### other configurations crash    
        if ( ("VP" in subDets) and ("FT" in subDets)):
            fitSeq.Members += [ eventfitter ]
        else:
            log.warning("Fitter crashes in this configuration")
