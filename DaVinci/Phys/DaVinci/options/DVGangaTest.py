########################################################################
#
# $Id: DVGangaTest.py,v 1.1 2008-12-05 11:39:27 pkoppenb Exp $
#
# Options for a typical DaVinci job for Ganga tests.
#
# Runs the stripping producing a DST called Presel_test.dst
#
# @author Patrick Koppenburg
# @date 2008-12-05
#
########################################################################
from Gaudi.Configuration import *
from DaVinci.Configuration import DaVinci
##############################################################################
#
#
##############################################################################
DaVinci().EvtMax = -1       # to be set by Ganga 
# DaVinci().DataType = "2008" # Default is "DC06" -  to be set by Ganga 
DaVinci().Simulation   = True
# DaVinci().Input = []
# DaVinci().UserAlgorithms = []
DaVinci().MainOptions  = "$DAVINCIROOT/options/DVDC06TestStripping.opts"
##############################################################################

