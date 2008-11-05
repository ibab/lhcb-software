########################################################################
#
# $Id: DaVinci-2008.py,v 1.3 2008-11-05 14:57:32 jpalac Exp $
#
# Options for a typical DaVinci job - this is to please Dirac
#
# @author Patrick Koppenburg
# @date 2008-10-24
#
########################################################################
from Gaudi.Configuration import *
from DaVinci.Configuration import DaVinciApp
##############################################################################
#
#
##############################################################################
DaVinciApp().EvtMax = 500
# DaVinciApp().skipEvents = 0
# DaVinciApp().DDDBtag      = "DC06-default"
# DaVinciApp().condDBtag    = "DC06-default"
# DaVinciApp().useOracleCondDB = False
# DaVinciApp().Input = []
# DaVinciApp().mainOptions  = "$DAVINCIROOT/options/DaVinci.opts"
DaVinciApp().mainOptions  = "$DAVINCIROOT/options/DaVinci.py"
DaVinciApp().applyConf()
##############################################################################
