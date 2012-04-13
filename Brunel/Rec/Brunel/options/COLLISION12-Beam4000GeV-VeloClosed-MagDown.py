# Example 2012 collisions options for Brunel

# Syntax is:
#   gaudirun.py COLLISION12-Beam4000GeV-VeloClosed-MagDown.py
#
from Gaudi.Configuration import FileCatalog, EventSelector
from Configurables import Brunel

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest 2012 database tags for real data
Brunel().DataType = "2012"

EventSelector().Input = [
  "DATAFILE='mdf:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/freezer/lhcb/data/2012/RAW/FULL/LHCb/COLLISION12/112181/112181_0000000182.raw?svcClass=lhcbdisk' SVC='LHCb::MDFSelector'"
    ]
