
from Brunel.Configuration import *
from Configurables import Brunel, UpdateManagerSvc, CondDB, CondDBAccessSvc, LHCbApp

importOptions("CommonOptions.py")

CondDB().PartitionConnectionString["LHCBCOND"] = "sqlite_file:databases/LHCBCOND-AeroSubTiles-20110709.db/LHCBCOND"
CondDB().PartitionConnectionString["DDDB"]     = "sqlite_file:databases/DDDB-AeroSubTiles-20110709.db/DDDB"

#importOptions("$APPCONFIGOPTS/UseOracle.py")

#importOptions("/usera/jonesc/MyDataOptions/Collisions10/Raw/RUN_76997_RealData_90000000_RAW_PFNs.py")
importOptions("/usera/jonesc/MyDataOptions/Collisions11/Raw/RUN_91732_PFNs.py")

from Configurables import Brunel

Brunel().EvtMax    = 100
Brunel().PrintFreq = 1
