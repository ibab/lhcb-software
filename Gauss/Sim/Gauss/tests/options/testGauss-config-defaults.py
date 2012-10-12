from Gauss.Configuration import *
from Configurables import Gauss
from Configurables import MessageSvc
# Set output level for various algos

# Main Algo
gaussSeq = GaudiSequencer("GaussSequencer");
gaussSeq.OutputLevel = DEBUG


# Generator Algos
generator = GaudiSequencer("Generator");
generator.OutputLevel = 2

genSlotMainSeq = GaudiSequencer("GeneratorSlotMainSeq")
genSlotMainSeq.OutputLevel = 2

from Configurables import GenInit
gaussGen = GenInit("GaussGen")
gaussGen.OutputLevel = 2

from Configurables import Generation
generation = Generation("Generation")
generation.OutputLevel = 2

genMonitor = GaudiSequencer("GenMonitor")
genMonitor.OutputLevel = 2

from Configurables import GenMonitorAlg
genMonitorAlg = GenMonitorAlg("GenMonitorAlg")
genMonitorAlg.OutputLevel = 2


# Simulation Algos
simulation = GaudiSequencer("Simulation");
simulation.OutputLevel = 2

from Configurables import GiGaDataStoreAlgorithm
gigaStore = GiGaDataStoreAlgorithm("GiGaStore")
gigaStore.OutputLevel = 2
 
from Configurables import GiGaInputStream
gigaInputStream = GiGaInputStream("Geo")
gigaInputStream.OutputLevel = 2

# Must be called by GiGaInputStream...
from Configurables import GiGaGeo
geo = GiGaGeo("GiGaGeo")
geo.OutputLevel = 2


mainEvtSeq = GaudiSequencer("MainEventSeq")
mainEvtSeq.OutputLevel = 2

from Configurables import SimInit
mainEventGaussSim = SimInit("MainEventGaussSim")
mainEventGaussSim.OutputLevel = 2

mainSim = GaudiSequencer("MainSimulation")
mainSim.OutputLevel = 2

makeMainSim = GaudiSequencer("MakeMainSim")
makeMainSim.OutputLevel = 2

simMonitor = GaudiSequencer("SimMonitor")
simMonitor.OutputLevel = 2


from Configurables import GenerationToSimulation
genToSim = GenerationToSimulation("GenToSim")
genToSim.OutputLevel = 2

from Configurables import GiGaFlushAlgorithm
gigaFlush = GiGaFlushAlgorithm("GiGaFlush")
gigaFlush.OutputLevel = 2

from Configurables import GiGaCheckEventStatus
gigaCheckEvent = GiGaCheckEventStatus("GiGaCheckEvent")
gigaCheckEvent.OutputLevel = 2

from Configurables import SimulationToMCTruth
simToMCTruth = SimulationToMCTruth("SimToMCTruth")
simToMCTruth.OutputLevel = 2

detectorsHits = GaudiSequencer("DetectorsHits")
detectorsHits.OutputLevel = 2

# lots here
from Configurables import GetTrackerHitsAlg
getVeloHits = GetTrackerHitsAlg("GetVeloHits")
getPuVetoHits = GetTrackerHitsAlg("GetPuVetoHits")
getTTHits = GetTrackerHitsAlg("GetTTHits")
getITHits = GetTrackerHitsAlg("GetITHits")
getOTHits = GetTrackerHitsAlg("GetOTHits")
getMuonHits = GetTrackerHitsAlg("GetMuonHits")

getVeloHits.OutputLevel = 2
getPuVetoHits.OutputLevel = 2
getTTHits.OutputLevel = 2
getITHits.OutputLevel = 2
getOTHits.OutputLevel = 2
getMuonHits.OutputLevel = 2

from Configurables import GetCaloHitsAlg
getSpdHits = GetCaloHitsAlg("GetSpdHits")
getPrsHits = GetCaloHitsAlg("GetPrsHits")
getEcalHits = GetCaloHitsAlg("GetEcalHits")
getHcalHits = GetCaloHitsAlg("GetHcalHits")
getSpdHits.OutputLevel = 2
getPrsHits.OutputLevel = 2
getEcalHits.OutputLevel = 2
getHcalHits.OutputLevel = 2

richHits = GaudiSequencer("RichHits")
richHits.OutputLevel = 2


mainEventDataPacking = GaudiSequencer("MainEventDataPacking")
mainEventDataPacking.OutputLevel = 2

from Configurables import PackMCParticle
packMCParticle = PackMCParticle("PackMCParticle")
packMCParticle.OutputLevel = 2

from Configurables import PackMCVertex
packMCVertex = PackMCVertex("PackMCVertex")
packMCVertex.OutputLevel = 2

# Confused about what to do here
#DataPacking::Pack<LHCb::MCVeloHitPacker>/MCVeloHitPacker
#DataPacking::Pack<LHCb::MCPuVetoHitPacker>/MCPuVetoHitPacker
#DataPacking::Pack<LHCb::MCTTHitPacker>/MCTTHitPacker
#DataPacking::Pack<LHCb::MCITHitPacker>/MCITHitPacker
#DataPacking::Pack<LHCb::MCOTHitPacker>/MCOTHitPacker
#DataPacking::Pack<LHCb::MCMuonHitPacker>/MCMuonHitPacker
#DataPacking::Pack<LHCb::MCPrsHitPacker>/MCPrsHitPacker
#DataPacking::Pack<LHCb::MCSpdHitPacker>/MCSpdHitPacker
#DataPacking::Pack<LHCb::MCEcalHitPacker>/MCEcalHitPacker
#DataPacking::Pack<LHCb::MCHcalHitPacker>/MCHcalHitPacker
#DataPacking::Pack<LHCb::MCRichHitPacker>/MCRichHitPacker
#DataPacking::Pack<LHCb::MCRichOpticalPhotonPacker>/MCRichOpPhotPacker
#DataPacking::Pack<LHCb::MCRichSegmentPacker>/MCRichSegmentPacker
#DataPacking::Pack<LHCb::MCRichTrackPacker>/MCRichTrackPacker

