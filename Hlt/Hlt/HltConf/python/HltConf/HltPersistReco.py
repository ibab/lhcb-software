"""Configures the persistence of reconstructed objects."""

import itertools
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import GaudiSequencer as Sequence
from Configurables import HltPackedDataWriter

from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedDownstreamTracking
from Configurables import CaloProcessor, RichRecSysConf, ChargedProtoANNPIDConf

__author__ = "Sean Benson, Rosen Matev"


class HltPersistRecoConf(LHCbConfigurableUser):
    # python configurables to be applied before me
    __queried_configurables__ = [
        Hlt2BiKalmanFittedForwardTracking,
        Hlt2BiKalmanFittedDownstreamTracking,
    ]
    # python configurables that I configure
    __used_configurables__ = [
        (CaloProcessor, "SharedCaloProcessor"),
        (RichRecSysConf, "Hlt2LongTracking_RichRecSysConf"),
        (RichRecSysConf, "Hlt2DownstreamTracking_RichRecSysConf"),
        (ChargedProtoANNPIDConf, "Hlt2LongTracking_ChargedProtoANNPIDConf")
    ]

    __slots__ = {
        "Sequence":        None,
        "CaloHypoPrefix":  "Hlt2/PID/CALO/Calo",
        "OutputLevel":     WARNING,
        "_packersCache":   None,
    }

    def __apply_configuration__(self):
        """Apply the HLT persist reco configuration."""

        from Configurables import GaudiSequencer
        from Configurables import LoKi__HDRFilter
        from Configurables import TrackToDST

        persistRecoSeq = self.getProp("Sequence")
        if not self.getProp("Sequence"):
            return
        persistRecoSeq.IgnoreFilterPassed = False
        persistRecoSeq.Members = []

        # This sequence expects the proper line filter to be applied already

        # Cut down states in tracks first
        tracks = Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks()
        tracksDown = Hlt2BiKalmanFittedDownstreamTracking().hlt2PrepareTracks()
        longStateCutter = TrackToDST("TrackToDSTLong")
        longStateCutter.TracksInContainer = tracks.outputSelection()
        downStateCutter = TrackToDST("TrackToDSTDown")
        downStateCutter.TracksInContainer = tracksDown.outputSelection()
        persistRecoSeq.Members += [longStateCutter]
        persistRecoSeq.Members += [downStateCutter]

        # Setup packers and add them to the sequence
        packerAlgs = self._packers()
        persistRecoSeq.Members += packerAlgs

        # Configure HltPackedDataWriter algorithm to add to the raw banks
        pdwriter = HltPackedDataWriter("Hlt2PackedDataWriter")
        pdwriter.Containers = [out for inp, out in self.packedObjectLocations()]
        persistRecoSeq.Members += [pdwriter]

        # Register the mapping of output locations and integers
        self._registerToHltANNSvc()

    def packedObjectLocations(self):
        """Return a list with the (unpacked, packed) object locations."""
        return [(alg.InputName, alg.OutputName) for alg in self._packers()]

    def _registerToHltANNSvc(self):
        """Register the packed object locations in the HltANNSvc."""
        from Configurables import HltANNSvc
        locations = list(itertools.chain(*self.packedObjectLocations()))
        # We need to register the locations of (non-reconstructed) data
        # that is referenced by the some of the packed objects.
        locations += [
            '/Event/Raw/Ecal/Digits',
            '/Event/Raw/Prs/Digits',
            '/Event/Raw/Spd/Digits',
        ]
        location_ids = {loc: i+1 for i, loc in enumerate(locations)}
        print {i: loc for loc, i in location_ids.items()}
        HltANNSvc().PackedObjectLocations = location_ids

    def _packers(self):
        """Return a list with the packer algorithms."""

        try:
            return self._packersCache
        except AttributeError:
            pass

        longTracking = Hlt2BiKalmanFittedForwardTracking()
        downstreamTracking = Hlt2BiKalmanFittedDownstreamTracking()
        caloHypoPrefix = self.getProp("CaloHypoPrefix")

        from Configurables import PackProtoParticle
        from Configurables import DataPacking__Pack_LHCb__RichPIDPacker_ as PackRichPIDs
        from Configurables import DataPacking__Pack_LHCb__MuonPIDPacker_ as PackMuonPIDs
        from Configurables import PackTrack
        from Configurables import PackProtoParticle
        from Configurables import DataPacking__Pack_LHCb__CaloClusterPacker_ as PackCaloClusters
        # from Configurables import DataPacking__Pack_LHCb__CaloHypoPacker_ as PackCaloHypos
        from Configurables import PackCaloHypo as PackCaloHypos

        algs = [
            # ProtoParticles
            PackProtoParticle(
                name="Hlt2PackChargedProtos",
                InputName=longTracking.hlt2ChargedAllPIDsProtos().outputSelection(),
                OutputName="Hlt2/pRec/long/Protos"),
            PackProtoParticle(
                name="Hlt2PackChargedDownProtos",
                InputName=downstreamTracking.hlt2ChargedNoPIDsProtos().outputSelection(),
                OutputName="Hlt2/pRec/down/Protos"),
            # RICH PIDs
            PackRichPIDs(
                name="Hlt2PackLongRichPIDs",
                InputName=longTracking.hlt2RICHID().outputSelection(),
                OutputName="Hlt2/pRec/long/RichPIDs"),
            PackRichPIDs(
                name="Hlt2PackDownRichPIDs",
                InputName=downstreamTracking.hlt2RICHID().outputSelection(),
                OutputName="Hlt2/pRec/down/RichPIDs"),
            # MUON PIDs
            PackMuonPIDs(
                name="Hlt2PackMuonPIDs",
                InputName=longTracking.hlt2MuonID().outputSelection(),
                OutputName="Hlt2/pRec/long/MuonIDs"),
            # Tracks
            PackTrack(
                name="Hlt2PackLongTracks",
                InputName=longTracking.hlt2PrepareTracks().outputSelection(),
                OutputName="Hlt2/pRec/long/Tracks"),
            PackTrack(
                name="Hlt2PackDownTracks",
                InputName=downstreamTracking.hlt2PrepareTracks().outputSelection(),
                OutputName="Hlt2/pRec/down/Tracks"),
            PackTrack(
                name="Hlt2PackDownPIDMuonSegments",
                InputName=downstreamTracking._trackifiedMuonIDLocation(),
                OutputName="Hlt2/pRec/down/PID/MuonSegments"),

            # Neutral protoparticles are in the same place as the charged with "Neutrals" replacing "Charged"
            PackProtoParticle(
                name="Hlt2PackNeutralProtoP",
                InputName=longTracking.hlt2NeutralProtos().outputSelection(),
                OutputName="Hlt2/pRec/neutral/Protos"),
            # Neutral CaloClusters
            PackCaloClusters(
                name="Hlt2PackCaloClusters",
                InputName="Hlt/Calo/EcalClusters",
                OutputName="Hlt2/pRec/neutral/CaloClusters"),
            PackCaloClusters(
                name="Hlt2PackEcalSplitClusters",
                InputName=caloHypoPrefix + "/EcalSplitClusters",
                OutputName="Hlt2/pRec/neutral/EcalSplitClusters"),
            # Neutral Calo Hypos
            PackCaloHypos(
                name="Hlt2PackCaloElectronHypos",
                InputName=caloHypoPrefix + "/Electrons",
                OutputName="Hlt2/pRec/neutral/Electrons"),
            PackCaloHypos(
                name="Hlt2PackCaloPhotonsHypos",
                InputName=caloHypoPrefix + "/Photons",
                OutputName="Hlt2/pRec/neutral/Photons"),
            PackCaloHypos(
                name="Hlt2PackCaloMergedPi0Hypos",
                InputName=caloHypoPrefix + "/MergedPi0s",
                OutputName="Hlt2/pRec/neutral/MergedPi0s"),
            PackCaloHypos(
                name="Hlt2PackCaloSplitPhotonHypos",
                InputName=caloHypoPrefix + "/SplitPhotons",
                OutputName="Hlt2/pRec/neutral/SplitPhotons"),
        ]

        for alg in algs:
            alg.AlwaysCreateOutput = True
            alg.DeleteInput = False
            alg.OutputLevel = self.getProp("OutputLevel")
            if not alg.InputName.startswith('/Event/'):
                # This is needed to have the full locations registered in ANNSvc
                alg.InputName = '/Event/' + alg.InputName

        # Check that the output locations are consistent with the decoder
        from DAQSys.Decoders import DecoderDB
        try:
            decoder = DecoderDB["HltPackedDataDecoder/Hlt2PackedDataDecoder"]
        except KeyError:
            # TODO remove that for a release on the most recent DAQSys
            decoder = None
        if decoder:
            decoder_outputs = sorted(decoder.listOutputs())
            configured_outputs = sorted(alg.OutputName for alg in algs)
            if configured_outputs != decoder_outputs:
                log.warning('Configured output locations: {}'.format(configured_outputs))
                log.warning('Configured decoder locations: {}'.format(decoder_outputs))
                raise ValueError('Configured output locations do not match the decoder!' +
                                 'Please update the DecoderDB (Decoders.py)!'
                                )

        self._packersCache = algs
        return algs
