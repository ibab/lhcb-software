##############################################################################
# $Id: DVMonitorDst.py,v 1.1 2009-10-02 13:16:59 pkoppenb Exp $
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
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2009/RDST/00005288/0000/00005288_00000001_1.rdst' TYP='POOL_ROOTTREE' OPT='READ'"]
    
