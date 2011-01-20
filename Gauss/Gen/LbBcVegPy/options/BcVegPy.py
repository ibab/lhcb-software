# $Id$ 

from Configurables import Generation, Special, BcVegPyProduction

BcVegPyListOfCommands = [
  "mixevnt imix 1" ,
  "mixevnt imixtype 1" ,
  "counter ibcstate 1",
  "vegasinf nitmx 20",
  "vegasinf number 1000000",
  "loggrade ivegasopen 0",
  "loggrade igrade 1",
  "upcom ecm 7000.0"
]

gen = Generation()
gen.addTool( Special , name = "Special" )
gen.Special.ProductionTool = "BcVegPyProduction"
gen.Special.addTool( BcVegPyProduction , name = "BcVegPyProduction" )
gen.Special.BcVegPyProduction.BcVegPyCommands += BcVegPyListOfCommands
gen.PileUpTool = "FixedLuminosityForRareProcess";


