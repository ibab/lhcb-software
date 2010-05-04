# Example 2008 Cosmics files for Brunel.

# Syntax is:
#   gaudirun.py Brunel-Cosmics.py 2008-Cosmic-Data.py
#
from Gaudi.Configuration import *
from Configurables import (Escher, TAlignment, LHCbApp, ATrackSelector)


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
Escher().SpecialData += ["fieldOff", "veloOpen", "earlyData"]
Escher().InputType = "DST"
Escher().EvtMax = 100
Escher().DatasetName = "63596_1"

TAlignment().TrackLocation = "Event/Alignment/OTTracks"
ATrackSelector().UseWeights = True


# add 2009 collision data files
EventSelector().Input = [
    "DATAFILE='file:/lhcb/data/2009/DST/00005843/0000/00005843_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"
#    "DATAFILE='file:/data/data/2009/DST/00005714/0000/00005714_00000001_1.dst' SVC='LHCb::MDFSelector'",
#    "DATAFILE='file:/media/disk-2/data/2009/DST/00005738/0000/00005738_00000001_1.dst' SVC='LHCb::MDFSelector'"
    ]

