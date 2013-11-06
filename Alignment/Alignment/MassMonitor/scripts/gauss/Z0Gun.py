# file IMADETHISNOTAUTOMATIC
#
# EventType: 59999993
#
# ASCII decay Descriptor: Z0 => mu- mu+
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
ParticleGun().MomentumRange.PdgCodes = [ 23 ]

from Configurables import ToolSvc
from Configurables import EvtGenDecay
ToolSvc().addTool( EvtGenDecay, name = "EvtGenDecay" )
ToolSvc().EvtGenDecay.UserDecayFile = "../dkfiles/z0gun.dec"
ParticleGun().DecayTool = "EvtGenDecay"
