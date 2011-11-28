# Pathological events from 2011 that caused Brunel crashes.
from Gaudi.Configuration import FileCatalog
from Configurables import Brunel

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest 2011 database tags for real data
Brunel().DataType = "2011"

# Reco12 crashes, bug #89269, fixed in Brunel v41r1p1 and >v41r2
from GaudiConf.IOHelper import IOHelper
IOHelper("MDF").inputFiles( [
  "DATAFILE='root:root://castorlhcb.cern.ch//castor/cern.ch/user/c/cattanem/testFiles/102896_0000000146-evt12267.raw?svcClass=default'"
, "DATAFILE='root:root://castorlhcb.cern.ch//castor/cern.ch/user/c/cattanem/testFiles/102897_0000000127-evt35859.raw?svcClass=default'" ] )
