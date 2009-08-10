########################################################################
#
# $Id: DVTestHlt12-MC09-DEV.py,v 1.2 2009-08-10 16:52:55 pkoppenb Exp $
#
# Test for Hlt1&2. This should be identical to Hlt1 except for the HltType.
#
# The Hlt1 efficiency should be identical to the one in DVTestHlt1-2008.py
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
DaVinci().HltThresholdSettings = ''  # Explicitly default
