from Gaudi.Configuration import *
from Gaudi.Configuration import FileCatalog
######################################################
# MicroDst stuff
#from MicroDST.MicroDSTAlgorithm import *
#####################################################
# import things needed for the Selection and tagging
#
from Configurables import GaudiSequencer
from Configurables import MCParticleArrayFilterAlg
from Configurables import FilterMCParticleArrayByDecay
from Configurables import MCDecayFinder
from Configurables import MCParticleCloner
from Configurables import MCVertexCloner
from Configurables import VertexCloner
from Configurables import ProtoParticleCloner
from Configurables import PrintHeader
from Configurables import PrintDecayTree
from Configurables import OutputStream
from Configurables import BTagging, BTaggingTool
from Configurables import PhysDesktop
from Configurables import PVReFitterAlg
from Configurables import CopyRecHeader
from Configurables import CopyMCParticles
from Configurables import CopyParticles
from Configurables import CopyPrimaryVertices
from Configurables import CopyParticle2PVRelations
from Configurables import CopyParticle2MCRelations
from Configurables import CopyHltDecReports
from Configurables import P2MCRelatorAlg
from Configurables import CopyFlavourTag
from Configurables import PVReFitterAlg
from Configurables import PVRelatorAlg
from Configurables import CopyODIN
from Configurables import CopyMCHeader
from Configurables import CopyL0DUReport
from Configurables import DaVinci
from Configurables import AcquireBsDsH
from Configurables import CreateBsDsHNtup
from Configurables import CreateBdDHNtup
from Configurables import EventClockSvc
##############################################################
EventClockSvc(InitialTime = 1)
###############################################################
# Trigger
from HltConf.Configuration import *
#HltConf().replaceL0BanksWithEmulated = True
#HltConf().Hlt2IgnoreHlt1Decision=True
#HltConf().hltType='Hlt1+Hlt2'
#HltConf().applyConf()

#some steering options
#Number of events to process
#NumEvents=100
NumEvents=-1
#Copy info for events not passing selection ?
copyAllEventInfo=False
#Copy MC info when signal MC decay is found ?
keepTrueDecays=False
allEventInfo =False
#Copy related MC particles and decay tree for selected candidates?
storeMCInfo=False
#Activate and Copy B Tagging
ActivateBtagging=True
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
# import the actual pre-selection python options for Bs2DsK where the
# sequence SeqSelBs2DsH is defined
#
importOptions("$BSDSHPRESELROOT/options/realdata/Bs2DsPiLoosePreSelection_RealData.py")
# catch the sequencer defined in the pre-selection file
SeqSelBs2DsH = GaudiSequencer("SeqSelBs2DsH")
#some definitions
dv=DaVinci()
MasterLocation="Phys/"
mainOutputLocationA=MasterLocation+"PreSelCombBs2DsK"
mainOutputLocationB=MasterLocation+"PreSelCombBs2DsPi"
mainOutputLocationC=MasterLocation+"PreSelCombBd2DdPi"
AuxiliaryOutputLocationDs=MasterLocation+"CombineDs2KKPiLoosePreSel"
AuxiliaryOutputLocationDd=MasterLocation+"CombineDd2KPiPiLoosePreSel"
#########################
# apply all the required options
#
dv.EvtMax=NumEvents
dv.DataType="2010"
dv.InputType='DST'
dv.Simulation=False
dv.UserAlgorithms= [SeqSelBs2DsH]
#dv.PrintFreq=100
dv.RedoMCLinks=False
dv.L0=True
#dv.ReplaceL0BanksWithEmulated=True
#dv.HltType="Hlt1+Hlt2"
#dv.HltType="Hlt2"
#dv.Hlt2Requires ='L0'
####################################################
# setup a special MicroDST output stream
#
############################################################
# Now for the Btagging
if (ActivateBtagging) :
    importOptions('$FLAVOURTAGGINGOPTS/BTaggingTool.py')
    BTaggingTool("BTaggingTool").OutputLevel=4
    BTagAlgoA = BTagging('BTaggingBs2DsK')
    BTagAlgoA.addTool(PhysDesktop())
    BTagAlgoA.InputLocations=[mainOutputLocationA]
    BTagLocationA = mainOutputLocationA+"/Tagging"
    BTagAlgoA.TagOutputLocation = BTagLocationA
    copyFlavTagA = CopyFlavourTag("CopyFlavourTagBs2DsK")
    copyFlavTagA.InputLocation = BTagLocationA

    BTagAlgoB = BTagging('BTaggingBs2DsPi')
    BTagAlgoB.addTool(PhysDesktop())
    BTagAlgoB.InputLocations=[mainOutputLocationB]
    BTagLocationB = mainOutputLocationB+"/Tagging"
    BTagAlgoB.TagOutputLocation = BTagLocationB
    copyFlavTagB = CopyFlavourTag("CopyFlavourTagBs2DsPi")
    copyFlavTagB.InputLocation = BTagLocationB

    BTagAlgoC = BTagging('BTaggingBd2DdPi')
    BTagAlgoC.addTool(PhysDesktop())
    BTagAlgoC.InputLocations=[mainOutputLocationC]
    BTagLocationC = "/FlavourTags"
    #BTagAlgoC.TagOutputLocation = BTagLocationC
    copyFlavTagC = CopyFlavourTag("CopyFlavourTagBd2DdPi")
    copyFlavTagC.InputLocation =mainOutputLocationC+ BTagLocationC


    
    if(activateBs2DsK):
        SeqSelBs2DsH.Members += [BTagAlgoA]
        SeqSelBs2DsH.Members += [copyFlavTagA]
        
    if(activateBs2DsPi):
        SeqSelBs2DsH.Members += [BTagAlgoB]
        SeqSelBs2DsH.Members += [copyFlavTagB]

    if(activateBd2DdPi):
        SeqSelBs2DsH.Members += [BTagAlgoC]
        SeqSelBs2DsH.Members += [copyFlavTagC]

        
########################################
## refit vertex
PVRefitterRelationLocationName= "/Particle2VertexRelations"        
### refit vertex Bs2DsK
PVReFitterAlgInstanceNameA="PVReFitterAlgBs2DsK"        
PVReFitterA = PVReFitterAlg(PVReFitterAlgInstanceNameA)
PVReFitterA.ParticleInputLocation = mainOutputLocationA+"/Particles"
PVReFitterA.OutputLevel=3
pvRelateInputLocationA=MasterLocation+PVReFitterAlgInstanceNameA+PVRefitterRelationLocationName
# Take Particle->PV relations table from previous step and produce
# new table, sorted according to an IRelatedPVFinder
#PVRelatorA = PVRelatorAlg("PVRelatorAlgBs2DsK")
#PVRelatorA.P2PVRelationsInputLocation = p2ReFitPVRelationsLocA
#p2pvSortedRelationsLocA = mainOutputLocationA + "/P2ReFitPVSortedRelations"
#PVRelatorA.P2PVRelationsOutputLocation = p2pvSortedRelationsLocA
#PVRelatorA.OutputLevel=4


### refit vertex Bs2DsPi
PVReFitterAlgInstanceNameB="PVReFitterAlgBs2DsPi"
PVReFitterB = PVReFitterAlg(PVReFitterAlgInstanceNameB)
PVReFitterB.ParticleInputLocation = mainOutputLocationB+"/Particles"
PVReFitterB.OutputLevel=3
pvRelateInputLocationB=MasterLocation+PVReFitterAlgInstanceNameB+PVRefitterRelationLocationName
# Take Particle->PV relations table from previous step and produce
# new table, sorted according to an IRelatedPVFinder
#PVRelatorB = PVRelatorAlg("PVRelatorAlgBs2DsPi")
#PVRelatorB.P2PVRelationsInputLocation = p2ReFitPVRelationsLocB
#p2pvSortedRelationsLocB = mainOutputLocationB + "/P2ReFitPVSortedRelations"
#PVRelatorB.P2PVRelationsOutputLocation = p2pvSortedRelationsLocB
#PVRelatorB.OutputLevel=4

### refit vertex Bd2DdPi
PVReFitterAlgInstanceNameC="PVReFitterAlgBd2DdPi"
PVReFitterC = PVReFitterAlg(PVReFitterAlgInstanceNameC)
PVReFitterC.ParticleInputLocation = mainOutputLocationC+"/Particles"
PVReFitterC.OutputLevel=3
pvRelateInputLocationC=MasterLocation+PVReFitterAlgInstanceNameC+PVRefitterRelationLocationName


if(activateBs2DsK):
    SeqSelBs2DsH.Members += [PVReFitterA]
#    SeqSelBs2DsH.Members += [PVRelatorA]
    
if(activateBs2DsPi):
    SeqSelBs2DsH.Members += [PVReFitterB]
#    SeqSelBs2DsH.Members += [PVRelatorB]

if(activateBd2DdPi):
    SeqSelBs2DsH.Members += [PVReFitterC]




########################################
#Activate my User part
acquireBsDsH = AcquireBsDsH()
acquireBsDsH.ActivateBs2DsK=activateBs2DsK
acquireBsDsH.ActivateBs2DsPi=activateBs2DsPi
acquireBsDsH.ActivateBd2DPi=activateBd2DdPi
acquireBsDsH.PVRefitterAlgBs2DsKName=PVReFitterAlgInstanceNameA
acquireBsDsH.PVRefitterAlgBs2DsPiName=PVReFitterAlgInstanceNameB
acquireBsDsH.PVRefitterAlgBd2DPiName=PVReFitterAlgInstanceNameC

acquireBsDsH.PVRefittedRelationLocationName=PVRefitterRelationLocationName
acquireBsDsH.ActivateOnMCData=False
SeqSelBs2DsH.Members += [acquireBsDsH]


createBsDsHNtup=CreateBsDsHNtup()
createBsDsHNtup.ActivateBs2DsKInput=activateBs2DsK
createBsDsHNtup.ActivateBs2DsPiInput=activateBs2DsPi
createBsDsHNtup.ActivateBd2DPiInput=activateBd2DdPi

SeqSelBs2DsH.Members += [createBsDsHNtup]
 
ApplicationMgr().HistogramPersistency = "ROOT"

