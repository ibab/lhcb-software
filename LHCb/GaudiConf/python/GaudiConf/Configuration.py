"""
High level configuration tools for LHCb applications
"""
__version__ = "$Id: Configuration.py,v 1.10 2008-07-25 12:53:19 cattanem Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from os import environ
from Gaudi.Configuration import *
from Configurables import ( CondDBAccessSvc, MagneticFieldSvc )

class LHCbApp(ConfigurableUser):
    __slots__ = {
        "EvtMax":                -1  # Maximum number of events to process
       ,"skipEvents":             0  # events to skip
       ,"DDDBtag":    "DC06-default" # geometry   database tag
       ,"condDBtag":  "DC06-default" # conditions database tag
       ,"useOracleCondDB":    False  # if False, use SQLDDDB instead
       ,"monitors"  :  []            # monitor actions
        }

    def knownMonitors(self):
        return ["SC", "FPE"]

    def getProp(self,name):
        if hasattr(self,name):
            return getattr(self,name)
        else:
            return self.getDefaultProperties()[name]

    def setProp(self,name,value):
        return setattr(self,name,value)

    def defineDB(self):
        condDBtag = self.getProp("condDBtag")
        DDDBtag   = self.getProp("DDDBtag")
            
        # If a default is requested, use it
        if DDDBtag.find("-default") != -1 or condDBtag.find("-default") != -1:
            if condDBtag.find("DC06") != -1 and DDDBtag.find("DC06") != -1 :
                importOptions( "$DDDBROOT/options/DC06.py" )
            elif condDBtag.find("2008") != -1 and DDDBtag.find("2008") != -1 :
                importOptions( "$DDDBROOT/options/LHCb-2008.py" )
            else :
                raise RuntimeError("Invalid combination of default tags. CondDB: '%s' DDDB: '%s'"%(condDBtag,DDDBtag))

        # Otherwise, take the tag supplied
        if DDDBtag.find("-default") == -1:
            CondDBAccessSvc( "DDDB",     DefaultTAG = DDDBtag )
        if condDBtag.find("-default") == -1:
            CondDBAccessSvc( "LHCBCOND", DefaultTAG = condDBtag )
            CondDBAccessSvc( "SIMCOND",  DefaultTAG = condDBtag )
            
    def defineEvents(self):
        # Set up transient store and data on demand service
        EventDataSvc( ForceLeaves        = True,
                      RootCLID           =    1,
                      EnableFaultHandler = True )

        skipEvents = self.getProp("skipEvents")
        if skipEvents > 0 :
            if hasattr(EventSelector(),"FirstEvent"):
                print "EventSelector().FirstEvent already defined, ignoring LHCbApp().skipEvents"
            else:
                EventSelector().FirstEvent = skipEvents + 1
        evtMax = self.getProp("EvtMax")
        if hasattr(ApplicationMgr(),"EvtMax"):
            print "ApplicationMgr().EvtMax already defined, ignoring LHCbApp().EvtMax"
        else:
            ApplicationMgr().EvtMax = evtMax

    def defineMonitors(self):
        for prop in self.getProp("monitors"):
            if prop not in self.knownMonitors():
                raise RuntimeError("Unknown monitor '%s'"%prop)
        if "SC" in self.getProp("monitors"):
            ApplicationMgr().StatusCodeCheck = True
        if "FPE" in self.getProp("monitors"):
            importOptions( "$STDOPTS/FPEAudit.opts" )

    def applyConf(self):
        self.defineDB()
        self.defineEvents()
        self.defineMonitors()
