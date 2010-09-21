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
NumEvents=-1
#NumEvents=20
#Copy info for events not passing selection ?
copyAllEventInfo=False
#Copy MC info when signal MC decay is found ?
keepTrueDecays=False
allEventInfo =False
#Copy related MC particles and decay tree for selected candidates?
storeMCInfo=False

#refit PV 
PVRefit=True
keepTrueDecays=True

activateBs2DsPi=True
activateBs2DsK=False
activateBd2DdPi=False
########################
###################################################
# import predefined sequences as options file

#importOptions("$STDOPTS/PreloadUnits.opts")
#importOptions("$STDOPTS/LHCbApplication.opts")
#############################################################
#some definitions
dv=DaVinci()
MasterLocation="Phys/"
BsLocalLocationA="PreSelCombBs2DsK"
BsLocalLocationB="PreSelCombBs2DsPi"
BsLocalLocationC="PreSelCombBd2DdPi"
mainOutputLocationA=MasterLocation+BsLocalLocationA
mainOutputLocationB=MasterLocation+BsLocalLocationB
mainOutputLocationC=MasterLocation+BsLocalLocationC
AuxiliaryOutputLocationDs=MasterLocation+"CombineDs2KKPiLoosePreSel"
AuxiliaryOutputLocationDd=MasterLocation+"CombineDd2KPiPiLoosePreSel"
#########################
# apply all the required options
#
dv.EvtMax=NumEvents
dv.DataType="2010"
dv.InputType='DST'
dv.Simulation=False
#dv.PrintFreq=100
dv.RedoMCLinks=False
dv.L0=True

########################################

#importOptions("$MVAPRESELB2DHROOT/options/realdata/Bs2DsPiLoosePreSelection_RealData.py")
importOptions("$MVAPRESELB2DHROOT/options/realdata/Bs2DHLoosePreSelection_RealData.py")
SeqMvASelection  = GaudiSequencer("SeqSelBs2DsH")

#storeExp = StoreExplorerAlg()
#SeqMvASelection.Members += [storeExp ]
#storeExp.Load = 1
#storeExp.PrintFreq = 1.0



Bs2DsPi_B2DHMvaPreSelectionAlg = B2DHMvaPreSelectionAlg ("Bs2DsPi_B2DHMvaPreSelectionAlg" )
Bs2DsPi_B2DHMvaPreSelectionParamTool=B2DHMvaPreSelectionParamTool("Bs2DsPi_B2DHMvaPreSelectionParamTool")
Bs2DsPi_B2DHPreselMvaFisherDMonitorHistoTool = B2DHPreselMvaFisherDMonitorHistoTool("Bs2DsPi_B2DHPreselMvaFisherDMonitorHistoTool")
B2DHPreselMvaUtilityTool = B2DHPreselMvaUtilityTool()
B2DHMvaPreSelectionCutsTool = B2DHMvaPreSelectionCutsTool()
B2DHPreselMvaUtilityTool.PathToWeightsFileForBs2DsPi= os.path.join("$MVAPRESELB2DHROOT","src","weights","FisherD_Bs2DsH/")
B2DHPreselMvaUtilityTool.FisherWeightsLocalFileNameForBs2DsPi="TMVAnalysis_Bs2DsH_F601Sph_FisherD.weights.xml"
B2DHPreselMvaUtilityTool.Bs2DsPiReconDir=BsLocalLocationB+"/"
# Channel number label for Bs2DsK is 0 and that for Bs2DsPi is 1.
#Bs2DsPi_B2DHSelectionParamTool.CurrentB2DHDecayChannelNumber=1; 


Bs2DsPi_B2DHMvaPreSelectionAlg.ActivateHistograms=True
Bs2DsPi_B2DHMvaPreSelectionAlg.ActivateL0Selection=True
Bs2DsPi_B2DHMvaPreSelectionAlg.CurrentB2DHDecayChannelNumber=1;

Bs2DsPi_B2DHMvaPreSelectionAlg.addTool(Bs2DsPi_B2DHMvaPreSelectionParamTool)
Bs2DsPi_B2DHMvaPreSelectionAlg.addTool(Bs2DsPi_B2DHPreselMvaFisherDMonitorHistoTool)
Bs2DsPi_B2DHMvaPreSelectionAlg.addTool(B2DHMvaPreSelectionCutsTool)



### refit vertex Bs2DsPi
PVRefitterRelationLocationName= "Particle2VertexRelations"
PVReFitterAlgInstanceNameB="PVReFitterAlgBs2DsPi"        
PVReFitterB = PVReFitterAlg(PVReFitterAlgInstanceNameB)
PVReFitterB.ParticleInputLocation = mainOutputLocationB+"/Particles"
PVReFitterB.OutputLevel=3
B2DHPreselMvaUtilityTool.Bs2DsPiPVRefitAlgName= PVReFitterAlgInstanceNameB+"/"
B2DHPreselMvaUtilityTool.RefittedPVRelationLoc=PVRefitterRelationLocationName 
#SeqMvASelection.Members += [PVReFitterB]


Bs2DsPi_B2DHMvaPreSelectionAlg.addTool(B2DHPreselMvaUtilityTool)
SeqMvASelection.Members += [Bs2DsPi_B2DHMvaPreSelectionAlg]






########################################
 
ApplicationMgr().HistogramPersistency = "ROOT"
dv.UserAlgorithms = [SeqMvASelection]
#HistogramPersistencySvc().OutputFile = "BsDsPiSignal_test_Example.root";
HistogramPersistencySvc().OutputFile = "/afs/cern.ch/user/s/seaso/scratch0/DavinciData/histoMva/BsDsPi_RealData_test_temp.root";

importOptions ("$MVAPRESELB2DHROOT/dataFiles/realdata/LHCb_Collision10_429155_RealData+RecoStripping-04_90000000__BHADRONDST_MagDown_Test_PFN.py" )
