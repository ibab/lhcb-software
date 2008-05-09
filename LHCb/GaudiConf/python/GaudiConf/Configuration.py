"""
High level configuration tools for LHCb applications
"""
__version__ = "$Id: Configuration.py,v 1.4 2008-05-09 14:01:26 cattanem Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from os import environ
from Gaudi.Configuration import *
from Configurables import ( CondDBAccessSvc, MagneticFieldSvc )

class LHCbApp(ConfigurableUser):
    __slots__ = {
        "EvtMax":                -1 # Maximum number of events to process
       ,"skipEvents":             0 # events to skip
       ,"DDDBtag":    "DC06-latest" # geometry   database tag
       ,"condDBtag":  "DC06-latest" # conditions database tag
       ,"monitors"  :  []           # monitor actions
        }

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
        # For all DC06 cases, use latest DC06 tag
        if condDBtag.find("DC06") != -1 and DDDBtag.find("DC06") != -1 :
            importOptions( "$DDDBROOT/options/DC06.py" )
        # For all 2008 cases, except DC06, use latest 2008 global tag
        elif condDBtag.find("-2008") and DDDBtag.find("-2008") != -1 :
            importOptions( "$DDDBROOT/options/LHCb-2008.py" )
        else :
            CondDBAccessSvc( "DDDB",     DefaultTAG = condDBtag )
            CondDBAccessSvc( "LHCBCOND", DefaultTAG = condDBtag )
            #-- Default field map set by Fieldmap package
            MagneticFieldSvc().FieldMapFile = os.environ['FIELDMAP']

    def defineEvents(self):
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
        if "SC" in self.getProp("monitors"):
            ApplicationMgr().StatusCodeCheck = True

    def applyConf(self):
        self.defineDB()
        self.defineEvents()
        self.defineMonitors()
