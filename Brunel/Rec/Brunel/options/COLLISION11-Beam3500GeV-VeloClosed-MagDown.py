# Example 2011 collisions options for Brunel

# Syntax is:
#   gaudirun.py COLLISION11-Beam3500GeV-VeloClosed-MagDown.py
#
from Gaudi.Configuration import FileCatalog, EventSelector
from Configurables import Brunel

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest 2011 database tags for real data
Brunel().DataType = "2011"

from Gaudi.Configuration import *
from GaudiConf import IOHelper
# Events from run 103370, TCK 0x790038
EventSelector().Input = [
  "DATAFILE='mdf:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/freezer/lhcb/LHCb/Collision11/RAW/00020559/0000/00020559_00000481_1.raw?svcClass=lhcbdisk' SVC='LHCb::MDFSelector'"
  ]

# Events from run 87968 from fill 1647, on 24th March 2011
#EventSelector().Input = [
#  "DATAFILE='mdf:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/freezer/lhcb/data/2011/RAW/EXPRESS/LHCb/COLLISION11/87968/087968_0000000001.raw?svcClass=lhcbdisk' SVC='LHCb::MDFSelector'"
#    ]
