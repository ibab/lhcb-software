"""
High level configuration tools for LHCb applications
"""
__version__ = "$Id: Configuration.py,v 1.21 2009-06-25 15:16:34 jonrob Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from os import environ
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import ( DDDBConf )

class LHCbApp(LHCbConfigurableUser):
    __slots__ = {
        "EvtMax"      : -1
       ,"SkipEvents"  : 0
       ,"DataType"    : "2008"
       ,"DDDBtag"     : ""
       ,"CondDBtag"   : ""
       ,"Simulation"  : False
       ,"Monitors"    : []
       ,"OutputLevel" : INFO 
       ,"TimeStamp"   : False
        }

    _propertyDocDct = { 
        'EvtMax'      : """ Maximum number of events to process """
       ,'SkipEvents'  : """ Number of events to skip """
       ,'DataType'    : """ Data type, can be ['DC06','2008']. Default '2008' """
       ,'DDDBtag'     : """ Tag for DDDB. Default as set in DDDBConf for DataType """
       ,'CondDBtag'   : """ Tag for CondDB. Default as set in DDDBConf for DataType """
       ,'Simulation'  : """ Flag to indicate usage of simulation conditions """
       ,'Monitors'    : """ List of monitors to execute """
       ,'OutputLevel' : """ The printout level to use (default INFO) """
       ,'TimeStamp'   : """ Flag to add a time stamp to messages (default False) """
       }

    __used_configurables__ = [ DDDBConf ]

    def knownMonitors(self):
        return ["SC", "FPE"]

    def knownAuditors(self):
        return ["NameAuditor","MemoryAuditor","ChronoAuditor"]

    def defineDB(self):
        # Delegate handling of properties to DDDBConf
        self.setOtherProps( DDDBConf(), ["Simulation", "DataType" ] )
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
            # Ensure output is always kept
            getConfigurable("StatusCodeSvc").OutputLevel = INFO
        if "FPE" in self.getProp("Monitors"):
            importOptions( "$STDOPTS/FPEAudit.opts" )

    def defineOutput(self):
        # Message service
        msgSvc = getConfigurable("MessageSvc")
        
        # Modify printout defaults
        if self.isPropertySet("OutputLevel"):
            level = self.getProp("OutputLevel")
            # Set the level
            msgSvc.OutputLevel                     = level
            getConfigurable("ToolSvc").OutputLevel = level
            # Information to be kept
            getConfigurable("EventSelector").OutputLevel = INFO
            getConfigurable("TimingAuditor").OutputLevel = INFO
            getConfigurable("EventLoopMgr").OutputLevel  = INFO

        if self.getProp( "TimeStamp" ):
            # add a time stamp to remaining messages
            msgSvc.Format = "%u % F%18W%S%7W%R%T %0W%M"
            msgSvc.timeFormat = "%Y-%m-%d %H:%M:%S UTC"

    def __apply_configuration__(self):
        self.defineDB()
        self.defineEvents()
        self.defineMonitors()
        self.defineOutput()
