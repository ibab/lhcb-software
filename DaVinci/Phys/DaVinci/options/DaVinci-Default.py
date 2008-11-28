##############################################################################
#$Id: DaVinci-Default.py,v 1.5 2008-11-28 13:07:49 jpalac Exp $
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
# DaVinci().Simulation   = False
# DaVinci().MainOptions  = "$DAVINCIROOT/options/DaVinci.opts"
DaVinci().MainOptions  = "$DAVINCIROOT/tests/options/DVTestB2DiMuon.opts"
# DaVinci().Input = []
# DaVinci().UserAlgorithms = []
##############################################################################
