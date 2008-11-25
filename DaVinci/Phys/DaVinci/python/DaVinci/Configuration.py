"""
High level configuration tools for DaVinci
At the moment this doesn't do anything. Waiting for re-structuring of
configuratables.
"""
__version__ = "$Id: Configuration.py,v 1.7 2008-11-25 14:44:13 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
import GaudiKernel.ProcessJobOptions

class DaVinciApp(LHCbConfigurableUser) :
    __slots__ = {
        "EvtMax"          :  -1, # Maximum number of events to process
        "SkipEvents"      :   0,     # events to skip
        "mainOptions"     : '$DAVINCIROOT/options/DaVinci.py',
        "DDDBtag"         : '', #
        "CondDBtag"         : '', #
        "DataType"       : 'DC06',
        "UseOracle"       : False,
        "Input"           : [],
        "userAlgorithms"  : []
        }

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
        self.setOtherProps(LHCbApp(),["DataType","CondDBtag","DDDBtag","UseOracle"]) 

    def hepMCBackComp(self) :
        # Special options for DC06 data processing
        if LHCbApp().getProp("DDDBtag").find("DC06") != -1 :
            ApplicationMgr().Dlls += [ "HepMCBack" ]

    def applyConf(self):
#        GaudiKernel.ProcessJobOptions.PrintOff()
        importOptions( self.getProp( "mainOptions" ) )
        self.defineEvents()
        self.defineInput()
        self.defineDB()
        self.hepMCBackComp()
        for alg in self.getProp("userAlgorithms"):
            ApplicationMgr().TopAlg += [ alg ]
        LHCbApp().applyConf()
