##############################################################################
# $Id: DVMonitorDst.py,v 1.2 2009-11-09 13:05:37 pkoppenb Exp $
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
DaVinci().EvtMax = -1
DaVinci().DataType = "MC09" # Default is "DC06"
DaVinci().Simulation = True
DaVinci().InputType = "RDST"
EventSelector().Input   = [
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2009/RDST/00004937/0000/00004937_00000001_1.rdst' TYP='POOL_ROOTTREE' OPT='READ'"]
    
