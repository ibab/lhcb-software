"""
High level configuration tools for Conditions Database.
"""
__version__ = "$Id: Configuration.py,v 1.1 2008-02-29 19:11:56 marcocle Exp $"
__author__  = "Marco Clemencic <Marco.Clemencic@cern.ch>"

from Gaudi.Configuration import allConfigurables
from Configurables import ( CondDBAccessSvc, 
                            CondDBDispatcherSvc, 
                            CondDBLayeringSvc, 
                            CondDBCnvSvc,
                            CondDBSQLiteCopyAccSvc )

# Exported symbols
__all__ = [ "addCondDBLayer", "addCondDBAlternative" ]

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
    if "CondDBCnvSvc" not in allConfigurables:
        raise RuntimeError("You cannot call 'addCondDBLayer' before the standard CondDB configuration")
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
    if "CondDBCnvSvc" not in allConfigurables:
        raise RuntimeError("You cannot call 'addCondDBAlternative' before the standard CondDB configuration")
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
