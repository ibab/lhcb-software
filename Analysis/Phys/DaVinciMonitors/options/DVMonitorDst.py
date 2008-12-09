##############################################################################
# $Id: DVMonitorDst.py,v 1.3 2008-12-09 10:18:01 pkoppenb Exp $
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
DaVinci().EvtMax = 1000
DaVinci().DataType = "2008" # Default is "DC06"
DaVinci().Simulation = True
DaVinci().MainOptions  = "$DAVINCIMONITORSROOT/options/JpsiMuonMonitor.py"

DaVinci().Input   = [
 "DATAFILE='PFN:castor:/castor/cern.ch/user/t/truf/MC2008/mbias_2008.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
##############################################################################
#
# Monitoring algorithms
#
##############################################################################
# importOptions( "$DAVINCIMONITORSROOT/options/JpsiMuonMonitor.py" )
##############################################################################
#
# Histograms
#
HistogramPersistencySvc().OutputFile = "DVMonitors.root"
