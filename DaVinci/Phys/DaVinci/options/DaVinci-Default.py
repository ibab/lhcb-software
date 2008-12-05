##############################################################################
#$Id: DaVinci-Default.py,v 1.7 2008-12-05 15:45:43 jpalac Exp $
#
# syntax: gaudirun.py DaVinci-Default.py
#
# Author: Juan Palacios <juan.palacios@nikhef.nl>
#
##############################################################################
from Configurables import DaVinci
##############################################################################
#
#
##############################################################################
DaVinci().EvtMax = 500 # Default is -1
# DaVinci().SkipEvents = 0
# DaVinci().DataType = "DC06"
# DaVinci().Simulation   = False
# DaVinci().MainOptions  = "$DAVINCIROOT/options/DaVinci.opts" # Default is ""
DaVinci().MainOptions  = "$DAVINCIROOT/tests/options/DVTestB2DiMuon.opts" # Default is ""
# DaVinci().Input = []
# DaVinci().UserAlgorithms = []
##############################################################################
