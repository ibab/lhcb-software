
########################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import DaVinci, PhysConf
########################################################################

DaVinci().EvtMax     = 100                  # Number of events
DaVinci().PrintFreq  = 10                   # Events to skip
DaVinci().DataType   = "2012"               # Must be given
DaVinci().HistogramFile = "dst-unpacking.root"

########################################################################

from Configurables import TrackMonitor
DaVinci().UserAlgorithms += [ TrackMonitor() ]
