"""
High level configuration tools for LHCb applications
"""
__version__ = "$Id: Configuration.py,v 1.18 2009-01-21 17:08:51 cattanem Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from os import environ
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import ( DDDBConf )

class LHCbApp(LHCbConfigurableUser):
    __slots__ = {
        "EvtMax"     : -1
       ,"SkipEvents" : 0
       ,"DataType"   : "2008"
       ,"DDDBtag"    : ""
       ,"CondDBtag"  : ""
       ,"UseOracle"  : False
       ,"Simulation" : False
       ,"Monitors"   : []
        }

    _propertyDocDct = { 
        'EvtMax'     : """ Maximum number of events to process """
       ,'SkipEvents' : """ Number of events to skip """
       ,'DataType'   : """ Data type, can be ['DC06','2008']. Default '2008' """
       ,'DDDBtag'    : """ Tag for DDDB. Default as set in DDDBConf for DataType """
       ,'CondDBtag'  : """ Tag for CondDB. Default as set in DDDBConf for DataType """
       ,'UseOracle'  : """ Flag to enable Oracle CondDB. Default False (use SQLDDDB) """
       ,'Simulation' : """ Flag to indicate usage of simulation conditions """
       ,'Monitors'   : """ List of monitors to execute """
       }

    __used_configurables__ = [ DDDBConf ]

    def knownMonitors(self):
        return ["SC", "FPE"]

    def knownAuditors(self):
        return ["NameAuditor","MemoryAuditor","ChronoAuditor"]

    def defineDB(self):
        # Delegate handling of properties to DDDBConf
        self.setOtherProps( DDDBConf(), ["Simulation", "UseOracle", "DataType" ] )
        # CondDB tags must be set
        from Configurables import CondDB
        tagsOK = True
        if hasattr( self, "DDDBtag" ):
            CondDB().Tags [ "DDDB" ] = self.getProp("DDDBtag")
        else:
            log.error( "DDDBtag property has not been set in the options" )
            tagsOK = False
        if hasattr( self, "CondDBtag" ):
            CondDB().Tags [ "LHCBCOND" ] = self.getProp("CondDBtag")
            CondDB().Tags [ "SIMCOND"  ] = self.getProp("CondDBtag")
        else:
            log.error( "CondDBtag property has not been set in the options" )
            tagsOK = False
        if not tagsOK : sys.exit(1)
            
    def defineEvents(self):
        # Set up transient store and data on demand service
        EventDataSvc( ForceLeaves        = True,
                      RootCLID           =    1,
                      EnableFaultHandler = True )

        SkipEvents = self.getProp("SkipEvents")
        if SkipEvents > 0 :
            if hasattr(EventSelector(),"FirstEvent"):
                log.warning( "EventSelector().FirstEvent and LHCBApp().SkipEvents both defined, using LHCbApp().SkipEvents")
            EventSelector().FirstEvent = SkipEvents + 1

        # Delegate handling to ApplicationMgr configurable
        self.setOtherProps(ApplicationMgr(),["EvtMax"])

    def evtMax(self):
        if hasattr(ApplicationMgr(),"EvtMax") and not hasattr(self,"EvtMax"):
            return ApplicationMgr().getProp("EvtMax")
        else:
            return self.getProp("EvtMax")

    def skipEvents(self):
        if hasattr(EventSelector(),"FirstEvent") and not hasattr(self,"SkipEvents"):
            return EventSelector().getProp("FirstEvent") - 1
        else:
            return self.getProp("SkipEvents")

    def defineMonitors(self):
        for prop in self.getProp("Monitors"):
            if prop not in self.knownMonitors():
                if prop in self.knownAuditors():
                    from Configurables import AuditorSvc
                    AuditorSvc().Auditors.append( prop )
                    theConf = getConfigurable(prop)
                    theConf.Enable = True
                else:
                    raise RuntimeError("Unknown monitor '%s'"%prop)
        if "SC" in self.getProp("Monitors"):
            ApplicationMgr().StatusCodeCheck = True
        if "FPE" in self.getProp("Monitors"):
            importOptions( "$STDOPTS/FPEAudit.opts" )

    def __apply_configuration__(self):
        self.defineDB()
        self.defineEvents()
        self.defineMonitors()
