"""
 Adds packers to afterburner and writes them out.

 @author SB
 @date 26-10-2015
"""
# =============================================================================
__author__  = "Sean Benson"
# =============================================================================
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import GaudiSequencer as Sequence

class HltPersistRecoConf(LHCbConfigurableUser):
    __used_configurables__ = []

    __slots__ = {"WriterName"              : "Writer",
                 "PreFilter"               : "HLT_TURBOPASS_RE('Hlt2(?!Forward)(?!DebugEvent)(?!Lumi)(?!Transparent)(?!PassThrough).*Decision')",
                 "Sequence"                : None,
                 "CaloHypoPrefix"          : "Hlt2/PID/CALO/Calo"
                }

    def __apply_configuration__(self):
        """
        HLT Persist reco. configuration
        """

        if self.getProp("Sequence") is None:
            return

        # First make sure we only do this is the Turbo lines fired
        from Configurables import GaudiSequencer
        persistReco = self.getProp("Sequence")
        from DAQSys.Decoders import DecoderDB
        decoder = DecoderDB["HltDecReportsDecoder/Hlt2DecReportsDecoder"]
        from Configurables import LoKi__HDRFilter
        hlt2Filter = LoKi__HDRFilter('HltPersistRecoHlt2Filter', Code = self.getProp("PreFilter"),
                                Location = decoder.listOutputs()[0])
        persistReco.Members += [hlt2Filter]

        # Setup packers and add the output locations to the writer
        algs = self.Packers()
        
        # THE LINES BELOW ONLY WORK VIA THRESHOLDS, NOT WITH A TCK
        # CHANGES TO THE RECONSTRUCTION LOCATIONS WILL REQUIRE CORRESPONDING
        # CHANGES TO THE MOORE CONFIGURATION
        #writer = InputCopyStream(self.getProp("WriterName"))
        #for alg in algs:
        #    writer.OptItemList+=[alg.OutputName+'#99']

        # The below is useful when you want to update the locations saved
        #print "Writing additional output locations:"
        #print outputs

        # Add packer algorithms to the afterburner
        persistReco.Members+= algs
        Afterburner = GaudiSequencer("HltAfterburner")
        Afterburner.Members+=[persistReco]

    def Packers(self) :
        algs = [ ]

        debugLevel=4

        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedDownstreamTracking
        tracks = Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks()
        tracksDown = Hlt2BiKalmanFittedDownstreamTracking().hlt2PrepareTracks()
        from HltTracking.HltTrackNames import Hlt2NeutralProtoParticleSuffix
        from HltTracking.HltTrackNames import Hlt2ChargedProtoParticleSuffix
        
        # ProtoParticles
        from Configurables import PackProtoParticle
        algs += [ PackProtoParticle( name = "Hlt2PackChargedProtos",
                                     AlwaysCreateOutput = True,
                                     DeleteInput        = False,
                                     OutputLevel        = debugLevel,
                                     InputName          = Hlt2BiKalmanFittedForwardTracking().hlt2ChargedAllPIDsProtos().outputSelection(),
                                     OutputName         = "Hlt2/pRec/long/Protos" ),
                  PackProtoParticle( name = "Hlt2PackChargedDownProtos",
                                     AlwaysCreateOutput = True,
                                     DeleteInput        = False,
                                     OutputLevel        = debugLevel,
                                     InputName          = Hlt2BiKalmanFittedDownstreamTracking().hlt2ChargedNoPIDsProtos().outputSelection(),
                                     OutputName         = "Hlt2/pRec/down/Protos" )
                  ]

        # RICH PIDs
        from Configurables import DataPacking__Pack_LHCb__RichPIDPacker_ as PackRichPIDs
        algs += [ 
                PackRichPIDs( name = "Hlt2PackRichPIDs",
                                AlwaysCreateOutput = True,
                                DeleteInput        = False,
                                OutputLevel        = debugLevel,
                                InputName          = Hlt2BiKalmanFittedForwardTracking().hlt2RICHID().outputSelection(),
                                OutputName         = "Hlt2/pRec/long/RichPIDs" )
                  ]
        
        # MUON PIDs 
        from Configurables import DataPacking__Pack_LHCb__MuonPIDPacker_ as PackMuonPIDs
        algs += [ PackMuonPIDs( name = "Hlt2PackMuonPIDs",
                                AlwaysCreateOutput = True,
                                DeleteInput        = False,
                                OutputLevel        = debugLevel,
                                InputName          = Hlt2BiKalmanFittedForwardTracking().hlt2MuonID().outputSelection(),
                                OutputName         = "Hlt2/pRec/long/MuonIDs" )
                  ]

        # Tracks
        from Configurables import PackTrack
        algs += [ PackTrack( name = "PackLongTracks",
                             AlwaysCreateOutput = True,
                             DeleteInput        = False,
                             OutputLevel        = debugLevel,
                             InputName          = tracks.outputSelection(),
                             OutputName         = "Hlt2/pRec/long/Tracks" ),
                  PackTrack( name = "PackDownTracks",
                             AlwaysCreateOutput = True,
                             DeleteInput        = False,
                             OutputLevel        = debugLevel,
                             InputName          = tracksDown.outputSelection(),
                             OutputName         = "Hlt2/pRec/down/Tracks" )
                  ]

        # Neutral protoparticles are in the same place as the charged with "Neutrals" replacing "Charged"
        from Configurables import PackProtoParticle
        algs += [ PackProtoParticle( name = "PackNeutralProtoP",
                             AlwaysCreateOutput = True,
                             DeleteInput        = False,
                             OutputLevel        = debugLevel,
                             #InputName          = "/Event/Hlt2/ProtoP/Fitted/Long/Neutrals",
                             InputName          = Hlt2BiKalmanFittedForwardTracking().hlt2NeutralProtos().outputSelection(),
                             OutputName         = "Hlt2/pRec/neutral/Protos" )
                  ]
        
        # Neutral CaloClusters
        from Configurables import DataPacking__Pack_LHCb__CaloClusterPacker_ as PackCaloClusters
        algs += [ PackCaloClusters( name = "PackCaloClusters",
                             AlwaysCreateOutput = True,
                             DeleteInput        = False,
                             OutputLevel        = debugLevel,
                             InputName          = "Hlt/Calo/EcalClusters", 
                             OutputName         = "Hlt2/pRec/neutral/CaloClusters" )
                  ]
        
        # Neutral Calo Hypos
        #from Configurables import DataPacking__Pack_LHCb__CaloHypoPacker_ as PackCaloHypos
        from Configurables import PackCaloHypo as PackCaloHypos
        algs += [ 
                PackCaloHypos( name = "PackCaloElectronHypos",
                             AlwaysCreateOutput = True,
                             DeleteInput        = False,
                             OutputLevel        = debugLevel,
                             InputName          = self.getProp("CaloHypoPrefix")+"/Electrons",
                             OutputName         = "Hlt2/pRec/neutral/Electrons" ),
                PackCaloHypos( name = "PackCaloPhotonsHypos",
                             AlwaysCreateOutput = True,
                             DeleteInput        = False,
                             OutputLevel        = debugLevel,
                             InputName          = self.getProp("CaloHypoPrefix")+"/Photons",
                             OutputName         = "Hlt2/pRec/neutral/Photons" ),
                PackCaloHypos( name = "PackCaloMergedPi0Hypos",
                             AlwaysCreateOutput = True,
                             DeleteInput        = False,
                             OutputLevel        = debugLevel,
                             InputName          = self.getProp("CaloHypoPrefix")+"/MergedPi0s",
                             OutputName         = "Hlt2/pRec/neutral/MergedPi0s" ),
                PackCaloHypos( name = "PackCaloSplitPhotonHypos",
                             AlwaysCreateOutput = True,
                             DeleteInput        = False,
                             OutputLevel        = debugLevel,
                             InputName          = self.getProp("CaloHypoPrefix")+"/SplitPhotons",
                             OutputName         = "Hlt2/pRec/neutral/SplitPhotons" )
                  ]
        
        # Finally return all the packers
        return algs
