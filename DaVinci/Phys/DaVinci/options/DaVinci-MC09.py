########################################################################
#
# $Id: DaVinci-MC09.py,v 1.1 2009-07-01 12:16:53 pkoppenb Exp $
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
DaVinci().EvtMax = 500
# DaVinci().SkipEvents = 0
DaVinci().DataType = "MC09" # Default is "DC06"
# DaVinci().Simulation   = False
# DaVinci().Input = []
# DaVinci().UserAlgorithms = []
# DaVinci().MainOptions  = "$DAVINCIROOT/options/DaVinci.py" # Default is ""
##############################################################################
