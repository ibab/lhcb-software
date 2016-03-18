from Configurables import Generation, MinimumBias, BoundProduction
from Configurables import Inclusive, SignalPlain, SignalRepeatedHadronization
from Configurables import Special

gen = Generation()
gen.addTool(MinimumBias , name = "MinimumBias")
gen.MinimumBias.ProductionTool = "BoundProduction"
gen.MinimumBias.addTool(BoundProduction , name = "BoundProduction")
gen.MinimumBias.BoundProduction.ProductionToolName = "Pythia8Production"

gen.addTool(Inclusive, name = "Inclusive")
gen.Inclusive.ProductionTool = "BoundProduction"
gen.Inclusive.addTool(BoundProduction , name = "BoundProduction" )
gen.Inclusive.BoundProduction.ProductionToolName = "Pythia8Production"

gen.addTool(SignalPlain, name = "SignalPlain")
gen.SignalPlain.ProductionTool = "BoundProduction"
gen.SignalPlain.addTool(BoundProduction, name = "BoundProduction")
gen.SignalPlain.BoundProduction.ProductionToolName = "Pythia8Production"

gen.addTool(SignalRepeatedHadronization, name = "SignalRepeatedHadronization")
gen.SignalRepeatedHadronization.ProductionTool = "BoundProduction"
gen.SignalRepeatedHadronization.addTool(BoundProduction, 
                                        name = "BoundProduction")
gen.SignalRepeatedHadronization.BoundProduction.ProductionToolName = (
    "Pythia8Production")

gen.addTool(Special, name = "Special")
gen.Special.ProductionTool = "BoundProduction"
gen.Special.addTool(BoundProduction, name = "BoundProduction")
gen.Special.BoundProduction.ProductionToolName = "Pythia8Production"
