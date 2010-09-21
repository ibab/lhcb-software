from Gaudi.Configuration import *
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

#
# DaVinci Monitoring is done first
#
importOptions( "$DAVINCIMONITORSROOT/options/DaVinciMonitoring.py") 

#
# Stripping job configuration
#
from StrippingSelections.Streams import allStreams
from StrippingConf.Configuration import StrippingConf
sc = StrippingConf( Streams = allStreams )

from Configurables import SelDSTWriter
dstWriter = SelDSTWriter("MyDSTWriter",
	SelectionSequences = sc.activeStreams(),
        OutputPrefix = 'Strip',
	OutputFileSuffix = '000000')
dstWriter.ExtraItems += ['/Event/DAQ/RawEvent#1', '/Event/Strip/Phys/DecReports#1']

#
# Prescales of StrippingLines
#
from Configurables import DeterministicPrescaler
DeterministicPrescaler("StrippingMBMicroBiasPreScaler").AcceptFraction = 0.01
DeterministicPrescaler("StrippingMBNoBiasPreScaler").AcceptFraction = 0.01
DeterministicPrescaler("StrippingHlt1L0AnyPreScaler").AcceptFraction = 0.01

#
# DaVinci configuration
#
from Configurables import DaVinci
DaVinci().DataType = "2010"
DaVinci().InputType = 'SDST'
DaVinci().EvtMax = 10000 
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ dstWriter.sequence() ] )
DaVinci().HistogramFile = "DVHistos.root"

#
# Additional options for accessing Oracle conditions DB and
# to disable the usage of LFC to get the credentials to access Oracle servers
#
from Configurables import CondDB
CondDB(UseOracle = True)

from Configurables import COOLConfSvc
def disableLFC():
    COOLConfSvc(UseLFCReplicaSvc = False)
appendPostConfigAction(disableLFC)
