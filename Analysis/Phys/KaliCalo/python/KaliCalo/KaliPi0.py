from Gaudi.Configuration import  *


importOptions('$CALORECOROOT/options/SinglePhotonRec.opts')
importOptions('$CALORECOROOT/options/CaloECorrection.opts')
importOptions('$CALORECOROOT/options/CaloSCorrection.opts')
importOptions('$CALORECOROOT/options/CaloLCorrection.opts')

from Configurables import DaVinci, HistogramPersistencySvc, GaudiSequencer 

cc = GaudiSequencer (
    'CaloClusterOnDemand' ,
    Members = [ 
    "CellularAutomatonAlg/EcalClust",
    "CaloSharedCellAlg/EcalShare",
    "CaloClusterCovarianceAlg/EcalCovar",
    "CaloClusterCorrect3x3Position/EcalPosition"]
    )

from Configurables import CaloSinglePhotonAlg, CaloSelectorNOT 

sp = CaloSinglePhotonAlg (
    'SinglePhotonRec'   ,
    PropertiesPrint = True ,
    SelectionTools = [
    "CaloSelectCluster/PhotonClusterSelect"          ,
    "CaloSelectNeutralClusterWithTracks/NeutralCluster" ] )

## "CaloSelectNeutralClusterWithSPD/NeutralCluster" ] )

# "CaloSelectNeutralClusterWithSPD/SPD" ] )

from Configurables import LoKi__Kali_Pi0 as Kali

from Configurables import NeutralProtoPAlg1
from Configurables import NeutralProtoPAlg

kali = Kali ( "Kali"             ,
              NTupleLUN  = "PI0" ,
              HistoPrint = True  ,
              InputLocations = [ 'StdLooseAllPhotons' ]
              )

from CommonParticles.StdLooseAllPhotons import StdLooseAllPhotons


np = NeutralProtoPAlg (
    'PhotonPP'       ,
    LightMode = True              ## ATTENTION!!!
    )

mySeq = GaudiSequencer(
    'MySeq', Members =
    [
    cc , sp  , np ,
    kali ] )

daVinci = DaVinci (
    DataType       = 'DC06' , # default  
    Simulation     = True   ,
    HltType        = ''     ,
    EvtMax         = 1000   ,
    UserAlgorithms = [ mySeq ] 
    ) 

from Configurables import NTupleSvc, HistogramPersistencySvc

HistogramPersistencySvc ( OutputFile = 'Pi0_Histos.root' ) 
NTupleSvc ( Output = [ "PI0 DATAFILE='Pi0_Tuples.root' TYPE='ROOT' OPT='NEW'"] )

from Configurables import EventSelector

EventSelector ( 
    PrintFreq = 1 ,
    Input = [
    #"DATAFILE='PFN:output.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:/afs/cern.ch/lhcb/group/calo/ecal/vol10/DATA/Bs2PsiPhi.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    #,"DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2PsiPhi.dst'    TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2PsiPhi_1.dst' TYP='POOL_ROOTTREE' OPT='READ'", 
    "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2PsiPhi_2.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2PsiPhi_3.dst' TYP='POOL_ROOTTREE' OPT='READ'"
    ]
    )


from Configurables import OutputStream

writer = OutputStream (
    'WRITER', 
    ItemList =  [
    "/Event#1"            ,
    "/Event/DAQ#1"        ,
    "/Event/DAQ/ODIN#1"   , 
    "/Event/Raw#1"        ,
    "/Event/Raw/Spd#1"    ,
    "/Event/Raw/Prs#1"    ,
    "/Event/Raw/Ecal#1"   ,
    "/Event/Raw/Hcal#1"   ,
    "/Event/Raw/Spd/Digits#1"  ,
    "/Event/Raw/Prs/Digits#1"  ,
    "/Event/Raw/Ecal/Digits#1" ,
    "/Event/Raw/Hcal/Digits#1"
    ] ,
    Output = "DATAFILE='PFN:output2.dst' TYP='POOL_ROOTTREE' OPT='REC'" ,
    RequireAlgs = [ "Kali" ] 
    )

am = ApplicationMgr( OutStream = [ writer ] )

def  action ( ) :
    
    dvinit = getConfigurable('DaVinciInitSeq')
    dvinit.Members = [
        getConfigurable('CaloClusterOnDemand')
        ]
    #dvmain = getConfigurable('DaVinciMainSequence')
    #dvmain.Members = [ cc , sp , kali ] 
    
appendPostConfigAction ( action )
