
from Brunel.Configuration import *
from Configurables import Brunel, UpdateManagerSvc, CondDB, CondDBAccessSvc, LHCbApp

importOptions("CommonOptions.py")

#CondDB().addLayer(CondDBAccessSvc("Alignment2015",ConnectionString="sqlite_file:databases/Alignment2015.db/LHCBCOND",DefaultTAG="HEAD"))
#CondDB().addLayer(CondDBAccessSvc("MDCS-RICH1-SecondScans-26052015",ConnectionString="sqlite_file:databases/MDCS-RICH1-SecondScans-26052015.db/LHCBCOND",DefaultTAG="HEAD"))
#CondDB().addLayer(CondDBAccessSvc("ResetHPDAlign-13062015",ConnectionString="sqlite_file:databases/ResetHPDAlign-13062015.db/LHCBCOND",DefaultTAG="HEAD"))
#CondDB().addLayer(CondDBAccessSvc("2015-MirrorAlign-V2-It3-15062015",ConnectionString="sqlite_file:databases/2015-MirrorAlign-V2-It3-15062015.db/LHCBCOND",DefaultTAG="HEAD"))

CondDB().addLayer(CondDBAccessSvc("2015-refscale-V1",ConnectionString="sqlite_file:databases/2015-refscale-V1.db/CALIBOFF",DefaultTAG="HEAD"))

# For testing only
#CondDB().IgnoreHeartBeat = True

#importOptions("/usera/jonesc/MyDataOptions/Collisions12/Raw/RUN_111193_PFNs.py")
#importOptions("/usera/jonesc/MyDataOptions/Collisions11/Raw/RUN_97487_PFNs.py")
importOptions("/usera/jonesc/MyDataOptions/Collisions15em/Raw/RUN_154873_PFNs.py")

from Configurables import Brunel

Brunel().EvtMax    = 100
Brunel().PrintFreq = 10
