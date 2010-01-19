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

LHCbApp().DDDBtag   = "head-20090330"
LHCbApp().CondDBtag = "head-20090402"

# Latest cosmic run, with CALO, OT and (!!) RICH2 (35569 events)

# wouter's dsts
data = [
    'PFN:/data/user/data/2008/RAW/LHCb/wouter/run34120.dst',
    'PFN:/data/user/data/2008/RAW/LHCb/wouter/run31225.dst',
    'PFN:/data/user/data/2008/RAW/LHCb/wouter/run31557.dst',
    'PFN:/data/user/data/2008/RAW/LHCb/wouter/run34083.dst',
    'PFN:/data/user/data/2008/RAW/LHCb/wouter/run34117.dst'
    ]

EventSelector().Input = []
for d in data:
    name = "DATAFILE='" + d + "' TYP='POOL_ROOTTREE' OPT='READ'" 
    EventSelector().Input.append( name )

EventSelector().PrintFreq = 1000
Escher().PrintFreq = 1000
Escher().skipBigCluster= True
Escher().DatasetName = 'Cosmics'
Escher().InputType = 'DST'
Escher().PrintFreq = 1000
Escher().SpecialData += ["fieldOff", "cosmics"]
Escher().TrackContainer = "Rec/Track/Seed"
Escher().Simulation = False


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
