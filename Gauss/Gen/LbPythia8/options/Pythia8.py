from Configurables import Generation, MinimumBias, Pythia8Production
from Configurables import Inclusive, SignalPlain, SignalRepeatedHadronization
from Configurables import Special

Pythia8ListOfCommands = [
    "Beam Settings" ,
    "skip" ,
    "PDG id code for the second incoming particle",
    "Mode commands for Pythia8" ,
    "Main:showAllSettings = on" ,
    "SoftQCD:all = on",
    "HardQCD:all = off",
    "PDF:useLHAPDF = on" ,
    "PDF:LHAPDFset = cteq6ll.LHpdf" ,
    "PDF:LHAPDFmember = 0" ,
    "SigmaProcess:alphaSorder = 2" ,
    "SigmaProcess:Kfactor = 1.0" ,
    "SigmaProcess:renormMultFac = 0.075" ,
    "PartonLevel:MI = on" ,
    "MultipleInteractions:Kfactor = 1.0" ,
    "MultipleInteractions:pTmin = 0.2" ,
    "MultipleInteractions:bProfile = 1" ,
    "MultipleInteractions:pT0Ref = 4.28" ,
    "MultipleInteractions:ecmRef = 14000.0" ,
    "MultipleInteractions:ecmPow = 0.238" ,
    "BeamRemnants:primordialKThard = 1.0" ,
    "Bottomonium:OUpsilon3P08 = 0.02" ,
    "Bottomonium:Ochib03P01 = 0.085" ,
    "StringFlav:mesonUDvector = 1.0" ,
    "StringFlav:mesonSvector = 0.6667" ,
    "ParticleDecays:mixB = off" ,
    "StringFragmentation:stopMass = 0.4" ,
    "StringFlav:mesonCL1S0J1 = 0.0405" ,
    "StringFlav:mesonCL1S1J0 = 0.0135" ,
    "StringFlav:mesonCL1S1J1 = 0.0405" ,
    "StringFlav:mesonCL1S1J2 = 0.0675" ,
    "StringFlav:mesonBL1S0J1 = 0.0405" ,
    "StringFlav:mesonBL1S1J0 = 0.0135" ,
    "StringFlav:mesonBL1S1J1 = 0.0405" ,
    "StringFlav:mesonBL1S1J2 = 0.0675" ,
#    "PDF:useLHAPDF = off" ,
#    "PDF:pSet = 8" ,
#    "HardQCD:nQuarkNew = 5",
#    "HardQCD:gg2ccbar = off",
#    "HardQCD:qqbar2ccbar = off",
#    "HardQCD:gg2bbbar = off",
#    "HardQCD:qqbar2bbbar = off" ,
#    "HardQCD:3parton = off" ,
#    "Charmonium:all = on",
#    "Bottomonium:all = on",
#    "PhaseSpace:mHatMin = 3.0" ,
#    "PhaseSpace:pTHatMin = 10.0" ,
#    "MultipleInteractions:pTmin = 1.9" ,
#    "SigmaProcess:renormScale2 = 3" ,
#    "StringFlav:mesonCvector = 3.0" ,
#    "StringFlav:mesonBvector = 3.0" ,
#    "StringFlav:mesonUDL1S0J1 = 0.0" ,
#    "StringFlav:mesonUDL1S0J1 = 0.018" ,
#    "StringFlav:mesonUDL1S1J1 = 0.054" ,
#    "StringFlav:mesonUDL1S1J2 = 0.131" ,
#    "StringFlav:mesonSL1S0J1 = 0.0" ,
#    "StringFlav:mesonSL1S0J1 = 0.018" ,
#    "StringFlav:mesonSL1S1J1 = 0.054" ,
#    "StringFlav:mesonSL1S1J2 = 0.131" ,
#    "StringFlav:mesonCL1S0J1 = 0.0" ,
#    "StringFlav:mesonCL1S0J1 = 0.018" ,
#    "StringFlav:mesonCL1S1J1 = 0.054" ,
#    "StringFlav:mesonCL1S1J2 = 0.131" ,
#    "StringFlav:mesonBL1S0J1 = 0.0" ,
#    "StringFlav:mesonBL1S0J1 = 0.018" ,
#    "StringFlav:mesonBL1S1J1 = 0.054" ,
#    "StringFlav:mesonBL1S1J2 = 0.131"
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
gen.Inclusive.Pythia8Production.ValidateHEPEVT = True

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
