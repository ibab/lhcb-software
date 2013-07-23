
from Brunel.Configuration import *
from Configurables import Brunel, UpdateManagerSvc, CondDB, CondDBAccessSvc, LHCbApp

importOptions("CommonOptions.py")

CondDB().addLayer(CondDBAccessSvc("2011-Tracking",ConnectionString="sqlite_file:databases/Tracking_v6.2series_20110425_20111031.db/LHCBCOND",DefaultTAG="HEAD"))

# For testing only
#CondDB().IgnoreHeartBeat = True

#importOptions("/usera/jonesc/MyDataOptions/Collisions12/Raw/RUN_111193_PFNs.py")
importOptions("/usera/jonesc/MyDataOptions/Collisions11/Raw/RUN_97487_PFNs.py")

from Configurables import Brunel

Brunel().EvtMax    = 100
Brunel().PrintFreq = 1
