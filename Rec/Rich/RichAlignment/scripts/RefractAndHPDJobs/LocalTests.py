
from Brunel.Configuration import *
from Configurables import Brunel, UpdateManagerSvc, CondDB, CondDBAccessSvc, LHCbApp

importOptions("CommonOptions.py")

#importOptions("$APPCONFIGOPTS/UseOracle.py")

#CondDB().PartitionConnectionString["LHCBCOND"] = "sqlite_file:databases/LHCBCOND-2001RePro.db/LHCBCOND"
#CondDB().PartitionConnectionString["DDDB"] = "sqlite_file:databases/DDDB-2001RePro.db/DDDB"

# For testing only
#CondDB().IgnoreHeartBeat = True

importOptions("/usera/jonesc/MyDataOptions/Collisions11/Raw/RUN_97487_PFNs.py")

from Configurables import Brunel

Brunel().EvtMax    = 100
Brunel().PrintFreq = 1
