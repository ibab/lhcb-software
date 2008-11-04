"""
High level configuration tools for DaVinci
At the moment this doesn't do anything. Waiting for re-structuring of
configuratables.
"""
__version__ = "$Id: Configuration.py,v 1.2 2008-11-04 11:21:36 jpalac Exp $"
__author__ == "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *

class DaVinci(LHCbConfigurableUser) :
    __slots__ = {
        "EvtMax"          :  -1, # Maximum number of events to process
        "skipEvents"      :   0,     # events to skip
        "mainOpts"        : '$DAVINCIROOT/options/DaVinci.py',
        "DDDBtag"         : 'DC06-default', #
        "CondDBtag"       : 'DC06-default',
        "useOracleCondDB" : False
        }

    def defineEvents(self):
        evtMax = self.getProp("EvtMax")
        if hasattr(LHCbApp(),"EvtMax"):
            print "# LHCbApp().EvtMax already defined, ignoring Boole().EvtMax"
        else:
            LHCbApp().EvtMax = evtMax

        skipEvents = self.getProp("skipEvents")
        if skipEvents > 0 :
            if hasattr(LHCbApp(),"skipEvents"):
                print "# LHCbApp().skipEvents already defined, ignoring Boole().skipEvents"
            else:
                LHCbApp().skipEvents = skipEvents

    def defineDB(self):
        # Prefer DaVinci default over LHCbApp default if not set explicitly
        self.setProp( "condDBtag", self.getProp("condDBtag") )
        self.setProp( "DDDBtag", self.getProp("DDDBtag") )
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["condDBtag","DDDBtag","useOracleCondDB"]) 

    def hepMCBackComp(self)
        # Special options for DC06 data processing
        if LHCbApp().getProp("DDDBtag").find("DC06") != -1 :
            ApplicationMgr().Dlls += [ "HepMCBack" ]

    def applyConf(self):
        GaudiKernel.ProcessJobOptions.PrintOff()
        importOptions( self.getProp( "mainOptions" ) )
        self.defineEvents()
        self.hepMCBack()
        LHCbApp().applyConf()
