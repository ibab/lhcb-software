
##############################################################################
from Configurables import DaVinci, LHCbApp
from Gaudi.Configuration import *
##############################################################################

importOptions("$APPCONFIGOPTS/DaVinci/DVMonitor-RealData.py")

#importOptions("$APPCONFIGOPTS/DaVinci/DataType-2010.py")

importOptions("$APPCONFIGOPTS/DaVinci/DataType-2011.py")

DaVinci().EvtMax = 1000

DaVinci().InputType = 'DST'

DaVinci().PrintFreq = 100

MessageSvc().Format = "% F%30W%S%7W%R%T %0W%M"

LHCbApp().DDDBtag   = "head-20110722" 
LHCbApp().CondDBtag = "head-20110722"
