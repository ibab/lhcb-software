# Import the necessary modules.
from Configurables import Generation, HerwigppProduction
from Configurables import MinimumBias, Inclusive, Special, SignalPlain
from Configurables import SignalRepeatedHadronization

# Add Herwig++ as minimum bias production tool.
Generation().addTool(MinimumBias)
Generation().MinimumBias.ProductionTool = "HerwigppProduction"
Generation().MinimumBias.addTool(HerwigppProduction)

# Add Herwig++ as inclusive production tool.
Generation().addTool(Inclusive)
Generation().Inclusive.ProductionTool = "HerwigppProduction"
Generation().Inclusive.addTool(HerwigppProduction)

# Add Herwig++ as special production tool.
Generation().addTool(Special)
Generation().Special.ProductionTool = "HerwigppProduction"
Generation().Special.addTool(HerwigppProduction)

# Add Herwig++ as plain signal production tool.
Generation().addTool(SignalPlain)
Generation().SignalPlain.ProductionTool = "HerwigppProduction"
Generation().SignalPlain.addTool(HerwigppProduction)

# Add Herwig++ as repeated signal hadronization production tool.
Generation().addTool(SignalRepeatedHadronization)
Generation().SignalRepeatedHadronization.ProductionTool = "HerwigppProduction"
Generation().SignalRepeatedHadronization.addTool(HerwigppProduction)

