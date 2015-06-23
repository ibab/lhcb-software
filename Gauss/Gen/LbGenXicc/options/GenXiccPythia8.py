# Import the necessary modules.
from Configurables import Generation, MinimumBias, Special, Pythia8Production
from Configurables import GenXiccProduction

# Add Pythia as minimum bias production tool.
Generation().addTool(MinimumBias)
Generation().MinimumBias.ProductionTool = "Pythia8Production"
Generation().MinimumBias.addTool(Pythia8Production)

# Add GenXicc as special production tool.
Generation().addTool(Special)
Generation().Special.ProductionTool = "GenXiccProduction"
Generation().Special.addTool(GenXiccProduction)
Generation().Special.GenXiccProduction.ShowerToolName = "Pythia8Production"
