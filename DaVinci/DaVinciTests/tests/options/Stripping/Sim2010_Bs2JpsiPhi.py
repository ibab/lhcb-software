#-- GAUDI jobOptions generated on Mon Jan 17 17:13:58 2011
#-- Contains event types : 
#--   13144006 - 1 files - 10000 events - 4.13 GBytes

from Gaudi.Configuration import * 

EventSelector().Input   = [
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/2010/DST/00008405/0000/00008405_00000506_1.dst?svcClass=lhcbdata' TYP='POOL_ROOTTREE' OPT='READ'"]
FileCatalog().Catalogs = [ 'xmlcatalog_file:$DAVINCITESTSROOT/tests/options/Stripping/Sim2010_Bs2JpsiPhi.xml' ]
