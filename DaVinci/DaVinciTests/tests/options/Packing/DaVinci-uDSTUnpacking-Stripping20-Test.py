
########################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import DaVinci, PhysConf
########################################################################

DaVinci().EvtMax     = 100                  # Number of events
DaVinci().PrintFreq  = 10                   # Events to skip
DaVinci().DataType   = "2012"               # Must be given
DaVinci().HistogramFile = "udst-unpacking.root"
DaVinci().InputType  = "MDST"

########################################################################

from Configurables import DataObjectVersionFilter
DaVinci().UserAlgorithms += [
    DataObjectVersionFilter( "CheckDecReports",
                             DataObjectLocation = "/Event/Strip/Phys/DecReports",
                             MaxVersion = 999,
                             OutputLevel = 3 ),
    DataObjectVersionFilter( "CheckTracks",
                             DataObjectLocation = "/Event/Bhadron/Rec/Track/Best",
                             MaxVersion = 999 ),
    DataObjectVersionFilter( "CheckVeloClusters",
                             DataObjectLocation = "/Event/Bhadron/Raw/Velo/Clusters",
                             MaxVersion = 999,
                             OutputLevel = 3 ),
    DataObjectVersionFilter( "CheckCaloElectrons",
                             DataObjectLocation = "/Event/Bhadron/Rec/Calo/Electrons",
                             MaxVersion = 999,
                             OutputLevel = 3 ),
    DataObjectVersionFilter( "CheckCaloPhotons",
                             DataObjectLocation = "/Event/Bhadron/Rec/Calo/Photons",
                             MaxVersion = 999,
                             OutputLevel = 3 ),
    DataObjectVersionFilter( "CheckLiteTTClusters",
                             DataObjectLocation = "/Event/Bhadron/Raw/TT/LiteClusters",
                             MaxVersion = 999,
                             OutputLevel = 3 ),
    DataObjectVersionFilter( "CheckLiteITClusters",
                             DataObjectLocation = "/Event/Bhadron/Raw/IT/LiteClusters",
                             MaxVersion = 999,
                             OutputLevel = 3 ),
    DataObjectVersionFilter( "CheckLiteVeloClusters",
                             DataObjectLocation = "/Event/Bhadron/Raw/Velo/LiteClusters",
                             MaxVersion = 999,
                             OutputLevel = 3 ),
    DataObjectVersionFilter( "CheckBhadronKaons",
                             DataObjectLocation = "/Event/Bhadron/Phys/StdAllNoPIDsKaons/Particles",
                             MaxVersion = 999,
                             OutputLevel = 3 ),
    DataObjectVersionFilter( "CheckPVs",
                             DataObjectLocation = "/Event/Bhadron/Rec/Vertex/Primary",
                             MaxVersion = 999,
                             OutputLevel = 3 )
    ]

from GaudiConf import IOHelper
IOHelper().inputFiles(['PFN:root://eoslhcb.cern.ch//eos/lhcb/LHCb/Collision12/BHADRON.MDST/00021211/0000/00021211_00001955_1.bhadron.mdst'])
