# Pathological events from 2012 that caused Brunel crashes or huge processing times.
from Gaudi.Configuration import FileCatalog
from Configurables import Brunel

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest 2012 database tags for real data
Brunel().DataType = "2012"

# Three events with huge processing times, fixed in Brunel v41r2p2, bug 93550
# One event to test bug 94407, fixed in Brunel v42r3p2
from GaudiConf.IOHelper import IOHelper
IOHelper("MDF").inputFiles( [
  "DATAFILE='$QMTESTFILESROOT/data/111473_0000000110_evt11603_bug93550.raw'"
, "DATAFILE='$QMTESTFILESROOT/data/111702_0000000001_evt110102_bug93550.raw'"
, "DATAFILE='$QMTESTFILESROOT/data/111800_0000000026_evt16533_bug93550.raw'"
, "DATAFILE='$QMTESTFILESROOT/data/114752_0000000076_evt377_bug94407.raw'" ] )
