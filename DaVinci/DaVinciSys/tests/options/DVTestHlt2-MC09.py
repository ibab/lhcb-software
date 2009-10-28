########################################################################
#
# $Id: DVTestHlt2-MC09.py,v 1.3 2009-10-28 17:41:57 pkoppenb Exp $
#
# Options for Hlt2 tests. Identical to Hlt1&2, except for type.
#
# @author Patrick Koppenburg
# @date 2008-08-06
#
########################################################################
from Gaudi.Configuration import *
importOptions("$DAVINCISYSROOT/tests/options/DVTestHlt12-MC09.py")
########################################################################
#
# HLT
#
from Configurables import DaVinci
DaVinci().Hlt = True            
DaVinci().HltThresholdSettings = 'Physics_320Vis_300L0_10Hlt1_Hlt2_Oct09'  ## 
DaVinci().Hlt2Requires = 'L0'                                              # ignore Hlt1
