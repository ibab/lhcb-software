##############################################################################
# $Id: DVMonitorDst.py,v 1.15 2009-04-16 07:20:16 pkoppenb Exp $
#
# syntax: gaudirun.py $DAVINCIMONITORSROOT/options/DVMonitorDst.py
#
# Author: Patrick Koppenburg <patrick.koppenburg@cern.ch>
#
##############################################################################
from DaVinci.Configuration import *
from Gaudi.Configuration import *
##############################################################################
#
# the stuff
#
importOptions( "$DAVINCIMONITORSROOT/options/DaVinciMonitoring.py") 
##############################################################################
#
# Histograms
#
DaVinci().HistogramFile = "DVMonitors.root"
##############################################################################
#
# Most of this will be configured from Dirac
#
##############################################################################
DaVinci().EvtMax = 1000
DaVinci().DataType = "2008" # Default is "DC06"
DaVinci().Simulation = True
