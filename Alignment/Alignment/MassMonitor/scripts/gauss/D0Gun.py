# file IMADETHISNOTAUTOMATIC
#
# EventType: 59999994
#
# ASCII decay Descriptor: D0 => K- pi+
#
from Configurables import ParticleGun
from Configurables import MomentumRange
ParticleGun().addTool( MomentumRange )
from GaudiKernel import SystemOfUnits
ParticleGun().MomentumRange.MomentumMin = 0.0*SystemOfUnits.GeV
from GaudiKernel import SystemOfUnits
ParticleGun().MomentumRange.MomentumMax = 0.0*SystemOfUnits.GeV
ParticleGun().EventType = 599999994
ParticleGun().ParticleGunTool = "MomentumRange"
ParticleGun().NumberOfParticlesTool = "FlatNParticles"
ParticleGun().MomentumRange.PdgCodes = [ 421, -421 ]

from Configurables import ToolSvc
from Configurables import EvtGenDecay
ToolSvc().addTool( EvtGenDecay, name = "EvtGenDecay" )
ToolSvc().EvtGenDecay.UserDecayFile = "../dkfiles/d0gun.dec"
ParticleGun().DecayTool = "EvtGenDecay"
