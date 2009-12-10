# Example 2009 collisions options for Brunel

# Syntax is:
#   gaudirun.py 2009-Collisions.py
#
from GaudiKernel.ProcessJobOptions import importOptions
importOptions("$APPCONFIGOPTS/Brunel/moff.py")
importOptions("$APPCONFIGOPTS/Brunel/veloOpen.py")
importOptions("$APPCONFIGOPTS/Brunel/earlyData.py")

from Gaudi.Configuration import FileCatalog, EventSelector
from Configurables import Brunel

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest 2009 database tags for real data
Brunel().DataType = "2009"

#-- GAUDI jobOptions generated on Tue Nov 24 10:30:05 2009
#-- Contains event types : 
#--   90000000 - 2 files - 251 events - 0.08 GBytes

EventSelector().Input = [
    "DATAFILE='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/BEAM1/62556/062556_0000000001.raw' SVC='LHCb::MDFSelector'"
    , "DATAFILE='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/BEAM1/62558/062558_0000000001.raw' SVC='LHCb::MDFSelector'"
    ]
