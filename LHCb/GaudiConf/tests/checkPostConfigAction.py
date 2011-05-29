from Gaudi.Configuration import *
from Configurables import LHCbApp

LHCbApp()

from GaudiConf import IOHelper

def myAction():
    print IOHelper().activeServices()
    IOHelper().changeServices()
    print IOHelper().activeServices()
    #IOHelper('ROOT','ROOT').changeServices()
    #print IOHelper().activeServices()
    #IOHelper().changeServices()
    #print IOHelper().activeServices()


appendPostConfigAction(myAction)
