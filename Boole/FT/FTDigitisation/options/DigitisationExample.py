from GaudiKernel.ProcessJobOptions import importOptions


from Gaudi.Configuration import *
from Configurables import Boole, LHCbApp, GaudiSequencer
from Configurables import MCFTDepositCreator, MCFTDigitCreator,FTClusterCreator
from Configurables import MCFTDepositMonitor, MCFTDigitMonitor, FTClusterMonitor
from Configurables import L0Conf, DDDBConf
L0Conf().TCK = '0x002A'

#-- Event input
DDDBConf().DbRoot = "/afs/cern.ch/user/o/ogruenbe/public/FT_upgrade/static_DDDB_FT_v4/lhcb.xml"
LHCbApp().DDDBtag   = "MC11-20111102"
LHCbApp().CondDBtag = "sim-20111111-vc-md100"



Datasetname="Bs_mumu_v4_nu20"
#Datasetname="Bs_mumu_v4_nu25"
#Datasetname="Bs_mumu_v4_nu50"


EventSelector().Input = ["DATAFILE='PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/o/ogruenbe/"+Datasetname+".sim?svcClass=default' TYP='POOL_ROOTTREE' OPT='READ'"]
Boole().DatasetName = Datasetname+"_EC"

Boole().DetectorDigi = {'VELO':[], 'MUON':[], 'RICH':[], 'TT':[], 'IT':[], 'L0':[], 'CALO':[], 'OT':[]}
Boole().LinkSequence = ['FT' ]
Boole().DetectorMoni = {'VELO':[], 'MUON':[], 'RICH':[], 'TT':[], 'IT':[], 'L0':[], 'CALO':[], 'OT':[],'MC':[]}
Boole().Outputs = []

myAlgDeposit = MCFTDepositCreator()
#myAlgDeposit.OutputLevel = DEBUG


myAlgDigit = MCFTDigitCreator()
#myAlgDigit.OutputLevel = DEBUG


myAlgCluster = FTClusterCreator()
#myAlgCluster.OutputLevel = DEBUG
myAlgCluster.ClusterMaxWidth = 1000
myAlgCluster.ClusterMinCharge = 1
myAlgCluster.ClusterMaxCharge = 1000


#myAlgEncoder = FTRawBankEncoder()
#myAlgEncoder.OutputLevel = DEBUG

#myAlgDecoder = FTRawBankDecoder()
#myAlgDecoder.OutputLevel = DEBUG
#myAlgEncoder.InputLocation = "MC/FT/Digits"

#GaudiSequencer("DigiFTSeq").Members = [myAlgDeposit,myAlgDigit,myAlgCluster,myAlgEncoder,myAlgDecoder]
GaudiSequencer("LinkFTSeq").Members = [myAlgDeposit,"MCFTDepositMonitor",
                                       myAlgDigit,"MCFTDigitMonitor",
                                       myAlgCluster,"FTClusterMonitor"]


#Boole().EvtMax = 1



#########################################################################################
####################  GaudiPython; Other Imports; Type Shortcuts  #######################
#########################################################################################
#import GaudiPython

#svc = gaudi.histosvc()


