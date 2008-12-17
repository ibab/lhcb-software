########################################################################
#
# $Id: DaVinci-DC06TestStripping.py,v 1.1 2008-12-17 19:49:36 pkoppenb Exp $
#
# Options for a typical DaVinci job - this is to please Dirac
#
# @author Patrick Koppenburg
# @date 2008-10-24
#
########################################################################
from Configurables import DaVinci
##############################################################################
#
#
##############################################################################
DaVinci().EvtMax = -1
# DaVinci().SkipEvents = 0
DaVinci().DataType = "DC06" # Default is "DC06"
DaVinci().Simulation   = True
# DaVinci().Input = []
# DaVinci().UserAlgorithms = []
DaVinci().MainOptions  = "$DAVINCIROOT/options/DVDC06TestStripping.opts" # Default is ""
##############################################################################
