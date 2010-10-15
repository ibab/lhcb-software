import os
from Gaudi.Configuration import *
from Gaudi.Configuration import FileCatalog
from Gaudi.Configuration import EventSelector
######################################################
# MicroDst stuff
#from MicroDST.MicroDSTAlgorithm import *
#####################################################
# import things needed for the Pre Selection 
#

####################################
from Configurables import DaVinci
from Configurables import GaudiSequencer
############################################
from Configurables import PrintHeader
from Configurables import PrintDecayTree
from Configurables import CopyL0DUReport
from Configurables import CopyHltDecReports

from Configurables import VertexCloner
from Configurables import ProtoParticleCloner


from Configurables import  PhysDesktop
from Configurables import PVReFitterAlg

from Configurables import B2DHMvaPreSelectionAlg
from Configurables import B2DHMvaPreSelectionCutsTool
from Configurables import B2DHMvaPreSelectionParamTool
from Configurables import B2DHPreselMvaFisherDMonitorHistoTool
from Configurables import B2DHPreselMvaUtilityTool

from Configurables import OdinTimeDecoder, EventClockSvc
from Configurables import HistogramPersistencySvc

from Configurables import SelDSTWriter
from StrippingConf.Configuration import StrippingConf
from Configurables import CheckPV
from Configurables import LoKi__VoidFilter as TesFilter
from Configurables import LoKi__HDRFilter as HDRFilter  


from StrippingSelections.StartupOptions import veloNZSKiller, redoPV
from Configurables import CondDB

#CondDB(UseOracle = True)
importOptions("$APPCONFIGOPTS/UseOracle.py")
##############################################################
# EventClockSvc(InitialTime = 1)
###############################################################
# Trigger
from HltConf.Configuration import *
#HltConf().replaceL0BanksWithEmulated = True
#HltConf().Hlt2IgnoreHlt1Decision=True
#HltConf().hltType='Hlt1+Hlt2'
#HltConf().applyConf()

#some steering options
#Number of events to process
NumEvents=1200
#NumEvents=10000

#NumEvents=-1



#refit PV 
PVRefit=False


activateBs2DsPi=True
activateBs2DsK=False
activateBd2DdPi=False
useStrippedDataForB=True


########################
###################################################
# import predefined sequences as options file

#importOptions("$STDOPTS/PreloadUnits.opts")
#importOptions("$STDOPTS/LHCbApplication.opts")
#############################################################
#some definitions
dv=DaVinci()
StrippedDataMasterLocation=""
MasterLocation="Phys/"
BsLocalLocationA="PreSelCombBs2DsK"
BsLocalLocationB="PreSelCombBs2DsPi"
BsLocalLocationC="PreSelCombBd2DdPi"
mainOutputLocationA=MasterLocation+BsLocalLocationA
mainOutputLocationB=MasterLocation+BsLocalLocationB
mainOutputLocationC=MasterLocation+BsLocalLocationC
AuxiliaryOutputLocationDs=MasterLocation+"CombineDs2KKPiLoosePreSel"
AuxiliaryOutputLocationDd=MasterLocation+"CombineDd2KPiPiLoosePreSel"
if(useStrippedDataForB):
    StrippedDataMasterLocation="Hadronic/"    
    MasterLocation="Hadronic/Phys/"
    mainOutputLocationA=MasterLocation+"B2DXChi2LooseWithD2hhhLine"
    mainOutputLocationB=MasterLocation+"B2DXChi2LooseWithD2hhhLine"
    mainOutputLocationC=MasterLocation+"B2DXChi2LooseWithD2hhhLine"

tesFilter=TesFilter('TESFilter',
                  Code="CONTAINS('/Event/Hadronic/Phys/B2DXChi2LooseWithD2hhhLine/Particles')>0")

#########################
# apply all the required options
#
dv.EvtMax=NumEvents
dv.DataType="2010"
#dv.InputType='DST'
dv.InputType='SDST'
dv.Simulation=False
dv.PrintFreq=100
dv.RedoMCLinks=False
dv.L0=True

########################################
CheckPV1=CheckPV('PV1',MinPVs=1)
dv.EventPreFilters +=[CheckPV1]
dv.EventPreFilters += [tesFilter]

#importOptions("$MVAPRESELB2DHROOT/options/realdata/Bs2DsPiLoosePreSelection_RealData.py")
importOptions("$MVAPRESELB2DHROOT/options/realdata/Bs2DHLoosePreSelection_RealData.py")
SeqMvASelection  = GaudiSequencer("SeqSelBs2DsH")

SeqMvASelection.Members += [veloNZSKiller() ]

#storeExp = StoreExplorerAlg()
#SeqMvASelection.Members += [storeExp ]
#storeExp.Load = 1
#storeExp.PrintFreq = 1.0



Bs2DH_B2DHMvaPreSelectionAlg = B2DHMvaPreSelectionAlg ("Bs2DH_B2DHMvaPreSelectionAlg" )
Bs2DH_B2DHMvaPreSelectionAlg.InputLocations = [BsLocalLocationB]
Bs2DH_B2DHMvaPreSelectionParamTool=B2DHMvaPreSelectionParamTool("Bs2DH_B2DHMvaPreSelectionParamTool")
Bs2DH_B2DHPreselMvaFisherDMonitorHistoTool = B2DHPreselMvaFisherDMonitorHistoTool("Bs2DH_B2DHPreselMvaFisherDMonitorHistoTool")
B2DHPreselMvaUtilityTool = B2DHPreselMvaUtilityTool()
B2DHMvaPreSelectionCutsTool = B2DHMvaPreSelectionCutsTool()
B2DHPreselMvaUtilityTool.PathToWeightsFileForBs2DsPi= os.path.join("$MVAPRESELB2DHROOT","src","weights","FisherD_Bs2DsH/")
B2DHPreselMvaUtilityTool.FisherWeightsLocalFileNameForBs2DsPi="TMVAnalysis_Bs2DsH_F601Sph_FisherD.weights.xml"
B2DHPreselMvaUtilityTool.Bs2DsPiReconDir=BsLocalLocationB+"/"
B2DHPreselMvaUtilityTool.StandardPVLocation = StrippedDataMasterLocation+"Rec/Vertex/Primary"
# Channel number label for Bs2DsK is 0 and that for Bs2DsPi is 1.
#Bs2DsPi_B2DHSelectionParamTool.CurrentB2DHDecayChannelNumber=1; 

Bs2DH_B2DHMvaPreSelectionAlg.ActivateHistograms=True
Bs2DH_B2DHMvaPreSelectionAlg.ActivateL0Selection=True
Bs2DH_B2DHMvaPreSelectionAlg.CurrentB2DHDecayChannelNumber=1;

Bs2DH_B2DHMvaPreSelectionAlg.addTool(Bs2DH_B2DHMvaPreSelectionParamTool)
Bs2DH_B2DHMvaPreSelectionAlg.addTool(Bs2DH_B2DHPreselMvaFisherDMonitorHistoTool)
Bs2DH_B2DHMvaPreSelectionAlg.addTool(B2DHMvaPreSelectionCutsTool)



### refit vertex Bs2DsPi
PVRefitterRelationLocationName= "Particle2VertexRelations"
PVReFitterAlgInstanceNameB="PVReFitterAlgBs2DsPi"        
PVReFitterB = PVReFitterAlg(PVReFitterAlgInstanceNameB)
PVReFitterB.ParticleInputLocation = mainOutputLocationB+"/Particles"
PVReFitterB.OutputLevel=3
B2DHPreselMvaUtilityTool.Bs2DsPiPVRefitAlgName= PVReFitterAlgInstanceNameB+"/"
B2DHPreselMvaUtilityTool.RefittedPVRelationLoc=PVRefitterRelationLocationName 
#SeqMvASelection.Members += [PVReFitterB]


Bs2DH_B2DHMvaPreSelectionAlg.addTool(B2DHPreselMvaUtilityTool)
SeqMvASelection.Members += [Bs2DH_B2DHMvaPreSelectionAlg]






########################################



 
#ApplicationMgr().HistogramPersistency = "ROOT"
#HistogramPersistencySvc().OutputFile = "/afs/cern.ch/user/s/seaso/scratch0/DavinciData/histoMva/BsDH_RealData_test.root";

dv.UserAlgorithms = [SeqMvASelection]
dv.HistogramFile = "/afs/cern.ch/user/s/seaso/scratch0/DavinciData/histoMva/BsDsH_RealData_Strippingtest_AllDMode.root" 
#HistogramPersistencySvc().OutputFile = "BsDsPiSignal_test_Example.root";

#HistogramPersistencySvc().OutputFile = "Stripping_RealData_test_Bs2dsPi.root";

#importOptions ("$MVAPRESELB2DHROOT/dataFiles/realdata/LHCb_Collision10_429155_RealData+RecoStripping-04_90000000__BHADRONDST_MagDown_Test_PFN.py" )
#importOptions ("$MVAPRESELB2DHROOT/dataFiles/realdata/StdStrippingTestFilesJuly2010_PFN_Test.py")
#importOptions ("$MVAPRESELB2DHROOT/dataFiles/realdata/StdStrippingTestFilesJuly2010_LFN.py")
#DaVinci().Input   = [
#"   DATAFILE='LFN:/lhcb/data/2010/SDST/00008037/0000/00008037_00001700_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'",
#"   DATAFILE='LFN:/lhcb/data/2010/SDST/00008037/0000/00008037_00001702_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'",
#"   DATAFILE='LFN:/lhcb/data/2010/SDST/00008037/0000/00008037_00001703_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'",
#"   DATAFILE='LFN:/lhcb/data/2010/SDST/00008037/0000/00008037_00001706_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'"]
#DaVinci().Input   = [
#"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00007574/0000/00007574_00000011_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'",
#"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00007574/0000/00007574_00000012_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'",
#"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00007574/0000/00007574_00000013_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'",
#"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00007574/0000/00007574_00000014_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'",
#"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00007574/0000/00007574_00000015_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'",
#"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00007574/0000/00007574_00000016_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'",
#"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00007574/0000/00007574_00000017_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'",
#"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00007574/0000/00007574_00000018_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'",
#"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00007574/0000/00007574_00000019_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'",
#"   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00007574/0000/00007574_00000020_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'"]

DaVinci().Input   = [
  "   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/HADRONIC.DST/00007580/0000/00007580_00000001_1.hadronic.dst' TYP='POOL_ROOTTREE' OPT='READ'",  
  "   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/HADRONIC.DST/00007580/0000/00007580_00000002_1.hadronic.dst' TYP='POOL_ROOTTREE' OPT='READ'",  
  "   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/HADRONIC.DST/00007580/0000/00007580_00000003_1.hadronic.dst' TYP='POOL_ROOTTREE' OPT='READ'",  
  "   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/HADRONIC.DST/00007580/0000/00007580_00000006_1.hadronic.dst' TYP='POOL_ROOTTREE' OPT='READ'",  
  "   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/HADRONIC.DST/00007580/0000/00007580_00000007_1.hadronic.dst' TYP='POOL_ROOTTREE' OPT='READ'",  
  "   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/HADRONIC.DST/00007580/0000/00007580_00000008_1.hadronic.dst' TYP='POOL_ROOTTREE' OPT='READ'"
    ]


    

FileCatalog().Catalogs= ["xmlcatalog_file:pool_xml_catalog.xml"]
