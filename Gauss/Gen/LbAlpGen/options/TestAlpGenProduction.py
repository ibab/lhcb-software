from Gaudi.Configuration import *
importOptions( "$DECFILESROOT/options/SwitchOffAllPythiaProcesses.opts" )
from Configurables import Generation, Special, FixedNInteractions, Pythia8Production, ToolSvc, EvtGenDecay, AlpGenProduction4q, MinimumBias
gen = Generation()

gen.SampleGenerationTool = "Special"
gen.addTool( Special )
gen.Special.ProductionTool = "AlpGenProduction4q"
gen.Special.addTool( AlpGenProduction4q )
alpgen=gen.Special.AlpGenProduction4q

alpgen.nevxiter = 10000
alpgen.niter = 2
alpgen.nwgtev = 100000
alpgen.FileLabel = "4q"
alpgen.njets = 0  ##number of light jets
#alpgen.eta1bmin = 1.5
alpgen.etabmin = 1.5
alpgen.ptbmin = 3.0
alpgen.drbmin = 0.1
alpgen.etabmax = 10
alpgen.ndns=9


Generation().Special.CutTool = ""

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

Generation().Special.PileUpProductionTool = "Pythia8Production"
Generation().PileUpTool = "FixedLuminosityForRareProcess"
#Generation().addTool( FixedLuminosityForRareProcess )
Generation().Special.addTool( Pythia8Production , name = "Pythia8Production" )
Generation().Special.Pythia8Production.Commands += Pythia8ListOfCommands
Generation().Special.ReinitializePileUpGenerator  = False 

