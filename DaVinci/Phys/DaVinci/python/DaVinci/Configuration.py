"""
High level configuration tools for DaVinci
"""
__version__ = "$Id: Configuration.py,v 1.8 2008-11-27 14:45:51 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
import GaudiKernel.ProcessJobOptions

class DaVinciApp(LHCbConfigurableUser) :
    __slots__ = {
        "EvtMax"          :  -1,
        "SkipEvents"      :   0,
        "DataType"        : 'DC06', # Data type, can be ['DC06','2008']
        "DDDBtag"         : '',     # Tag for DDDB. Default as set in DDDBConf for DataType
        "CondDBtag"       : '',     # Tag for CondDB. Default as set in DDDBConf for DataType
        "UseOracle"       : False,
        "Simulation"      : False,  # set to True to use SimCond
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
        self.setOtherProps(LHCbApp(),["DataType","CondDBtag","DDDBtag","UseOracle", "Simulation"]) 
        # Special options for DC06 data processing
        if self.getProp("DataType") == "DC06" :
            LHCbApp().Simulation = True
            
    def hepMCBackComp(self) :
        # Special options for DC06 data processing
        if self.getProp("DataType").find("DC06") != -1 :
            ApplicationMgr().Dlls += [ "HepMCBack" ]

    def __apply_configuration__(self):
        GaudiKernel.ProcessJobOptions.PrintOff()
        importOptions( self.getProp( "MainOptions" ) )
        self.defineEvents()
        self.defineInput()
        self.defineDB()
        self.hepMCBackComp()
        for alg in self.getProp("UserAlgorithms"):
            ApplicationMgr().TopAlg += [ alg ]
        LHCbApp().applyConf()
