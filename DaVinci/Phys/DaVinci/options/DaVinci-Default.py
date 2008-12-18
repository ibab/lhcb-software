##############################################################################
#$Id: DaVinci-Default.py,v 1.8 2008-12-18 09:57:52 pkoppenb Exp $
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
DaVinci().MainOptions  = "$DAVINCIROOT/tests/options/DVTestB2DiMuon.opts" # Default is ""
# DaVinci().Input = []
# DaVinci().UserAlgorithms = []
##############################################################################
