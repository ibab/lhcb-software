from Gaudi.Configuration import * 
# Defines standard conditions for DC06 production data

from Configurables import CondDBAccessSvc, MagneticFieldSvc
import os

# DC06 database tags
tag = "DC06-20081002"
CondDBAccessSvc("DDDB",DefaultTAG = tag)
CondDBAccessSvc("LHCBCOND",DefaultTAG = tag)

# Force negative polarity (i.e. don't take it from ONLINE)
MagneticFieldSvc().Polarity = -1
MagneticFieldSvc().OutputLevel = ERROR
