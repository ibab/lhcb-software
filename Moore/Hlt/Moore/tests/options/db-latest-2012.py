from Gaudi.Configuration import *
from Configurables import LHCbApp, CondDB, DDDBConf

dataType="2012"

DDDBConf(DataType=dataType)
CondDB(IgnoreHeartBeat=True)

if "UseLatestTags" in CondDB().__slots__:
    CondDB().UseLatestTags=[dataType]
else:
    CondDB().useLatestTags(DataType=dataType, OnlyGlobalTags=False)
