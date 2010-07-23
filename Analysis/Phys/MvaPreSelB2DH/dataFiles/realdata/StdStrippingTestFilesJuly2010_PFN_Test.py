#-- GAUDI jobOptions generated on Tue Jun 15 17:55:13 2010
#-- Contains event types : 
#--  for stripping selection tests

from Gaudi.Configuration import * 

EventSelector().Input   = [
"   DATAFILE='PFN:/scratch/seaso/lhcb/DavinciInput/realdata/StrippingTest/00006563_00002181_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:/scratch/seaso/lhcb/DavinciInput/realdata/StrippingTest/00006563_00002182_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:/scratch/seaso/lhcb/DavinciInput/realdata/StrippingTest/00006563_00002183_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:/scratch/seaso/lhcb/DavinciInput/realdata/StrippingTest/00006563_00002199_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'"]

FileCatalog().Catalogs= ["xmlcatalog_file:pool_xml_catalog.xml"]

