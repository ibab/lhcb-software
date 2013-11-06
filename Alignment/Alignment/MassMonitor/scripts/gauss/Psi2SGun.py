# file IMADETHISNOTAUTOMATIC
#
# EventType: 59999991
#
# ASCII decay Descriptor: psi(2S) => mu- mu+
#
from Configurables import ParticleGun
from Configurables import MomentumRange
ParticleGun().addTool( MomentumRange )
from GaudiKernel import SystemOfUnits
ParticleGun().MomentumRange.MomentumMin = 0.0*SystemOfUnits.GeV
from GaudiKernel import SystemOfUnits
ParticleGun().MomentumRange.MomentumMax = 0.0*SystemOfUnits.GeV
ParticleGun().EventType = 59999999
ParticleGun().ParticleGunTool = "MomentumRange"
ParticleGun().NumberOfParticlesTool = "FlatNParticles"
ParticleGun().MomentumRange.PdgCodes = [ 100443 ]

from Configurables import ToolSvc
from Configurables import EvtGenDecay
ToolSvc().addTool( EvtGenDecay, name = "EvtGenDecay" )
ToolSvc().EvtGenDecay.UserDecayFile = "../dkfiles/psi2sgun.dec"
ParticleGun().DecayTool = "EvtGenDecay"
