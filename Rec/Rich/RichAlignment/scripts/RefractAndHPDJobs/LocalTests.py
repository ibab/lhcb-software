
from Brunel.Configuration import *
from Configurables import Brunel, UpdateManagerSvc, CondDB, CondDBAccessSvc, LHCbApp

importOptions("CommonOptions.py")

#CondDB().PartitionConnectionString["LHCBCOND"] = "sqlite_file:databases/LHCBCOND-AeroSubTiles-20110709.db/LHCBCOND"
#CondDB().PartitionConnectionString["DDDB"]     = "sqlite_file:databases/DDDB-AeroSubTiles-20110709.db/DDDB"

#importOptions("$APPCONFIGOPTS/UseOracle.py")

# For testing only
CondDB().IgnoreHeartBeat = True

importOptions("/usera/jonesc/MyDataOptions/Collisions11/Raw/RUN_97487_PFNs.py")

from Configurables import Brunel

Brunel().EvtMax    = 100
Brunel().PrintFreq = 1
