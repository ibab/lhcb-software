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
                 "Sequence"                : None
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
        writer = InputCopyStream(self.getProp("WriterName"))
        for alg in algs:
            writer.OptItemList+=[alg.OutputName]
        
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
        # ProtoParticles
        from Configurables import PackProtoParticle
        algs += [ PackProtoParticle( name = "Hlt2PackChargedProtos",
                                     AlwaysCreateOutput = True,
                                     DeleteInput        = False,
                                     OutputLevel        = debugLevel,
                                     InputName          = Hlt2BiKalmanFittedForwardTracking().hlt2ChargedNoPIDsProtos().outputSelection(),
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
                                OutputName         = "Hlt2/pRec/long/RichPIDs" ),
                PackRichPIDs( name = "Hlt2PackDownRichPIDs",
                                AlwaysCreateOutput = True,
                                DeleteInput        = False,
                                OutputLevel        = debugLevel,
                                InputName          = Hlt2BiKalmanFittedDownstreamTracking().hlt2RICHID().outputSelection(),
                                OutputName         = "Hlt2/pRec/down/RichPIDs" )
                  ]
        
        # MUON PIDs (long)
        from Configurables import DataPacking__Pack_LHCb__MuonPIDPacker_ as PackMuonPIDs
        algs += [ PackMuonPIDs( name = "Hlt2PackMuonPIDs",
                                AlwaysCreateOutput = True,
                                DeleteInput        = False,
                                OutputLevel        = debugLevel,
                                InputName          = Hlt2BiKalmanFittedForwardTracking().hlt2MuonID().outputSelection(),
                                OutputName         = "Hlt2/pRec/long/MuonIDs" ),
                  # MUON PIDs (downstream)
                  PackMuonPIDs( name = "Hlt2PackDownMuonPIDs",
                                AlwaysCreateOutput = True,
                                DeleteInput        = False,
                                OutputLevel        = debugLevel,
                                InputName          = Hlt2BiKalmanFittedDownstreamTracking().hlt2MuonID().outputSelection(),
                                OutputName         = "Hlt2/pRec/down/MuonIDs" )
                  ]

        # Tracks
        from Configurables import PackTrack
        algs += [ PackTrack( name = "PackLongTracks",
                             AlwaysCreateOutput = True,
                             DeleteInput        = False,
                             OutputLevel        = debugLevel,
                             InputName          = Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks().outputSelection(),
                             OutputName         = "Hlt2/pRec/long/Tracks" ),
                  PackTrack( name = "PackMuonTracks",
                             AlwaysCreateOutput = True,
                             DeleteInput        = False,
                             OutputLevel        = debugLevel,
                             InputName          = Hlt2BiKalmanFittedDownstreamTracking().hlt2PrepareTracks().outputSelection(),
                             OutputName         = "Hlt2/pRec/down/Tracks" )
                  ]

        # Finally return all the packers
        return algs
