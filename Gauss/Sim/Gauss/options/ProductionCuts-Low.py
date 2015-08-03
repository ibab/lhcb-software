##
##  File containing options to lower productions cuts
##

from Gaudi.Configuration import *
from GaudiKernel import SystemOfUnits

def setProductionCuts():
    from Configurables import ( GiGa, GiGaPhysListModular )
    giga = GiGa()
    giga.addTool( GiGaPhysListModular("ModularPL") , name="ModularPL" ) 

    giga.ModularPL.CutForElectron = 0.01 * SystemOfUnits.mm
    giga.ModularPL.CutForPositron = 0.01 * SystemOfUnits.mm
    giga.ModularPL.CutForGamma    = 0.01 * SystemOfUnits.mm


appendPostConfigAction(setProductionCuts)

