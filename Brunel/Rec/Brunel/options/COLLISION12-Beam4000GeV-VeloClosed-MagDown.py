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

# BEWARE BEWARE BEWARE BEWARE BEWARE BEWARE BEWARE BEWARE BEWARE BEWARE
# For time being this is a 2011 file, until we get 2012 data!
# Events from run 87968 from fill 1647, on 24th March 2011
# BEWARE BEWARE BEWARE BEWARE BEWARE BEWARE BEWARE BEWARE BEWARE BEWARE
EventSelector().Input = [
  "DATAFILE='mdf:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/freezer/lhcb/data/2011/RAW/EXPRESS/LHCb/COLLISION11/87968/087968_0000000001.raw?svcClass=lhcbdisk' SVC='LHCb::MDFSelector'"
    ]
