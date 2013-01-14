from Gaudi.Configuration import EventSelector
from Configurables import Brunel, RecInit

Brunel().DataType = "2010"
Brunel().DatasetName = "bigEvents-2010"

RecInit("BrunelInit").OutputLevel = 2

from GaudiConf.IOHelper import IOHelper
# file of selected large events
IOHelper("MDF").inputFiles( [
  "root:root://castorlhcb.cern.ch//castor/cern.ch/user/c/cattanem/testFiles/run_69669_large_2ev.mdf?svcClass=default"
, "root:root://castorlhcb.cern.ch//castor/cern.ch/user/c/cattanem/testFiles/run_70122_large_1ev.mdf?svcClass=default"
, "root:root://castorlhcb.cern.ch//castor/cern.ch/user/c/cattanem/testFiles/bug67364.mdf?svcClass=default"
    ] )
