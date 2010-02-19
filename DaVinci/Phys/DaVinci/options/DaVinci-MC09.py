########################################################################
#
# $Id: DaVinci-MC09.py,v 1.2 2010-02-19 17:03:59 pkoppenb Exp $
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
DaVinci().DataType = "2009" # Must be given
# DaVinci().Simulation   = False
# DaVinci().Input = []
# DaVinci().UserAlgorithms = []
