"""
High level configuration tools for Conditions Database.
"""
__version__ = "$Id: Configuration.py,v 1.2 2008-03-03 11:34:37 marcocle Exp $"
__author__  = "Marco Clemencic <Marco.Clemencic@cern.ch>"

from Gaudi.Configuration import allConfigurables
from Configurables import ( CondDBAccessSvc, 
                            CondDBDispatcherSvc, 
                            CondDBLayeringSvc, 
                            CondDBCnvSvc,
                            CondDBSQLiteCopyAccSvc,
                            CondDBLogger )

# Exported symbols
__all__ = [ "addCondDBLayer", "addCondDBAlternative", "useCondDBLogger" ]
# Configurables provided by the package
__all__ += [ "CondDBAccessSvc",
             "CondDBDispatcherSvc", "CondDBLayeringSvc",
             "CondDBSQLiteCopyAccSvc", "CondDBLogger",
             "CondDBCnvSvc" ]

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
    if type(accessSvc) not in [ CondDBAccessSvc,
                                CondDBDispatcherSvc, 
                                CondDBLayeringSvc,
                                CondDBSQLiteCopyAccSvc,
                                str ]:
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
    if type(accessSvc) not in [ CondDBAccessSvc,
                                CondDBDispatcherSvc, 
                                CondDBLayeringSvc,
                                CondDBSQLiteCopyAccSvc,
                                str ]:
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
        # - get initial mapping
        mapping = {}
        for l in originalReader.Alternatives:
            k,v = l.split('=')
            mapping[k] = v
        # - add the new one (it will replace the existing one, if any)
        mapping[path] = accessSvcName
        # - reset the mapping property and fill it again
        originalReader.Alternatives = []
        for p in mapping:
            originalReader.Alternatives.append("%s=%s"%(p,mapping[p]))
    else:
        # We have to create a new dispatcher
        name = "CondDBDispatcherSvc"
        i = 0
        while name in allConfigurables:
            i += 1
            name = "CondDBDispatcherSvc_%d"%i
        cnvSvc.CondDBReader = CondDBDispatcherSvc(name,
                                                  MainAccessSvc = originalReader,
                                                  Alternatives = [ "%s=%s"%(path,accessSvcName) ]
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
