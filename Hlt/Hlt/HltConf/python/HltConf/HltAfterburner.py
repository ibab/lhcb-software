"""
 script to configure HLT Afterburner

 @author R. Aaij
 @date 13-05-2015
"""
# =============================================================================
__author__  = "R. Aaij roel.aaij@cern.ch"
# =============================================================================
import types
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import GaudiSequencer as Sequence

class HltAfterburnerConf(LHCbConfigurableUser):
    __used_configurables__ = []

    __slots__ = {"EnableHltRecSummary" : True,
                 "Hlt2Filter"          : "HLT_PASS_RE('Hlt2(?!Forward)(?!DebugEvent)(?!Lumi)(?!Transparent)(?!PassThrough).*Decision')"
                }

###################################################################################
#
# Main configuration
#
    def __apply_configuration__(self):
        """
        HLT Afterburner configuration
        """
        Afterburner = Sequence("HltAfterburner")
        if self.getProp("Hlt2Filter"):
            from DAQSys.Decoders import DecoderDB
            decoder = DecoderDB["HltDecReportsDecoder/Hlt2DecReportsDecoder"]
            from Configurables import LoKi__HDRFilter   as HDRFilter
            hlt2Filter = HDRFilter('HltAfterburnerHlt2Filter', Code = self.getProp("Hlt2Filter"),
                                   Location = decoder.listOutputs()[0])
            Afterburner.Members += [hlt2Filter]

        if self.getProp("EnableHltRecSummary"):
            from Configurables import RecSummaryAlg
            seq = Sequence("RecSummarySequence")

            from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
            tracks = Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks()
            muonID = Hlt2BiKalmanFittedForwardTracking().hlt2MuonID()

            from HltLine.HltDecodeRaw import DecodeVELO, DecodeIT, DecodeTT, DecodeSPD, DecodeMUON
            decoders = {"Velo"   : (DecodeVELO, "VeloLiteClustersLocation"),
                        "TT"     : (DecodeTT,   "clusterLocation"),
                        "IT"     : (DecodeIT,   "clusterLocation"),
                        "SPD"    : (DecodeSPD,  "DigitsContainer"),
                        'Muon'   : (DecodeMUON, "OutputLocation"),
                        'MuonTr' : (muonID,     "MuonTrackLocation")}
            decoders = {k : (bm.members(), bm.members()[-1].getProp(loc)) for (k, (bm, loc)) in decoders.iteritems()}

            from HltTracking.HltPVs import PV3D
            PVs = PV3D("Hlt2")
            summary = RecSummaryAlg('Hlt2RecSummary', SummaryLocation = "Hlt2/RecSummary",
                                    TracksLocation = tracks.outputSelection(),
                                    PVsLocation = PVs.output,
                                    UseLiteClusters = True,
                                    VeloClustersLocation = decoders['Velo'][1],
                                    ITClustersLocation = decoders['IT'][1],
                                    TTClustersLocation = decoders['TT'][1],
                                    SpdDigitsLocation  = decoders['SPD'][1],
                                    MuonCoordsLocation = decoders['Muon'][1],
                                    MuonTracksLocation = decoders['MuonTr'][1])
            from itertools import chain
            from Hlt2Lines.Utilities.Utilities import uniqueEverseen
            seq.Members = list(uniqueEverseen(chain.from_iterable([dec[0] for dec in decoders.itervalues()]
                                                                  + [PVs, tracks, muonID]))) + [summary]
            Afterburner.Members += [seq]
