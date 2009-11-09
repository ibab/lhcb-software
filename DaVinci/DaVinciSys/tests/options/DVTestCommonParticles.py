##############################################################################
# $Id: DVTestCommonParticles.py,v 1.2 2009-11-09 13:05:37 pkoppenb Exp $
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
# CommonParticles
# Thanks to Vanya for this trick
#
from CommonParticles import StandardBasic
from CommonParticles import StandardIntermediate
from CommonParticles.Utils import particles  # particles() method returns DoD dictionary
List = []
for a,b in particles().iteritems():
    List.append(b)
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
DaVinci().UserAlgorithms = List
DaVinci().EvtMax = 1000
DaVinci().DataType = "MC09" # Default is "DC06"
DaVinci().Simulation = True
DaVinci().InputType = "RDST"
EventSelector().Input   = [
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2009/RDST/00004937/0000/00004937_00000001_1.rdst' TYP='POOL_ROOTTREE' OPT='READ'"]
    
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
