########################################################################
#
# $Id: DVTestHlt1-MC09.py,v 1.3 2009-08-10 16:52:55 pkoppenb Exp $
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
DaVinci().HltType = 'Hlt1'             ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
