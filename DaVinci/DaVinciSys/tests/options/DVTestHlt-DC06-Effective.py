########################################################################
#
# $Id: DVTestHlt-DC06-Effective.py,v 1.6 2009-10-16 08:39:00 pkoppenb Exp $
#
# Options for a typical DaVinci job
#
# @author Patrick Koppenburg
# @date 2008-08-06
#
########################################################################
from Gaudi.Configuration import *
importOptions( "$DAVINCISYSROOT/tests/options/DVTestHlt-DC06-None.py")
#
# now overwrite threshold setting
#
from Configurables import DaVinci
DaVinci().HltThresholdSettings = 'Physics_10000Vis_1000L0_40Hlt1_EffectiveHlt2_Jul09'

