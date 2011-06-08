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
from CommonParticles import StandardIntermediate
#from CommonParticles.Utils import particles  # particles() method returns DoD dictionary
List = []
Locations = []
for a,b in StandardIntermediate.locations.iteritems():
    print "DVTestCommonParticles adding location", a
    List.append(b)                      # ist of algorithms
    Locations.append(a)

from Configurables import CountParticles
CP = CountParticles(Inputs = Locations)
##############################################################################
#
# Histograms
#
DaVinci().HistogramFile = "DVStandardIntermediate.root"
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

#importOptions("../dimuon_testfile.py")

#EventSelector().Input   = [
#    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2009/RDST/00004937/0000/00004937_00000001_1.rdst' TYP='POOL_ROOTTREE' OPT='READ'"]
    
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
