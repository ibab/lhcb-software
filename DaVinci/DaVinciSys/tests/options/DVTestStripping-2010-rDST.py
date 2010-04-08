#
#   Stripping selections ETC writing on real data
#   Used in full stream reconstruction
#
#   @author P. Koppenburg
#   @date 2010-03-01
#

from Gaudi.Configuration import *

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
#
# Stripping job configuration
#
from StrippingSelections.StrippingPass import StrippingPassConf
StrippingPassConf().PassPrescale = 1.0
StrippingPassConf().PhysPrescale = 1.0

from StrippingConf.Configuration import StrippingConf
from StrippingSelections.Streams import allStreams

sc = StrippingConf( Streams = allStreams )

from Configurables import CondDB
CondDB(UseOracle = True)

#
# Configure DaVinci
#

from Configurables import L0DUFromRawAlg, L0DUFromRawTool
l0du = L0DUFromRawAlg('L0DUFromRaw')
l0du.addTool(L0DUFromRawTool, name = 'L0DUFromRawTool')
l0du.L0DUFromRawTool.RawLocation = "pRec/RawEvent"

from Configurables import DaVinci

DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().EvtMax = 1000
DaVinci().DataType = "2010"     
DaVinci().Simulation = False
DaVinci().HistogramFile = "DVHistos.root"
DaVinci().Input = [
    "DATAFILE='/castor/cern.ch/grid/lhcb/data/2010/RDST/00006165/0000/00006165_00000004_1.rdst' TYP='POOL_ROOTTREE' OPT='READ'",
    ]
