from DaVinci.Configuration import *
importOptions("$APPCONFIGOPTS/DisableLFC.py")
from Configurables import CondDB
CondDB().IgnoreHeartBeat=True
#CondDB().LocalTags["LHCBCOND"]=["lumi-20100825"]
DaVinci().EvtMax=-1
DaVinci().DataType="2010"
DaVinci().Simulation=False
#DaVinci().CondDBtag="head-20100730"
DaVinci().CondDBtag="head-20100826"
DaVinci().DDDBtag="head-20100518"
#ApplicationMgr().OutputLevel=4

