###################
# Define InputData
###################

from PRConfig import TestFileDB
TestFileDB.test_file_db["R08S14_smallfiles"].run()

from Gaudi.Configuration import *
from GaudiConf import IOHelper


######################
#choose number of files
######################

IOHelper().outStream("merged.dst","InputCopyStream")

IOHelper().setupServices()

##############################
#fill summary every event
##############################

from Configurables import ( XMLSummary )
XMLSummary().XMLSummary="summary.xml"
from Configurables import XMLSummarySvc
XMLSummarySvc("CounterSummarySvc").UpdateFreq=1

##############################
#Run the merger, this bit should soon be made a configurable,
# or automatic through either LHCbApp or some other Merger()
##############################

from Configurables import GaudiSequencer
LumiSeq=GaudiSequencer("LumiSeq")
from Configurables import FSRCleaner, LumiMergeFSR, EventAccounting
LumiSeq.Members=[EventAccounting("EventAccount"),LumiMergeFSR("MergeFSR"),FSRCleaner()]
ApplicationMgr().TopAlg+=[LumiSeq]

##############################################
#Debug printout, lists all cleaned directories
##############################################

FSRCleaner().OutputLevel=DEBUG
#FSRCleaner().Enable=False
