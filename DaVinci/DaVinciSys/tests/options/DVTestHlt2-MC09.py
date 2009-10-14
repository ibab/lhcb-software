########################################################################
#
# $Id: DVTestHlt2-MC09.py,v 1.2 2009-10-14 14:33:40 pkoppenb Exp $
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
DaVinci().Hlt = True            ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
