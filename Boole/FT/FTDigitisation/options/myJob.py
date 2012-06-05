from GaudiKernel.ProcessJobOptions import importOptions
#importOptions("$APPCONFIGROOT/options/Boole/Upgrade-WithTruth.py")

from Gaudi.Configuration import *
from Configurables import Boole, LHCbApp, GaudiSequencer, MCFTDepositCreator, MCFTDigitCreator,FTClusterCreator#,FTRawBankEncoder,FTRawBankDecoder
from Configurables import L0Conf, DDDBConf
L0Conf().TCK = '0x002A'

#-- Event input
LHCbApp().DDDBtag   = "MC11-20111102"
LHCbApp().CondDBtag = "sim-20111111-vc-md100"

#DDDBConf().DbRoot = "/afs/cern.ch/user/o/ogruenbe/public/FT_upgrade/myDDDB-LHCb-Feb2012/lhcb.xml"
DDDBConf().DbRoot = "/afs/cern.ch/user/p/phopchev/public/FT/DDDBSlice_FT_v3/lhcb.xml"

EventSelector().Input = ["DATAFILE='PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/o/ogruenbe/Bs_mumu.sim?svcClass=default' TYP='POOL_ROOTTREE' OPT='READ'"]
Boole().DatasetName = "FTTest"
Boole().DetectorDigi = {'VELO':[], 'MUON':[], 'RICH':[], 'TT':[], 'IT':[], 'L0':[], 'CALO':[], 'OT':[]}
Boole().LinkSequence = ['FT' ]
Boole().DetectorMoni = {'VELO':[], 'MUON':[], 'RICH':[], 'TT':[], 'IT':[], 'L0':[], 'CALO':[], 'OT':[],'MC':[]}
Boole().Outputs = []

myAlgDeposit = MCFTDepositCreator()
myAlgDeposit.OutputLevel = DEBUG
#myAlgDeposit.InputLocation = "MC/FT/Hits"

myAlgDigit = MCFTDigitCreator()
myAlgDigit.OutputLevel = DEBUG
#myAlgDigit.InputLocation = "MC/FT/Deposits"

myAlgCluster = FTClusterCreator()
myAlgCluster.OutputLevel = DEBUG
myAlgCluster.ClusterMaxWidth = 8
#myAlgCluster.ClusterMinCharge = 2
#myAlgCluster.ClusterMaxCharge = 8
#myAlgCluster.InputLocation = "MC/FT/Digits"

#myAlgEncoder = FTRawBankEncoder()
#myAlgEncoder.OutputLevel = DEBUG

#myAlgDecoder = FTRawBankDecoder()
#myAlgDecoder.OutputLevel = DEBUG
#myAlgEncoder.InputLocation = "MC/FT/Digits"

#GaudiSequencer("LinkFTSeq").Members = [myAlgDeposit,myAlgDigit,myAlgCluster,myAlgEncoder,myAlgDecoder]
GaudiSequencer("LinkFTSeq").Members = [myAlgDeposit,myAlgDigit,myAlgCluster]
#mySeq["MCFTDigitCreator"].OutputLevel = DEBUG
#mySeq["MCFTDigitCreator"].InputLocation = "MC/FT/Hits"
Boole().EvtMax = 1
