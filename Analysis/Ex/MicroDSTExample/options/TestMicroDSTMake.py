#$Id: TestMicroDSTMake.py,v 1.8 2008-11-18 16:37:29 jpalac Exp $
from Gaudi.Configuration import *
from MicroDST.MicroDSTAlgorithm import *
from Configurables import CopyRelatedMCParticles
from Configurables import CopyParticle2PVLink
from Configurables import MCParticleArrayFilterAlg
from Configurables import FilterMCParticleArrayByDecay
from Configurables import MCDecayFinder
from Configurables import MCParticleCloner
from Configurables import MCVertexCloner
from Configurables import VertexCloner
from Configurables import ProtoParticleCloner
from Configurables import PrintHeader
from Configurables import OutputStream
from Configurables import BTagging, BTaggingTool
from Configurables import PhysDesktop
from Configurables import PVReFitterAlg
#==============================================================================
# Some steering options
#
# number of events to process
nEvents = 5000
# Copy information for events not passing the selection?
allEventInfo = False
# Copy MC particles when signal MC decay is found?
keepTrueDecays = False
# Copy related MCParticles and decay tree for selected candidates?
storeMCInfo = True
# B tagging?
BTaggingInfo = True
# re-fit PV?
PVRefit = True
#==============================================================================
importOptions( "$DAVINCIROOT/options/DaVinciCommon.opts" )
importOptions( "$MICRODSTEXAMPLEROOT/options/JpsiPhiData.opts")
importOptions( "$CCBARROOT/options/DoDC06SelBs2Jpsi2MuMuPhi2KK_lifetime_unbiased.opts"  )
mainLocation = "Phys/DC06selBs2JpsiPhi_unbiased"
# get rid of some spam
printSel=PrintHeader('PrintDC06selBs2JpsiPhi')
printSel.OutputLevel=4
#
if (storeMCInfo) :
    DSTMC = "WithMC"
else :
    DSTMC = "NoMC"
    
importOptions('$MICRODSTOPTS/MicroDSTStream.py')
MicroDSTStream=OutputStream('MicroDSTStream')
MicroDSTStream.Output = "DATAFILE='testBs2JpsiPhi_"+DSTMC+"_"+str(nEvents/1000.) +"_Kevt.dst' TYP='POOL_ROOTTREE' OPT='REC'";
MicroDSTStream.OutputLevel=4;

# Create sequence for stuff to be copied for all events.
# Add to OutputStream's AcceptAlgs list (events passing OR
# of this list are written out)
if (allEventInfo) :
    AllEvents = GaudiSequencer('AllEvents')
    ApplicationMgr().TopAlg += [ AllEvents ]
    MicroDSTStream.AcceptAlgs.append( 'AllEvents' )
    
# Create selection sequence and add to OutputStream's AcceptAlgs list
MySelection = GaudiSequencer('SeqDC06selBs2JpsiPhi')
MicroDSTStream.AcceptAlgs.append( MySelection.name() )

#==============================================================================
# Copy RecHeader
MySelection.Members += [CopyRecHeader()]
#==============================================================================
# Copy MC decays matching signal decay descriptor
if (keepTrueDecays) :
    filterMCDecays = MCParticleArrayFilterAlg('FilterMCDecays')
    filterMCDecays.OutputLocation = 'MC/Decays'
    filterMCDecays.addTool( FilterMCParticleArrayByDecay(), name = 'IMCParticleArrayFilter' )
    filterMCDecays.IMCParticleArrayFilter.addTool( MCDecayFinder(), name = 'MCDecayFinder' )
    filterMCDecays.IMCParticleArrayFilter.MCDecayFinder.Decay = '[B_s0 -> (J/psi(1S) -> mu+ mu- {,gamma} {,gamma}) (phi(1020) -> K+ K-)]cc'
    filterMCDecays.OutputLevel = 4;

    CopyMCParticles().InputLocation = "MC/Decays"
    CopyMCParticles().addTool( MCParticleCloner(), name = 'MCParticleCloner' )
    CopyMCParticles().MCParticleCloner.addTool( MCVertexCloner(), name = 'ICloneMCVertex' )
    CopyMCParticles().OutputLevel=4

    if (allEventInfo) :
        AllEvents.Members += [filterMCDecays]
        AllEvents.Members += [CopyMCParticles()]
    else :
        MySelection.Members += [filterMCDecays]
        MySelection.Members += [CopyMCParticles()]
#==============================================================================
# Copy selected particles, daughters, and decay vertices
copyParticles = CopyParticles('CopyParticles')
copyParticles.InputLocation = mainLocation+"/Particles"
copyParticles.addTool(VertexCloner(), name='VertexCloner')
copyParticles.addTool(ProtoParticleCloner(), name='ProtoParticleCloner')
copyParticles.OutputLevel=4
MySelection.Members += [copyParticles]
#==============================================================================
# Copy primary vertex
copyPV=CopyPrimaryVertices('CopyPrimaryVertices')
copyPV.OutputLevel = 4
MySelection.Members += [copyPV]
#==============================================================================
# copy PV->Particle link
copyP2PVLink = CopyParticle2PVLink()
copyP2PVLink.InputLocation = mainLocation+"/Particle2VertexRelations"
copyP2PVLink.OutputLevel=4;
MySelection.Members += [copyP2PVLink]
#==============================================================================
# copy related MC particles of candidates plus daughters
if (storeMCInfo) :
    copyMC = CopyRelatedMCParticles()
    copyMC.InputLocation = mainLocation+'/Particles'
    copyMC.addTool(MCParticleCloner(), name= 'MCParticleCloner')
    copyMC.MCParticleCloner.addTool(MCVertexCloner(), name = 'ICloneMCVertex')
    copyMC.OutputLevel=4;
    MySelection.Members += [copyMC]
#==============================================================================
# B tagging
if (BTaggingInfo) :
    importOptions('$FLAVOURTAGGINGOPTS/BTaggingTool.opts')
    BTagAlgo = BTagging('BTagging')
    BTagAlgo.addTool(PhysDesktop())
    BTaggingTool("BTaggingTool").OutputLevel=4
    BTagAlgo.PhysDesktop.InputLocations=[mainLocation]
    BTagLocation = mainLocation+"/Tagging"
    BTagAlgo.TagOutputLocation = BTagLocation
    MySelection.Members += [BTagAlgo]
    copyFlavTag = CopyFlavourTag()
    copyFlavTag.InputLocation = BTagLocation
    MySelection.Members += [copyFlavTag]
#==============================================================================
# PV re-fit
if (PVRefit) :
    PVReFitter = PVReFitterAlg("PVReFitterAlg")
    PVReFitter.ParticleInputLocation = mainLocation+"/Particles"
    refittedPVLocation = mainLocation+"/RefittedVertices"
    PVReFitter.VertexOutputLocation = refittedPVLocation
    p2ReFitPVRelationsLoc = mainLocation+"/Particle2ReFittedVertexRelations"
    PVReFitter.P2VRelationsOutputLocation = p2ReFitPVRelationsLoc
    MySelection.Members += [PVReFitter]
    # put the re-fitted vertices on the MicroDST
    copyReFittedPVs = CopyPrimaryVertices('CopyReFittedPVs')
    copyReFittedPVs.InputLocation = refittedPVLocation
    MySelection.Members += [copyReFittedPVs]
    # copy the Particle->PV relations table
    copyP2RefitPVLink = CopyParticle2PVLink("CopyP2RefitPVLink")
    copyP2RefitPVLink.InputLocation = p2ReFitPVRelationsLoc
#    copyP2RefitPVLink.OutputLevel=1
    MySelection.Members += [copyP2RefitPVLink]
#==============================================================================
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
ApplicationMgr().EvtMax = nEvents
ApplicationMgr().HistogramPersistency = "ROOT";
HistogramPersistencySvc().OutputFile = "DVHistos.root";
EventSelector().FirstEvent = 1 
EventSelector().PrintFreq = 100
#==============================================================================
