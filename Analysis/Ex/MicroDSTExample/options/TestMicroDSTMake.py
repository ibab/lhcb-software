#$Id: TestMicroDSTMake.py,v 1.6 2008-11-12 14:24:03 jpalac Exp $
from Gaudi.Configuration import *
from DaVinci.MicroDSTAlgorithm import *
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
#==============================================================================
# Some steering options
#
# number of events to process
nEvents = 1500
# Copy information for events not passing the selection?
allEventInfo = False
# Copy MC particles when signal MC decay is found?
keepTrueDecays = False
# Copy related MCParticles and decay tree for selected candidates?
storeMCInfo = True
#==============================================================================
importOptions( "$DAVINCIROOT/options/DaVinciCommon.opts" )
importOptions( "$MICRODSTEXAMPLEROOT/options/JpsiPhiData.opts")
importOptions( "$CCBARROOT/options/DoDC06SelBs2Jpsi2MuMuPhi2KK_lifetime_unbiased.opts"  )
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
    CopyMCParticles().MCParticleCloner.addTool( MCVertexCloner(), name = 'MCVertexCloner' )
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
copyParticles.InputLocation = "Phys/DC06selBs2JpsiPhi_unbiased/Particles"
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
copyP2PVLink.InputLocation = "Phys/DC06selBs2JpsiPhi_unbiased/Particle2VertexRelations"
copyP2PVLink.OutputLevel=4;
MySelection.Members += [copyP2PVLink]
#==============================================================================
# copy related MC particles of candidates plus daughters
if (storeMCInfo) :
    copyMC = CopyRelatedMCParticles()
    copyMC.InputLocation = 'Phys/DC06selBs2JpsiPhi_unbiased/Particles'
    copyMC.addTool(MCParticleCloner(), name= 'MCParticleCloner')
    copyMC.MCParticleCloner.addTool(MCVertexCloner(), name = 'MCVertexCloner')
    copyMC.OutputLevel=4;
    MySelection.Members += [copyMC]
#==============================================================================
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
ApplicationMgr().EvtMax = nEvents
ApplicationMgr().HistogramPersistency = "ROOT";
HistogramPersistencySvc().OutputFile = "DVHistos.root";
EventSelector().FirstEvent = 1 
EventSelector().PrintFreq = 100
#==============================================================================
