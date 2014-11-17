# Pathological events from 2012 that caused huge processing times in Stripping21.
from Gaudi.Configuration import FileCatalog
from Configurables import Brunel

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest 2012 database tags for real data
Brunel().DataType = "2012"
Brunel().InputType = "DST"

#-- Twelve events with huge processing times in Reco14/Stripping21 draft.
from GaudiConf.IOHelper import IOHelper
IOHelper().inputFiles( [
  "DATAFILE='$QMTESTFILESROOT/data/12Events_Stripping21Reco14_SlowEvents.dst'" ] )
