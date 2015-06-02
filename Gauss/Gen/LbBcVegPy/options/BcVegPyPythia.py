# Import the necessary modules.
from Configurables import Generation, MinimumBias, Special, PythiaProduction
from Configurables import BcVegPyProduction

# Add Pythia as minimum bias production tool.
Generation().addTool(MinimumBias)
Generation().MinimumBias.ProductionTool = "PythiaProduction"
Generation().MinimumBias.addTool(PythiaProduction)

# Add BcVegPy as special production tool.
Generation().addTool(Special)
Generation().Special.ProductionTool = "BcVegPyProduction"
Generation().Special.addTool(BcVegPyProduction)
Generation().Special.BcVegPyProduction.ShowerToolName = "PythiaProduction"
