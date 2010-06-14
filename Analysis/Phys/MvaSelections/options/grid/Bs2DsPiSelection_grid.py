from Gaudi.Configuration import *
from Gaudi.Configuration import FileCatalog
from Gaudi.Configuration import EventSelector
####################################
from Configurables import DaVinci
from Configurables import GaudiSequencer
############################################
from Configurables import PrintHeader
from Configurables import PrintDecayTree
from Configurables import CopyMCHeader
from Configurables import CopyL0DUReport
from Configurables import CopyHltDecReports

from Configurables import  PhysDesktop
from Configurables import PVReFitterAlg
from Configurables import P2MCRelatorAlg
from Configurables import PVRelatorAlg

from Configurables import  B2DHMvaSelectionAlg
from Configurables import  Bs2DsHMvaUtilityTool
from Configurables import  B2DHSelectionParamTool
from Configurables import  B2DHSelectionCutsTool
from Configurables import  B2DHMvaMonitorHistoTool

from Configurables import OdinTimeDecoder, EventClockSvc
from Configurables import HistogramPersistencySvc
from Configurables import B2DHMvaSelectionAlg


# steering options

NumEvents=-1

###################################################
# import predefined sequences as options file

importOptions("$STDOPTS/PreloadUnits.opts")
importOptions("$STDOPTS/LHCbApplication.opts")
#############################################################
SeqMvASelection = GaudiSequencer("SeqMvASelection") 
dv=DaVinci()
MasterLocation="Phys/"
BsLocalLocation="PreSelCombBs2DsPi"
mainOutputLocationB=MasterLocation+BsLocalLocation
AuxiliaryOutputLocationDs=MasterLocation+"CombineDs2KKPiLoosePreSel"

###################################################
dv.EvtMax=NumEvents
dv.DataType="MC09"
dv.Simulation=True
dv.RedoMCLinks=False
dv.PrintFreq=1000
#dv.InputType="MDST"
dv.InputType="DST"

Bs2DsPi_B2DHMvaSelectionAlg = B2DHMvaSelectionAlg("Bs2DsPi_B2DHMvaSelectionAlg" )
Bs2DsPi_B2DHSelectionParamTool=B2DHSelectionParamTool("Bs2DsPi_B2DHSelectionParamTool")
Bs2DsPi_B2DHMvaFisherDMonitorHistoTool = B2DHMvaMonitorHistoTool("Bs2DsPi_B2DHMvaFisherDMonitorHistoTool")
Bs2DsHMvaUtilityTool = Bs2DsHMvaUtilityTool()
B2DHSelectionCutsTool = B2DHSelectionCutsTool()
Bs2DsHMvaUtilityTool.PathToWeightsFileForBs2DsPi= "$MVASELECTIONSROOT/src/weights/FisherD/Bs2DsPi/"
Bs2DsHMvaUtilityTool.FisherWeightsLocalFileNameForBs2DsPi="TMVAnalysis_BsDsPi_F401Sph_FisherD.weights.xml"
Bs2DsHMvaUtilityTool.Bs2DsPiReconDir=BsLocalLocation+"/"
# Channel number label for Bs2DsK is 0 and that for Bs2DsPi is 1.
#Bs2DsPi_B2DHSelectionParamTool.CurrentB2DHDecayChannelNumber=1; 

Bs2DsPi_B2DHMvaSelectionAlg.ActivateHistograms=True
Bs2DsPi_B2DHMvaSelectionAlg.ActivateL0Selection=True
Bs2DsPi_B2DHMvaSelectionAlg.CurrentB2DHDecayChannelNumber=1;

Bs2DsPi_B2DHMvaSelectionAlg.addTool(Bs2DsPi_B2DHSelectionParamTool)
Bs2DsPi_B2DHMvaSelectionAlg.addTool(Bs2DsPi_B2DHMvaFisherDMonitorHistoTool)
Bs2DsPi_B2DHMvaSelectionAlg.addTool(B2DHSelectionCutsTool)




#Activate the following if the corresponding defaults are to be changed.
#Bs2DsHMvaUtilityTool.PhysicsPathString="Phys/";
#Bs2DsHMvaUtilityTool.ParticleLocation="Particles";
#Bs2DsHMvaUtilityTool.RefittedPVSortedLoc="P2ReFitPVSortedRelations";
#Bs2DsHMvaUtilityTool.StandardPVLocation="Rec/Vertex/Primary";
#ActivateHistograms=False
#Bs2DsPi_B2DHMvaSelectionAlg.ActivateL0Selection=True


importOptions("$MVASELECTIONSROOT/options/Bs2DsPiLoosePreSelection.py")
SeqMvASelection  = GaudiSequencer("SeqSelBs2DsH")

### refit vertex Bs2DsPi
PVRefitterRelationLocationName= "Particle2VertexRelations"
PVReFitterAlgInstanceNameB="PVReFitterAlgBs2DsPi"        
PVReFitterB = PVReFitterAlg(PVReFitterAlgInstanceNameB)
PVReFitterB.ParticleInputLocation = mainOutputLocationB+"/Particles"
PVReFitterB.OutputLevel=3
Bs2DsHMvaUtilityTool.Bs2DsPiPVRefitAlgName= PVReFitterAlgInstanceNameB+"/"
Bs2DsHMvaUtilityTool.RefittedPVRelationLoc=PVRefitterRelationLocationName 
SeqMvASelection.Members += [PVReFitterB]



#from Configurables import StoreExplorerAlg
#storeExp = StoreExplorerAlg()
#SeqMvASelection.Members +=[storeExp]
#storeExp.Load = 1
#storeExp.PrintFreq = 1.0

Bs2DsPi_B2DHMvaSelectionAlg.addTool(Bs2DsHMvaUtilityTool)
SeqMvASelection.Members += [Bs2DsPi_B2DHMvaSelectionAlg]



#ApplicationMgr().ExtSvc += [ "NTupleSvc" ]
ApplicationMgr().HistogramPersistency = "ROOT"
#dv.TupleFile = '/afs/cern.ch/user/s/seaso/scratch0/DavinciData/ntupMva/BsDsPiSignal_Set_test_ntuple_Example.root'
dv.UserAlgorithms = [SeqMvASelection]
#HistogramPersistencySvc().OutputFile = "BsDsPiSignal_test.root";
#HistogramPersistencySvc().OutputFile = "/afs/cern.ch/user/s/seaso/scratch0/DavinciData/histoMva/BsDsPiSignal_test.root";
#HistogramPersistencySvc().OutputFile = "/afs/cern.ch/user/s/seaso/scratch0/DavinciData/histoMva/BsDsPiBBarBackgr_test.root";


#importOptions ("$MVASELECTIONSROOT/datafiles/Bs2DsPi/Bs2DsPi_MC09_04March2010_Signal_PFN.py" )
#importOptions ("$MVASELECTIONSROOT/datafiles/Backgr/BBar_MC09_StrippedFiles_pfn.py" )

