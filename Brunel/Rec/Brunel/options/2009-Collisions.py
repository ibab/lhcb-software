# Example 2009 collisions options for Brunel

# Syntax is:
#   gaudirun.py 2009-Collisions.py
#
from GaudiKernel.ProcessJobOptions import importOptions
importOptions("$APPCONFIGOPTS/Brunel/veloOpen.py")

from Gaudi.Configuration import FileCatalog, EventSelector
from Configurables import Brunel

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest 2009 database tags for real data
Brunel().DataType = "2009"

# First collisions with magnet off
#importOptions("$APPCONFIGOPTS/Brunel/moff.py")
#EventSelector().Input = [
#     "DATAFILE='root:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/BEAM1/62558/062558_0000000001.raw' SVC='LHCb::MDFSelector'"
#    ]

# Collisions on 6th December with magnet on and all detectors in readout
EventSelector().Input = [
    "DATAFILE='mdf:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/COLLISION09/63497/063497_0000000001.raw?svcClass=lhcbtape' SVC='LHCb::MDFSelector'"
    ]
