
__author__ = "Marco Clemencic <Marco.Clemencic@cern.ch>"

from Gaudi.Configuration import *
from Configurables import ( CondDBAccessSvc, 
                            CondDBDispatcherSvc, 
                            CondDBLayeringSvc, 
                            CondDBCnvSvc,
                            CondDBTimeSwitchSvc,
                            CondDBEntityResolver,
                            XmlCnvSvc,
                            XmlParserSvc )
from GaudiConf.Configuration import LHCbApp

# Helper function that should be moved to Gaudi 
def getConfigurable(name, defaultType = None):
    """Helper function to get a configurable with the given name regardless
    for the type.
    If defaultType can be a class derived from configurable or a string. If not
    specified, the tool name is used as type."""
    if name in allConfigurables:
        return allConfigurables[name]
    else:
        # if the configurable is not found, we need to instantiate it
        if defaultType is None:
            # try to use the name of the configurable as default type
            defaultType = name
        if type(defaultType) is str:
            # we need to convert from string to actual class
            if defaultType in globals():
                # We the type is defined in the global namespace
                defaultType = globals()[defaultType]
            else:
                # otherwise we try to get it from the Configurables database
                exec "from Configurables import %s" % defaultType
                defaultType = locals()[defaultType]
        return defaultType(name)

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
                              DetDbLocation = "conddb:/lhcb.xml",
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
ApplicationMgr().ExtSvc += [ detDataSvc ]

##########################################################################
# Technology dependent options
##########################################################################
if getConfigurable("LHCbApp").getProp("useOracleCondDB"):
    importOptions("$SQLDDDBROOT/options/SQLDDDB-Oracle.py")
else:   
    importOptions("$SQLDDDBROOT/options/SQLDDDB.py")

##########################################################################
# Access to ConditionsDB
##########################################################################
# DB partitions

DDDB     = getConfigurable("DDDB",     CondDBAccessSvc)
LHCBCOND = getConfigurable("LHCBCOND", CondDBAccessSvc)
ONLINE   = getConfigurable("ONLINE",   CondDBTimeSwitchSvc)
SIMCOND  = getConfigurable("SIMCOND",  CondDBAccessSvc)

# Standard configurations
#  - Reconstruction / analysis
CondDBDispatcherSvc("MainCondDBReader",
                     MainAccessSvc = DDDB,
                     Alternatives = {
                       "/Conditions": LHCBCOND.getFullName(),
                       "/Conditions/Online": ONLINE.getFullName()
                       }
                    )

#  - Simulation
CondDBDispatcherSvc("SimulationCondDBReader",
                    MainAccessSvc = DDDB,
                    Alternatives = {
                      "/Conditions": SIMCOND.getFullName()
                      }
                    )

# Default is real data
CondDBCnvSvc( CondDBReader = getConfigurable("MainCondDBReader") )

# Use this for simulated data
#CondDBCnvSvc( CondDBReader = getConfigurable("SimulationCondDBReader") )

# Suppress pointless warning from COOL_2_5_0
MessageSvc().setError.append("RelationalDatabase")
