from Gaudi.Configuration import *

from Configurables import GaudiSequencer, EventCountAlg
from Configurables import XMLSummarySvc
from Configurables import LHCbApp

#Set up POOL persistency
importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")

#// The input catalog describes the input file.
#// In this example it is the catalog produced by WriteMin.opts
#FileCatalog().Catalogs = ["xmlcatalog_file:MyCatalog.xml"]

#// Unpack the packed containers on demand
ApplicationMgr().ExtSvc += ["DataOnDemandSvc"]
EventDataSvc().EnableFaultHandler = True 
importOptions("$STDOPTS/UnpackDigi.opts")

#importOptions("$IOEXAMPLEROOT/options/ReadDst.opts")

xmlsummary=XMLSummarySvc("CounterSummarySvc")
#xmlsummary.xmlfile=""
#xmlsummary.xmlfile="summary.xml"
xmlsummary.OutputLevel=1
#xmlsummary.CatalogType="Gaudi::XMLFileCatalog/XMLFileCatalog"
#xmlsummary.CounterList=[".*RWL.*"]

EvtAlg=EventCountAlg("TestCounter")

ApplicationMgr().TopAlg+=[EvtAlg]
ApplicationMgr().ExtSvc+=[xmlsummary]
#ApplicationMgr().ExtSvc+=["IncidentSvc"]
#ApplicationMgr().ExtSvc+=["Gaudi::IODataManager/IODataManager"]


#LHCbApp().XMLSummary="summary.xml"
LHCbApp().DataType="DC06"
LHCbApp().Simulation=True
LHCbApp().EvtMax=100
EventSelector().Input   = [
"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v4-lumi2/00002142/DST/0000/00002142_00000001_5.dst' TYP='POOL_ROOTTREE' OPT='READ'"
]
