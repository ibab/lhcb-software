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

# Latest cosmic run, with CALO, OT and (!!) RICH2 (35569 events)
Escher().DatasetName = 'Cosmics'

# wouter's dsts
Escher().InputType = 'DST'
data = [
    'PFN:castor:/castor/cern.ch/user/w/wouter/otcosmics/run34083.dst',
    'PFN:castor:/castor/cern.ch/user/w/wouter/otcosmics/run34117.dst',
    'PFN:castor:/castor/cern.ch/user/w/wouter/otcosmics/run34120.dst'
    ]

EventSelector().Input = []
for d in data:
    name = "DATAFILE='" + d + "' TYP='POOL_ROOTTREE' OPT='READ'" 
    EventSelector().Input.append( name )

EventSelector().PrintFreq = 100
Escher().PrintFreq = 100

# do not do the TES check because these data do not have the required lists
GaudiSequencer("InitReprocSeq").Enable = False

# raw data
#Escher().InputType = 'MDF'
#data = [
#   'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/34120/034120_0000085567.raw' ]
#EventSelector().Input = []
#for d in data:
#    name = "DATAFILE='" + d + "' SVC='LHCb::MDFSelector'"
#    EventSelector().Input.append( name )
