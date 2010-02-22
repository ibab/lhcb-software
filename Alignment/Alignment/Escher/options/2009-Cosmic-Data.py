# Example 2008 Cosmics files for Brunel.

# Syntax is:
#   gaudirun.py Brunel-Cosmics.py 2008-Cosmic-Data.py
#
from Gaudi.Configuration import *
from Configurables import (Escher, LHCbApp, TrackSys, ATrackSelector)

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest database tags for real data
#LHCbApp().DDDBtag   = "head-20090330"
#LHCbApp().CondDBtag = "head-20090402"

LHCbApp().DDDBtag   = "head-20100119"
LHCbApp().CondDBtag = "head-20091112"


# Latest cosmic run, with CALO, OT and (!!) RICH2 (35569 events)

from Configurables import CountingPrescaler
CountingPrescaler("EscherPrescaler").OutputLevel = 5

# do not do the TES check because these data do not have the required lists
GaudiSequencer("InitReprocSeq").Enable = False

Escher().DataType = "2009"
Escher().Simulation = False
Escher().SpecialData += ["fieldOff", "cosmics"]
Escher().skipBigCluster= True
Escher().InputType = 'DST'
Escher().Detectors = ["OT"]
Escher().EvtMax = 1000
Escher().AlignmentLevel = "layers"
Escher().Millepede = True
Escher().Kalman = False
Escher().Incident = "GlobalMPedeFit"
Escher().DatasetName = "Cosmics"

ATrackSelector().UseWeights = False

ApplicationMgr().ExtSvc +=  [ "NTupleSvc" ]
tupleFile = "2009-Cosmic.root"
tuple = "FILE1 DATAFILE='"+tupleFile+"' TYP='ROOT' OPT='NEW'"
NTupleSvc().Output = [ tuple ]

data = [
    '58739','58744',
    '59051','59052',
    '59054','59075',
    '59077','59081',
    '59088','59092',
    '59093','59096',
    '59097','59195',
    '59198','59306',
    '59307','59565',
    '59568'
    ]


EventSelector().Input = []
for d in data:
    name = "DATAFILE='" + d + "' TYP='POOL_ROOTTREE' OPT='READ'" 
    EventSelector().Input.append( name )



#from Configurables import (OTMultiBXRawBankDecoder)
#OTMultiBXRawBankDecoder().RawEventLocations = ['DAQ/RawEvent',
#                                               'Prev1/DAQ/RawEvent',
#                                               'Next1/DAQ/RawEvent',
#                                               'Prev2/DAQ/RawEvent',
#                                               'Next2/DAQ/RawEvent']

