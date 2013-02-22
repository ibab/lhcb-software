from GaudiKernel.ProcessJobOptions import importOptions


from Gaudi.Configuration import *
from Configurables import Boole, LHCbApp, GaudiSequencer
from Configurables import MCFTDepositCreator, MCFTDigitCreator,FTClusterCreator
from Configurables import MCFTDepositMonitor, MCFTDigitMonitor, FTClusterMonitor
from Configurables import FTRawBankEncoder,  FTRawBankDecoder
from Configurables import L0Conf, DDDBConf
L0Conf().TCK = '0x002A'

#-- Event input
DDDBConf().DbRoot = "/afs/cern.ch/user/o/ogruenbe/public/FT_upgrade/static_DDDB_FT_v6/lhcb.xml"
LHCbApp().DDDBtag   = "MC11-20111102"
LHCbApp().CondDBtag = "sim-20111111-vc-md100"



Datasetname="Bs_mumu_v6_nu325"
#Datasetname="Bs_mumu_v6_nu65"

#Datasetname="MiniBias_v6_nu325"
#Datasetname="MiniBias_v6_nu65"


EventSelector().Input = ["DATAFILE='PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/o/ogruenbe/"+Datasetname+".sim?svcClass=default' TYP='POOL_ROOTTREE' OPT='READ'"]
Boole().DatasetName = Datasetname+"_EC"
#Boole().OutputLevel = DEBUG
#Boole().DetectorDigi = {'VELO':[], 'MUON':[], 'RICH':[], 'TT':[], 'IT':[], 'L0':[], 'CALO':[], 'OT':[]}
Boole().DetectorDigi = ['Velo','FT']

#Boole().DetectorDigi = {'VELO':['Velo'], 'MUON':[''], 'RICH':[''], 'TT':['TT'], 'IT':['IT'], 'L0':[''], 'CALO':[''], 'OT':['OT']}

#Boole().DigiSequence = ['FT']
#Boole().LinkSequence = ['Velo', 'TT', 'Tr' ,'FT' ]
#Boole().DetectorMoni = {'VELO':[], 'MUON':[], 'RICH':[], 'TT':[], 'IT':[], 'L0':[], 'CALO':[], 'OT':[],'MC':[]}
Boole().Outputs = []


myAlgDeposit = MCFTDepositCreator()
#myAlgDeposit.ReflexionCoefficient = 0.8
#myAlgDeposit.ShortAttenuationLength = 1260
#myAlgDeposit.OutputLevel = DEBUG


myAlgDigit = MCFTDigitCreator()
#myAlgDigit.SiPMGain = 4
#myAlgDigit.OutputLevel = DEBUG


myAlgCluster = FTClusterCreator()
#myAlgCluster.OutputLevel = DEBUG
#myAlgCluster.ClusterMaxWidth = 1000
#myAlgCluster.ClusterMinCharge = 2
#myAlgCluster.ClusterMaxCharge = 1000


#myAlgEncoder = FTRawBankEncoder()
#myAlgEncoder.OutputLevel = DEBUG

#myAlgDecoder = FTRawBankDecoder()
#myAlgDecoder.OutputLevel = DEBUG




GaudiSequencer("DigiFTSeq").Members = [myAlgDeposit,
                                       myAlgDigit,
                                      myAlgCluster]#,
                                       #myAlgEncoder,
                                       #myAlgDecoder]

#GaudiSequencer("LinkTrSeq").Members = ["BuildMCTrackInfoUpgrade"]

#OutputStream("DigiWriter").ItemList = ["/Event/Gen/Header#1", "/Event/MC/Header#1", "/Event/MC/DigiHeader#1", "/Event/DAQ/RawEvent#1", "/Event/pSim/MCVertices#1", "/Event/pSim/MCParticles#1", "/Event/Link/Raw/Velo/Clusters#1", "/Event/Link/Raw/TT/Clusters#1", "/Event/Link/Raw/FT/Clusters#1", "/Event/MC/TrackInfo#1" ]


#Boole().EvtMax = 1



#########################################################################################
####################  GaudiPython; Other Imports; Type Shortcuts  #######################
#########################################################################################
#import GaudiPython

#svc = gaudi.histosvc()


