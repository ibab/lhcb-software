## change PDF
command = [ "pypars mstp 51 10770" ]

from Configurables import Generation, MinimumBias, PythiaProduction
from Configurables import Inclusive, SignalPlain, SignalRepeatedHadronization
from Configurables import Special

gen = Generation()
gen.addTool( MinimumBias , name = "MinimumBias" )
gen.MinimumBias.addTool( PythiaProduction , name = "PythiaProduction" )
gen.MinimumBias.PythiaProduction.Commands += command

gen = Generation()
gen.addTool( Inclusive , name = "Inclusive" )
gen.Inclusive.addTool( PythiaProduction , name = "PythiaProduction" )
gen.Inclusive.PythiaProduction.Commands += command

gen = Generation()
gen.addTool( SignalPlain , name = "SignalPlain" )
gen.SignalPlain.addTool( PythiaProduction , name = "PythiaProduction" )
gen.SignalPlain.PythiaProduction.Commands += command

gen = Generation()
gen.addTool( SignalRepeatedHadronization , name = "SignalRepeatedHadronization" )
gen.SignalRepeatedHadronization.addTool( PythiaProduction , name = "PythiaProduction" )
gen.SignalRepeatedHadronization.PythiaProduction.Commands += command

gen = Generation()
gen.addTool( Special , name = "Special" )
gen.Special.addTool( PythiaProduction , name = "PythiaProduction" )
gen.Special.PythiaProduction.Commands += command



