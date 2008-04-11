from Gaudi.Configuration import * 
# Defines standard conditions for DC06 production data

from Configurables import CondDBAccessSvc, MagneticFieldSvc
import os

# DC06 database tags
tag = "DC06-20080407"
CondDBAccessSvc("DDDB",DefaultTAG = tag)
CondDBAccessSvc("LHCBCOND",DefaultTAG = tag)

# DC06 magnetic field
MagneticFieldSvc(FieldMapFile = 
                 os.path.join(os.environ["FIELDMAPROOT"],"cdf","field047.cdf"))
