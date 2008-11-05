##############################################################################
#$Id: DaVinci-Default.py,v 1.1 2008-11-05 14:50:06 jpalac Exp $
#
# syntax: gaudirun.py DaVinci-Default.py
#
# Author: Juan Palacios <juan.palacios@nikhef.nl>
#
##############################################################################
from DaVinci.Configuration import *
##############################################################################
#
#
##############################################################################
DaVinciApp().EvtMax = 500
# DaVinciApp().skipEvents = 0
# DaVinciApp().DDDBtag      = "DC06-default"
# DaVinciApp().condDBtag    = "DC06-default"
# DaVinciApp().useOracleCondDB = False
# DaVinciApp().mainOptions  = "$DAVINCIROOT/options/DaVinci.opts"
DaVinciApp().mainOptions  = "$DAVINCIROOT/tests/options/DVTestB2DiMuon.py"
DaVinciApp().applyConf()
##############################################################################
