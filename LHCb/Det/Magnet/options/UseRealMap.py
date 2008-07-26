from Gaudi.Configuration import * 
# Options to switch on measured magnetic field

from Configurables import MagneticFieldSvc
import os

MagneticFieldSvc().FieldMapFiles = [
    os.path.join( "$FIELDMAPROOT", "cdf", "field048.c1.vp.cdf" ),
    os.path.join( "$FIELDMAPROOT", "cdf", "field048.c2.vp.cdf" ),
    os.path.join( "$FIELDMAPROOT", "cdf", "field048.c3.vp.cdf" ),
    os.path.join( "$FIELDMAPROOT", "cdf", "field048.c4.vp.cdf" ) ]
