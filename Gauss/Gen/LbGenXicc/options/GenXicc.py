# $Id$ 

from Configurables import Generation, Special, GenXiccProduction

GenXiccListOfCommands = [
  "loggrade iusecurdir 1",
]

gen = Generation()
gen.addTool( Special , name = "Special" )
gen.Special.ProductionTool = "GenXiccProduction"
gen.Special.addTool( GenXiccProduction , name = "GenXiccProduction" )
gen.Special.GenXiccProduction.GenXiccCommands += GenXiccListOfCommands
gen.PileUpTool = "FixedLuminosityForRareProcess";


