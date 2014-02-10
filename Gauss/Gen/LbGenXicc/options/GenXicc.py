# $Id$ 

from Configurables import Gauss, Generation, Special, GenXiccProduction, ToolSvc

GenXiccListOfCommands = [
  "loggrade iusecurdir 0",
  "vegasinf number 10000",
  "vegasinf nitmx 2",
  "vegasbin nvbin 100",
  "confine ptcut 0.2",
  "counter xmaxwgt 1000000"
]

gen = Generation()
gen.addTool( Special , name = "Special" )
gen.Special.ProductionTool = "GenXiccProduction"
gen.Special.addTool( GenXiccProduction , name = "GenXiccProduction" )
gen.Special.GenXiccProduction.GenXiccCommands += GenXiccListOfCommands


