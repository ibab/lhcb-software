from Gaudi.Configuration import * 
# Defines standard conditions for DC06 production data

from Configurables import CondDBAccessSvc, MagneticFieldSvc

# DC06 database tags
tag = "DC06-repro0710"
CondDBAccessSvc("DDDB",DefaultTAG = tag)
CondDBAccessSvc("LHCBCOND",DefaultTAG = tag)

# DC06 magnetic field
MagneticFieldSvc(FieldMapFile = "$FIELDMAPROOT/cdf/field047.cdf")

