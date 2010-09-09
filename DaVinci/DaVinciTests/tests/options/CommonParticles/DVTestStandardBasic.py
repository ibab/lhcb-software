##############################################################################
# $Id: DVTestCommonParticles.py,v 1.3 2010/01/06 08:51:50 pkoppenb Exp $
#
# syntax: gaudirun.py $DAVINCIMONITORSROOT/options/DVMonitorDst.py
#
# Author: Patrick Koppenburg <patrick.koppenburg@cern.ch>
#
##############################################################################
from DaVinci.Configuration import DaVinci
from Gaudi.Configuration import *
##############################################################################
#
# CommonParticles
# Thanks to Vanya for this trick
#
from CommonParticles import StandardBasic
#from CommonParticles import StandardIntermediate
#from CommonParticles.Utils import particles  # particles() method returns DoD dictionary
List = []
Locations = []
for a,b in StandardBasic.locations.iteritems():
    print "DVTestCommonParticles adding location", a
    List.append(b)                      # ist of algorithms
    Locations.append(a.split('/')[1])   # whatever comes after Phys/

from Configurables import CountParticles
CP = CountParticles(InputLocations = Locations)
##############################################################################
#
# Histograms
#
DaVinci().HistogramFile = "DVStandardBasic.root"
##############################################################################
#
# Most of this will be configured from Dirac
#
##############################################################################
# DaVinci().UserAlgorithms = List   # explicitly run them
DaVinci().UserAlgorithms = [ CP ]   # count them all
DaVinci().EvtMax = 500
DaVinci().DataType = "2010"
DaVinci().Simulation = False
DaVinci().InputType = "DST"
EventSelector().Input   = [
    "   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00007557/0000/00007557_00000996_1.dimuon.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
    "   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00007557/0000/00007557_00001004_1.dimuon.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
    "   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00007557/0000/00007557_00001008_1.dimuon.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'",
    "   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00007557/0000/00007557_00001009_1.dimuon.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'"]

#EventSelector().Input   = [
#    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2009/RDST/00004937/0000/00004937_00000001_1.rdst' TYP='POOL_ROOTTREE' OPT='READ'"]
    
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
