from Configurables import Generation, MinimumBias, Pythia8Production
from Configurables import Inclusive, SignalPlain, SignalRepeatedHadronization
from Configurables import Special

Pythia8ListOfCommands = [
    "Beam Settings" ,
    "skip" ,
    "PDG id code for the second incoming particle",
    "Mode commands for Pythia8" ,
    # do not smear beams
    "Beams:allowMomentumSpread = on",
    # control verbosity
    "Init:showAllSettings = off" ,
    "Init:showMultipartonInteractions = off" ,
    "Init:showProcesses = off" ,
    "Init:showChangedSettings = off" ,
    "Next:numberShowInfo = 0" ,
    "Next:numberShowProcess = 0" ,
    "Next:numberShowEvent = 1" ,
    "SoftQCD:all = on",
    "HardQCD:all = off",
    "PDF:useLHAPDF = on" ,
    "PDF:LHAPDFset = cteq6ll.LHpdf" ,
    "PDF:LHAPDFmember = 0" ,
    "SigmaProcess:alphaSorder = 2" ,
    "SigmaProcess:Kfactor = 1.0" ,
    "SigmaProcess:renormMultFac = 0.075" ,
    "PartonLevel:MI = on" ,
    "MultipartonInteractions:Kfactor = 1.0" ,
    "MultipartonInteractions:pTmin = 0.2" ,
    "MultipartonInteractions:bProfile = 1" ,
    # old tuning from pyhtia6
    "MultipartonInteractions:pT0Ref = 4.28" ,
    "MultipartonInteractions:ecmRef = 14000.0" ,
    "MultipartonInteractions:ecmPow = 0.238" ,
    # new tuning proposed for pyhtia8
    "MultipartonInteractions:ecmRef = 7000" ,
    "MultipartonInteractions:pT0Ref = 2.88" ,
    "MultipartonInteractions:ecmPow = 0.238" ,    
    "BeamRemnants:primordialKThard = 1.0" ,
    "ParticleDecays:mixB = off" ,
    "StringFragmentation:stopMass = 0.4" ,
    "StringFlav:mesonUDvector = 0.6" ,
    "StringFlav:mesonSvector = 0.6" , 
    "StringFlav:mesonCvector = 3.0" ,
    "StringFlav:mesonBvector = 3.0" ,
    "StringFlav:probStoUD = 0.30" ,
    "StringFlav:probQQtoQ = 0.10" ,
    "StringFlav:probSQtoQQ = 0.4" ,
    "StringFlav:probQQ1toQQ0 = 0.05" ,
    "StringFlav:mesonUDL1S0J1 = 0.0989" ,
    "StringFlav:mesonUDL1S1J0 = 0.0132" ,
    "StringFlav:mesonUDL1S1J1 = 0.0597" ,
    "StringFlav:mesonUDL1S1J2 = 0.0597" ,
    "StringFlav:mesonSL1S0J1 = 0.0989" ,
    "StringFlav:mesonSL1S1J0 = 0.0132" ,
    "StringFlav:mesonSL1S1J1 = 0.0597" ,
    "StringFlav:mesonSL1S1J2 = 0.0597" ,
    "StringFlav:mesonCL1S0J1 = 0.0990" ,
    "StringFlav:mesonCL1S1J0 = 0.0657" ,
    "StringFlav:mesonCL1S1J1 = 0.2986" ,
    "StringFlav:mesonCL1S1J2 = 0.2986" ,
    "StringFlav:mesonBL1S0J1 = 0.0990" ,
    "StringFlav:mesonBL1S1J0 = 0.0657" ,
    "StringFlav:mesonBL1S1J1 = 0.2986" ,
    "StringFlav:mesonBL1S1J2 = 0.2986" ,
    "StringFlav:etaSup = 1." ,
    "StringFlav:etaPrimeSup = 0.4" ,
    "StringZ:aLund = 0.3" ,
    "StringZ:bLund = 0.58" ,
    "StringZ:rFactB = 1." ,
    "StringPT:sigma = 0.36"
    ]

Pythia8TurnOffFragmentation = [ "HadronLevel:all = off" ]

gen = Generation()
gen.addTool( MinimumBias , name = "MinimumBias" )
gen.MinimumBias.ProductionTool = "Pythia8Production"
gen.MinimumBias.addTool( Pythia8Production , name = "Pythia8Production" )
gen.MinimumBias.Pythia8Production.Commands += Pythia8ListOfCommands 

gen.addTool( Inclusive , name = "Inclusive" )
gen.Inclusive.ProductionTool = "Pythia8Production"
gen.Inclusive.addTool( Pythia8Production , name = "Pythia8Production" )
gen.Inclusive.Pythia8Production.Commands += Pythia8ListOfCommands 

gen.addTool( SignalPlain , name = "SignalPlain" )
gen.SignalPlain.ProductionTool = "Pythia8Production"
gen.SignalPlain.addTool( Pythia8Production , name = "Pythia8Production" )
gen.SignalPlain.Pythia8Production.Commands += Pythia8ListOfCommands 

gen.addTool( SignalRepeatedHadronization , name = "SignalRepeatedHadronization" )
gen.SignalRepeatedHadronization.ProductionTool = "Pythia8Production"
gen.SignalRepeatedHadronization.addTool( Pythia8Production , name = "Pythia8Production" )
gen.SignalRepeatedHadronization.Pythia8Production.Commands += Pythia8ListOfCommands 
gen.SignalRepeatedHadronization.Pythia8Production.Commands += Pythia8TurnOffFragmentation

gen.addTool( Special , name = "Special" )
gen.Special.ProductionTool = "Pythia8Production"
gen.Special.addTool( Pythia8Production , name = "Pythia8Production" )
gen.Special.Pythia8Production.Commands += Pythia8ListOfCommands 
