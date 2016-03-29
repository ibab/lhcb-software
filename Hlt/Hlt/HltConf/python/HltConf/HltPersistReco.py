"""Configures the persistence of reconstructed objects."""

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import GaudiSequencer as Sequence
from Configurables import HltPackedDataWriter

from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedDownstreamTracking


__author__ = "Sean Benson, Rosen Matev"


class HltPersistRecoConf(LHCbConfigurableUser):
    # python configurables to be applied before me
    __queried_configurables__ = [
        Hlt2BiKalmanFittedForwardTracking,
        Hlt2BiKalmanFittedDownstreamTracking,
    ]
    # python configurables that I configure
    __used_configurables__ = []

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
        from DAQSys.Decoders import DecoderDB

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
        pdwriter.Containers = self.packedObjectLocations()
        persistRecoSeq.Members += [pdwriter]

        # Register the mapping of output locations and integers
        self._registerToHltANNSvc()

    def packedObjectLocations(self):
        """Return a list with the packed object locations."""
        return [alg.OutputName for alg in self._packers()]

    def _registerToHltANNSvc(self):
        """Register the packed object locations in the HltANNSvc."""
        from Configurables import HltANNSvc
        d = dict((loc, i) for i, loc in enumerate(self.packedObjectLocations()))
        HltANNSvc().PackedObjectLocations = d

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
                name="Hlt2PackRichPIDs",
                InputName=longTracking.hlt2RICHID().outputSelection(),
                OutputName="Hlt2/pRec/long/RichPIDs"),
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

        self._packersCache = algs
        return algs
