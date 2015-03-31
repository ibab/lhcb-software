from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import GaudiSequencer,RawEventJuggler
from Configurables import DaVinci
from Configurables import DecodeRawEvent

DaVinci()
DaVinci().EvtMax=-1
DaVinci().Lumi=True
DaVinci().DataType="2012"

from GaudiConf import IOHelper
IOHelper().inputFiles( ["Turbo_FSRcreated.dst"] , clear=True ) 
