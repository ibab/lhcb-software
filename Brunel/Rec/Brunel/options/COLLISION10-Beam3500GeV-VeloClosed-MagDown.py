# Example 2010 collisions options for Brunel

# Syntax is:
#   gaudirun.py COLLISION10-Beam3500GeV-VeloClosed-MagDown.py
#
from Gaudi.Configuration import FileCatalog, EventSelector
from Configurables import Brunel

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest 2010 database tags for real data
Brunel().DataType = "2010"

# 2188 events from run 81684
EventSelector().Input = [
  "DATAFILE='/usera/jonesc/NFS/data/Collision10/LHCb/Raw/81684/081684_0000000335.raw' SVC='LHCb::MDFSelector'"
    ]
