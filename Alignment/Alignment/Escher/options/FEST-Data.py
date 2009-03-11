# Example 2008 Cosmics files for Brunel.

# Syntax is:
#   gaudirun.py Brunel-Cosmics.py 2008-Cosmic-Data.py
#
from Gaudi.Configuration import *
from Configurables import Escher, LHCbApp

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest database tags for real data
#LHCbApp().DDDBtag   = "default"
#LHCbApp().CondDBtag = "default"
LHCbApp().DDDBtag   = "head-20090112"
LHCbApp().CondDBtag = "head-20090112"

# Latest cosmic run, with CALO, OT and (!!) RICH2 (35569 events)
Escher().DatasetName = 'FEST'
Escher().InputType = 'MDF'
#Escher().Simulation = True

# full data
data = ['PFN:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/FEST/FEST/44878/044878_0000000001.raw',
        'PFN:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/FEST/FEST/44878/044878_0000000002.raw']

# express stream
data = ['/castor/cern.ch/grid/lhcb/data/2009/RAW/EXPRESS/FEST/FEST/44878/044878_0000000002.raw']
 
EventSelector().Input = []
for d in data:
    name = "DATA='castor:" + d + "' SVC='LHCb::MDFSelector'"
    EventSelector().Input.append( name )
    
from Configurables import CondDB
CondDB().UseOracle = True
