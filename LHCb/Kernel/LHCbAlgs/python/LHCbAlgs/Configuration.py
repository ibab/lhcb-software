"""
High level configuration tools for LHCb applications
"""
__version__ = "$Id: $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from os import environ
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import ( DDDBConf )
from Configurables import ( XMLSummary )

class LHCbApp(LHCbConfigurableUser):
    __slots__ = {
        "EvtMax"        : -1
       ,"SkipEvents"    : 0
       ,"DataType"      : "2012"
       ,"Detectors"     : []
       ,"LocalDataTypes": []
       ,"DDDBtag"       : ""
       ,"CondDBtag"     : ""
       ,"DQFLAGStag"    : ""
       ,"Simulation"    : False
       ,"Monitors"      : []
       ,"OutputLevel"   : INFO
       ,"TimeStamp"     : False
       ,"XMLSummary"    : None
       ,"Persistency"   : None
       ,"IgnoreDQFlags" : True
        }

    _propertyDocDct = {
        'EvtMax'      : """ Maximum number of events to process """
       ,'SkipEvents'  : """ Number of events to skip """
       ,'DataType'    : """ Data type, see DDDBConf for allowed values. Default '2009' """
       ,"Detectors"   : """ Maximal list of detector components used in the simulation """
       ,"LocalDataTypes" : """ Set of Local DataTypes uses to set DB """
       ,'DDDBtag'     : """ Tag for DDDB. Default as set in DDDBConf for DataType """
       ,'CondDBtag'   : """ Tag for CondDB. Default as set in DDDBConf for DataType """
       ,"DQFLAGStag"  : """ Tag for DQFLAGS. Default as set in DDDBConf for DataType """
       ,'Simulation'  : """ Flag to indicate usage of simulation conditions """
       ,'Monitors'    : """ List of monitors to execute """
       ,'OutputLevel' : """ The printout level to use (default INFO) """
       ,'TimeStamp'   : """ Flag to add a time stamp to messages (default False) """
       ,'XMLSummary'  : """ Add an XML summary file, default None """
       ,'Persistency'  : """ Overwrite the default persistency with something else. """
       ,'IgnoreDQFlags': """ If False, process only events with good DQ. Default is True (process all events)"""
       }
    
    __used_configurables__ = [ DDDBConf, XMLSummary ]

    __nominalDetectors = [
        'velo',
        'puveto',
        'rich1',
        'rich2',
        'rich',
        'tt',
        'it',
        'ot',
        'spd',
        'prs',
        'ecal',
        'hcal',
        'muon',
        'magnet'
        ]

    __dtMapping = {
        "velo"     : "Velo",
        "puveto"     : "PuVeto",
        "muon"     : "Muon",
        "muonnom1" : "MuonNoM1",
        "tt"       : "TT",
        "ut"       : "UT",
        "ft"       : "FT",
        "vp"       : "VP",
        "vl"       : "VL",
        "rich"     : "Rich",
        "richpmt"  : "RichPmt",
        "rich1"    : "Rich1",
        "rich2"    : "Rich2",
        "rich1pmt" : "Rich1Pmt",
        "rich2pmt" : "Rich2Pmt",
        "rich1horiz" : "Rich1Horiz",
        "magnet"   : "Magnet"
        }


    # Following the xml structure
    __DTRegion = {
        "UpstreamRegion"     : {},
        "BeforeMagnetRegion" : {
            "veloRegion"   : [],
            "ttRegion"     : [],
            "rich1Region"  : []
            },
        "MagnetRegion"       : {
            "magnetRegion" : []
            },
        "AfterMagnetRegion"  : {
            "itRegion"     : [],
            "otRegion"     : [],
            "ftRegion"     : [],
            "rich2Region"  : []
            },
        "DownstreamRegion"   : {
            "muonRegion"   : []
            }
        }

    # Following the xml structure
    __sectionOrder = {
        "UpstreamRegion" : {
            "regions" : [],
            "ignore" : []
            },
        "BeforeMagnetRegion" : {
            "regions" : ['veloRegion', 'rich1Region', 'ttRegion'],
            "ignore" : ['rich1', 'rich1pmt', 'rich', 'richpmt']
            },
        "MagnetRegion" : {
            "regions" : ['magnetRegion'],
            "ignore" : ['magnet']
            },
        "AfterMagnetRegion" : {
            "regions" : ['itRegion', 'otRegion', 'ftRegion', 'rich2Region'],
            "ignore" : ['rich2', 'rich2pmt', 'rich', 'richpmt']
            },
        "DownstreamRegion" : {
            "regions" : ['muonRegion'],
            "ignore" : ['muon']
            }
        }
    
    __possDTRegion = {
        "UpstreamRegion"     : {},
        "BeforeMagnetRegion" : {
            "veloRegion"  : ["velo", "vl", "vp"],
            "rich1Region" : ["rich1", "rich1pmt", "rich", "rich1horiz"],
            "ttRegion"    : ["tt", "ut"]
            },
        "MagnetRegion"       : {
            "magnetRegion" : ["magnet"]
            },
        "AfterMagnetRegion"  : {
            "itRegion"    : ["it", "ot", "ft"],
            "rich2Region" : ["rich2", "rich2pmt", "rich", "superrich"]
            },
        "DownstreamRegion"   : {
            "muonRegion" : ["muon", "muonnom1"]
            }
        }


    def createDetList(self):
        detList = []
        if hasattr(self, "Detectors"):
            import string
            for det in self.getProp("Detectors"):
                detList.append( det.lower() )
        return detList

    def upgradeDetectors(self):
        upgradeDetectorList = []
        if self.isPropertySet("Detectors"):
            tmpUpgradeDetectorList = [det for det in self.createDetList() if det not in self.__nominalDetectors]
            for det in tmpUpgradeDetectorList:
                if self.__dtMapping.has_key(det):
                    upgradeDetectorList.append( self.__dtMapping[det] )
                else:
                    log.warning("Unknown Upgrade detector: %s" %(det))
        return upgradeDetectorList


    def knownMonitors(self):
        return ["SC", "FPE"]
    
    def knownAuditors(self):
        return ["NameAuditor","MemoryAuditor","ChronoAuditor"]
    
    def checkIncompatibleDetectors(self):

        myDetList = self.createDetList();
    
        _veloRegion   = [ det for det in myDetList if det in self.__possDTRegion['BeforeMagnetRegion']['veloRegion']]
        _muonRegion   = [ det for det in myDetList if det in self.__possDTRegion['DownstreamRegion']['muonRegion']]
        _track1Region = [ det for det in myDetList if det in self.__possDTRegion['BeforeMagnetRegion']['ttRegion']]
        _track2Region = [ det for det in myDetList if det in self.__possDTRegion['AfterMagnetRegion']['itRegion']]
        _rich1Region  = [ det for det in myDetList if det in self.__possDTRegion['BeforeMagnetRegion']['rich1Region']]
        _rich2Region  = [ det for det in myDetList if det in self.__possDTRegion['AfterMagnetRegion']['rich2Region']]

        # check for only one velo
        if len(_veloRegion) > 1:
            raise RuntimeError("FATAL: Incompatible Velo detectors defined in LHCbApp().Detectors! '%s'" %(_veloRegion))
        elif _veloRegion:
            if (
                ("vp" in _veloRegion) or ("vl" in _veloRegion)
                ) and ("puveto" in myDetList):
                raise RuntimeError("FATAL: Incompatible Velo detectors defined in LHCbApp().Detectors! '%s' and '%s'" %(_veloRegion, "PuVeto"))
            self.__DTRegion["BeforeMagnetRegion"]["veloRegion"] += _veloRegion
        else:
            pass
            
        # check for detectors in muon region
        if len(_muonRegion) > 1:
            raise RuntimeError("FATAL: Incompatible Muon detectors defined in LHCbApp().Detectors! '%s'" %(_muonRegion))
        elif _muonRegion:
            self.__DTRegion["DownstreamRegion"]["muonRegion"] += _muonRegion
        else:
            pass

        # check for detectors in tt/ut region
        if len(_track1Region) > 1:
            raise RuntimeError("FATAL: Incompatible tracking detectors defined in LHCbApp().Detectors! '%s'" %(_track1Region))
        elif _track1Region:
            self.__DTRegion["BeforeMagnetRegion"]["ttRegion"] = _track1Region
        else:
            pass

        # check for detectors in it/ot/ft region
        if len(_track2Region) > 2:
            raise RuntimeError("FATAL: Incompatible tracking detectors defined in LHCbApp().Detectors! '%s'" %(_track2Region))
        elif len(_track2Region) > 1:
            if "ft" in _track2Region:
                raise RuntimeError("FATAL: Incompatible tracking detectors defined in LHCbApp().Detectors! '%s'" %(_track2Region))
        elif _track2Region:
            self.__DTRegion["AfterMagnetRegion"]["itRegion"] = _track2Region
        else:
            pass

        # check for detectors in rich1 region
        if len(_rich1Region) > 1:
            raise RuntimeError("FATAL: Incompatible tracking detectors defined in LHCbApp().Detectors! '%s'" %(_rich1Region))
        elif _rich1Region:
            self.__DTRegion["BeforeMagnetRegion"]["rich1Region"] = _rich1Region
        # No dataTypes needed
        else:
            pass

        # check for detectors in rich2 region
        if len(_rich2Region) > 1:
            raise RuntimeError("FATAL: Incompatible tracking detectors defined in LHCbApp().Detectors! '%s'" %(_rich2Region))
        elif _rich2Region:
            self.__DTRegion["AfterMagnetRegion"]["rich2Region"] = _rich2Region
        else:
            pass



    def defineDB(self):
        # Delegate handling of properties to DDDBConf
        self.setOtherProps( DDDBConf(), ["Simulation", "DataType" ] )
        # Set CondDB tags if given, otherwise use default defined in DDDBConf
        from Configurables import CondDB
        if hasattr( self, "DDDBtag" ):
            CondDB().Tags [ "DDDB" ] = self.getProp("DDDBtag")
        if hasattr( self, "CondDBtag" ):
            CondDB().Tags [ "LHCBCOND" ] = self.getProp("CondDBtag")
            CondDB().Tags [ "SIMCOND"  ] = self.getProp("CondDBtag")
        if hasattr( self, "DQFLAGStag" ):
            CondDB().Tags [ "DQFLAGS" ] = self.getProp("DQFLAGStag")


    def defineDBDataTypes(self):
        self.checkIncompatibleDetectors()
        myDataTypes = []
        for section in self.__DTRegion:
            myTag = ""
            joinText = "" 
            for step in self.__sectionOrder[section]["regions"]:
                for det in  self.__DTRegion[section][step]:
                    if det not in self.__sectionOrder[section]["ignore"]:
                        if not joinText:
                            myTag += "%s" %(self.__dtMapping[det])
                            joinText = "+"
                        else:
                            myTag = "%s%s%s" %(myTag, joinText, self.__dtMapping[det])
            if myTag:
                myDataTypes.append(myTag)

        if myDataTypes:
            from Configurables import CondDB
            if hasattr(CondDB(), "AllLocalTagsByDataType"):
                if not CondDB().AllLocalTagsByDataType:
                    CondDB().AllLocalTagsByDataType = myDataTypes
        return
    
    def defineEvents(self):
        SkipEvents = self.getProp("SkipEvents")
        if SkipEvents > 0 :
            if hasattr(EventSelector(),"FirstEvent"):
                log.warning( "EventSelector().FirstEvent and LHCBApp().SkipEvents both defined, using LHCbApp().SkipEvents")
            EventSelector().FirstEvent = SkipEvents + 1
        
        # Delegate handling to ApplicationMgr configurable
        self.setOtherProps(ApplicationMgr(),["EvtMax"])

        isSim = self.getProp("Simulation")
        if not isSim:
            ignoreDQ = self.getProp("IgnoreDQFlags")
            if not ignoreDQ:
                from Configurables import DQFilterSvc
                ApplicationMgr().ExtSvc.append(DQFilterSvc())
    
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
    
    def defineXMLSum(self):
        if hasattr( self, "XMLSummary" ):
            self.setOtherProps( XMLSummary(), ["XMLSummary" ] )
    
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
    
    def definePersistency(self):
        '''Use IOHelper to set up the tes and IO services etc.'''
        persistency=None
        if hasattr( self, "Persistency" ):
            persistency=self.getProp("Persistency")
        # Set up TES and I/O services
        from GaudiConf.IOHelper import IOHelper
        IOHelper(persistency,persistency).setupServices()
    
    def __apply_configuration__(self):
        self.defineDB()
        self.defineDBDataTypes()
        self.defineEvents()
        self.defineMonitors()
        self.defineXMLSum()
        self.defineOutput()
        self.definePersistency()
        
