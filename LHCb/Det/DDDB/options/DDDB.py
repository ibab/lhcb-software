# $Id: DDDB.py,v 1.1 2008-02-29 19:10:17 marcocle Exp $
__author__ = "Marco Clemencic <Marco.Clemencic@cern.ch>"

from Gaudi.Configuration import *
from Configurables import ( CondDBAccessSvc, 
                            CondDBDispatcherSvc, 
                            CondDBLayeringSvc, 
                            CondDBCnvSvc,
                            CondDBEntityResolverSvc,
                            XmlCnvSvc,
                            XmlParserSvc )

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
                            EntityResolverSvc = CondDBEntityResolverSvc()
                            )

##########################################################################
# Use the XmlCnvSvc in the persistency service for the detector description
DetectorPersistencySvc( CnvServices = [ xmlCnvSvc ] )


##########################################################################
# DetetectorDataSvc must be external, to finalize after ToolSvc
ApplicationMgr().ExtSvc += [ detDataSvc ]

##########################################################################
# Access to ConditionsDB
##########################################################################
# DB partitions
DDDB     = CondDBAccessSvc("DDDB")
LHCBCOND = CondDBAccessSvc("LHCBCOND")
ONLINE   = CondDBAccessSvc("ONLINE")
SIMCOND  = CondDBAccessSvc("SIMCOND")

# Standard configurations
#  - Reconstruction / analisys
mainReader = CondDBDispatcherSvc("MainCondDBReader",
                                 MainAccessSvc = DDDB,
                                 Alternatives = [
                                    "/Conditions=" + LHCBCOND.getFullName(),
                                    # Not yet available
                                    # "/Online=" + ONLINE.getFullName()
                                 ]
                                 )

#  - Simulation
simReader = CondDBDispatcherSvc("SimulationCondDBReader",
                                 MainAccessSvc = DDDB,
                                 Alternatives = [
                                    "/Conditions=" + SIMCOND.getFullName(),
                                    "/Online="     + SIMCOND.getFullName()
                                 ]
                                 )

condDBCnvSvc = CondDBCnvSvc( CondDBReader = mainReader )

##########################################################################
# Technology dependent options
##########################################################################
importOptions("$SQLDDDBROOT/options/SQLDDDB.py")
