from Gaudi.Configuration import * 
# Defines standard conditions for DC06 production data

from Configurables import CondDBAccessSvc, MagneticFieldSvc
import os

# DC06 database tags
tag = "DC06-20080725"
CondDBAccessSvc("DDDB",DefaultTAG = tag)
CondDBAccessSvc("LHCBCOND",DefaultTAG = tag)
