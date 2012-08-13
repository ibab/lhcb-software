###################
# Define InputData
###################

InputData=[]

for i in range(94):
    InputData.append("castor:/castor/cern.ch/user/r/rlambert/smallfiles/R08S14EW/R08S14EW_small_1_#.dst".replace("#",str(i+1)))

RecoDict= {"Reco" : "R08S14EW"}

from Gaudi.Configuration import *
from GaudiConf import IOHelper

id=InputData

######################
#choose number of files
######################

IOHelper().inputFiles(id[:5])

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
