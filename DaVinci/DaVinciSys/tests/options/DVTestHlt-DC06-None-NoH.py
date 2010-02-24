########################################################################
#
# $Id: DVTestHlt-DC06-None-NoH.py,v 1.4 2010-02-24 15:07:43 pkoppenb Exp $
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
DaVinci().EvtMax = 200                       # Number of events 
DaVinci().HistogramFile = "" # to make it crash
