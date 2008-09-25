#$Id: TestMicroDSTMake.py,v 1.3 2008-09-25 08:05:52 jpalac Exp $
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
#
importOptions( "$DAVINCIROOT/options/DaVinciCommon.opts" )
importOptions("$DAVINCIROOT/options/DaVinciReco.opts" )
importOptions( "JpsiPhiData.opts")
importOptions( "$CCBARROOT/options/DoDC06SelBs2Jpsi2MuMuPhi2KK_lifetime_unbiased.opts"  )
importOptions("$DAVINCIROOT/options/MicroDSTCommon.opts")
#
AllEvents = GaudiSequencer('AllEvents')
ApplicationMgr().TopAlg += [ AllEvents ]
MySelection = GaudiSequencer('SeqDC06selBs2JpsiPhi')
#
filterMCDecays = MCParticleArrayFilterAlg('FilterMCDecays')
filterMCDecays.OutputLocation = "MC/Decays"
filterMCDecays.addTool( FilterMCParticleArrayByDecay(), name = 'IMCParticleArrayFilter' )
filterMCDecays.IMCParticleArrayFilter.addTool( MCDecayFinder(), name = 'MCDecayFinder' )
filterMCDecays.IMCParticleArrayFilter.MCDecayFinder.Decay = "[B_s0 -> (J/psi(1S) -> mu+ mu- {,gamma} {,gamma}) (phi(1020) -> K+ K-)]cc"
filterMCDecays.OutputLevel = 4;

AllEvents.Members  = [CopyRecHeader()]
AllEvents.OutputLevel=4
AllEvents.Members += [filterMCDecays]

CopyMCParticles().InputLocation = "MC/Decays"
CopyMCParticles().addTool( MCParticleCloner(), name = 'MCParticleCloner' )
CopyMCParticles().MCParticleCloner.addTool( MCVertexCloner(), name = 'MCVertexCloner' )
CopyMCParticles().OutputLevel=4
AllEvents.Members += [CopyMCParticles()]

copyParticles = CopyParticles('CopyParticles')
copyParticles.InputLocation = "Phys/DC06selBs2JpsiPhi_unbiased/Particles"
copyParticles.addTool(VertexCloner(), name='VertexCloner')
copyParticles.addTool(ProtoParticleCloner(), name='ProtoParticleCloner')
copyParticles.OutputLevel=4

copyPV=CopyPrimaryVertices('CopyPrimaryVertices')
copyPV.OutputLevel = 4

copyP2PVLink = CopyParticle2PVLink()
copyP2PVLink.InputLocation = "Phys/DC06selBs2JpsiPhi_unbiased/Particle2VertexRelations"
copyP2PVLink.OutputLevel=4;

copyMC = CopyRelatedMCParticles()
copyMC.InputLocation = "Phys/DC06selBs2JpsiPhi_unbiased/Particles"
copyMC.addTool(MCParticleCloner(), name= "MCParticleCloner")
copyMC.MCParticleCloner.addTool(MCVertexCloner(), name = "MCVertexCloner")
copyMC.OutputLevel=4;

MySelection.Members += [copyParticles]
MySelection.Members += [copyPV]
MySelection.Members += [copyP2PVLink]
MySelection.Members += [copyMC]

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
ApplicationMgr().EvtMax =50
ApplicationMgr().HistogramPersistency = "ROOT";
HistogramPersistencySvc().OutputFile = "DVHistos.root";
EventSelector().FirstEvent = 1 
EventSelector().PrintFreq = 100
