"""
High level configuration tools for LHCb applications
"""
__version__ = "$Id: Configuration.py,v 1.12 2008-11-11 16:28:21 cattanem Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from os import environ
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import ( DDDBConf )

class LHCbApp(LHCbConfigurableUser):
    __slots__ = {
        "EvtMax"     : -1
       ,"skipEvents" : 0
       ,"DDDBtag"    : "2008-default"
       ,"condDBtag"  : "2008-default"
       ,"UseOracle"  : False
       ,"Simulation" : False
       ,"monitors"   : []
        }

    _propertyDocDct = { 
        'EvtMax'     : """ Maximum number of events to process """
       ,'skipEvents' : """ Number of events to skip """
       ,'DDDBtag'    : """ Tag to use for DDDB. Default '2008-default' """
       ,'condDBtag'  : """ Tag to use for CondDB. Default '2008-default' """
       ,'UseOracle'  : """ Flag to enable Oracle CondDB. Default False (use SQLDDDB) """
       ,'Simulation' : """ Flag to indicate usage of simulation conditions """
       ,'monitors'   : """ List of monitors to execute """
       }

    __used_configurables__ = [ DDDBConf ]

    def knownMonitors(self):
        return ["SC", "FPE"]

    def defineDB(self):
        DDDBConf().Simulation = self.getProp("Simulation")
        DDDBConf().UseOracle  = self.getProp("UseOracle")

        condDBtag = self.getProp("condDBtag")
        DDDBtag   = self.getProp("DDDBtag")

        # If a default is requested, use it
        if DDDBtag.find("-default") != -1 or condDBtag.find("-default") != -1:
            if condDBtag.find("DC06") != -1 and DDDBtag.find("DC06") != -1 :
                DDDBConf().DataType = "DC06"
            elif condDBtag.find("2008") != -1 and DDDBtag.find("2008") != -1 :
                DDDBConf().DataType = "2008"
            else :
                raise RuntimeError("Invalid combination of default tags. CondDB: '%s' DDDB: '%s'"%
                                   (condDBtag,DDDBtag) )

        # Otherwise, take the tag supplied
        from Configurables import CondDB
        if DDDBtag.find("-default") == -1:
            CondDB().Tags [ "DDDB" ] = DDDBtag
        if condDBtag.find("-default") == -1:
            CondDB().Tags [ "LHCBCOND" ] = condDBtag
            CondDB().Tags [ "SIMCOND"  ] = condDBtag
            
    def defineEvents(self):
        # Set up transient store and data on demand service
        EventDataSvc( ForceLeaves        = True,
                      RootCLID           =    1,
                      EnableFaultHandler = True )

        skipEvents = self.getProp("skipEvents")
        if skipEvents > 0 :
            if hasattr(EventSelector(),"FirstEvent"):
                log.warning( "EventSelector().FirstEvent and LHCBApp().skipEvents both defined, using LHCbApp().skipEvents")
            EventSelector().FirstEvent = skipEvents + 1

        # Delegate handling to ApplicationMgr configurable
        self.setOtherProps(ApplicationMgr(),["EvtMax"])

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
