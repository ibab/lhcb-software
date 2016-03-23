#################################
# the default settings of pythia8
#################################
# -------------------------------
# Beam settings
# -------------------------------
Beams:frameType = 3
Beams:idA = 2212
Beams:idB = 2212
Beams:allowMomentumSpread = on
# -------------------------------
# Particle Decays and mixing
# -------------------------------
ParticleDecays:mixB = off
# HadronLevel:Decay = off
# -------------------------------
# control verbosity
# -------------------------------
Init:showAllSettings = off                   
Init:showMultipartonInteractions = off 
Init:showProcesses = off 
Init:showChangedSettings = off
Init:showChangedParticleData = off
Next:numberShowInfo = 0 
Next:numberShowProcess = 0 
Next:numberShowEvent = 0
Next:numberCount = 0
# -------------------------------
# process selection
# -------------------------------
SoftQCD:all = on
Charmonium:all = on
Bottomonium:all = on
# -------------------------------
# onia tuning
# -------------------------------
LhcbHooks:pT0Ref = 1.43
LhcbHooks:ecmRef = 13000
LhcbHooks:ecmPow = 0.09
Bottomonium:O(3S1)[3S1(1)] = 9.28,4.63,3.54
Bottomonium:O(3S1)[3S1(8)] = 0.15,0.045,0.075
Bottomonium:O(3S1)[1S0(8)] = 0.02,0.006,0.01
Bottomonium:O(3S1)[3P0(8)] = 0.02,0.006,0.01
# -------------------------------
# couplings and scales
# -------------------------------
SigmaProcess:alphaSorder = 2
# -------------------------------
# pdf selection
# be carefull it changes (behind the scene) pythia8 settings
# -------------------------------
PDF:useLHAPDF = on 
PDF:LHAPDFset = CT09MCS 
PDF:LHAPDFmember = 0
# -------------------------------
# First tuning for pythia8 and ct09mcs: Tune 1
# -------------------------------
SpaceShower:rapidityOrder = off      # General
MultipartonInteractions:bProfile = 1 # reset the default bProfile
SpaceShower:alphaSvalue             = 0.130
MultipartonInteractions:alphaSvalue = 0.130
StringFlav:mesonSvector             = 7.474387e-01 # Flavour selection
StringFlav:probQQtoQ                = 1.615701e-01
StringFlav:probStoUD                = 3.501613e-01
MultipartonInteractions:ecmRef      = 7000
MultipartonInteractions:pT0Ref      = 2.742289e+00' # Multiparton interactions
MultipartonInteractions:ecmPow      = 0.238
# -------------------------------
# flavour selection and excited states
# tuning adapted from pythia6)
# -------------------------------
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
# -------------------------------
# cross sections tuning (as in pythia6 default)
# -------------------------------
#SigmaProcess:renormMultFac = 0.075 
#BeamRemnants:primordialKThard = 1.0
# -------------------------------
# Lund fragmentation tuning (as in pythia6 default)
# -------------------------------
#StringFragmentation:stopMass = 0.4
#StringZ:aLund = 0.3 
#StringZ:bLund = 0.58 
#StringZ:rFactB = 1.
#StringPT:sigma = 0.36
