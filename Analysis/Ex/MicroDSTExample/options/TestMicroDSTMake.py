#$Id: TestMicroDSTMake.py,v 1.15 2009-05-11 15:11:17 jpalac Exp $
from Gaudi.Configuration import *
from Configurables import DaVinci
from Configurables import MCParticleArrayFilterAlg
from Configurables import FilterMCParticleArrayByDecay
from Configurables import MCDecayFinder
from Configurables import MCParticleCloner
from Configurables import MCVertexCloner
from Configurables import VertexCloner
from Configurables import ParticleCloner
from Configurables import ProtoParticleCloner
from Configurables import PrintHeader
from Configurables import OutputStream
from Configurables import BTagging, BTaggingTool
from Configurables import PhysDesktop
from Configurables import PVReFitterAlg
from Configurables import CopyODIN
from Configurables import CopyRecHeader
from Configurables import CopyMCParticles
from Configurables import CopyParticles
from Configurables import CopyPrimaryVertices
from Configurables import CopyFlavourTag
from Configurables import CopyParticle2PVRelations
from Configurables import CopyParticle2MCRelations
from Configurables import P2MCRelatorAlg
from Configurables import MakeResonances
from MicroDSTExample import Selections
#==============================================================================
# Some steering options
#
# number of events to process
nEvents = 1000
# Copy information for events not passing the selection?
allEventInfo = False
# Copy MC particles when signal MC decay is found?
keepTrueDecays = False
# Copy related MCParticles and decay tree for selected candidates?
storeMCInfo = True
# B tagging?
BTaggingInfo = True
# re-fit PV?
PVRefit = False
#==============================================================================
importOptions("$STDOPTS/LHCbApplication.opts")
#mySelection = Selections.KstarSel
#mySelection = Selections.Phi2KKSel
mySelection = Selections.Bs2JpsiPhiSel
#mySelection = Selections.BdJpsiKstarSel
importOptions(mySelection.mainOptions)
importOptions(mySelection.dataFiles)
mainLocation = mySelection.mainLocation
mainSelector = MakeResonances(mySelection.mainSelector)
mainSelector.ReFitPVs=True
mainSelector.OutputLevel=4
#
if (storeMCInfo) :
    DSTMC = "WithMC"
else :
    DSTMC = "NoMC"
    
importOptions('$MICRODSTOPTS/MicroDSTStream.py')
MicroDSTStream=OutputStream('MicroDSTStream')
ApplicationMgr().OutStream.append(MicroDSTStream)
evtString = ""
if not (nEvents==-1) :
    evtString = str(nEvents/1000.)
outputName =  "DATAFILE='"+ mySelection.mainSequence +DSTMC+"_"+ evtString +"_Kevt_NewReFitPVs.dst' TYP='POOL_ROOTTREE' OPT='REC'"
MicroDSTStream.Output = outputName
MicroDSTStream.OutputLevel=4;

# Create sequence for stuff to be copied for all events.
# Add to OutputStream's AcceptAlgs list (events passing OR
# of this list are written out)
if (allEventInfo) :
    AllEvents = GaudiSequencer('AllEvents')
    ApplicationMgr().TopAlg += [ AllEvents ]
    MicroDSTStream.AcceptAlgs.append( 'AllEvents' )
    
# Create selection sequence and add to OutputStream's AcceptAlgs list
MySelection = GaudiSequencer(mySelection.mainSequence)
MicroDSTStream.AcceptAlgs.append( MySelection.name() )
#==============================================================================

#from Configurables import FilterEventByMCDecay
#signalKiller = FilterEventByMCDecay()
#signalKiller.Select = False
##signalKiller.Select = True
#signalKiller.addTool(MCDecayFinder)
#signalKiller.MCDecayFinder.Decay = '[B_s0 -> (J/psi(1S) -> mu+ mu- {,gamma} {,gamma} {,gamma}) (phi(1020) -> K+ K- {,gamma} {,gamma} {,gamma})]cc'
#MySelection.Members += [signalKiller]

#==============================================================================
# Copy RecHeader
MySelection.Members += [CopyRecHeader()]
#==============================================================================
# Copy RecHeader
MySelection.Members += [CopyODIN()]
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
    CopyMCParticles().addTool( MCParticleCloner )
    CopyMCParticles().MCParticleCloner.addTool( MCVertexCloner(),
                                                name = 'ICloneMCVertex' )
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
#copyParticles.addTool(ParticleCloner, name='ClonerType')
#copyParticles.ParticleCloner.addTool(VertexCloner, name='IClonerVertex')
#copyParticles.ParticleCloner.addTool(ProtoParticleCloner, name='ICloneProtoParticle')
copyParticles.OutputLevel=4
MySelection.Members += [copyParticles]
#==============================================================================
# Copy all primary vertices
copyPV=CopyPrimaryVertices('CopyPrimaryVertices')
copyPV.OutputLevel = 4
MySelection.Members += [copyPV]
#==============================================================================
# copy PV->Particle link
# This will only copy related PVs, and only if they haven't been copied before
copyP2PVRel = CopyParticle2PVRelations()
copyP2PVRel.InputLocation = mainLocation+"/Particle2VertexRelations"
copyP2PVRel.OutputLevel=4
MySelection.Members += [copyP2PVRel]
#==============================================================================
# copy related MC particles of candidates plus daughters
if (storeMCInfo) :
    # first, get matches MCParticles for selected candidates.
    # This will make a relations table in mainLocation+"/P2MCPRelations"
    p2mcRelator = P2MCRelatorAlg()
    p2mcRelator.ParticleLocation = mainLocation+'/Particles'
    p2mcRelator.OutputLevel=4
    MySelection.Members += [p2mcRelator]
    # Now copy relations table + matched MCParticles to MicroDST
    copyP2MCRel = CopyParticle2MCRelations()
    copyP2MCRel.addTool(MCParticleCloner)
    copyP2MCRel.MCParticleCloner.addTool(MCVertexCloner,
                                         name = 'ICloneMCVertex')
    copyP2MCRel.MCParticleCloner.OutputLevel=4
    copyP2MCRel.InputLocation = mainLocation+"/P2MCPRelations"
    copyP2MCRel.OutputLevel=4
    MySelection.Members += [copyP2MCRel]
#==============================================================================
# B tagging
if (BTaggingInfo) :
    importOptions('$FLAVOURTAGGINGOPTS/BTaggingTool.py')
    BTagAlgo = BTagging('BTagging')
    BTagAlgo.addTool(PhysDesktop)
    BTaggingTool("BTaggingTool").OutputLevel=4
    BTagAlgo.InputLocations=[mainLocation]
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
    PVReFitter.OutputLevel=4
    MySelection.Members += [PVReFitter]
    # put the re-fitted vertices on the MicroDST
    copyReFittedPVs = CopyPrimaryVertices('CopyReFittedPVs')
    copyReFittedPVs.InputLocation = refittedPVLocation
    MySelection.Members += [copyReFittedPVs]
    # copy the Particle->PV relations table
    copyP2RefitPVRel = CopyParticle2PVRelations("CopyP2RefitPVRelations")
    copyP2RefitPVRel.InputLocation = p2ReFitPVRelationsLoc
    copyP2RefitPVRel.OutputLevel=4
    MySelection.Members += [copyP2RefitPVRel]
#==============================================================================
# make a histogram counting events processed
from Configurables import EventCountHisto
evtCountAlg = EventCountHisto('EvtCountAlg')
#evtCountAlg.OutputLevel= 1
# make a DaVinci application configurable and add the crucial sequence to it.
dv = DaVinci()
dv.EvtMax = nEvents
dv.HistogramFile = "EventCount.root"
#dv.SkipEvents = 2*nEvents
dv.UserAlgorithms = [evtCountAlg, MySelection]
#==============================================================================
