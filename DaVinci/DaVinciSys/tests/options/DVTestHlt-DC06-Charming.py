########################################################################
#
# $Id: DVTestHlt-DC06-Charming.py,v 1.2 2009-10-14 14:33:40 pkoppenb Exp $
#
# Options for a typical DaVinci job
#
# @author Patrick Koppenburg
# @date 2008-08-06
#
########################################################################
from Gaudi.Configuration import *
importOptions( "$DAVINCISYSROOT/tests/options/DVTestHlt-DC06-Effective.py")
#
# now overwrite threshold setting
#
from Configurables import DaVinci
DaVinci().HltThresholdSettings = 'Physics_10000Vis_1000L0_40Hlt1_CharmingHlt2_Jul09'
