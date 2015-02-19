from Gaudi.Configuration import *

from Configurables import MagneticFieldSvc
# MBXW compensator
mbxwh = MagneticFieldSvc("MBXWHFieldSvc")
# MBXWS correctors
mbxwsL = MagneticFieldSvc("MBXWSLFieldSvc")
mbxwsR = MagneticFieldSvc("MBXWSRFieldSvc")

mbxwh.UseConditions = False
mbxwsL.UseConditions = False
mbxwsR.UseConditions = False

mbxwh.FieldMapFiles = ["$FIELDMAPROOT/cdf/compensator.MBXWH.001.cdf"]
mbxwsL.FieldMapFiles = ["$FIELDMAPROOT/cdf/compensator.MBXWS.1L8.001.cdf"]
mbxwsR.FieldMapFiles = ["$FIELDMAPROOT/cdf/compensator.MBXWS.1R8.001.cdf"]

mbxwsL.ForcedSignedCurrentScaling = -1.

from Configurables import MultipleMagneticFieldSvc
MultipleMagneticFieldSvc().MagneticFieldServices += ["MagneticFieldSvc/MBXWHFieldSvc", \
                                                     "MagneticFieldSvc/MBXWSLFieldSvc", \
                                                     "MagneticFieldSvc/MBXWSRFieldSvc"]

