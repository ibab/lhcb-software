#-- GAUDI jobOptions generated on Tue Feb  9 15:43:54 2010
#-- Contains event types : 
#--   90000000 - 6 files - 381107 events - 8.04 GBytes

from Gaudi.Configuration import * 

EventSelector().Input   = [
"   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005848/0000/00005848_00000001_1.V0.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005848/0000/00005848_00000002_1.V0.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005848/0000/00005848_00000003_1.V0.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005848/0000/00005848_00000004_1.V0.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005848/0000/00005848_00000005_1.V0.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005848/0000/00005848_00000006_1.V0.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'"]
FileCatalog().Catalogs = [ 'xmlcatalog_file:2009-RealData-Reco07V0.xml' ]
