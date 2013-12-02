from Gaudi.Configuration import *

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml",
                           "xmlcatalog_file:$BRUNELROOT/job/NewCatalog.xml" ]


from Configurables import Brunel
Brunel().DataType =  "2013"
Brunel().InputType = "DST"
file = "$QMTESTFILESROOT/data/136237_10ev_bug_svn164539.dst"
EventSelector().Input.append(file)
Brunel().SplitRawEventInput=2.0  # Stripping20

from Configurables import TrackSys

TrackSys().GlobalCuts = { 'Velo':80000, 'IT':999999, 'OT':999999 }

from Configurables import RichRecSysConf
rConf = RichRecSysConf("RichOfflineRec")
rConf.pixelConfig().MaxPixels      = 30000
rConf.trackConfig().MaxInputTracks = 99999
rConf.trackConfig().MaxUsedTracks  = 10000 # this used to be 1000
rConf.photonConfig().MaxPhotons    = 900000 # this used to be 250000
rConf.gpidConfig().MaxUsedPixels   = 90000 # this used to be 30000
rConf.gpidConfig().MaxUsedTracks   = 10000 # this used to be 1000
