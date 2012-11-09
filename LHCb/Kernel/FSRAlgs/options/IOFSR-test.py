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
id=["castor:/castor/cern.ch/user/r/rlambert/smallfiles/R08S14EW/R08S14EW_merged_1.dst", "castor:/castor/cern.ch/user/r/rlambert/smallfiles/R08S14EW/R08S14EW_merged_2.dst"]

outputname = "merged_pah.dst"

######################
#choose number of files
######################

IOHelper().inputFiles(id)

myAlgs=IOHelper().outputAlgs(outputname,"InputCopyStream",writeFSR=False)

IOHelper().setupServices()

ApplicationMgr().TopAlg+=myAlgs

##############################
# test adding the new IOFSR service
##############################
from Configurables import IOFSRSvc, FSRNavigator
IOFSRSvc().OutputLevel=DEBUG
#ApplicationMgr().ExtSvc  += [ IOFSRSvc() ]
IOFSRSvc().addTool(FSRNavigator,name="FSRNavigator")
IOFSRSvc().FSRNavigator.OutputLevel=INFO
IOFSRSvc().PrintIOFSR=True
#IOFSRSvc().DefaultStatus="VERIFIED"
#IOFSRSvc().OverrideStatus=True


##############################
#fill summary every event
##############################

from Configurables import ( XMLSummary )
XMLSummary().XMLSummary="summary.xml"
from Configurables import XMLSummarySvc
XMLSummarySvc("CounterSummarySvc").UpdateFreq=1

##############################
# Run the merger, this bit should soon be made a configurable,
# or automatic through either LHCbApp or some other Merger()
##############################

from Configurables import GaudiSequencer
LumiSeq=GaudiSequencer("LumiSeq")
from Configurables import FSRCleaner, LumiMergeFSR, EventAccounting
LumiSeq.Members=[LumiMergeFSR("MergeFSR")]
ApplicationMgr().TopAlg+=[LumiSeq]

##############################
# test adding the new IOFSR writing alg
##############################
#winstance=IOHelper()._fsrWriter(outputname,"LHCbFSRStream")
#winstance.OutputLevel=DEBUG
#winstance.CleanTree=True
#winstance.AddIOFSR=True
#ApplicationMgr().TopAlg  += [ winstance ]

##############################################
#Debug printout, lists all cleaned directories
##############################################

IOHelper().debugIO()

#IODataManager().AgeLimit=3
