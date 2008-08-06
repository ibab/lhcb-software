### @file 
 #  Test file for HLT
 #
 #  @author P. Koppenburg
 #  @date 2006-02-01
 #
 ####################################################
from Gaudi.Configuration import *
from Configurables import Z0Checker, HltBackgroundCategory
importOptions ("$HLTSELECTIONSROOT/options/DVTestHlt2.py")
#
# background category
#
ApplicationMgr().TopAlg += [ HltBackgroundCategory() ]
# HltBackgroundCategory().PrintTree = TRUE
#
# various instances of Z0 checker
#
Z0 = GaudiSequencer("Z0")
ApplicationMgr().TopAlg += [ Z0 ]  
Z0.Members += [ Z0Checker("OffNoIDZ0Checker") ]  
Z0Checker("OffNoIDZ0Checker").Locations = [ "/Event/Phys/StdNoPIDsMuons/Particles" ]
Z0.Members += [ Z0Checker("OffMuZ0Checker") ]  
Z0Checker("OffMuZ0Checker").Locations = [ "/Event/Phys/StdLooseMuons/Particles" ]
Z0.Members += [ Z0Checker("HltPiZ0Checker") ]  
Z0Checker("HltPiZ0Checker").Locations = [ "/Event/Phys/Hlt2Pions/Particles" ]
Z0.Members += [ Z0Checker("HltMuZ0Checker") ]  
Z0Checker("HltMuZ0Checker").Locations = [ "/Event/Phys/Hlt2Muons/Particles" ]

MessageSvc().Format = "% F%50W%S%7W%R%T %0W%M"

ApplicationMgr().EvtMax = -1 
EventSelector().PrintFreq = 1 

EventSelector().Input   = [
  "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001929/DST/0000/00001929_00000001_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
  "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001929/DST/0000/00001929_00000002_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
  "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001929/DST/0000/00001929_00000006_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
  "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001929/DST/0000/00001929_00000007_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
  "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001929/DST/0000/00001929_00000008_5.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
]
