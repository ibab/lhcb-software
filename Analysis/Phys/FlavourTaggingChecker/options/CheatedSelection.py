########################################################################
#
# Options for CheatedSelection algorithm
#
# @author Marco Musy
# @date 2009-02-23
#
########################################################################
#
from GaudiConf.Configuration import *
from Configurables import DaVinci, CheatedSelection


cheat = CheatedSelection("CheatedSelection")
cheat.InputLocations = [ "Phys/TaggingPions" ]

#cheat.AssociatorInputData = [ "Phys/CheatedSelection/Particles" ]
DaVinci().UserAlgorithms += [ cheat ]

