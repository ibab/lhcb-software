
########################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import DaVinci, PhysConf
########################################################################

DaVinci().EvtMax     = 100                  # Number of events
DaVinci().PrintFreq  = 10                   # Events to skip
DaVinci().DataType   = "2012"                # Must be given
DaVinci().HistogramFile = "dst-annpid.root"

########################################################################

from Configurables import ANNGlobalPID__ChargedProtoANNPIDMoni
DaVinci().UserAlgorithms += [
    ANNGlobalPID__ChargedProtoANNPIDMoni("ANNPIDMoni")
    ]

from GaudiConf import IOHelper
IOHelper().inputFiles(['PFN:root://eoslhcb.cern.ch//eos/lhcb/freezer/lhcb/LHCb/Collision12/BHADRONCOMPLETEEVENT.DST/00021317/0000/00021317_00003475_1.bhadroncompleteevent.dst'])
