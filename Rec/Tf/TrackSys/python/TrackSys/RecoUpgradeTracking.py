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
        if not (("VP" in subDets)):
            raise RuntimeError("Specify VP.")
    
    if "Seeding" in trackTypes:
        if not ("FT" in subDets) :
            raise RuntimeError("Specify T-Stations.")

    if "Match" in trackTypes:
        if not ("Velo" in trackTypes):
            log.warning("Velo tracks added to tracking sequence.")
            trackTypes += ["Velo"]

        if not ("Seeding" in trackTypes):
            log.warning("Seed tracks added to tracking sequence.")
            trackTypes += ["Seeding"]

    ### Do the decoding of the detectors
   


    decodingSeq = GaudiSequencer("RecoDecodingSeq")
    from DAQSys.Decoders import DecoderDB
    from DAQSys.DecoderClass import decodersForBank
    decs=[]
    # Are these the right decoders?
    if "VP" in subDets:
        decs=decs+decodersForBank(DecoderDB,"VP")
    if "UT" in subDets:
        decs=decs+decodersForBank(DecoderDB,"UT")
        from Configurables import STOfflinePosition
        UT = STOfflinePosition('ToolSvc.UTClusterPosition')
        UT.DetType = "UT"
    if "FT" in subDets:
        decs=decs+decodersForBank(DecoderDB,"FTCluster")
        
    decodingSeq.Members+=[d.setup() for d in decs]
    ### Define the pattern recognition
    if "Velo" in trackTypes:
        veloSeq = GaudiSequencer("TrVeloSeq")
        GaudiSequencer("RecoTrSeq").Members += [ veloSeq ]
        if "VP" in subDets:
            from Configurables import PrPixelTracking, PrPixelStoreClusters
            veloSeq.Members += [ PrPixelTracking(), PrPixelStoreClusters() ]

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
            prVeloUT.Fitter.MeasProvider.IgnoreTT = True
            prVeloUT.Fitter.MeasProvider.IgnoreIT = True
            prVeloUT.Fitter.MeasProvider.IgnoreOT = True
            prVeloUT.Fitter.MeasProvider.IgnoreUT = False
            if ("VP" in subDets):
                prVeloUT.Fitter.MeasProvider.IgnoreVP = False
            upSeq.Members += [ PrVeloUT() ]

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
            #seedingSeq.Members += [ PrSeedingAlgorithm() ]
            from Configurables import PrSeedingXLayers
            seedingSeq.Members += [PrSeedingXLayers()]
   
            

    if "Match" in trackTypes:
        matchSeq = GaudiSequencer("TrMatchSeq")
        GaudiSequencer("RecoTrSeq").Members += [ matchSeq ]
        from Configurables import PrMatch
        #from Configurables import PatMatch, PatMatchTool, PrAddUTCoord
        matchSeq.Members += [ PrMatch() ]
        #PatMatch().addTool(PatMatchTool, "PatMatchTool")
        #PatMatch().PatMatchTool.AddTTClusters = True
        #PatMatch().PatMatchTool.AddTTClusterName = "PrAddUTCoord"

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
    if "Match" in trackTypes:
        tracklists += ["Rec/Track/Match"]

    from Configurables import TrackBestTrackCreator, TrackMasterFitter
    from TrackFitter.ConfiguredFitters import ConfiguredMasterFitter
    bestTrackCreator = TrackBestTrackCreator( TracksInContainers = tracklists )
    bestTrackCreator.addTool( TrackMasterFitter, name="Fitter") 
    bestTrackCreator.FitTracks = True
    bestTrackCreator.InitTrackStates = False
    ConfiguredMasterFitter( bestTrackCreator.Fitter )
    bestSeq.Members += [ bestTrackCreator ]


    ## Extra track information sequence
    extraInfos = TrackSys().getProp("TrackExtraInfoAlgorithms")
    if len(extraInfos) > 0 :

        ## ghost probability using a Neural Net
        if "GhostProbability" in extraInfos :
            from Configurables import TrackAddNNGhostId, TrackNNGhostId
            ghostID = TrackAddNNGhostId()
            ghostID.GhostIdTool = "UpgradeGhostId"
            GaudiSequencer("TrackAddExtraInfoSeq").Members += [ ghostID ]
            

        addExtraInfoSeq = GaudiSequencer("TrackAddExtraInfoSeq")
        GaudiSequencer("RecoTrSeq").Members += [ addExtraInfoSeq ]
