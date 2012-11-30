# ------------
# the default settings of pythia8
# ------------
# Beam settings
Beams:frameType = 3  # CM energy 1
Beams:idA = 2212
Beams:idB = 2212
# allow LHCb beam tool
Beams:allowMomentumSpread = on
# mixing is handled by EvtGen
ParticleDecays:mixB = off
# control verbosity
Init:showAllSettings = off                   
Init:showMultipartonInteractions = off 
Init:showProcesses = off 
Init:showChangedSettings = on 
Next:numberShowInfo = 0 
Next:numberShowProcess = 0 
Next:numberShowEvent = 1
# process selection
SoftQCD:all = on
# HardQCD:all = off
# couplings and scales
SigmaProcess:alphaSorder = 2
# pdf selection
PDF:useLHAPDF = on 
PDF:LHAPDFset = cteq6ll.LHpdf 
PDF:LHAPDFmember = 0
# cross sections tuning
SigmaProcess:Kfactor = 1.0 
SigmaProcess:renormMultFac = 0.075 
PartonLevel:MI = on
BeamRemnants:primordialKThard = 1.0
# MultipartonInteractions:Kfactor = 1.0 
# MultipartonInteractions:pTmin = 0.2 
# MultipartonInteractions:bProfile = 1 
# charged density at eta=0 tuning (pythia6 default)
MultipartonInteractions:pT0Ref = 4.28 
MultipartonInteractions:ecmRef = 14000.0 
MultipartonInteractions:ecmPow = 0.238 
# new tuning introduced for pythia8 and cteq6L1
MultipartonInteractions:ecmRef = 7000 
MultipartonInteractions:pT0Ref = 2.88 
MultipartonInteractions:ecmPow = 0.238
#
StringFragmentation:stopMass = 0.4 
# flavour selection and excited states tuning (adapted from pythia6)
StringFlav:mesonUDvector = 0.6 
StringFlav:mesonSvector = 0.6  
StringFlav:mesonCvector = 3.0 
StringFlav:mesonBvector = 3.0 
StringFlav:probStoUD = 0.30 
StringFlav:probQQtoQ = 0.10 
StringFlav:probSQtoQQ = 0.4 
StringFlav:probQQ1toQQ0 = 0.05 
StringFlav:mesonUDL1S0J1 = 0.0989 
StringFlav:mesonUDL1S1J0 = 0.0132 
StringFlav:mesonUDL1S1J1 = 0.0597 
StringFlav:mesonUDL1S1J2 = 0.0597 
StringFlav:mesonSL1S0J1 = 0.0989 
StringFlav:mesonSL1S1J0 = 0.0132 
StringFlav:mesonSL1S1J1 = 0.0597 
StringFlav:mesonSL1S1J2 = 0.0597 
StringFlav:mesonCL1S0J1 = 0.0990 
StringFlav:mesonCL1S1J0 = 0.0657 
StringFlav:mesonCL1S1J1 = 0.2986 
StringFlav:mesonCL1S1J2 = 0.2986 
StringFlav:mesonBL1S0J1 = 0.0990 
StringFlav:mesonBL1S1J0 = 0.0657 
StringFlav:mesonBL1S1J1 = 0.2986 
StringFlav:mesonBL1S1J2 = 0.2986 
StringFlav:etaSup = 1. 
StringFlav:etaPrimeSup = 0.4 
# Lund fragmentation tuning (as in pythia6 default)
StringZ:aLund = 0.3 
StringZ:bLund = 0.58 
StringZ:rFactB = 1.
# 
StringPT:sigma = 0.36
