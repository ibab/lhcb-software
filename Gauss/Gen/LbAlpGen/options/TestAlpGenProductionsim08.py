from Gaudi.Configuration import *
importOptions( "$DECFILESROOT/options/SwitchOffAllPythiaProcesses.py" )
from Configurables import Generation, Special, FixedNInteractions, Pythia8Production, ToolSvc, EvtGenDecay, AlpGenProduction4q, MinimumBias
gen = Generation()

gen.SampleGenerationTool = "Special"
gen.addTool( Special )
gen.Special.ProductionTool = "AlpGenProduction4q"
gen.Special.addTool( AlpGenProduction4q )
alpgen=gen.Special.AlpGenProduction4q

alpgen.nevxiter = 100
alpgen.niter = 2
alpgen.nwgtev = 1000
alpgen.FileLabel = "4q"
alpgen.njets = 0  ##number of light jets
#alpgen.eta1bmin = 1.5
alpgen.etabmin = 1.5
alpgen.ptbmin = 3.0
alpgen.drbmin = 0.1
alpgen.etabmax = 10
alpgen.ndns=9

MessageSvc().OutputLevel = DEBUG 

Generation().Special.CutTool = ""

Generation().Special.PileUpProductionTool = "Pythia8Production"
Generation().PileUpTool = "FixedLuminosityForRareProcess"
Generation().Special.addTool( Pythia8Production , name = "Pythia8Production" )
Generation().Special.Pythia8Production.Tuning = "LHCbDefault.cmd"
Generation().Special.ReinitializePileUpGenerator  = False
