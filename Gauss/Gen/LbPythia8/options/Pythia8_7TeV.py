from Configurables import Generation
from Configurables import MinimumBias
from Configurables import Inclusive
from Configurables import ( SignalPlain, SignalRepeatedHadronization, Special )
from Configurables import Pythia8Production

commandsTuning_XST_7TeV = [
    'SigmaTotal:setOwn         = on', 
    'SigmaTotal:sigmaTot       = 9.407715e+01',
    'SigmaTotal:sigmaAX        = 7.560716e+00',
    'SigmaTotal:sigmaXB        = 7.391497e+00',
    'SigmaTotal:sigmaEl        = 2.946597e+01',
    'SigmaTotal:sigmaXX        = 1.530803e+00',
    'SigmaDiffractive:dampen   = off',
    ]

Pythia8TurnOffFragmentation = [ "HadronLevel:all = off" ]

gen = Generation("Generation")

gen.addTool( MinimumBias , name = "MinimumBias" )
gen.MinimumBias.ProductionTool = "Pythia8Production"
gen.MinimumBias.addTool( Pythia8Production , name = "Pythia8Production" )
gen.MinimumBias.Pythia8Production.Tuning = "LHCbDefault.cmd"
gen.MinimumBias.Pythia8Production.Commands += commandsTuning_XST_7TeV

gen.addTool( Inclusive , name = "Inclusive" )
gen.Inclusive.ProductionTool = "Pythia8Production"
gen.Inclusive.addTool( Pythia8Production , name = "Pythia8Production" )
gen.Inclusive.Pythia8Production.Tuning = "LHCbDefault.cmd"
gen.MinimumBias.Pythia8Production.Commands += commandsTuning_XST_7TeV

gen.addTool( SignalPlain , name = "SignalPlain" )
gen.SignalPlain.ProductionTool = "Pythia8Production"
gen.SignalPlain.addTool( Pythia8Production , name = "Pythia8Production" )
gen.SignalPlain.Pythia8Production.Tuning = "LHCbDefault.cmd"
gen.SignalPlain.Pythia8Production.Commands += commandsTuning_XST_7TeV

gen.addTool( SignalRepeatedHadronization , name = "SignalRepeatedHadronization" )
gen.SignalRepeatedHadronization.ProductionTool = "Pythia8Production"
gen.SignalRepeatedHadronization.addTool( Pythia8Production , name = "Pythia8Production" )
gen.SignalRepeatedHadronization.Pythia8Production.Tuning = "LHCbDefault.cmd"
gen.SignalRepeatedHadronization.Pythia8Production.Commands += Pythia8TurnOffFragmentation
gen.SignalRepeatedHadronization.Pythia8Production.Commands += commandsTuning_XST_7TeV

gen.addTool( Special , name = "Special" )
gen.Special.ProductionTool = "Pythia8Production"
gen.Special.addTool( Pythia8Production , name = "Pythia8Production" )
gen.Special.Pythia8Production.Tuning = "LHCbDefault.cmd"
gen.Special.Pythia8Production.Commands += commandsTuning_XST_7TeV
gen.Special.PileUpProductionTool = "Pythia8Production/Pythia8PileUp"
gen.Special.addTool(Pythia8Production, name = "Pythia8PileUp")
gen.Special.Pythia8PileUp.Tuning = "LHCbDefault.cmd"
gen.Special.Pythia8PileUp.Commands += commandsTuning_XST_7TeV
gen.Special.ReinitializePileUpGenerator  = False

# Use same generator and configuration for spillover
from Configurables import Gauss
spillOverList = Gauss().getProp("SpilloverPaths")
for slot in spillOverList:
    genSlot = Generation("Generation"+slot)
    genSlot.addTool(MinimumBias, name = "MinimumBias")
    genSlot.MinimumBias.ProductionTool = "Pythia8Production"
    genSlot.MinimumBias.addTool(Pythia8Production, name = "Pythia8Production")
    genSlot.MinimumBias.Pythia8Production.Commands += commandsTuning_XST_7TeV
    genSlot.MinimumBias.Pythia8Production.Tuning = "LHCbDefault.cmd"




