##############################################################################
# $Id: DVMonitorDst.py,v 1.14 2009-02-26 17:11:09 pkoppenb Exp $
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
# Count Events
#
from Configurables import EventCountHisto
DaVinci().MoniSequence += [EventCountHisto("DaVinciMonitor") ] 
##############################################################################
#
# Get right selection @todo does not work yet 
#
# from Configurables import LoKi__HDRFilter   as HltDecReportsFilter
# HltDecReportsFilter  ( 'myname', Code = "HLT_PASS('somedecisionname')" )
#
#
# Imported stuff
#
importOptions( "$DAVINCIMONITORSROOT/options/Jpsi2MuPi.py") 
importOptions( "$DAVINCIMONITORSROOT/options/RichCalib.py") 
importOptions( "$DAVINCIMONITORSROOT/options/MuonPidJpCalib.py") 
importOptions( "$DAVINCIMONITORSROOT/options/MuonPidLaCalib.py") 
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
