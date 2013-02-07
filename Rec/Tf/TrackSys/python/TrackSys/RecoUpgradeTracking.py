from os import environ
from Gaudi.Configuration import *
import GaudiKernel.ProcessJobOptions
from TrackSys.Configuration import *
from GaudiKernel.SystemOfUnits import mm


def RecoUpgradeTracking(exclude=[]):
    ## Start TransportSvc, needed by track fit  ???
    ApplicationMgr().ExtSvc.append("TransportSvc")
    subDets = []
    from Configurables import LHCbApp
    #Test if LHCbApp has this method (same revision as property)
    if hasattr(LHCbApp(),"Detectors"):
        if LHCbApp().isPropertySet("Detectors"):
            subDets = LHCbApp().upgradeDetectors()
    trackTypes = TrackSys().getProp("TrackTypes")

    ### Sanity checks
    if "Forward" in trackTypes:
        if not ("Velo" in trackTypes):
            log.warning("Velo tracks added to tracking sequence.")
            trackTypes += ["Velo"]
        if not (("FT" in subDets)):
            raise RuntimeError("Specify T-Stations.")


    if "Upstream" in trackTypes:
        if not ("Velo" in trackTypes):
            log.warning("Velo tracks added to tracking sequence.")
            trackTypes += ["Velo"]
        if not (("UT" in subDets)):
            raise RuntimeError("Specify UT.")

    if "Downstream" in trackTypes:
        if not ("Seeding" in trackTypes):
            log.warning("Seed tracks added to tracking sequence.")
            trackTypes += ["Seeding"]
        if not (("UT" in subDets)):
            raise RuntimeError("Specify UT.")

    if "Velo" in trackTypes:
        if not (("VL" in subDets) or ("VP" in subDets)):
            raise RuntimeError("Specify a Velo. VL or VP.")
    
    if "Seeding" in trackTypes:
        if not ("FT" in subDets) :
            raise RuntimeError("Specify T-Stations.")



    ### Do the decoding of the detectors
   


    decodingSeq = GaudiSequencer("RecoDecodingSeq")

    # Are these the right decoders?
    if "VL" in subDets:
        from Configurables import VLRawBankDecoder
        VLRawBankDecoder().DecodeToClusters = True
        VLRawBankDecoder().DecodeToLiteClusters = True
        decodingSeq.Members += [ VLRawBankDecoder() ]
    if "VP" in subDets:
        from Configurables import VPRawBankToLiteCluster,VPRawBankToPartialCluster
        decodingSeq.Members += [ VPRawBankToLiteCluster() ]
    if "UT" in subDets:
        from Configurables import RawBankToSTClusterAlg, RawBankToSTLiteClusterAlg
        createUTClusters = RawBankToSTClusterAlg("CreateUTClusters")
        createUTLiteClusters = RawBankToSTLiteClusterAlg("CreateUTLiteClusters")
        createUTClusters.DetType     = "UT"
        createUTLiteClusters.DetType = "UT"
        decodingSeq.Members += [ createUTClusters, createUTLiteClusters ]
        from Configurables import STOfflinePosition
        UT = STOfflinePosition('ToolSvc.UTClusterPosition')
        UT.DetType = "UT"
    if "FT" in subDets:
        from Configurables import FTRawBankDecoder
        decodingSeq.Members += [ FTRawBankDecoder() ]

    ### Define the pattern recognition
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
    if "Seeding" in trackTypes:
        seedingSeq = GaudiSequencer("TrSeedingSeq")
        GaudiSequencer("RecoTrSeq").Members += [ seedingSeq ]
        if "FT" in subDets:
            from Configurables import PrSeedingAlgorithm
            seedingSeq.Members += [ PrSeedingAlgorithm() ]

    if "Upstream" in trackTypes:
        upSeq = GaudiSequencer("TrUpSeq")
        GaudiSequencer("RecoTrSeq").Members += [ upSeq ]
        if "UT" in subDets:
            from Configurables import PrVeloUT
            prVeloUT = PrVeloUT()
            from Configurables import TrackMasterFitter
            prVeloUT.addTool(TrackMasterFitter,"Fitter")
            prVeloUT.Fitter.MeasProvider.IgnoreVelo = True
            prVeloUT.Fitter.MeasProvider.IgnoreVP = True
            prVeloUT.Fitter.MeasProvider.IgnoreVL = True
            prVeloUT.Fitter.MeasProvider.IgnoreTT = True
            prVeloUT.Fitter.MeasProvider.IgnoreIT = True
            prVeloUT.Fitter.MeasProvider.IgnoreOT = True
            
            prVeloUT.Fitter.MeasProvider.IgnoreUT = False
            if ("VP" in subDets):
                prVeloUT.Fitter.MeasProvider.IgnoreVP = False
            if ("VL" in subDets):
                prVeloUT.Fitter.MeasProvider.IgnoreVL = False
            upSeq.Members += [ PrVeloUT() ]
            

    if "Downstream" in trackTypes:
        downSeq = GaudiSequencer("TrDownSeq")
        GaudiSequencer("RecoTrSeq").Members += [ downSeq ]
        if "UT" in subDets:
            from Configurables import PrDownstream
            downSeq.Members += [ PrDownstream() ]


    # Do the Clone Killing and create Best tracks container
    bestSeq = GaudiSequencer("TrBestSeq")
    GaudiSequencer("RecoTrSeq").Members += [ bestSeq ]
  
    #Fill list of Tracks to write out
    tracklists = []
    if "Velo" in trackTypes:
        tracklists += ["Rec/Track/Velo"]
    if "Seeding" in trackTypes:
        tracklists += ["Rec/Track/Seed"]
    if "Forward" in trackTypes:
        tracklists += ["Rec/Track/Forward"]
    if "Upstream" in trackTypes:
        tracklists += ["Rec/Track/VeloTT"]
    if "Downstream" in trackTypes:
        tracklists += ["Rec/Track/Downstream"]


    # create the best track creator
    from Configurables import UpgradeBestTrackCreator
    bestTrackCreator = UpgradeBestTrackCreator( TracksInContainers = tracklists )
    
    bestSeq.Members += [ bestTrackCreator ]


    ### Do the track fit
    fitSeq = GaudiSequencer("TrFitSeq")
    GaudiSequencer("RecoTrSeq").Members += [ fitSeq ]
    from Configurables import TrackEventFitter, TrackMasterFitter, MeasurementProvider
    from TrackFitter.ConfiguredFitters import ConfiguredEventFitter, ConfiguredMasterFitter
    eventfitter = TrackEventFitter("FitBest")
    eventfitter.TracksInContainer = "Rec/Track/Best"
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
    eventfitter.Fitter.MeasProvider.IgnoreVP = True
    eventfitter.Fitter.MeasProvider.IgnoreVL = True
    eventfitter.Fitter.MeasProvider.IgnoreTT = True
    eventfitter.Fitter.MeasProvider.IgnoreUT = True
    eventfitter.Fitter.MeasProvider.IgnoreIT = True
    eventfitter.Fitter.MeasProvider.IgnoreOT = True
    eventfitter.Fitter.MeasProvider.IgnoreFT = True
    
    
    if ("VP" in subDets):
        eventfitter.Fitter.MeasProvider.IgnoreVP = False
    if ("VL" in subDets):
        eventfitter.Fitter.MeasProvider.IgnoreVL = False
    if ("FT" in subDets):
        eventfitter.Fitter.MeasProvider.IgnoreFT = False
    if ("UT" in subDets):
        eventfitter.Fitter.MeasProvider.IgnoreUT = False
        

    fitSeq.Members += [ eventfitter ]


