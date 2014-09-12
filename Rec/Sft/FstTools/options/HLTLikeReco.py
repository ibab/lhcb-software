from Configurables import Brunel
from Configurables import MessageSvc
from Configurables import RecMoniConf
from Configurables import GaudiSequencer, PrTrackAssociator, PrChecker

from GaudiConf import IOHelper
import Gaudi.Configuration as GC
from FstTools.Configuration import StagedRecoConf


RecMoniConf().MoniSequence = []

Brunel().RecoSequence = ["L0", "HLT"]
Brunel().MCLinksSequence = ["Unpack", "Tr"]
Brunel().MCCheckSequence = ["Pat"]
Brunel().OutputType = "NONE"
Brunel().DataType = "2012"
Brunel().InputType = "DIGI"
Brunel().WithMC = True
Brunel().PrintFreq = 100
Brunel().Simulation = True
Brunel().EvtMax = 1000
Brunel().CondDBtag = 'sim-20131023-vc-md100' # use the mu100 for MagUp data
Brunel().DDDBtag = 'dddb-20130929-1'
Brunel().DatasetName = "full-hlt-plus-offline-noSkipping"

MessageSvc().Format = '% F%50W%S%7W%R%T %0W%M'

staged = StagedRecoConf()
# Include the offline PatForward in the "Best"
# container, remove OfflienFwd if you do not
# care for it
staged.AddToBest = ["Fwd1", "Fwd2", "OfflineFwd", "Match"]

# sim://MC/Dev/Beam6500GeV-RunII-MagDown-Nu1.5-25ns-Pythia8/Sim08e/30000000 ( minbias )/XDIGI
#
# dirac-dms-get-file /lhcb/MC/Dev/XDIGI/00034696/0000/00034696_00000024_1.xdigi
#inputFiles = ['/tmp/thead/minbias/00034696_00000024_1.xdigi']

# sim://MC/Dev/Beam6500GeV-RunII-MagDown-Nu1.5-25ns-Pythia8/Sim08e/11114001 ( Bd_Kstmumu=DecProdCut )/XDIGI
#
# dirac-dms-get-file /lhcb/MC/Dev/XDIGI/00034672/0000/00034672_00000001_1.xdigi
inputFiles= ['/tmp/thead/kstmumu/00034672_00000001_1.xdigi']
IOHelper('ROOT').inputFiles(inputFiles)

def setup_mc_truth_matching():
    GaudiSequencer("CaloBanksHandler").Members = []
    GaudiSequencer("DecodeTriggerSeq").Members = []
    GaudiSequencer("MCLinksTrSeq").Members = ["PrLHCbID2MCParticle", "PrTrackAssociator"]
    GaudiSequencer("CheckPatSeq" ).Members = ["PrChecker"]
    
    PrTrackAssociator().RootOfContainers = "/Event/Fst/Track"

    PrChecker().TriggerNumbers = True
    PrChecker().Eta25Cut = True
    PrChecker().WriteVeloHistos = 2
    PrChecker().WriteForwardHistos = 2
    PrChecker().WriteMatchHistos = 2
    PrChecker().WriteDownHistos = 2
    PrChecker().WriteUpHistos = 2
    PrChecker().WriteTTrackHistos = 2
    PrChecker().WriteBestHistos = 2
    PrChecker().WriteBestLongHistos = 2
    PrChecker().WriteBestDownstreamHistos = 2
    PrChecker().VeloTracks = "/Event/Fst/Track/Velo"
    PrChecker().UpTracks = "/Event/Fst/Track/VeloTT"
    PrChecker().ForwardTracks = "/Event/Fst/Track/Forward"
    PrChecker().SeedTracks = "/Event/Fst/Track/Seed"
    PrChecker().MatchTracks = "/Event/Fst/Track/Match"
    PrChecker().DownTracks = "/Event/Fst/Track/Downstream"
    PrChecker().BestTracks = "/Event/Fst/Track/Best"

    # Look just at PatForward used to catch up to offline
    # after the HLT is done
    offline = PrChecker("CheckOffline")
    offline.TriggerNumbers = True
    offline.Eta25Cut = True
    offline.ForwardTracks = "/Event/Fst/Track/OfflineFwd"
    GaudiSequencer("CheckPatSeq" ).Members += [offline]
    
GC.appendPostConfigAction(setup_mc_truth_matching)
