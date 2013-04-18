# Pathological events from 2011 that caused Brunel crashes.
from Gaudi.Configuration import FileCatalog
from Configurables import Brunel

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest 2011 database tags for real data
Brunel().DataType = "2011"

# Two Reco12 crashes, bug #89269, fixed in Brunel v41r1p1 and >v41r2
# One crash, bug #93382, introduced in Brunel v42r2, fixed in v42r2p1
from GaudiConf.IOHelper import IOHelper
IOHelper("MDF").inputFiles( [
  "DATAFILE='$QMTESTFILESROOT/data/102896_0000000146_evt12267_bug89269.raw'"
, "DATAFILE='$QMTESTFILESROOT/data/102897_0000000127_evt35859_bug89269.raw'"
, "DATAFILE='$QMTESTFILESROOT/data/103127_0000000001_evt2277_bug93382.raw'" ] )
