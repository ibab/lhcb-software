from Gaudi.Configuration import *
from Configurables import LHCbApp
from GaudiConf import IOHelper

IOHelper("ROOT","ROOT").inputFiles(['PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/c/cattanem/Boole/v18r0/30000000-100ev-20090407-MC09.digi?svcClass=default'])


LHCbApp().EvtMax=1
LHCbApp().Persistency="ROOT"

ics=IOHelper("ROOT","ROOT").outputAlgs('testRootOP.dst')

ApplicationMgr().TopAlg+=ics


def names(streams):
    for stream in streams:
        print stream.Output

def myB4Action():
    print "Before, i.e. ROOT"
    print IOHelper().activeServices()
    names(IOHelper().activeStreams())
    print EventSelector().Input

def myAction():
    print "After, i.e. POOL"
    print IOHelper().activeServices()
    names(IOHelper().activeStreams())
    print EventSelector().Input

appendPostConfigAction(myB4Action)

IOHelper("POOL","POOL").postConfigServices()

appendPostConfigAction(myAction)

IOHelper().postConfigDebug()
