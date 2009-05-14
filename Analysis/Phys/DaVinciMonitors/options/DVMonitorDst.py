##############################################################################
# $Id: DVMonitorDst.py,v 1.16 2009-05-14 11:25:57 jonrob Exp $
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
DaVinci().EvtMax = 10000
DaVinci().DataType = "2008" # Default is "DC06"
DaVinci().Simulation = True

EventSelector().Input   = [ ]

#importOptions('/usera/jonesc/MyDataOptions/2008/Test3/13144002-DST-Local.py')

#importOptions('/usera/jonesc/MyDataOptions/DC09/Production/MC09-MinBias-WithMC-LocalPFNs.py')

importOptions('/usera/jonesc/MyDataOptions/DC09/Production/MC09-MinBias-WithMC-LFN.py')

#MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
