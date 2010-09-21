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


from StrippingSelections.StartupOptions import veloNZSKiller, redoPV
from Configurables import CondDB

#CondDB(UseOracle = True)
##############################################################
# EventClockSvc(InitialTime = 1)
###############################################################
# Trigger
from HltConf.Configuration import *






activateBs2DsPi=True
activateBs2DsK=False
activateBd2DdPi=False

activatePVRefit=False
usePVRefitResult=True

########################
###################################################
# import predefined sequences as options file

#############################################################
#some definitions
MasterLocation="Phys/"
BsLocalLocationA="PreSelCombBs2DsK"
BsLocalLocationB="PreSelCombBs2DsPi"
BsLocalLocationC="PreSelCombBd2DdPi"
mainOutputLocationA=MasterLocation+BsLocalLocationA
mainOutputLocationB=MasterLocation+BsLocalLocationB
mainOutputLocationC=MasterLocation+BsLocalLocationC

########################################


importOptions("$MVAPRESELB2DHROOT/options/Bs2DHLoosePreSelection.py")
SeqMvASelection  = GaudiSequencer("SeqSelBs2DsH")
DaVinci().UserAlgorithms = [SeqMvASelection]

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
# Channel number label for Bs2DsPi=1. It is used for all B2DH channels.


Bs2DH_B2DHMvaPreSelectionAlg.ActivateHistograms=False
Bs2DH_B2DHMvaPreSelectionAlg.ActivateL0Selection=True
Bs2DH_B2DHMvaPreSelectionAlg.ActivatePrimaryVertexRefit=usePVRefitResult
Bs2DH_B2DHMvaPreSelectionAlg.CurrentB2DHDecayChannelNumber=1

Bs2DH_B2DHMvaPreSelectionAlg.addTool(Bs2DH_B2DHMvaPreSelectionParamTool)
Bs2DH_B2DHMvaPreSelectionAlg.addTool(Bs2DH_B2DHPreselMvaFisherDMonitorHistoTool)
Bs2DH_B2DHMvaPreSelectionAlg.addTool(B2DHMvaPreSelectionCutsTool)



##PV refit not activated.
### refit vertex Bs2DsPi
PVRefitterRelationLocationName= "Particle2VertexRelations"
PVReFitterAlgInstanceNameB="PVReFitterAlgBs2DsPi"        
PVReFitterB = PVReFitterAlg(PVReFitterAlgInstanceNameB)
PVReFitterB.ParticleInputLocation = mainOutputLocationB+"/Particles"
PVReFitterB.OutputLevel=3
B2DHPreselMvaUtilityTool.Bs2DsPiPVRefitAlgName= PVReFitterAlgInstanceNameB+"/"
B2DHPreselMvaUtilityTool.RefittedPVRelationLoc=PVRefitterRelationLocationName 
if(activatePVRefit):
    SeqMvASelection.Members += [PVReFitterB]

Bs2DH_B2DHMvaPreSelectionAlg.addTool(B2DHPreselMvaUtilityTool)
SeqMvASelection.Members += [Bs2DH_B2DHMvaPreSelectionAlg]


########################################



 
