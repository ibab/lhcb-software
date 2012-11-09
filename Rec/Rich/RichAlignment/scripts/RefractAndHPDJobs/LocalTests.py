
from Brunel.Configuration import *
from Configurables import Brunel, UpdateManagerSvc, CondDB, CondDBAccessSvc, LHCbApp

importOptions("CommonOptions.py")

CondDB().addLayer(CondDBAccessSvc("2012AerogelCalib-V3-20082012",ConnectionString="sqlite_file:databases/2012AerogelCalib-V3-20082012.db/LHCBCOND",DefaultTAG="HEAD"))
CondDB().addLayer(CondDBAccessSvc("MDCS-RICH1-26072012",ConnectionString="sqlite_file:databases/MDCS-RICH1-26072012.db/LHCBCOND",DefaultTAG="HEAD"))
CondDB().addLayer(CondDBAccessSvc("Tracking-v6.0series-15082012",ConnectionString="sqlite_file:databases/Tracking-v6.0series-15082012.db/LHCBCOND",DefaultTAG="HEAD"))
CondDB().addLayer(CondDBAccessSvc("2012MirrorAlign-20082012",ConnectionString="sqlite_file:databases/2012MirrorAlign-20082012.db/LHCBCOND",DefaultTAG="HEAD"))
CondDB().addLayer(CondDBAccessSvc("2012-RootFiles-RunAligned-Sobel-Smoothed1.5hours-HPDAlign-06082012",ConnectionString="sqlite_file:databases/2012-RootFiles-RunAligned-Sobel-Smoothed1.5hours-HPDAlign-06082012.db/LHCBCOND",DefaultTAG="HEAD"))
CondDB().addLayer(CondDBAccessSvc("2012-RootFiles-RunAligned-Sobel-Smoothed0.5hours-HPDOcc-06082012",ConnectionString="sqlite_file:databases/2012-RootFiles-RunAligned-Sobel-Smoothed0.5hours-HPDOcc-06082012.db/LHCBCOND",DefaultTAG="HEAD"))

# For testing only
#CondDB().IgnoreHeartBeat = True

importOptions("/usera/jonesc/MyDataOptions/Collisions12/Raw/RUN_111193_PFNs.py")
#importOptions("/usera/jonesc/MyDataOptions/Collisions11/Raw/RUN_97487_PFNs.py")

from Configurables import Brunel

Brunel().EvtMax    = 100
Brunel().PrintFreq = 1
