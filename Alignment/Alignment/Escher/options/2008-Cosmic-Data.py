from GaudiKernel.ProcessJobOptions import importOptions

from Configurables import (Escher, LHCbApp, ApplicationMgr, NTupleSvc, ATrackSelector)
from Gaudi.Configuration import FileCatalog, EventSelector


#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

Escher().DataType = "2008"
Escher().Simulation = False
Escher().SpecialData += ["fieldOff", "cosmics"]
Escher().skipBigCluster= True
Escher().InputType = 'DST'
Escher().Detectors = ["OT"]
Escher().EvtMax = 100
Escher().PrintFreq = 1000
Escher().AlignmentLevel = "layers"
Escher().Millepede = True
Escher().Kalman = False
Escher().Incident = "GlobalMPedeFit"
Escher().DatasetName = '2008-Cosmics'

ATrackSelector().UseWeights = False

#-- Use latest database tags for real data
LHCbApp().DDDBtag   = "head-20100119"
LHCbApp().CondDBtag = "head-20091112"


# Latest cosmic run, with CALO, OT and (!!) RICH2 (35569 events)

# wouter's dsts
data = [
    'PFN:/data/user/data/2008/RAW/LHCb/wouter/run34120.dst',
    'PFN:/data/user/data/2008/RAW/LHCb/wouter/run31225.dst',
    'PFN:/data/user/data/2008/RAW/LHCb/wouter/run31557.dst',
    'PFN:/data/user/data/2008/RAW/LHCb/wouter/run34083.dst',
    'PFN:/data/user/data/2008/RAW/LHCb/wouter/run34117.dst'
    ]

# do not do the TES check because these data do not have the required lists
#GaudiSequencer("InitReprocSeq").Enable = False

EventSelector().Input = []
for d in data:
    name = "DATAFILE='" + d + "' TYP='POOL_ROOTTREE' OPT='READ'" 
    EventSelector().Input.append( name )

# raw data
#Escher().InputType = 'MDF'
#data = [
#   'PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM/34120/034120_0000085567.raw' ]
#EventSelector().Input = []
#for d in data:
#    name = "DATAFILE='" + d + "' SVC='LHCb::MDFSelector'"
#    EventSelector().Input.append( name )
