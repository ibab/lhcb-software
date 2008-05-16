from Gaudi.Configuration import * 
# Defines default conditions for 2008 LHCb detector

from Configurables import CondDBAccessSvc, MagneticFieldSvc
import os

# 2008 database tags
tag = "head-20080516"
CondDBAccessSvc("DDDB",DefaultTAG = tag)
CondDBAccessSvc("LHCBCOND",DefaultTAG = tag)

# Measured magnetic field
MagneticFieldSvc().UseRealMap = True
MagneticFieldSvc().FieldMapFileQ1 =  os.path.join(os.environ["FIELDMAPROOT"],"cdf","field048.c1.vp.cdf")
MagneticFieldSvc().FieldMapFileQ2 =  os.path.join(os.environ["FIELDMAPROOT"],"cdf","field048.c2.vp.cdf")
MagneticFieldSvc().FieldMapFileQ3 =  os.path.join(os.environ["FIELDMAPROOT"],"cdf","field048.c3.vp.cdf")
MagneticFieldSvc().FieldMapFileQ4 =  os.path.join(os.environ["FIELDMAPROOT"],"cdf","field048.c4.vp.cdf")
