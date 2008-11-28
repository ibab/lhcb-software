########################################################################
#
# $Id: DaVinci-2008.py,v 1.7 2008-11-28 13:07:49 jpalac Exp $
#
# Options for a typical DaVinci job - this is to please Dirac
#
# @author Patrick Koppenburg
# @date 2008-10-24
#
########################################################################
from Gaudi.Configuration import *
from DaVinci.Configuration import DaVinci
##############################################################################
#
#
##############################################################################
DaVinci().EvtMax = 500
# DaVinci().SkipEvents = 0
DaVinci().DataType = "2008" # Default is "DC06"
# DaVinci().Simulation   = False
# DaVinci().Input = []
# DaVinci().UserAlgorithms = []
DaVinci().MainOptions  = "$DAVINCIROOT/options/DaVinci.py"
##############################################################################
