# Test unpacking of a packed DST. Currently only tests tracks, to be extended...
from Gaudi.Configuration import *
from Configurables import DumpTracks, DstConf

ApplicationMgr().TopAlg += [ DumpTracks(OutputLevel = DEBUG) ]

ApplicationMgr().EvtMax      = 5
DataOnDemandSvc().OutputLevel = WARNING

# Set up POOL persistency
importOptions( "$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")

FileCatalog().Catalogs = ["xmlcatalog_file:MyCatalog.xml"]

# Unpack the packed containers on demand
ApplicationMgr().ExtSvc += [ "DataOnDemandSvc" ]
EventDataSvc().EnableFaultHandler = True
DstConf().EnableUnpack = True

EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/c/cattanem/Brunel/v34r1p1/11144101-100ev-20090112.dst' TYP='POOL_ROOTTREE' OPT='READ'"]
