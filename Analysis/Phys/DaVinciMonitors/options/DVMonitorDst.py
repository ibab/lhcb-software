##############################################################################
#$Id: DVMonitorDst.py,v 1.1.1.1 2008-12-05 16:41:05 pkoppenb Exp $
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
# Most of this will be configured from Dirac
#
##############################################################################
DaVinci().EvtMax = -1
##############################################################################
#
# Monitoring algorithms
#
##############################################################################
importOptions( "$DAVINCIMONITORSROOT/options/JpsiMuonMonitor.py" )
##############################################################################
#
# Test data
#
##############################################################################

