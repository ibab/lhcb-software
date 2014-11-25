#####################################################
# Particle gun for EM physics studies and valdation #
#	Useful configurables set in Emstudy_config.py	#
#			Peter Griffith 21.11.14					#
#####################################################

import sys
sys.path.append('./')
from Gaudi.Configuration import *
from Configurables import ParticleGun
from GaudiKernel.SystemOfUnits import *
from EMstudy_config import config

#get congif
opts = config()
pgunID = opts['pgunID']
pgunE = opts['pgunE']


# Set up ParticleGun
ParticleGun = ParticleGun("ParticleGun")

# Control of the ParticleGun
# Event type is set as described in LHCb-2005-034 : G=5! (Def = 0)
ParticleGun.EventType = 53210205;

from Configurables import MomentumRange
ParticleGun.addTool(MomentumRange, name="MomentumRange")
ParticleGun.ParticleGunTool = "MomentumRange"

from Configurables import FlatNParticles
ParticleGun.addTool(FlatNParticles, name="FlatNParticles")
ParticleGun.NumberOfParticlesTool = "FlatNParticles"

ParticleGun.MomentumRange.PdgCodes = [-1*pgunID, pgunID]

ParticleGun.MomentumRange.MomentumMin = pgunE*GeV
ParticleGun.MomentumRange.MomentumMax = pgunE*GeV
ParticleGun.MomentumRange.ThetaMin    = 0.0*rad 
ParticleGun.MomentumRange.ThetaMax    = 0.39*rad 

