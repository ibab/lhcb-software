"""
High level configuration tools for LHCb applications
"""
__version__ = "$Id: Configuration.py,v 1.1 2008-04-17 11:51:37 cattanem Exp $"
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
        }

    def getProp(self,name):
        if hasattr(self,name):
            return getattr(self,name)
        else:
            return self.getDefaultProperties()[name]

    def defineDB(self):
        condDBtag = self.getProp("condDBtag")
        DDDBtag   = self.getProp("DDDBtag")
        # For all DC06 cases, use latest DC06 tag
        if condDBtag.find("DC06") != -1 and DDDBtag.find("DC06") != -1 :
            importOptions( "$DDDBROOT/options/DC06.py" )
        else:
            CondDBAccessSvc( "DDDB",     DefaultTAG = condDBtag )
            CondDBAccessSvc( "LHCBCOND", DefaultTAG = condDBtag )
            #-- Always DC06 magnetic field for now....
            MagneticFieldSvc().FieldMapFile = os.environ['FIELDMAPROOT']+'/cdf/field047.cdf'

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

    def applyConf(self):
        self.defineDB()
        self.defineEvents()
