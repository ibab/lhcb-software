from Gaudi.Configuration import EventSelector
from Configurables import Brunel

Brunel().DataType = "2010"
Brunel().DatasetName = "bigEvents-2010"

from GaudiConf.IOHelper import IOHelper
# file of selected large events
IOHelper("MDF").inputFiles( [
  "$QMTESTFILESROOT/data/069669_2ev_bug65441.raw"
, "$QMTESTFILESROOT/data/070122_1ev_large.raw"
, "$QMTESTFILESROOT/data/071491_0000000091_evt35027_bug67364.raw"
    ] )
