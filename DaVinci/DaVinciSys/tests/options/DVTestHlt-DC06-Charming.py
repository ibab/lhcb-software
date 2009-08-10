########################################################################
#
# $Id: DVTestHlt-DC06-Charming.py,v 1.1 2009-08-10 15:08:02 pkoppenb Exp $
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
DaVinci().HltThresholdSettings = 'Charming_Nominal'
