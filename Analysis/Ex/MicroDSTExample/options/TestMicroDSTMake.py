#$Id: TestMicroDSTMake.py,v 1.2 2008-09-23 18:01:12 jpalac Exp $
from Gaudi.Configuration import *
from DaVinci.MicroDSTAlgorithm import *
from Configurables import CopyRelatedMCParticles
from Configurables import CopyParticle2PVLink
from Configurables import MCParticleArrayFilterAlg
from Configurables import FilterMCParticleArrayByDecay
from Configurables import MCDecayFinder
#
AllEvents = GaudiSequencer('AllEvents')
ApplicationMgr().TopAlg += [ AllEvents ]
#
filterMCDecays = MCParticleArrayFilterAlg('FilterMCDecays')
filterMCDecays.OutputLocation = "MC/Decays"
FilterMCParticleArrayByDecay().addTool( MCDecayFinder() )
filterMCDecays.addTool( FilterMCParticleArrayByDecay() )
filterMCDecays.FilterMCParticleArrayByDecay.MCDecayFinder.Decay = "[B_s0 -> (J/psi(1S) -> mu+ mu- {,gamma} {,gamma}) (phi(1020) -> K+ K-)]cc"
filterMCDecays.OutputLevel = 4;
#importOptions( "$DAVINCIROOT/options/TestNewMicroDSTStuff.opts")
importOptions( "JPsiPhiData.opts")
importOptions("$DAVINCIROOT/options/MicroDSTCommon.opts")

AllEvents.Members  = [CopyRecHeader()]
AllEvents.Members += ['FilterMCDecays']

ApplicationMgr().EvtMax = 100
EventSelector().FirstEvent = 1 
EventSelector().PrintFreq = 10 
