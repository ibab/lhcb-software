# Example 2011 collisions options for Brunel

# Syntax is:
#   gaudirun.py COLLISION11-Beam3500GeV-VeloClosed-MagDown.py
#
from Gaudi.Configuration import FileCatalog, EventSelector
from Configurables import Escher

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest 2011 database tags for real data
Escher().DataType = "2011"
Escher().InputType  = "MDF"

# Events from run 87968 from fill 1647, on 24th March 2011
EventSelector().Input = [
  "DATAFILE='mdf:srm://srm-lhcb.cern.ch//castor/cern.ch/grid/lhcb/freezer/lhcb/data/2011/RAW/EXPRESS/LHCb/COLLISION11/87968/087968_0000000001.raw?svcClass=lhcbdisk' SVC='LHCb::MDFSelector'"
    ]
EventSelector().Input = [
  "DATAFILE='mdf:srm://srm-lhcb.cern.ch//castor/cern.ch/grid/lhcb/freezer/lhcb/data/2011/RAW/FULL/LHCb/COLLISION11/97121/097121_0000000003.raw?svcClass=lhcbdisk' SVC='LHCb::MDFSelector'"
    ]
