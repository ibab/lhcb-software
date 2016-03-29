# Import the necessary modules.
from Configurables import Generation, MinimumBias, Special, Pythia8Production
from Configurables import BcVegPyProduction

# Add Pythia as minimum bias production tool.
Generation().addTool(MinimumBias)
Generation().MinimumBias.ProductionTool = "Pythia8Production"
Generation().MinimumBias.addTool(Pythia8Production)

# Add BcVegPy as special production tool.
Generation().addTool(Special)
Generation().Special.ProductionTool = "BcVegPyProduction"
Generation().Special.addTool(BcVegPyProduction)
Generation().Special.BcVegPyProduction.ShowerToolName = "Pythia8Production"
Generation().Special.PileUpProductionTool = "Pythia8Production/Pythia8PileUp"
Generation().Special.ReinitializePileUpGenerator  = False

