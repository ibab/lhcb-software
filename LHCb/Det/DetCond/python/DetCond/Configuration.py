"""
High level configuration tools for Conditions Database.
"""
__version__ = "$Id: Configuration.py,v 1.8 2008-07-30 12:08:10 marcocle Exp $"
__author__  = "Marco Clemencic <Marco.Clemencic@cern.ch>"

from Gaudi.Configuration import allConfigurables
from Configurables import ( CondDBAccessSvc, 
                            CondDBDispatcherSvc, 
                            CondDBLayeringSvc, 
                            CondDBTimeSwitchSvc, 
                            CondDBCnvSvc,
                            CondDBSQLiteCopyAccSvc,
                            CondDBLogger )

# Exported symbols
__all__ = [ "addCondDBLayer", "addCondDBAlternative", "useCondDBLogger",
            "configureOnlineSnapshots" ]
# Configurables provided by the package
__all__ += [ "CondDBAccessSvc",
             "CondDBDispatcherSvc", "CondDBLayeringSvc", "CondDBTimeSwitchSvc", 
             "CondDBSQLiteCopyAccSvc", "CondDBLogger",
             "CondDBCnvSvc" ]

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
    # Check for supported types
    if type(accessSvc) not in __CondDBReaders__:
        raise TypeError("addCondDBLayer does not support '%s'"%accessSvc.__class__.__name__)
    
    # Check for basic configuration 
    _assertConfig('addCondDBLayer')
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

def addCondDBAlternative(accessSvc,path):
    """
    Add the given CondDBReader as an alternative to the existing configuration
    for the specified path.
    
    Example:
    addCondDBAlternative(myDB,"/Conditions/MyDetector/Alignment")
    """
    # Check for supported types
    if type(accessSvc) not in __CondDBReaders__:
        raise TypeError("addCondDBAlternative does not support '%s'"%accessSvc.__class__.__name__)
    
    if type(accessSvc) is str:
        accessSvcName = accessSvc
    else:
        accessSvcName = accessSvc.getFullName()
    
    # Check for basic configuration
    _assertConfig('addCondDBAlternative')
    cnvSvc = allConfigurables["CondDBCnvSvc"]
    
    originalReader = cnvSvc.CondDBReader
    if type(originalReader) == CondDBDispatcherSvc:
        # If the original reader is already a dispatcher, we can extend the
        # configuration:
        originalReader.Alternatives[path] = accessSvcName
    else:
        # We have to create a new dispatcher
        name = "CondDBDispatcherSvc"
        i = 0
        while name in allConfigurables:
            i += 1
            name = "CondDBDispatcherSvc_%d"%i
        cnvSvc.CondDBReader = CondDBDispatcherSvc(name,
                                                  MainAccessSvc = originalReader,
                                                  Alternatives = { path: accessSvcName }
                                                  )

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
    
    
    