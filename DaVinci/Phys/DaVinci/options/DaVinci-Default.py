##############################################################################
#$Id: DaVinci-Default.py,v 1.4 2008-11-27 15:35:53 jpalac Exp $
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
DaVinci().EvtMax = 500
# DaVinci().SkipEvents = 0
# DaVinci().DataType = "DC06"
# DaVinci().DDDBtag      = ""
# DaVinci().CondDBtag    = ""
# DaVinci().UseOracle    = False
# DaVinci().Simulation   = False
# DaVinci().MainOptions  = "$DAVINCIROOT/options/DaVinci.opts"
DaVinci().MainOptions  = "$DAVINCIROOT/tests/options/DVTestB2DiMuon.opts"
# DaVinci().Input = []
# DaVinci().UserAlgorithms = []
##############################################################################
