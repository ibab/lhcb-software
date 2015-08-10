# Import the necessary modules.
from Configurables import Generation, Special, LPairProduction

# Add LPair as special production tool.
Generation().addTool(Special)
Generation().Special.ProductionTool = "LPairProduction"
Generation().Special.addTool(LPairProduction)
