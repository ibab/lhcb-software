#$Id: TestMicroDSTMake.py,v 1.21 2009-07-01 15:20:52 jpalac Exp $
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
from Configurables import CopyHltDecReports
from Configurables import CopyL0DUReport
from Configurables import PVRelatorAlg
from Configurables import CopyParticle2MCRelations
from Configurables import P2MCRelatorAlg
from Configurables import MakeResonances
from MicroDSTExample import Selections
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
PVRefit = False
# L0DUReport?
L0DUInfo = False
# HltDecReports?
HltInfo = False
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
#mainSelector.ReFitPVs=True
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
outputName =  "DATAFILE='"+ mySelection.mainSequence +DSTMC+"_"+ evtString +"_Kevt.mdst' TYP='POOL_ROOTTREE' OPT='REC'"
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
    filterMCDecays.addTool( FilterMCParticleArrayByDecay, name = 'IMCParticleArrayFilter' )
    filterMCDecays.IMCParticleArrayFilter.addTool( MCDecayFinder, name = 'MCDecayFinder' )
    filterMCDecays.IMCParticleArrayFilter.MCDecayFinder.Decay = '[B_s0 -> (J/psi(1S) -> mu+ mu- {,gamma} {,gamma}) (phi(1020) -> K+ K-)]cc'
    filterMCDecays.OutputLevel = 4;
    copyMC = CopyMCParticles()
    copyMC.InputLocation = "MC/Decays"
    copyMC.addTool( MCParticleCloner, name = 'ClonerType' )
    copyMC.ClonerType.addTool( MCVertexCloner,
                               name = 'ICloneMCVertex' )
    copyM.OutputLevel=4

    if (allEventInfo) :
        AllEvents.Members += [filterMCDecays]
        AllEvents.Members += [copyMC]
    else :
        MySelection.Members += [filterMCDecays]
        MySelection.Members += [copyMC]
#==============================================================================
# Copy selected particles, daughters, and decay vertices
copyParticles = CopyParticles('CopyParticles')
copyParticles.InputLocation = mainLocation+"/Particles"
copyParticles.addTool(ParticleCloner, name='ClonerType')
# ParticleCloner is already used by CopyParticles, but need to add configurable
# if we want to modify its configuration, including adding some tools.
#copyParticles.addTool(ParticleCloner, name='ClonerType')
#copyParticles.ClonerType.addTool(ProtoParticleCloner, name='ICloneProtoParticle')
# Vertex cloner is already used by ParticleCloner, but need to add it if we
# want to modify its configuration
#copyParticles.ClonerType.addTool(VertexCloner, name='IClonerVertex')

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
    # re-fit PVs for each selected particle
    # This produces a relations table linking each particle to the PVs that
    # have been re-fitted after extracting it's tracks. The order is the
    # same as in the original PV container. No best PV criteria applied!
    PVReFitter = PVReFitterAlg("PVReFitterAlg")
    PVReFitter.ParticleInputLocation = mainLocation+"/Particles"
    PVReFitter.VertexOutputLocation = mainLocation+"/RefittedVertices"
    p2ReFitPVRelationsLoc = mainLocation+"/Particle2ReFittedVertexRelations"
    PVReFitter.P2VRelationsOutputLocation = p2ReFitPVRelationsLoc
    PVReFitter.OutputLevel=4
    MySelection.Members += [PVReFitter]
    # Take Particle->PV relations table from previous step and produce
    # new table, sorted according to an IRelatedPVFinder
    PVRelator = PVRelatorAlg()
    PVRelator.OutputLevel=4
    PVRelator.P2PVRelationsInputLocation = p2ReFitPVRelationsLoc
    p2pvSortedRelationsLoc = mainLocation + "/P2ReFitPVSortedRelations"
    PVRelator.P2PVRelationsOutputLocation = p2pvSortedRelationsLoc
    MySelection.Members += [PVRelator]
    # copy sorted table. This also copies the re-fitted PVs.
    copyP2RefitPVRel = CopyParticle2PVRelations("CopyP2RefitPVRelations")
    copyP2RefitPVRel.InputLocation = p2pvSortedRelationsLoc
    copyP2RefitPVRel.OutputLevel=4
    MySelection.Members += [copyP2RefitPVRel]
#==============================================================================
if HltInfo :
    copyHlt = CopyHltDecReports()
    copyHlt.OutputLevel = 4
    MySelection.Members += [copyHlt]
#==============================================================================
if L0DUInfo :
    copyL0 = CopyL0DUReport()
    copyL0.OutputLevel = 4
    MySelection.Members += [copyL0]
#==============================================================================
# make a histogram counting events processed
from Configurables import EventCountHisto
evtCountAlg = EventCountHisto('EvtCountAlg')
#evtCountAlg.OutputLevel= 1
# make a DaVinci application configurable and add the crucial sequence to it.
dv = DaVinci()
dv.EvtMax = nEvents
if L0DUInfo :
    dv.ReplaceL0BanksWithEmulated = True
if HltInfo :
    dv.HltType = 'Hlt1+Hlt2'
dv.HistogramFile = "EventCount.root"
#dv.SkipEvents = 2*nEvents
dv.UserAlgorithms = [evtCountAlg, MySelection]
#==============================================================================
