#-- GAUDI jobOptions generated on Fri Aug  1 14:53:07 2014
#-- Contains event types : 
#--   90000000 - 2 files - 35137 events - 3.62 GBytes


#--  Extra information about the data processing phases:

from Gaudi.Configuration import * 
from GaudiConf import IOHelper
IOHelper('ROOT').inputFiles(['LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004271_1.full.dst',
'LFN:/lhcb/LHCb/Collision12/FULL.DST/00020736/0000/00020736_00004272_1.full.dst'
], clear=True)

FileCatalog().Catalogs = [ 'xmlcatalog_file:$STRIPPINGSELECTIONSROOT/tests/data/Reco14_Run130395.xml' ]
