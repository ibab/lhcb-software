"""
High level configuration tools for Conditions Database.
"""
__version__ = "$Id: Configuration.py,v 1.10 2008-11-14 13:33:41 cattanem Exp $"
__author__  = "Marco Clemencic <Marco.Clemencic@cern.ch>"

from Gaudi.Configuration import allConfigurables, ConfigurableUser, importOptions, getConfigurable, log
from Configurables import ( CondDBAccessSvc, 
                            CondDBDispatcherSvc,
                            CondDBLayeringSvc, 
                            CondDBTimeSwitchSvc, 
                            CondDBCnvSvc,
                            CondDBSQLiteCopyAccSvc,
                            CondDBLogger,
                            MessageSvc )

class CondDB(ConfigurableUser):
    """
    Configurable user to allow high-level configuration of the access to the
    conditions database.
    """
    __slots__ = { "Tags"        : {},
                  "Simulation"  : False,
                  "UseOracle"   : False,
                  "LocalTags"   : {},
                  "LogFile"     : "",
                  "Overrides"   : [], 
                  "PartitionConnectionString": {},  }
    _propertyDocDct = { 
                       'Tags' : """ Dictionary of tags (partition:tag) to use for the COOL databases """,
                       'Simulation' : """ Boolean flag to select the simulation or real-data configuration """,
                       'UseOracle' : """ Boolean flag to enable the usage of the CondDB from Oracle servers """,
                       'LocalTags' : """ Dictionary with local tags to use to override the global ones (partition: list of local tags) """,
                       'LogFile' : """ Record the requests to the database the specified file """,
                       'Overrides' : """ Internal property used to add layers or alternatives """,
                       'PartitionConnectionString' : """ Dictionary with alternative connection strings for the CondDB partitions """,
                       }
    LAYER = 0
    ALTERNATIVE = 1
    # List of known implementations of ICondDBReader (str is used for backward compatibility)
    __CondDBReaders__ = [ CondDBAccessSvc,
                          CondDBDispatcherSvc,
                          CondDBLayeringSvc,
                          CondDBTimeSwitchSvc,
                          CondDBSQLiteCopyAccSvc,
                          str ]

    def addLayer(self, accessSvc):
        """
        Add the given CondDBReader as a layer on top of the existing configuration.
    
        Example:
        CondDB().addLayer(myDB)
        """
        # Check for supported types
        if type(accessSvc) not in __CondDBReaders__:
            raise TypeError("CondDB.addLayer does not support '%s'"%accessSvc.__class__.__name__)
        self.Overrides.append((self.LAYER, accessSvc))
    
    def _addLayer(self, accessSvc):
        cnvSvc = allConfigurables["CondDBCnvSvc"]
        
        originalReader = cnvSvc.CondDBReader
        if type(originalReader) == CondDBLayeringSvc:
            # If the original reader is already a layering svc, we can extend the
            # configuration.
            originalReader.Layers.insert(0,accessSvc)
        else:
            # We have to create a new layering svc.
            name = "CondDBLayeringSvc"
            i = 0
            while name in allConfigurables:
                i += 1
                name = "CondDBLayeringSvc_%d"%i
            cnvSvc.CondDBReader = CondDBLayeringSvc(name,
                                                    Layers = [accessSvc,
                                                              originalReader])

    def addAlternative(self, accessSvc, path):
        """
        Add the given CondDBReader as an alternative to the existing configuration
        for the specified path.
        
        Example:
        CondDB().addAlternative(myDB,"/Conditions/MyDetector/Alignment")
        """
        # Check for supported types
        if type(accessSvc) not in __CondDBReaders__:
            raise TypeError("CondDB.addAlternative does not support '%s'"%accessSvc.__class__.__name__)
        self.Overrides.append((self.ALTERNATIVE, accessSvc, path))
        
    def _addAlternative(self, accessSvc, path):
        cnvSvc = allConfigurables["CondDBCnvSvc"]
        
        originalReader = cnvSvc.CondDBReader
        if type(originalReader) == CondDBDispatcherSvc:
            # If the original reader is already a dispatcher, we can extend the
            # configuration:
            originalReader.Alternatives[path] = accessSvc
        else:
            # We have to create a new dispatcher
            name = "CondDBDispatcherSvc"
            i = 0
            while name in allConfigurables:
                i += 1
                name = "CondDBDispatcherSvc_%d"%i
            cnvSvc.CondDBReader = CondDBDispatcherSvc(name,
                                                      MainAccessSvc = originalReader,
                                                      Alternatives = { path: accessSvc }
                                                      )
    
    def __apply_configuration__(self):
        """
        Converts the high-level information passed as properties into low-level configuration.
        """
        if self.getProp("UseOracle"):
            importOptions("$SQLDDDBROOT/options/SQLDDDB-Oracle.py")
        else:   
            importOptions("$SQLDDDBROOT/options/SQLDDDB.py")
        
        #########################################################################
        # Access to ConditionsDB
        ##########################################################################
        conns = self.getProp("PartitionConnectionString")
        tags = self.getProp("Tags")
        # DB partitions
        partition = {}
        for p, t in [ ("DDDB",     CondDBAccessSvc),
                      ("LHCBCOND", CondDBAccessSvc),
                      ("ONLINE",   CondDBTimeSwitchSvc),
                      ("SIMCOND",  CondDBAccessSvc) ]:
            partition[p] = getConfigurable(p, t)
            # Override connection strings:
            if p in conns and type(partition[p]) is CondDBAccessSvc:
                partition[p].ConnectionString = conns[p]
                del conns[p]
            # Override tags
            if p in tags and p != "ONLINE":
                partition[p].DefaultTAG = tags[p]
                del tags[p]
            
        if conns:
            log.warning("Cannot override the connection strings of the partitions %r", conns.keys()) 
        if tags:
            log.warning("Cannot set the tag for partitions %r", tags.keys()) 
            
        if not self.getProp("Simulation"):
            # Standard configurations
            #  - Reconstruction / analysis
            disp = CondDBDispatcherSvc("MainCondDBReader",
                                       MainAccessSvc = partition["DDDB"],
                                       Alternatives = {
                                         "/Conditions": partition["LHCBCOND"],
                                         "/Conditions/Online": partition["ONLINE"]
                                       })
        else:
            #  - Simulation
            disp = CondDBDispatcherSvc("SimulationCondDBReader",
                                       MainAccessSvc = partition["DDDB"],
                                       Alternatives = {
                                         "/Conditions": partition["SIMCOND"]
                                       })
        CondDBCnvSvc( CondDBReader = disp )
        
        localTags = self.getProp("LocalTags")
        not_applied = []
        for p in localTags:
            if p in partition:
                taglist = list(localTags[p])
                taglist.reverse() # we need to stack the in reverse order to use the first as on top of the others
                i = 0 # counter
                for t in taglist:
                    self._addLayer(partition[p].clone("%s_%d" % (p, i),
                                                      DefaultTAG = t))
                    i += 1
            else:
                not_applied.append(p)
        if not_applied:
            log.warning("Cannot set the local tags for partitions %r", not_applied) 
        
        # Add layers and alternatives
        call = { self.LAYER : self._addLayer,
                 self.ALTERNATIVE : self._addAlternative }
        for override in self.getProp("Overrides"):
            apply(call[override[0]], override[1:])
        
        # Add the logger
        filename = self.getProp("LogFile")
        if filename:
            cnvSvc = allConfigurables["CondDBCnvSvc"]
            cnvSvc.CondDBReader = CondDBLogger(LoggedReader = cnvSvc.CondDBReader,
                                               LogFile = filename)
        
        # Suppress pointless warning from COOL_2_5_0
        MessageSvc().setError.append("RelationalDatabase")

        # Set up Virtual File System service, can be used by ParticlePropertySvc
        from Gaudi.Configuration import VFSSvc
        from Configurables       import CondDBEntityResolver
        VFSSvc().FileAccessTools.append(CondDBEntityResolver())


# Exported symbols
__all__ = [ "addCondDBLayer", "addCondDBAlternative", "useCondDBLogger",
            "configureOnlineSnapshots" ]
# Configurables provided by the package
__all__ += [ "CondDBAccessSvc",
             "CondDBDispatcherSvc", "CondDBLayeringSvc", "CondDBTimeSwitchSvc", 
             "CondDBSQLiteCopyAccSvc", "CondDBLogger",
             "CondDBCnvSvc",
             "CondDB" ]

# List of known implementations of ICondDBReader (str is used for backward compatibility)
__CondDBReaders__ = [ CondDBAccessSvc,
                      CondDBDispatcherSvc,
                      CondDBLayeringSvc,
                      CondDBTimeSwitchSvc,
                      CondDBSQLiteCopyAccSvc,
                      str ]

def _assertConfig(funcname):
    """
    Check if the default configuration has been loaded.
    """
    if "CondDBCnvSvc" not in allConfigurables:
        raise RuntimeError("You cannot call '%s' before the standard CondDB configuration"%funcname)
     
def addCondDBLayer(accessSvc):
    """
    Add the given CondDBReader as a layer on top of the existing configuration.
    
    Example:
    addCondDBLayer(myDB)
    """
    DetCond().addLayer(accessSvc)

def addCondDBAlternative(accessSvc, path):
    """
    Add the given CondDBReader as an alternative to the existing configuration
    for the specified path.
    
    Example:
    addCondDBAlternative(myDB,"/Conditions/MyDetector/Alignment")
    """
    DetCond().addAlternative(accessSvc, path)

def useCondDBLogger(filename = None, logger = None):
    """
    Add the CondDBLogger to the chain of CondDBReaders.
    
    The simplest usage is to call the function without options (use defaults), or
    pass a file name. 
    """
    _assertConfig('useCondDBLogger')
    cnvSvc = allConfigurables["CondDBCnvSvc"]
    if logger is None:
        # use the default configuration
        cnvSvc.CondDBReader = CondDBLogger(LoggedReader = cnvSvc.CondDBReader)
    elif type(logger) is CondDBLogger:
        # use the user-provided configurable
        logger.LoggedReader = cnvSvc.CondDBReader
        cnvSvc.CondDBReader = logger
    else:
        raise TypeError("useCondDBLogger does not support '%s'"%logger.__class__.__name__)
    # Use the user specified filename, if any
    if filename:
        cnvSvc.CondDBReader.LogFile = filename

def _timegm(t):
    """Inverse of time.gmtime. Implementation from Gaudi::Time."""
    import time
    if t[8] != 0: # ensure that dst is not set
        t = tuple(list(t[0:8]) + [0])
    t1 = time.mktime(t)
    gt = time.gmtime(t1)
    t2 = time.mktime(gt)
    return t1 + (t1 - t2)

def defConnStrFunc(ym_tuple):
    return "sqlite_file:$SQLITEDBPATH/ONLINE-%04d%02d.db/ONLINE" % ym_tuple
    
def configureOnlineSnapshots(start = None, end = None, connStrFunc = None):
    if connStrFunc is None:
        connStrFunc = defConnStrFunc
    
    # prepare the configurable instance
    ONLINE = CondDBTimeSwitchSvc("ONLINE")
    
    # Set the first available pair (year,month) 
    if start is None:
        first_snapshot = (2008,6)
    else:
        first_snapshot = start
    
    # Set the last available pair (year,month) 
    if end is None:
        # By default it is the previous month
        import time
        now = time.gmtime()[0:2]
        if now[1] == 1:
            last_snapshot = (now[0]-1,12)
        else:
            last_snapshot = (now[0],now[1]-1)
    else:
        last_snapshot = end
    
    # reset the list of readers, for safety
    ONLINE.Readers = []
    # loop from first to last-1
    i = first_snapshot
    until = 0 # this makes the first service used from times starting from 0
    while i < last_snapshot:
        name = "ONLINE_%04d%02d" % i
        accSvc = CondDBAccessSvc(name, ConnectionString = connStrFunc(i))
        since = until
        # increment
        if i[1] == 12:
            i = (i[0]+1,1)
        else:
            i = (i[0],i[1]+1)
        until = int(_timegm(tuple([i[0], i[1], 1, 0, 0, 0, 0, 0, 0]))) * 1000000000
        descr = "'%s':(%d,%d)" % ( accSvc.getFullName(), since, until )
        ONLINE.Readers.append(descr)
    
    # append the last database with validity extended to the maximum validity
    name = "ONLINE_%04d%02d" % i
    accSvc = CondDBAccessSvc(name, ConnectionString = connStrFunc(i))
    since = until
    until = 0x7fffffffffffffffL # Defined in PyCool.cool as ValidityKeyMax
    descr = "'%s':(%d,%d)" % ( accSvc.getFullName(), since, until )
    ONLINE.Readers.append(descr)
    
    
    
