
########################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import DaVinci, PhysConf
########################################################################

DaVinci().EvtMax     = 100                  # Number of events
DaVinci().PrintFreq  = 10                   # Events to skip
DaVinci().DataType   = "2012"                # Must be given
DaVinci().HistogramFile = "dst-unpacking.root"

########################################################################



from Configurables import TrackMonitor
DaVinci().UserAlgorithms += [ TrackMonitor() ]

from GaudiConf import IOHelper
IOHelper().inputFiles(['PFN:root://eoslhcb.cern.ch//eos/lhcb/LHCb/Collision12/BHADRONCOMPLETEEVENT.DST/00017936/0000/00017936_00001645_1.bhadroncompleteevent.dst'])
