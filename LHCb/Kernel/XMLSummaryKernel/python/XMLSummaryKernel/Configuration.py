"""
Configuration of the XMLSummarySvc
"""
__version__ = "$Id: Configuration.py,v 1.1 2009-09-18 14:02:42 rlambert Exp $"
__author__  = "Rob Lambert"

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import ( XMLSummarySvc )

class XMLSummary(LHCbConfigurableUser):
    __slots__ = {
        "XMLSummary"   : None
        }
    
    _propertyDocDct = { 
       'XMLSummary'   : """ Add an XML summary file, default None """
       }
    
    __used_configurables__ = [ XMLSummarySvc ]
    
    def __apply_configuration__(self):
        if hasattr( self, "XMLSummary" ):
            if self.getProp("XMLSummary") is None: return
            xmlSum=XMLSummarySvc("CounterSummarySvc")
            xmlSum.xmlfile=self.getProp("XMLSummary")
            ApplicationMgr().ExtSvc+=[ xmlSum ]
            if "Gaudi::IODataManager/IODataManager" not in ApplicationMgr().ExtSvc:
                ApplicationMgr().ExtSvc+=[ "Gaudi::IODataManager/IODataManager" ]
            
