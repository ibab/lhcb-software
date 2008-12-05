##############################################################################
#$Id: DaVinci-Default.py,v 1.6 2008-12-05 11:31:02 jpalac Exp $
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
# DaVinci().MainOptions  = "$DAVINCIROOT/options/DaVinci.opts" # Default is ""
DaVinci().MainOptions  = "$DAVINCIROOT/tests/options/DVTestB2DiMuon.opts" # Default is ""
# DaVinci().Input = []
# DaVinci().UserAlgorithms = []
##############################################################################
