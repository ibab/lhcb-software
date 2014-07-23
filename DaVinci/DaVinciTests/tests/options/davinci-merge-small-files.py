###################
# Define InputData
###################
from Gaudi.Configuration import *

from PRConfig import TestFileDB
InputData = TestFileDB.test_file_db["R08S14_smallfiles"].filenames

######################
#choose number of files
######################

from GaudiConf import IOHelper

IOHelper().inputFiles( InputData[:5] )

IOHelper().outStream("merged.dst","InputCopyStream")

IOHelper().setupServices()

##############################
#fill summary every event
##############################

from Configurables import XMLSummary
XMLSummary().XMLSummary="summary.xml"
from Configurables import XMLSummarySvc
XMLSummarySvc("CounterSummarySvc").UpdateFreq=1

##############################
#Run the merger
##############################

from Configurables import GaudiSequencer
from Configurables import LumiAlgsConf
LumiSeq=GaudiSequencer("LumiSeq")
LumiAlgsConf().LumiSequencer=LumiSeq
LumiAlgsConf().MergeFSR=True
LumiAlgsConf().InputType="MDST"
ApplicationMgr().TopAlg+=[LumiSeq]

##############################################
#Debug printout, lists all cleaned directories
##############################################


from Configurables import FSRCleaner
FSRCleaner().OutputLevel=DEBUG
#FSRCleaner().Enable=False
