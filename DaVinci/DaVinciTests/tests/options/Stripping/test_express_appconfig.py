from Gaudi.Configuration import *
importOptions("$APPCONFIGOPTS/DaVinci/DVMonitor-RealData.py")
importOptions("$APPCONFIGOPTS/DaVinci/DataType-2010.py")
from Configurables import DaVinci
DaVinci().EvtMax=1000
EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/DST/00006390/0000/00006390_00000005_2.dst' TYP='POOL_ROOTTREE' OPT='READ'"]
