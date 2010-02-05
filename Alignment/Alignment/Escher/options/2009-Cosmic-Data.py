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
    'PFN:/auto/data/krokovny/RAW/2009/058705.dst',
    'PFN:/auto/data/krokovny/RAW/2009/058709.dst',
    'PFN:/auto/data/krokovny/RAW/2009/058739.dst',
    'PFN:/auto/data/krokovny/RAW/2009/058744.dst',
    'PFN:/auto/data/krokovny/RAW/2009/059051.dst',
    'PFN:/auto/data/krokovny/RAW/2009/059052.dst',
    'PFN:/auto/data/krokovny/RAW/2009/059054.dst',
    'PFN:/auto/data/krokovny/RAW/2009/059075.dst',
    'PFN:/auto/data/krokovny/RAW/2009/059077.dst',
    'PFN:/auto/data/krokovny/RAW/2009/059081.dst',
    'PFN:/auto/data/krokovny/RAW/2009/059088.dst',
    'PFN:/auto/data/krokovny/RAW/2009/059092.dst',
    'PFN:/auto/data/krokovny/RAW/2009/059093.dst',
    'PFN:/auto/data/krokovny/RAW/2009/059096.dst',
    'PFN:/auto/data/krokovny/RAW/2009/059097.dst',
    'PFN:/auto/data/krokovny/RAW/2009/059195.dst',
    'PFN:/auto/data/krokovny/RAW/2009/059198.dst',
    'PFN:/auto/data/krokovny/RAW/2009/059306.dst',
    'PFN:/auto/data/krokovny/RAW/2009/059307.dst',
    'PFN:/auto/data/krokovny/RAW/2009/059565.dst',
    'PFN:/auto/data/krokovny/RAW/2009/059568.dst'
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

