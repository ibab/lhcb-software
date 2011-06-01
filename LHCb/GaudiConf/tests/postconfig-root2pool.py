from Gaudi.Configuration import *
from Configurables import LHCbApp, InputCopyStream

EventSelector().Input=["DATAFILE='PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/c/cattanem/Boole/v18r0/30000000-100ev-20090407-MC09.digi?svcClass=default' SVC='Gaudi::RootEvtSelector' OPT='READ'"]



LHCbApp().EvtMax=1
LHCbApp().Persistency="ROOT"

ics=OutputStream("ICS")
ics.Output="DATAFILE='PFN:testRootOP.dst' SVC='Gaudi::RootEvtSelector' OPT='REC'"

ApplicationMgr().TopAlg+=[ics]

def names(streams):
    for stream in streams:
        print stream.Output

from GaudiConf import IOHelper

def myAction():
    print IOHelper("ROOT","ROOT").activeServices()
    print EventSelector().Input
    names(IOHelper().activeStreams())
    IOHelper("ROOT","ROOT").changeServices()
    print IOHelper().activeServices()
    print EventSelector().Input
    names(IOHelper().activeStreams())
    IOHelper("POOL","POOL").changeServices()
    print IOHelper().activeServices()
    print EventSelector().Input
    names(IOHelper().activeStreams())
    #IOHelper().changeServices()
    #print IOHelper().activeServices()

def myPrint():
    print EventSelector().Input
    names(IOHelper().activeStreams())

appendPostConfigAction(myAction)

appendPostConfigAction(myPrint)
