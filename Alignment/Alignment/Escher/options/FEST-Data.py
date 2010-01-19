# Example 2008 Cosmics files for Brunel.

# Syntax is:
#   gaudirun.py Brunel-Cosmics.py 2008-Cosmic-Data.py
#
from Gaudi.Configuration import *
from Configurables import Escher, LHCbApp

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest database tags for real data
LHCbApp().DDDBtag   = "default"
LHCbApp().CondDBtag = "default"
#LHCbApp().DDDBtag   = "head-20090112"
#LHCbApp().CondDBtag = "head-20090112"

# Latest cosmic run, with CALO, OT and (!!) RICH2 (35569 events)
Escher().DatasetName = 'MC'
Escher().InputType = 'digi'
#Escher().Simulation = True

# full data
data = [ '/data/user/data/Alignment/Boole_450GeV_Boff_Beam12_CloseVelo_H_1000_93.digi']

# express stream

EventSelector().Input = []
for d in data:
    name = "DATA='file:" + d + "'  TYP='POOL_ROOTTREE' OPT='READ'"
    EventSelector().Input.append( name )
    
#from Configurables import CondDB
#CondDB().UseOracle = True
