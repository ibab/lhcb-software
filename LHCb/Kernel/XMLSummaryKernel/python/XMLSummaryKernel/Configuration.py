"""
Configuration of the XMLSummarySvc
"""
__version__ = "$Id: Configuration.py,v 1.2 2009-09-19 13:57:30 cattanem Exp $"
__author__  = "Rob Lambert"

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class XMLSummary(LHCbConfigurableUser):
    __slots__ = {
        "XMLSummary"   : None
        }
    
    _propertyDocDct = { 
       'XMLSummary'   : """ Add an XML summary file, default None """
       }
    
    def __apply_configuration__(self):
        if hasattr( self, "XMLSummary" ):
            if self.getProp("XMLSummary") is None: return
            from Configurables import ( XMLSummarySvc )
            xmlSum=XMLSummarySvc("CounterSummarySvc")
            xmlSum.xmlfile=self.getProp("XMLSummary")
            ApplicationMgr().ExtSvc+=[ xmlSum ]
            if "Gaudi::IODataManager/IODataManager" not in ApplicationMgr().ExtSvc:
                ApplicationMgr().ExtSvc+=[ "Gaudi::IODataManager/IODataManager" ]
            
