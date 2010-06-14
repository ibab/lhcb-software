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
BsLocalLocation="PreSelCombBs2DsK"
mainOutputLocationA=MasterLocation+BsLocalLocation
AuxiliaryOutputLocationDs=MasterLocation+"CombineDs2KKPiLoosePreSel"

###################################################
dv.EvtMax=NumEvents
dv.DataType="MC09"
dv.Simulation=True
dv.RedoMCLinks=False
dv.PrintFreq=1000
#dv.InputType="MDST"
dv.InputType="DST"

Bs2DsK_B2DHMvaSelectionAlg = B2DHMvaSelectionAlg("Bs2DsK_B2DHMvaSelectionAlg" )
Bs2DsK_B2DHSelectionParamTool=B2DHSelectionParamTool("Bs2DsK_B2DHSelectionParamTool")
Bs2DsK_B2DHMvaFisherDMonitorHistoTool = B2DHMvaMonitorHistoTool("Bs2DsK_B2DHMvaFisherDMonitorHistoTool")
Bs2DsHMvaUtilityTool = Bs2DsHMvaUtilityTool()
B2DHSelectionCutsTool = B2DHSelectionCutsTool()
Bs2DsHMvaUtilityTool.PathToWeightsFileForBs2DsK= "$MVASELECTIONSROOT/src/weights/FisherD/Bs2DsK/"
Bs2DsHMvaUtilityTool.FisherWeightsLocalFileNameForBs2DsK="TMVAnalysis_BsDsK_F401Sph_FisherD.weights.xml"
Bs2DsHMvaUtilityTool.Bs2DsKReconDir=BsLocalLocation+"/"
# Channel number label for Bs2DsK is 0.


Bs2DsK_B2DHMvaSelectionAlg.ActivateHistograms=True
Bs2DsK_B2DHMvaSelectionAlg.ActivateL0Selection=True
Bs2DsK_B2DHMvaSelectionAlg.CurrentB2DHDecayChannelNumber=0

Bs2DsK_B2DHMvaSelectionAlg.addTool(Bs2DsK_B2DHSelectionParamTool)
Bs2DsK_B2DHMvaSelectionAlg.addTool(Bs2DsK_B2DHMvaFisherDMonitorHistoTool)
Bs2DsK_B2DHMvaSelectionAlg.addTool(B2DHSelectionCutsTool)




#Activate the following if the corresponding defaults are to be changed.
#Bs2DsHMvaUtilityTool.PhysicsPathString="Phys/";
#Bs2DsHMvaUtilityTool.ParticleLocation="Particles";
#Bs2DsHMvaUtilityTool.RefittedPVSortedLoc="P2ReFitPVSortedRelations";
#Bs2DsHMvaUtilityTool.StandardPVLocation="Rec/Vertex/Primary";
#ActivateHistograms=False
#Bs2DsK_B2DHMvaSelectionAlg.ActivateL0Selection=True


importOptions("$MVASELECTIONSROOT/options/Bs2DsKLoosePreSelection.py")
SeqMvASelection  = GaudiSequencer("SeqSelBs2DsH")

### refit vertex Bs2DsK
PVRefitterRelationLocationName= "Particle2VertexRelations"
PVReFitterAlgInstanceNameA="PVReFitterAlgBs2DsK"        
PVReFitterA = PVReFitterAlg(PVReFitterAlgInstanceNameA)
PVReFitterA.ParticleInputLocation = mainOutputLocationA+"/Particles"
PVReFitterA.OutputLevel=3
Bs2DsHMvaUtilityTool.Bs2DsKPVRefitAlgName= PVReFitterAlgInstanceNameA+"/"
Bs2DsHMvaUtilityTool.RefittedPVRelationLoc=PVRefitterRelationLocationName 
SeqMvASelection.Members += [PVReFitterA]



#from Configurables import StoreExplorerAlg
#storeExp = StoreExplorerAlg()
#SeqMvASelection.Members +=[storeExp]
#storeExp.Load = 1
#storeExp.PrintFreq = 1.0

Bs2DsK_B2DHMvaSelectionAlg.addTool(Bs2DsHMvaUtilityTool)
SeqMvASelection.Members += [Bs2DsK_B2DHMvaSelectionAlg]



#ApplicationMgr().ExtSvc += [ "NTupleSvc" ]
ApplicationMgr().HistogramPersistency = "ROOT"
#dv.TupleFile = '/afs/cern.ch/user/s/seaso/scratch0/DavinciData/ntupMva/BsDsKSignal_Set_test_ntuple_Example.root'
dv.UserAlgorithms = [SeqMvASelection]
#HistogramPersistencySvc().OutputFile = "BsDsKSignal_Set_test__histo_ExampleA.root";
#HistogramPersistencySvc().OutputFile = "/afs/cern.ch/user/s/seaso/scratch0/DavinciData/histoMva/BsDsKSignal_Set_test__histo_Example_CVS.root";
#HistogramPersistencySvc().OutputFile = "/afs/cern.ch/user/s/seaso/scratch0/DavinciData/histoMva/BsDsKBBarBackgr_testA.root";


#importOptions ("$MVASELECTIONSROOT/datafiles/Bs2DsK/Bs2DsK_MC09_04March2010_Signal_PFN.py" )
#importOptions ("$MVASELECTIONSROOT/datafiles/Backgr/BBar_MC09_StrippedFiles_pfn.py" )
