##
##  File containing options to activate the Standard Em
##  Physics in Geant4 (the default for production is Em
##  Option1)
##

from Gauss.Configuration import *

Gauss().PhysicsList = {"Em":'Std', "Hadron":'LHEP', "GeneralPhys":True, "LHCbPhys":True}
