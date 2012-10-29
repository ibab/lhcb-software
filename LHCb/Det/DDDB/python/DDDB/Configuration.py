"""
High level configuration tools for the detector description.
"""
__author__ = "Marco Clemencic <Marco.Clemencic@cern.ch>"

from Gaudi.Configuration import *
from Configurables import ( CondDBEntityResolver,
                            XmlCnvSvc,
                            XmlParserSvc )
from DetCond.Configuration import CondDB
from datetime import datetime, timedelta

__all__ = ["DDDBConf"]

class DDDBConf(ConfigurableUser):
    """
    ConfigurableUser for the configuration of the detector description.
    """
    __slots__ = { "DbRoot"    : "conddb:/lhcb.xml",
                  "DataType"  : "2012",
                  "Simulation": False
                   }
    _propertyDocDct = {
                       'DbRoot' : """ Root file of the detector description """,
                       'DataType' : """ Symbolic name for the data type. Allowed values: ["2012", "2011", "2010", "2009","2008","MC09","Upgrade"] """,
                       'Simulation' : """ Boolean flag to select the simulation or real-data configuration """,
                       }

    __used_configurables__ = [ CondDB ]

    def __apply_configuration__(self):
        """
        Converts the high-level information passed as properties into low-level configuration.
        """
        ##########################################################################
        # Detector Transient Store Configuration
        ##########################################################################
        # Notes:
        #   - DetDbLocation must use '/' because it is a URL (never use '\')
        #   - DetStorageType:
        #     + 7 => XML
        #     + 9 => pure CondDB (remove "conddb:" from URL)
        ##########################################################################
        detDataSvc = DetectorDataSvc( UsePersistency = True,
                                      DetDbRootName = "dd",
                                      DetDbLocation = self.getProp("DbRoot"),
                                      DetStorageType = 7 )

        ##########################################################################
        # XML Conversion Service configuration
        ##########################################################################
        # Notes:
        #   - AllowGenericConversion triggers generic detector element conversion
        #     Useful in the case of visualization or in a case the required user
        #     defined detector element converters are somehow not available.
        ##########################################################################
        xmlCnvSvc = XmlCnvSvc(AllowGenericConversion = True)

        xmlParserSvc = XmlParserSvc(
                                    # Set the maximum number of files to be put in the
                                    # cache of the XmlCnvSvc
                                    MaxDocNbInCache = 10,

                                    # Change the behavior of the cache. A 0 value makes
                                    # it FIFO while a bigger value tends to make it keep
                                    # only reused files.
                                    CacheBehavior = 3,

                                    # Tell to the XmlParserSvc how to resolve entities
                                    # in the CondDB
                                    EntityResolver = CondDBEntityResolver()
                                    )

        ##########################################################################
        # Use the XmlCnvSvc in the persistency service for the detector description
        DetectorPersistencySvc( CnvServices = [ xmlCnvSvc ] )


        ##########################################################################
        # DetetectorDataSvc must be external, to finalize after ToolSvc
        ApplicationMgr().ExtSvc.insert(0, detDataSvc)

        # Propagate the properties to the used Configurables
        self.propagateProperties()

        dataType = self.getProp("DataType")
        if not dataType in self.__data_types_handlers__:
            raise ValueError("Invalid value %r for property DetDesc().DataType."
                             " (allowed: %r)"% (dataType,
                                                self.__data_types_handlers__.keys()))
        if dataType in [ "MC09", "Upgrade" ]:
            sim = self.getProp("Simulation")
            if not sim:
                log.info("%s data is _always_ simulation", dataType )

        # calls the specific configuration function for the requested data type
        self.__data_types_handlers__[dataType](self)

        # Get particle properties table from condDB
        from Configurables import LHCb__ParticlePropertySvc
        LHCb__ParticlePropertySvc( ParticlePropertiesFile = 'conddb:///param/ParticleTable.txt' )

    def __set_tag__(self, partitions, tag):
        cdb = CondDB()
        for p in partitions:
            if p not in cdb.Tags:
                cdb.Tags[p] = tag
                log.warning("Using default tag %s for partition %s", tag, p)
            elif cdb.Tags[p].upper() == "DEFAULT" :
                cdb.Tags[p] = tag
                log.warning("Default tag requested for partition %s (using %s)", p, tag )

    def __set_init_time__(self, utcDatetime):
        """
        Configure the initialization time using the lower between the proposed time and
        the current time,
        """
        utcDatetime = min(datetime.utcnow(), utcDatetime)
        from Configurables import EventClockSvc
        ecs = EventClockSvc()
        # do not overwrite already set values
        if not ecs.isPropertySet("InitialTime"):
            dt = utcDatetime - datetime(1970, 1, 1, 0)
            ns = (dt.days * 24 * 60 * 60 + dt.seconds) * 1000000000
            ecs.InitialTime = ns
        else:
            t = datetime(1970, 1, 1, 0) + timedelta(seconds=ecs.InitialTime/1000000000)
            log.warning("EventClockSvc().InitialTime already set to %s UTC (requested %s UTC)",
                        t.isoformat(), utcDatetime.isoformat())

    def __2012_conf__(self):
        """
        Default configuration for 2012 data (and MonteCarlo for DDDB)
        """
        # Set the tags
        self.__set_tag__(["DDDB"], "dddb-20120831")
        self.__set_tag__(["LHCBCOND"], "cond-20121025")
        self.__set_tag__(["DQFLAGS"], "dq-20121016")
        if not self.getProp("Simulation"):
            # set initialization time to a safe default
            self.__set_init_time__(datetime(2012, 12, 31, 21, 0))

    def __2011_conf__(self):
        """
        Default configuration for 2011 data (and MonteCarlo for DDDB)
        """
        # Set the tags
        self.__set_tag__(["DDDB"], "dddb-20120727")
        self.__set_tag__(["LHCBCOND"], "cond-20120724")
        self.__set_tag__(["DQFLAGS"], "dq-20120910")
        if not self.getProp("Simulation"):
            self.__set_init_time__(datetime.utcfromtimestamp(1319991087)) # End of fill 2267

    def __2010_conf__(self):
        """
        Default configuration for 2010 data (and MonteCarlo for DDDB)
        """
        # Set the tags
        self.__set_tag__(["DDDB"], "head-20110721")
        self.__set_tag__(["LHCBCOND"], "head-20110614")
        self.__set_tag__(["DQFLAGS"], "tt-20110126")
        if not self.getProp("Simulation"):
            self.__set_init_time__(datetime.utcfromtimestamp(1288505611)) # End of fill 1459

    def __2009_conf__(self):
        """
        Default configuration for 2009 data and corresponding MonteCarlo
        """
        # Set the tags
        self.__set_tag__(["DDDB"], "head-20101206")
        self.__set_tag__(["LHCBCOND"], "head-20110614")
        self.__set_tag__(["SIMCOND"], "MC-20101026-vc15mm-md100")
        self.__set_tag__(["DQFLAGS"], "tt-20110126")
        if not self.getProp("Simulation"):
            self.__set_init_time__(datetime(2009, 12, 31, 21, 0)) # 31/12/2009 21:00

    def __2008_conf__(self):
        """
        Default configuration for 2008 data and corresponding MonteCarlo
        """
        # Set the tags
        self.__set_tag__(["DDDB"], "head-20101003")
        self.__set_tag__(["LHCBCOND"], "head-20101010")
        self.__set_tag__(["SIMCOND"], "sim-20090212")
        self.__set_tag__(["DQFLAGS"], "tt-20110126")
        if not self.getProp("Simulation"):
            self.__set_init_time__(datetime(2008, 12, 31, 21, 0)) # 31/12/2008 21:00

    def __MC09_conf__(self):
        """
        Default configuration for MC09 MonteCarlo production and analysis
        """
        # Set the tags
        self.__set_tag__(["DDDB"], "MC09-20090602")
        self.__set_tag__(["SIMCOND"], "MC09-20100921-vc-md100")

    def __Upgrade_conf__(self):
        """
        Default database for Upgrade MonteCarlo production and analysis
        """
        # Need also to change connection string to DDDB
        CondDB().PartitionConnectionString = { "DDDB":"sqlite_file:$SQLITEDBPATH/DDDB_upgrade.db/DDDB"}

    __data_types_handlers__ =  { "2012": __2012_conf__,
                                 "2011": __2011_conf__,
                                 "2010": __2010_conf__,
                                 "2009": __2009_conf__,
                                 "2008": __2008_conf__,
                                 "MC09": __MC09_conf__,
                                 "Upgrade": __Upgrade_conf__ }
