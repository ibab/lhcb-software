########################################################################
#
# Options for CheatedSelection algorithm
#
# @author Marco Musy
# @date 2009-02-23
#
########################################################################
#
from Configurables import DaVinci, PhysDesktop, CheatedSelection, CombinedParticleMaker, ProtoParticleMUONFilter, ProtoParticleCALOFilter, ProtoParticleRICHFilter


cheat = CheatedSelection("CheatedSelection")
pd = PhysDesktop()
pd.ParticleMakerType = "CombinedParticleMaker"
pd.addTool( CombinedParticleMaker() )
pd.CombinedParticleMaker.ExclusiveSelection = True;
##
cheat.addTool( pd )

DaVinci().UserAlgorithms += [ cheat ]

cheat.OutputLevel = 2

