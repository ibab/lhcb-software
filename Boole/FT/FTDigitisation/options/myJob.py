from GaudiKernel.ProcessJobOptions import importOptions
#importOptions("$APPCONFIGROOT/options/Boole/Upgrade-WithTruth.py")

from Gaudi.Configuration import *
from Configurables import Boole, LHCbApp, GaudiSequencer, MCFTDepositCreator, MCFTDigitCreator,FTClusterCreator#,FTRawBankEncoder,FTRawBankDecoder
from Configurables import L0Conf, DDDBConf
L0Conf().TCK = '0x002A'

#-- Event input
LHCbApp().DDDBtag   = "MC11-20111102"
LHCbApp().CondDBtag = "sim-20111111-vc-md100"

DDDBConf().DbRoot = "/afs/cern.ch/user/o/ogruenbe/public/FT_upgrade/static_DDDB_FT_v4/lhcb.xml"
#DDDBConf().DbRoot = "/afs/cern.ch/user/p/phopchev/public/FT/DDDBSlice_FT_v4/lhcb.xml"

EventSelector().Input = ["DATAFILE='PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/o/ogruenbe/Bs_mumu_v4_nu20.sim?svcClass=default' TYP='POOL_ROOTTREE' OPT='READ'"]
Boole().DatasetName = "FTTest"
Boole().DetectorDigi = {'VELO':[], 'MUON':[], 'RICH':[], 'TT':[], 'IT':[], 'L0':[], 'CALO':[], 'OT':[]}
Boole().LinkSequence = ['FT' ]
Boole().DetectorMoni = {'VELO':[], 'MUON':[], 'RICH':[], 'TT':[], 'IT':[], 'L0':[], 'CALO':[], 'OT':[],'MC':[]}
Boole().Outputs = []

myAlgDeposit = MCFTDepositCreator()
#myAlgDeposit.OutputLevel = DEBUG


myAlgDigit = MCFTDigitCreator()
myAlgDigit.OutputLevel = DEBUG


myAlgCluster = FTClusterCreator()
myAlgCluster.OutputLevel = DEBUG
myAlgCluster.ClusterMaxWidth = 1000
myAlgCluster.ClusterMinCharge = 1
myAlgCluster.ClusterMaxCharge = 1000


#myAlgEncoder = FTRawBankEncoder()
#myAlgEncoder.OutputLevel = DEBUG

#myAlgDecoder = FTRawBankDecoder()
#myAlgDecoder.OutputLevel = DEBUG
#myAlgEncoder.InputLocation = "MC/FT/Digits"

#GaudiSequencer("LinkFTSeq").Members = [myAlgDeposit,myAlgDigit,myAlgCluster,myAlgEncoder,myAlgDecoder]
GaudiSequencer("LinkFTSeq").Members = [myAlgDeposit,myAlgDigit]


Boole().EvtMax = 1
