from Gaudi.Configuration import * 
# Options to switch on measured magnetic field

from Configurables import MagneticFieldSvc
import os

MagneticFieldSvc().UseRealMap = True
MagneticFieldSvc().FieldMapFileQ1 = os.path.join( "$FIELDMAPROOT", "cdf", "field048.c1.vp.cdf" )
MagneticFieldSvc().FieldMapFileQ2 = os.path.join( "$FIELDMAPROOT", "cdf", "field048.c2.vp.cdf" )
MagneticFieldSvc().FieldMapFileQ3 = os.path.join( "$FIELDMAPROOT", "cdf", "field048.c3.vp.cdf" )
MagneticFieldSvc().FieldMapFileQ4 = os.path.join( "$FIELDMAPROOT", "cdf", "field048.c4.vp.cdf" )
