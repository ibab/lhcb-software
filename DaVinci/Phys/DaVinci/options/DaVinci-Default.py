##############################################################################
#$Id: DaVinci-Default.py,v 1.3 2008-11-27 14:42:37 jpalac Exp $
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
# DaVinciApp().SkipEvents = 0
# DaVinciApp().DataType = "DC06"
# DaVinciApp().DDDBtag      = ""
# DaVinciApp().CondDBtag    = ""
# DaVinciApp().UseOracle    = False
# DaVinciApp().Simulation   = False
# DaVinciApp().MainOptions  = "$DAVINCIROOT/options/DaVinci.opts"
DaVinciApp().MainOptions  = "$DAVINCIROOT/tests/options/DVTestB2DiMuon.opts"
# DaVinciApp().Input = []
# DaVinciApp().UserAlgorithms = []
##############################################################################
