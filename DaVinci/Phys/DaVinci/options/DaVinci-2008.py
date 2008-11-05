########################################################################
#
# $Id: DaVinci-2008.py,v 1.2 2008-11-05 14:55:10 jpalac Exp $
#
# Options for a typical DaVinci job - this is to please Dirac
#
# @author Patrick Koppenburg
# @date 2008-10-24
#
########################################################################
from Gaudi.Configuration import *
from Davinci.Configuration import DavinciApp
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
DaVinciApp().mainOptions  = "$DAVINCIROOT/opitons/DaVinci.py"
DaVinciApp().applyConf()
##############################################################################
