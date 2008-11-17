########################################################################
#
# $Id:
#
# Test associators
#
# @author Patrick Koppenburg
# @date 2008-11-17
#
########################################################################
from Gaudi.Configuration import *
from Configurables import CompositeParticle2MCLinks, Particle2MCLinks, TestLinker
########################################################################
#
# for tests just load opts file
#
########################################################################
importOptions( "$DAVINCIROOT/options/DaVinciCommon.opts" )

ApplicationMgr().TopAlg += [ TestLinker() ]
TestLinker().InputData = [ "Phys/StdLoosePions/Particles", "Phys/StdLooseKstar2KPi/Particles" ]

## some output levels

links = Particle2MCLinks("TestLinker.Particle2MCLinks")
links.OutputLevel = 1 
comp = CompositeParticle2MCLinks("TestLinker.CompositeParticle2MCLinks")
comp.OutputLevel = 1 

ApplicationMgr().EvtMax = 1

#
# do not forget to define some input data
#
