#
# B->DX selection for stripping (consider as an example). 
# This is an example of a simple wrapper over user's own algorithm. 
# No StrippingMember functionality is used here. 
#
# B->DX,
# where X=pi,K,K*,phi,
# and D=D+,D0,Ds.
# The allowed decay modes of the intermediates are
# K* -> Kpi, pipi (to include the pho)
# phi-> KK
# D+ -> hhh (h=pi,K all combinations except KKK),
# Ds -> hhh (h=pi,K all combinations except KKK), 
# D0 -> hh (h=pi,K all combinations).
# D0 -> hhhh (K3pi FAV and DCS, and KKpipi are considered), 
# D0 -> Kpipi0
# D0 -> Kshh (h=pi,K all combinations)
#
# The selection is based on a chi2 cuts on the impact paramters
# and Litefitime cuts

# This selection deliberately uses no RICH information, since the channels in
# the B->DX family are kinematically identical to each other and are the dominant
# non-combinatoric backgrounds to each other.  
#
# A GEC is included to filter out extremely noisy events, it requires
# the size of the Rec/Track/Best container to be less than 240.
#
# The selection is tuned on the L0xHLT1 sample of minbias, achieving 
# a reduction factor of 7/1000 for MC09 mbias data. 
# The signal efficiency for the various channels ranges between 80 and 100%.
# 
# Will further update the selection in stages.
#
# Authors: J. Nardulli 

from os import environ
from Gaudi.Configuration import *
from Configurables import DecayTreeTuple
from Configurables import CombineParticles, FilterDesktop
from Configurables import TupleToolMCBackgroundInfo, TupleToolMCTruth, BackgroundCategory
from StrippingConf.StrippingLine import StrippingLine

# We need the Void filter for the GEC
from Configurables import LoKi__VoidFilter as VoidFilter
from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
modules =  CoreFactory('CoreFactory').Modules
for i in [ 'LoKiTrigger.decorators' ] :
    if i not in modules : modules.append(i)

########################################################################
#Define the shared cuts
Dcut = "((M > 1768.*MeV) & (M < 2068.*MeV) & (VFASPF(VCHI2/VDOF)<12) & (BPVLTCHI2() > 100)) "

Dptcut = "PT>1*GeV"
Dcombcut = "(A" + Dptcut + ")"   #Common pt cut for D, K*, phi, second version to use
                                 #before the vertexing  
KPhiMothercut = "((VFASPF(VCHI2/VDOF) < 12) & (MIPCHI2DV(PRIMARY) > 4.))"

Kstarcut = "(M > 0)"
Phicut = "(M > 0)"

Bcut = "((VFASPF(VCHI2/VDOF)<12)  & (BPVLTFITCHI2() < 49) & (BPVLTIME()>0.15*ps) & (BPVLTCHI2() > 144)  & (BPVDIRA > 0.9995))"
Bcombcut = "((AM > 4800.*MeV) & (AM < 5900.*MeV))"
BSignalcombcut = "((M > 5229.*MeV) & (M < 5329.*MeV)) | ((M > 5316.*MeV ) & (M < 5416.*MeV))"

Daughtercut = "((TRCHI2DOF<10) & (PT >250*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 4.))"
Bachelorcut = "((TRCHI2DOF<10) & (PT >350*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 4.))"
PhiDaughterCut = "( 2 == NINTREE( (ABSID=='K+') & (" + Daughtercut + ")))" 
########################################################################
########################################################################
#Now the various selections etc.
########################################################################
########################################################################
# the K*
StrippingSelKstarChi2LT = CombineParticles("StrippingSelKstarChi2LT")
StrippingSelKstarChi2LT.InputLocations = ["StdNoPIDsPions","StdNoPIDsKaons"]
StrippingSelKstarChi2LT.DecayDescriptors = ["[K*(892)0 -> K+ pi-]cc","[K*(892)0 -> pi+ pi-]cc"]
StrippingSelKstarChi2LT.DaughtersCuts =  { "K+"        : Daughtercut,
                                           "pi+"        : Daughtercut }
StrippingSelKstarChi2LT.CombinationCut = Dcombcut
StrippingSelKstarChi2LT.MotherCut = KPhiMothercut
########################################################################
# the Phi
StrippingSelPhiChi2LT = CombineParticles("StrippingSelPhiChi2LT")
StrippingSelPhiChi2LT.InputLocations = ["StdNoPIDsKaons"]
StrippingSelPhiChi2LT.DecayDescriptor = "phi(1020) -> K+ K-"
StrippingSelPhiChi2LT.DaughtersCuts =  { "K+"        : Daughtercut}
StrippingSelPhiChi2LT.CombinationCut = Dcombcut
StrippingSelPhiChi2LT.MotherCut = KPhiMothercut
########################################################################
#The bachelor
StrippingBachelorChi2LT = FilterDesktop("StrippingBachelorChi2LT")
StrippingBachelorChi2LT.InputLocations  = [ "StdNoPIDsPions","StdNoPIDsKaons" ]
StrippingBachelorChi2LT.Code = Bachelorcut
########################################################################
SubXChi2LT = GaudiSequencer("SubXChi2LT")
SubXChi2LT.ModeOR = 1
SubXChi2LT.IgnoreFilterPassed = True
SubXChi2LT.Members += [StrippingSelKstarChi2LT]
SubXChi2LT.Members += [StrippingSelPhiChi2LT]
SubXChi2LT.Members += [StrippingBachelorChi2LT]
#######################################################################
# All possible D decays starting from the D->hh
StrippingSelD0Chi2LT = CombineParticles("StrippingSelD0Chi2LT")
StrippingSelD0Chi2LT.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD0Chi2LT.DecayDescriptors = ["[D0 -> K- pi+]cc","[D0 -> pi- pi+]cc","[D0 -> K- K+]cc","[D0 -> K+ pi-]cc"]
StrippingSelD0Chi2LT.DaughtersCuts =  { "K+"        : Daughtercut,"pi+"        : Daughtercut }
StrippingSelD0Chi2LT.CombinationCut = Dcombcut
StrippingSelD0Chi2LT.MotherCut = Dcut

#now the D ->hhh
StrippingSelD2hhhChi2LT = CombineParticles("StrippingSelD2hhhChi2LT")
StrippingSelD2hhhChi2LT.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD2hhhChi2LT.DecayDescriptors = ["[D+ -> K- pi+ pi+]cc","[D+ -> K- K+ pi+]cc","[D+ -> pi- pi+ pi+]cc","[D+ -> pi- pi+ K+]cc"]
StrippingSelD2hhhChi2LT.DaughtersCuts =  { "K+"        : Daughtercut,"pi+"        : Daughtercut }
StrippingSelD2hhhChi2LT.CombinationCut = Dcombcut
StrippingSelD2hhhChi2LT.MotherCut = Dcut

# D->KPiPi0
StripSelD2KPiPi0Chi2LT = CombineParticles("StripSelD2KPiPi0Chi2LT")
StripSelD2KPiPi0Chi2LT.InputLocations  = [ "StdLooseResolvedPi0","StdLooseMergedPi0","StdNoPIDsPions", "StdNoPIDsKaons"]
StripSelD2KPiPi0Chi2LT.DecayDescriptor = "[D0 -> K- pi+ pi0]cc"
StripSelD2KPiPi0Chi2LT.DaughtersCuts = { "K-"        : Daughtercut + "& (PT>400*MeV)",
                                         "pi+"        : Daughtercut + "& (PT>400*MeV)",
                                         "pi0": "(PT>800*MeV) & (P>4500*MeV)"}
StripSelD2KPiPi0Chi2LT.CombinationCut =  "(APT>2000*MeV)"
StripSelD2KPiPi0Chi2LT.MotherCut = Dcut+" & (BPVDIRA>0) & (P>3000*MeV)"

# D->hhhhh
StrippingSelD2hhhhChi2LT = CombineParticles("StrippingSelD2hhhhChi2LT")
StrippingSelD2hhhhChi2LT.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD2hhhhChi2LT.DecayDescriptors  = ["[D0 -> K- pi+ pi- pi+]cc","[D0 -> K+ pi- pi- pi+]cc","[D0 -> K+ K- pi+ pi-]cc"]
StrippingSelD2hhhhChi2LT.DaughtersCuts = { "K+"        : Daughtercut+ "& (PT>400*MeV)","pi+"        : Daughtercut+ "& (PT>400*MeV)"}
StrippingSelD2hhhhChi2LT.CombinationCut =  Dcombcut
StrippingSelD2hhhhChi2LT.MotherCut = Dcut+" & (BPVDIRA>0) & (P>3000*MeV)"

#Kshorts
KsDDChi2LT = CombineParticles("KsDDChi2LT")
KsDDChi2LT.InputLocations = [ "StdNoPIDsDownPions" ]
KsDDChi2LT.DecayDescriptor = "KS0 -> pi+ pi-"
KsDDChi2LT.DaughtersCuts = { "pi+" : Daughtercut }
KsDDChi2LT.CombinationCut = "(ADAMASS('KS0')<50*MeV)"
KsDDChi2LT.MotherCut = "(ADMASS('KS0')<30.0*MeV) & (VFASPF(VCHI2/VDOF)<16.0)"
KsLLChi2LT = KsDDChi2LT.clone("KsLLChi2LT")
KsLLChi2LT.InputLocations = [ "StdNoPIDsPions" ]
SubKshortsChi2LT = GaudiSequencer("SubKshortsChi2LT")
SubKshortsChi2LT.ModeOR = 1
SubKshortsChi2LT.IgnoreFilterPassed = True
SubKshortsChi2LT.Members += [KsDDChi2LT]
SubKshortsChi2LT.Members += [KsLLChi2LT]

#D->kspipi
StrippingSelD2kshhChi2LT = CombineParticles("StrippingSelD2kshhChi2LT")
StrippingSelD2kshhChi2LT.InputLocations = ["KsDDChi2LT", "KsLLChi2LT","StdNoPIDsPions","StdNoPIDsKaons"]
StrippingSelD2kshhChi2LT.DecayDescriptors = ["[D0 -> KS0 pi+ pi-]cc","[D0 -> KS0 K+ K-]cc","[D0 -> KS0 K+ pi-]cc","[D0 -> KS0 pi+ K-]cc"]
StrippingSelD2kshhChi2LT.DaughtersCuts = { "pi+" : Daughtercut,"K+" : Daughtercut, "KS0" : "ALL" }
StrippingSelD2kshhChi2LT.CombinationCut = Dcombcut
StrippingSelD2kshhChi2LT.MotherCut = Dcut

# D as a or of all possible Ds
SubDChi2LT = GaudiSequencer("SubDChi2LT")
SubDChi2LT.ModeOR = 1
SubDChi2LT.IgnoreFilterPassed = True
SubDChi2LT.Members += [StrippingSelD0Chi2LT]
SubDChi2LT.Members += [StrippingSelD2hhhChi2LT]
SubDChi2LT.Members += [StripSelD2KPiPi0Chi2LT]
SubDChi2LT.Members += [StrippingSelD2hhhhChi2LT]
SubDChi2LT.Members += [StrippingSelD2kshhChi2LT]
########################################################################
B2DXStrippingSelChi2LT = CombineParticles("B2DXStrippingSelChi2LT")
B2DXStrippingSelChi2LT.InputLocations  = ["StrippingSelD0Chi2LT",
                                          "StrippingSelD2hhhChi2LT",
                                          "StripSelD2KPiPi0Chi2LT",
                                          "StrippingSelD2hhhhChi2LT",
                                          "StrippingSelD2kshhChi2LT",
                                          "StrippingSelKstarChi2LT",
                                          "StrippingSelPhiChi2LT",
                                          "StrippingBachelorChi2LT"]

B2DXStrippingSelChi2LT.DecayDescriptors  = ["[B+ -> D~0 K+]cc",
                                            "[B+ -> D~0 pi+]cc",
                                            "[B+ -> D0 K+]cc",
                                            "[B+ -> D0 pi+]cc",
                                            "[B0 -> D~0 K*(892)0]cc",
                                            "[B0 -> D0 K*(892)0]cc",
                                            "[B0 -> D~0 phi(1020)]cc",
                                            "[B0 -> D0 phi(1020)]cc",
                                            "[B0 -> D- pi+]cc",
                                            "[B0 -> D- K+]cc"] 

B2DXStrippingSelChi2LT.DaughtersCuts = { "D+" : Dcut , "K*(892)0" : Kstarcut ,  "D0" : Dcut , "phi(1020)": Phicut, "pi+" : Bachelorcut, "K+" : Bachelorcut}
B2DXStrippingSelChi2LT.CombinationCut = Bcombcut
B2DXStrippingSelChi2LT.MotherCut = Bcut
########################################################################
# Define the GEC on the number of tracks, needed in order to control
# the time for the combinatorics (especially once we add the 4-body D states)
B2DXStrippingNumTracksGECChi2LT = VoidFilter('B2DXStrippingNumTracksGECChi2LT'
                                             , Code = "TrSOURCE('Rec/Track/Best') >> (TrSIZE < 240 )"
                                             )
########################################################################
B2DXStrippingSequenceChi2LT = GaudiSequencer("B2DXStrippingSequenceChi2LT")
B2DXStrippingSequenceChi2LT.Members += [SubKshortsChi2LT]
B2DXStrippingSequenceChi2LT.Members += [SubXChi2LT]
B2DXStrippingSequenceChi2LT.Members += [SubDChi2LT]
B2DXStrippingSequenceChi2LT.Members += [B2DXStrippingSelChi2LT]
########################################################################


# Create the lines for this selection
line1 = StrippingLine('B2DXChi2LT'
                      , prescale = 1.0
                      ,  algos = [ B2DXStrippingNumTracksGECChi2LT, B2DXStrippingSequenceChi2LT ]
                      #,  algos = [ B2DXStrippingSequence ]
                      , postscale = 1.0
                      )

