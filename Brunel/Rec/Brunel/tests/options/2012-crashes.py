# Pathological events from 2012 that caused Brunel crashes or huge processing times.
from Gaudi.Configuration import FileCatalog
from Configurables import Brunel

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest 2012 database tags for real data
Brunel().DataType = "2012"

# Three events with uge processing times, fixed in Brunel v41r2p2
from GaudiConf.IOHelper import IOHelper
IOHelper("MDF").inputFiles( [
  "DATAFILE='root:root://castorlhcb.cern.ch//castor/cern.ch/user/c/cattanem/testFiles/111473_0000000110_evt11603.raw?svcClass=default'"
, "DATAFILE='root:root://castorlhcb.cern.ch//castor/cern.ch/user/c/cattanem/testFiles/111702_0000000001_evt110102.raw?svcClass=default'"
, "DATAFILE='root:root://castorlhcb.cern.ch//castor/cern.ch/user/c/cattanem/testFiles/111800_0000000026_evt16533.raw?svcClass=default'"
, "DATAFILE='root:root://castorlhcb.cern.ch//castor/cern.ch/user/c/cattanem/testFiles/114752_0000000076_evt377.raw?svcClass=default'" ] )
