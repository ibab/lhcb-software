########################################################################
#
# $Id: DaVinci-2008.py,v 1.5 2008-11-27 14:42:37 jpalac Exp $
#
# Options for a typical DaVinci job - this is to please Dirac
#
# @author Patrick Koppenburg
# @date 2008-10-24
#
########################################################################
from Gaudi.Configuration import *
from DaVinci.Configuration import DaVinciApp
##############################################################################
#
#
##############################################################################
DaVinciApp().EvtMax = 500
# DaVinciApp().SkipEvents = 0
DaVinciApp().DataType = "2008" # Default is "DC06"
# DaVinciApp().DDDBtag      = ""
# DaVinciApp().CondDBtag    = "" 
# DaVinciApp().UseOracle = False
# DaVinciApp().Simulation   = False
# DaVinciApp().Input = []
# DaVinciApp().UserAlgorithms = []
DaVinciApp().MainOptions  = "$DAVINCIROOT/options/DaVinci.py"
##############################################################################
