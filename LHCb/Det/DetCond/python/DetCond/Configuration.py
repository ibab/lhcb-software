"""
High level configuration tools for Conditions Database.
"""
__version__ = "$Id: Configuration.py,v 1.11 2009-01-07 13:28:03 marcocle Exp $"
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

import os, re

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
                  "PartitionConnectionString": {},
                  "SQLiteLocalCopiesDir": "",
                  "OverwriteSQLiteLocalCopy": False  }
    _propertyDocDct = { 
                       'Tags' : """ Dictionary of tags (partition:tag) to use for the COOL databases """,
                       'Simulation' : """ Boolean flag to select the simulation or real-data configuration """,
                       'UseOracle' : """ Boolean flag to enable the usage of the CondDB from Oracle servers """,
                       'LocalTags' : """ Dictionary with local tags to use to override the global ones (partition: list of local tags) """,
                       'LogFile' : """ Record the requests to the database the specified file """,
                       'Overrides' : """ Internal property used to add layers or alternatives """,
                       'PartitionConnectionString' : """ Dictionary with alternative connection strings for the CondDB partitions """,
                       'SQLiteLocalCopiesDir' : """ The directory where to copy the SQLite files before accessing them """,
                       'OverwriteSQLiteLocalCopy' : """ When using SQLite local copies, overwrite existing files """,
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

    def _checkOverrideArgs(self, accessSvc, connStr, dbFile, dbName):
        """
        Check if the accessSvc is a valid CondDBReader or build one using the
        other arguments.
        """
        kwargs = { "accessSvc": accessSvc, "connStr": connStr, "dbFile": dbFile, "dbName": dbName }
        if accessSvc is None:
            if not connStr:
                if dbFile:
                    if not dbName:
                        dbName = os.path.basename(dbFile)
                        m = re.match(r'([A-Z][A-Z0-9_]{0,7})(_\w+)?.db', dbName)
                        if m:
                            dbName = m.group(1)
                        else:
                            raise ValueError('invalid arguments %r' % kwargs)
                    connStr = "sqlite_file:%s/%s" % (dbFile, dbName)
                else:
                    raise ValueError('invalid arguments %r' % kwargs)
                name = dbName
            else:
                name = connStr.rsplit('/')[-1]
                if not re.match(r'[A-Z][A-Z0-9_]{0,7}', name):
                    name = 'CondDBAccessSvc'
            # make a unique name for the configurable
            name = "automatic_" + name
            name_format = name + '_%d'
            i = 0
            while name in allConfigurables:
                i += 1
                name = name_format % i
            accessSvc = CondDBAccessSvc(name, ConnectionString = connStr)
        elif type(accessSvc) not in __CondDBReaders__: # Check for supported types
            raise TypeError("'%s' not supported as CondDBReader"%accessSvc.__class__.__name__)
        return accessSvc
        
    def addLayer(self, accessSvc = None, connStr = None, dbFile = None, dbName = None):
        """
        Add the given CondDBReader as a layer on top of the existing configuration.
    
        Example:
        CondDB().addLayer(myDB)
        """
        # Check the arguments and/or prepare a valid access svc 
        accessSvc = self._checkOverrideArgs(accessSvc, connStr, dbFile, dbName)
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

    def addAlternative(self, accessSvc = None, path = None, connStr = None, dbFile = None, dbName = None):
        """
        Add the given CondDBReader as an alternative to the existing configuration
        for the specified path.
        
        Example:
        CondDB().addAlternative(myDB,"/Conditions/MyDetector/Alignment")
        """
        if path is None:
            raise ValueError("'path' must be specified")
        # Check the arguments and/or prepare a valid access svc 
        accessSvc = self._checkOverrideArgs(accessSvc, connStr, dbFile, dbName)
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
    def __make_sqlite_local_copy__(self, accsvc, local_dir = None, force_copy = None):
        if isinstance(accsvc, str):
            # convert the string in an actual configurable instance
            # This is both for backward compatibility and CondDBTimeSwitchSvc
            if "/" in accsvc:
                tp, name = accsvc.split("/",1)
            else:
                tp = name = accsvc
            accsvc = getConfigurable(name, tp)
        if local_dir is None:
            local_dir = self.getProp("SQLiteLocalCopiesDir")
        if force_copy is None:
            force_copy = self.getProp("OverwriteSQLiteLocalCopy")
        # If the directory for the local copies is not specified, we do nothing        
        if not local_dir:
            return accsvc
        # Modify partitions to use local copies of the DBs
        newaccsvc = accsvc # fallback return value (no change)
        if isinstance(accsvc, CondDBAccessSvc):
            # replace the reader with another
            m = re.match(r"^sqlite_file:(.*)/([_0-9A-Z]{1,8})$",
                         accsvc.getProp("ConnectionString"))
            if not m: # not SQLite connection string
                return accsvc
            newaccsvc = CondDBSQLiteCopyAccSvc(accsvc.name() + "_local")
            newaccsvc.OriginalFile = m.group(1)
            newaccsvc.DestinationFile = os.path.join(local_dir,
                                                     os.path.basename(m.group(1)))
            newaccsvc.DBName = m.group(2)
            newaccsvc.ForceCopy = force_copy
            newaccsvc.IgnoreCopyError = not force_copy # ignore copy errors if we do not overwrite (needed for local tags) 
            if hasattr(accsvc, "CacheHighLevel"): 
                newaccsvc.CacheHighLevel = accsvc.CacheHighLevel
        elif isinstance(accsvc, CondDBDispatcherSvc):
            # use the same dispatcher replacing its content 
            mainAccSvc = accsvc.getProp("MainAccessSvc")
            accsvc.MainAccessSvc = self.__make_sqlite_local_copy__(mainAccSvc, local_dir)
            alternatives = accsvc.getProp("Alternatives")
            for alt in alternatives.keys():
                accsvc.Alternatives[alt] = \
                   self.__make_sqlite_local_copy__(alternatives[alt], local_dir)
        elif isinstance(accsvc, CondDBLayeringSvc):
            # use the same layering service replacing its content 
            new_layers = []
            for layer in accsvc.getProp("Layers"):
                new_layers.append(self.__make_sqlite_local_copy__(layer, local_dir))
            accsvc.Layers = new_layers
        elif isinstance(accsvc, CondDBTimeSwitchSvc):
            # use the same time switcher replacing its content,
            # but we need to parse its options (in format "'%s':(%d,%d)")
            readers_list = accsvc.getProp("Readers")
            readers = eval("{%s}" % (",".join(readers_list)))
            new_readers = []
            for r, iov in readers.items():
                new_reader = self.__make_sqlite_local_copy__(r, local_dir)
                new_readers.append("'%s':(%d,%d)" %
                                   (new_reader.getFullName(), iov[0], iov[1]))
            accsvc.Readers = new_readers
        elif isinstance(accsvc, CondDBLogger):
            # use the same logger replacing its content 
            logged = accsvc.getProp("LoggedReader")
            accsvc.LoggedReader = self.__make_sqlite_local_copy__(logged, local_dir)
        elif isinstance(accsvc, CondDBCnvSvc):
            # use the same conversion service replacing its content 
            reader = accsvc.getProp("CondDBReader")
            accsvc.CondDBReader = self.__make_sqlite_local_copy__(reader, local_dir)
        return newaccsvc
        
    def __apply_configuration__(self):
        """
        Converts the high-level information passed as properties into low-level configuration.
        """
        # Check options consistency
        if self.getProp("UseOracle") and self.getProp("SQLiteLocalCopiesDir"):
            raise RuntimeError("Conflicting properties in CondDB Configurable: UseOracle and SQLiteLocalCopiesDir")
        
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
        
        # Modify partitions to use local copies of the DBs
        # before adding user layers and alternatives, which should be already local.
        # This is a no-operation if the property is not set
        self.__make_sqlite_local_copy__(CondDBCnvSvc()) 
        
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
    
    
    
