##
##  File containing options to activate the QGSP_BERT Hadronic
##  Physics List in Geant4 (the default for production is 
##  LHEP)
##

from Gauss.Configuration import *

Gauss().PhysicsList = {"Em":'Opt1', "Hadron":'QGSP_BERT', "GeneralPhys":True, "LHCbPhys":True}
