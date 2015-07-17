# Import the necessary modules.
from Configurables import Generation, Special, SuperChicProduction

# Add SuperChic as special production tool.
Generation().addTool(Special)
Generation().Special.ProductionTool = "SuperChicProduction"
Generation().Special.addTool(SuperChicProduction)
