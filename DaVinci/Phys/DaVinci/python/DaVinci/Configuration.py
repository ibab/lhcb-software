"""
High level configuration tools for DaVinci
"""
__version__ = "$Id: Configuration.py,v 1.10 2008-11-28 13:05:47 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
import GaudiKernel.ProcessJobOptions

class DaVinci(LHCbConfigurableUser) :
    __slots__ = {
        "EvtMax"          :  -1,
        "SkipEvents"      :   0,
        "DataType"        : 'DC06', # Data type, can be ['DC06','2008']
        "Simulation"      : True,  # set to True to use SimCond
        "MainOptions"     : '$DAVINCIROOT/options/DaVinci.py',
        "Input"           : [],
        "UserAlgorithms"  : []
        }

    __used_configurables__ = [ LHCbApp ]


    def defineEvents(self):
        evtMax = self.getProp("EvtMax")
        print "Re-defining EvtMax to ", evtMax
        ApplicationMgr().EvtMax = evtMax
        skipEvents = self.getProp("SkipEvents")
        if skipEvents > 0 :
            ApplicationMgr().SkipEvents = skipEvents

    def defineInput(self):
        input = self.getProp("Input")
        print "DaVinciApp input is ", input
        if ( len(input) > 0) :
            print "Re-defining input to\n", input
            EventSelector().Input = input
        else :
            print "Input is\n", getattr(EventSelector(),"Input")

    def evtMax(self):
        if hasattr(ApplicationMgr(),"EvtMax"):
            return getattr(ApplicationMgr(),"EvtMax")
        else:
            return ApplicationMgr().getDefaultProperties()["EvtMax"]

    def defineDB(self):
        self.setOtherProps(LHCbApp(),["DataType","Simulation"]) 
            
    def __apply_configuration__(self):
        GaudiKernel.ProcessJobOptions.PrintOff()
        importOptions( self.getProp( "MainOptions" ) )
        self.defineEvents()
        self.defineInput()
        self.defineDB()
        for alg in self.getProp("UserAlgorithms"):
            ApplicationMgr().TopAlg += [ alg ]
