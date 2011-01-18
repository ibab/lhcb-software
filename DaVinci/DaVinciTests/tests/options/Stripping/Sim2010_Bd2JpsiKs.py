#-- GAUDI jobOptions generated on Mon Jan 17 17:06:59 2011
#-- Contains event types : 
#--   11144103 - 1 files - 10000 events - 4.10 GBytes

from Gaudi.Configuration import * 

EventSelector().Input   = [
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/2010/DST/00008414/0000/00008414_00000106_1.dst?svcClass=lhcbdata' TYP='POOL_ROOTTREE' OPT='READ'"]
FileCatalog().Catalogs = [ 'xmlcatalog_file:Sim2010_Bd2JpsiKs.xml' ]
