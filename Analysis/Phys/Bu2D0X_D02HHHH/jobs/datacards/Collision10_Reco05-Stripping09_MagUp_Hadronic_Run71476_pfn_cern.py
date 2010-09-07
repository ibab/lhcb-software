#-- GAUDI jobOptions generated on Mon Aug 30 06:28:41 2010
#-- Contains event types : 
#--   90000000 - 4 files - 179414 events - 19.48 GBytes

from Gaudi.Configuration import * 

EventSelector().Input   = [
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/HADRONIC.DST/00007581/0000/00007581_00000003_1.hadronic.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/HADRONIC.DST/00007581/0000/00007581_00000005_1.hadronic.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/HADRONIC.DST/00007581/0000/00007581_00000006_1.hadronic.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/HADRONIC.DST/00007581/0000/00007581_00000007_1.hadronic.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'"]
FileCatalog().Catalogs = ['xmlcatalog_file:Collision10_Reco05-Stripping09_MagUp_Hadronic_Run71476_pfn_cern.xml' ]
