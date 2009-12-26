# Example 2008 Cosmics files for Brunel.

# Syntax is:
#   gaudirun.py Brunel-Cosmics.py 2008-Cosmic-Data.py
#
from Gaudi.Configuration import *
from Configurables import Escher, LHCbApp, TrackSys

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest database tags for real data
LHCbApp().DDDBtag   = "default"
LHCbApp().CondDBtag = "default"

#LHCbApp().DDDBtag   = "head-20090330"
#LHCbApp().CondDBtag = "head-20090330"

# Latest cosmic run, with CALO, OT and (!!) RICH2 (35569 events)
Escher().DatasetName = 'Cosmics'

EventSelector().PrintFreq = 100
Escher().PrintFreq = 100
from Configurables import CountingPrescaler
CountingPrescaler("EscherPrescaler").OutputLevel = 5

# do not do the TES check because these data do not have the required lists
GaudiSequencer("InitReprocSeq").Enable = False

TrackSys().SpecialData += ['fieldOff','cosmics']

# raw data
#Escher().InputType = 'MDF'
#data = [
#   'PFN:castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/COSMICS/58739/058739_0000000001.raw',
#   'PFN:castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/COSMICS/58739/058739_0000000002.raw']

#data = [
#   'PFN:castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/COSMICS/59306/059306_0000000001.raw']
#EventSelector().Input = []
#for d in data:
#    name = "DATAFILE='" + d + "' SVC='LHCb::MDFSelector'"
#    EventSelector().Input.append( name )

# wouter's dsts
Escher().InputType = 'DST'
data = [
'run34120',
'run31225',
'run31557',
'run34083'
    ]

EventSelector().Input = []
for d in data:
    name = "DATAFILE='file:/data/data/" + d + ".dst' TYP='POOL_ROOTTREE' OPT='READ'" 
    EventSelector().Input.append( name )


from Configurables import (OTMultiBXRawBankDecoder)
OTMultiBXRawBankDecoder().RawEventLocations = ['DAQ/RawEvent',
                                               'Prev1/DAQ/RawEvent',
                                               'Next1/DAQ/RawEvent',
                                               'Prev2/DAQ/RawEvent',
                                               'Next2/DAQ/RawEvent']

