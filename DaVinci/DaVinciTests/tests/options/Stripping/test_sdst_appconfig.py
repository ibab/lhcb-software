from GaudiConf.Configuration import *
importOptions('$APPCONFIGOPTS/DaVinci/StrippingLinesPrescales-1003.py')
importOptions('$APPCONFIGOPTS/DaVinci/DVStrippingDST-RealData.py')
importOptions('$APPCONFIGOPTS/DaVinci/DataType-2010.py')
importOptions('$APPCONFIGOPTS/DaVinci/InputType-SDST.py')
from Configurables import DaVinci
DaVinci().EvtMax=-1
EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/c/cattanem/testFiles/Brunel-v37r1-069857_0000000006-1000ev.sdst' TYP='POOL_ROOTTREE' OPT='READ'"]
FileCatalog().Catalogs =['xmlcatalog_file:../options/TestSDSTCatalog.xml']
