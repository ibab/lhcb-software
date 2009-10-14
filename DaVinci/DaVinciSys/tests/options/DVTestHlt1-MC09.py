########################################################################
#
# $Id: DVTestHlt1-MC09.py,v 1.4 2009-10-14 14:33:40 pkoppenb Exp $
#
# Options for Hlt1 tests. Identical to Hlt1&2, except for type.
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
DaVinci().HltThresholdSettings = 'Physics_10000Vis_1000L0_40Hlt1_Apr09'  ## need to replace with MC09
