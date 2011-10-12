# Special options for 2011 25ns fill, without Hlt2

# Syntax is:
#   gaudirun.py COLLISION11_25.py
#
from Gaudi.Configuration import FileCatalog, EventSelector
from Configurables import Brunel

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest 2011 database tags for real data
from GaudiKernel.ProcessJobOptions import importOptions
importOptions("$APPCONFIGOPTS/Brunel/2011-25ns.py")

# Events from run 103053, fill 2186 on 2011-10-07, 25ns bunch spacing
EventSelector().Input = [
  "DATAFILE='mdf:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/freezer/lhcb/data/2011/RAW/FULL/LHCb/COLLISION11_25/103053/103053_0000000035.raw?svcClass=lhcbdisk' SVC='LHCb::MDFSelector'"
    ]
