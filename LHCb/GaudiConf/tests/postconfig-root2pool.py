from Gaudi.Configuration import *
from Configurables import LHCbApp

EventSelector().Input=["DATAFILE='PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/c/cattanem/Boole/v18r0/30000000-100ev-20090407-MC09.digi?svcClass=default' SVC='Gaudi::RootEvtSelector' OPT='READ'"]

LHCbApp().EvtMax=1
LHCbApp().Persistency="ROOT"

from GaudiConf import IOHelper

def myAction():
    print IOHelper("ROOT","ROOT").activeServices()
    print EventSelector().Input
    IOHelper("ROOT","ROOT").changeServices()
    print IOHelper().activeServices()
    print EventSelector().Input
    IOHelper("POOL","POOL").changeServices()
    print IOHelper().activeServices()
    print EventSelector().Input
    #IOHelper().changeServices()
    #print IOHelper().activeServices()

def myPrint():
    print EventSelector().Input

appendPostConfigAction(myAction)

appendPostConfigAction(myPrint)
