# Import the necessary modules.
from Configurables import Generation, MinimumBias, Special, PythiaProduction
from Configurables import GenXiccProduction

# Add Pythia as minimum bias production tool.
Generation().addTool(MinimumBias)
Generation().MinimumBias.ProductionTool = "PythiaProduction"
Generation().MinimumBias.addTool(PythiaProduction)

# Add GenXicc as special production tool.
Generation().addTool(Special)
Generation().Special.ProductionTool = "GenXiccProduction"
Generation().Special.addTool(GenXiccProduction)
Generation().Special.GenXiccProduction.ShowerToolName = "PythiaProduction"
