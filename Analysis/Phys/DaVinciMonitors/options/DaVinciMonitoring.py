##############################################################################
# $Id: DaVinciMonitoring.py,v 1.1 2009-04-16 07:20:16 pkoppenb Exp $
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
