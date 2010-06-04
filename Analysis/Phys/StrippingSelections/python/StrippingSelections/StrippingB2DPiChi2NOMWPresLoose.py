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
# and flight significances (wrt. the offline selections),
#
# This selection deliberately uses no RICH information, since the channels in
# the B->DX family are kinematically identical to each other and are the dominant
# non-combinatoric backgrounds to each other.  
#
# A GEC is included to filter out extremely noisy events, it requires
# the size of the Rec/Track/Best container to be less than 240.
#
# The selection is tuned on the L0xHLT1 sample of minbias, achieving 
# a reduction factor of 1/100 for MC09 mbias data. 
# The signal efficiency for the various channels ranges between 85 and 100%.
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
Dcut = "((M > 1768.*MeV) & (M < 2068.*MeV) & (VFASPF(VCHI2/VDOF)<12) & (BPVVDCHI2 > 100)) "

Dptcut = "PT>1*GeV"
Dcombcut = "(A" + Dptcut + ")"   #Common pt cut for D, K*, phi, second version to use
                                 #before the vertexing  
KPhiMothercut = "((VFASPF(VCHI2/VDOF) < 12) & (MIPCHI2DV(PRIMARY) > 4.))"

Kstarcut = "(M > 0)"
Phicut = "(M > 0)"

Bcut = "((VFASPF(VCHI2/VDOF)<12)  & (BPVIPCHI2() < 49) & (BPVVDCHI2 > 100)  & (BPVDIRA > 0.9995))"
Bcombcut = "((AM > 4800.*MeV) & (AM < 5900.*MeV))"

Daughtercut = "((TRCHI2DOF<20) & (PT >250*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 4.))"
Bachelorcut = "((TRCHI2DOF<20) & (PT >350*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 4.))"
PhiDaughterCut = "( 2 == NINTREE( (ABSID=='K+') & (" + Daughtercut + ")))" 

########################################################################
########################################################################
#Now the various selections etc.
########################################################################
########################################################################
# the K*
StrippingSelKstarChi2NOMWPresLoose = CombineParticles("StrippingSelKstarChi2NOMWPresLoose")
StrippingSelKstarChi2NOMWPresLoose.InputLocations = ["StdNoPIDsPions","StdNoPIDsKaons"]
StrippingSelKstarChi2NOMWPresLoose.DecayDescriptors = ["[K*(892)0 -> K+ pi-]cc","[K*(892)0 -> pi+ pi-]cc"]
StrippingSelKstarChi2NOMWPresLoose.DaughtersCuts =  { "K+"        : Daughtercut,
                                             "pi+"        : Daughtercut }
StrippingSelKstarChi2NOMWPresLoose.CombinationCut = Dcombcut
StrippingSelKstarChi2NOMWPresLoose.MotherCut = KPhiMothercut
########################################################################
# the Phi
StrippingSelPhiChi2NOMWPresLoose = CombineParticles("StrippingSelPhiChi2NOMWPresLoose")
StrippingSelPhiChi2NOMWPresLoose.InputLocations = ["StdNoPIDsKaons"]
StrippingSelPhiChi2NOMWPresLoose.DecayDescriptor = "phi(1020) -> K+ K-"
StrippingSelPhiChi2NOMWPresLoose.DaughtersCuts =  { "K+"        : Daughtercut}
StrippingSelPhiChi2NOMWPresLoose.CombinationCut = Dcombcut
StrippingSelPhiChi2NOMWPresLoose.MotherCut = KPhiMothercut
########################################################################
#The bachelor
StrippingBachelorChi2NOMWPresLoose = FilterDesktop("StrippingBachelorChi2NOMWPresLoose")
StrippingBachelorChi2NOMWPresLoose.InputLocations  = [ "StdNoPIDsPions","StdNoPIDsKaons" ]
StrippingBachelorChi2NOMWPresLoose.Code = Bachelorcut
########################################################################
SubXChi2NOMWPresLoose = GaudiSequencer("SubXChi2NOMWPresLoose")
SubXChi2NOMWPresLoose.ModeOR = 1
SubXChi2NOMWPresLoose.IgnoreFilterPassed = True
SubXChi2NOMWPresLoose.Members += [StrippingSelKstarChi2NOMWPresLoose]
SubXChi2NOMWPresLoose.Members += [StrippingSelPhiChi2NOMWPresLoose]
SubXChi2NOMWPresLoose.Members += [StrippingBachelorChi2NOMWPresLoose]
#######################################################################
# All possible D decays starting from the D->hh
StrippingSelD0Chi2NOMWPresLoose = CombineParticles("StrippingSelD0Chi2NOMWPresLoose")
StrippingSelD0Chi2NOMWPresLoose.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD0Chi2NOMWPresLoose.DecayDescriptors = ["[D0 -> K- pi+]cc","[D0 -> pi- pi+]cc","[D0 -> K- K+]cc","[D0 -> K+ pi-]cc"]
StrippingSelD0Chi2NOMWPresLoose.DaughtersCuts =  { "K+"        : Daughtercut,"pi+"        : Daughtercut }
StrippingSelD0Chi2NOMWPresLoose.CombinationCut = Dcombcut
StrippingSelD0Chi2NOMWPresLoose.MotherCut = Dcut
#now the D ->hhh
StrippingSelD2hhhChi2NOMWPresLoose = CombineParticles("StrippingSelD2hhhChi2NOMWPresLoose")
StrippingSelD2hhhChi2NOMWPresLoose.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD2hhhChi2NOMWPresLoose.DecayDescriptors = ["[D+ -> K- pi+ pi+]cc","[D+ -> K- K+ pi+]cc","[D+ -> pi- pi+ pi+]cc","[D+ -> pi- pi+ K+]cc"]
StrippingSelD2hhhChi2NOMWPresLoose.DaughtersCuts =  { "K+"        : Daughtercut,"pi+"        : Daughtercut }
StrippingSelD2hhhChi2NOMWPresLoose.CombinationCut = Dcombcut
StrippingSelD2hhhChi2NOMWPresLoose.MotherCut = Dcut

# D->KPiPi0
StripSelD2KPiPi0Chi2NOMWPresLoose = CombineParticles("StripSelD2KPiPi0Chi2NOMWPresLoose")
StripSelD2KPiPi0Chi2NOMWPresLoose.InputLocations  = [ "StdResolvedPi0","StdMergedPi0","StdNoPIDsPions", "StdNoPIDsKaons"]
StripSelD2KPiPi0Chi2NOMWPresLoose.DecayDescriptor = "[D0 -> K- pi+ pi0]cc"
StripSelD2KPiPi0Chi2NOMWPresLoose.DaughtersCuts = { "K-"        : Daughtercut + "& (PT>400*MeV)",
                                           "pi+"        : Daughtercut + "& (PT>400*MeV)",
                                           "pi0": "(PT>800*MeV) & (P>4500*MeV)"}
StripSelD2KPiPi0Chi2NOMWPresLoose.CombinationCut =  "(APT>2000*MeV)"
StripSelD2KPiPi0Chi2NOMWPresLoose.MotherCut = Dcut+" & (BPVDIRA>0) & (P>3000*MeV)"

# D->hhhhh
StrippingSelD2hhhhChi2NOMWPresLoose = CombineParticles("StrippingSelD2hhhhChi2NOMWPresLoose")
StrippingSelD2hhhhChi2NOMWPresLoose.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD2hhhhChi2NOMWPresLoose.DecayDescriptors  = ["[D0 -> K- pi+ pi- pi+]cc","[D0 -> K+ pi- pi- pi+]cc","[D0 -> K+ K- pi+ pi-]cc"]
StrippingSelD2hhhhChi2NOMWPresLoose.DaughtersCuts = { "K+"        : Daughtercut+ "& (PT>400*MeV)","pi+"        : Daughtercut+ "& (PT>400*MeV)"}
StrippingSelD2hhhhChi2NOMWPresLoose.CombinationCut =  Dcombcut
StrippingSelD2hhhhChi2NOMWPresLoose.MotherCut = Dcut+" & (BPVDIRA>0) & (P>3000*MeV)"

#Kshorts
KsDDChi2NOMWPresLoose = CombineParticles("KsDDChi2NOMWPresLoose")
KsDDChi2NOMWPresLoose.InputLocations = [ "StdNoPIDsDownPions" ]
KsDDChi2NOMWPresLoose.DecayDescriptor = "KS0 -> pi+ pi-"
KsDDChi2NOMWPresLoose.DaughtersCuts = { "pi+" : Daughtercut }
KsDDChi2NOMWPresLoose.CombinationCut = "(ADAMASS('KS0')<50*MeV)"
KsDDChi2NOMWPresLoose.MotherCut = "(ADMASS('KS0')<30.0*MeV) & (VFASPF(VCHI2/VDOF)<16.0)"
KsLLChi2NOMWPresLoose = KsDDChi2NOMWPresLoose.clone("KsLLChi2NOMWPresLoose")
KsLLChi2NOMWPresLoose.InputLocations = [ "StdNoPIDsPions" ]
SubKshortsChi2NOMWPresLoose = GaudiSequencer("SubKshortsChi2NOMWPresLoose")
SubKshortsChi2NOMWPresLoose.ModeOR = 1
SubKshortsChi2NOMWPresLoose.IgnoreFilterPassed = True
SubKshortsChi2NOMWPresLoose.Members += [KsDDChi2NOMWPresLoose]
SubKshortsChi2NOMWPresLoose.Members += [KsLLChi2NOMWPresLoose]

#D->kspipi
StrippingSelD2kshhChi2NOMWPresLoose = CombineParticles("StrippingSelD2kshhChi2NOMWPresLoose")
StrippingSelD2kshhChi2NOMWPresLoose.InputLocations = ["KsDDChi2NOMWPresLoose", "KsLLChi2NOMWPresLoose","StdNoPIDsPions","StdNoPIDsKaons"]
StrippingSelD2kshhChi2NOMWPresLoose.DecayDescriptors = ["[D0 -> KS0 pi+ pi-]cc","[D0 -> KS0 K+ K-]cc","[D0 -> KS0 K+ pi-]cc","[D0 -> KS0 pi+ K-]cc"]
StrippingSelD2kshhChi2NOMWPresLoose.DaughtersCuts = { "pi+" : Daughtercut,"K+" : Daughtercut, "KS0" : "ALL" }
StrippingSelD2kshhChi2NOMWPresLoose.CombinationCut = Dcombcut
StrippingSelD2kshhChi2NOMWPresLoose.MotherCut = Dcut

# D as a or of all possible Ds
SubDChi2NOMWPresLoose = GaudiSequencer("SubDChi2NOMWPresLoose")
SubDChi2NOMWPresLoose.ModeOR = 1
SubDChi2NOMWPresLoose.IgnoreFilterPassed = True
SubDChi2NOMWPresLoose.Members += [StrippingSelD0Chi2NOMWPresLoose]
SubDChi2NOMWPresLoose.Members += [StrippingSelD2hhhChi2NOMWPresLoose]
SubDChi2NOMWPresLoose.Members += [StripSelD2KPiPi0Chi2NOMWPresLoose]
SubDChi2NOMWPresLoose.Members += [StrippingSelD2hhhhChi2NOMWPresLoose]
SubDChi2NOMWPresLoose.Members += [StrippingSelD2kshhChi2NOMWPresLoose]
########################################################################
B2DXStrippingSelChi2NOMWPresLoose = CombineParticles("B2DXStrippingSelChi2NOMWPresLoose")
B2DXStrippingSelChi2NOMWPresLoose.InputLocations  = ["StrippingSelD0Chi2NOMWPresLoose",
                                                     "StrippingSelD2hhhChi2NOMWPresLoose",
                                                     "StripSelD2KPiPi0Chi2NOMWPresLoose",
                                                     "StrippingSelD2hhhhChi2NOMWPresLoose",
                                                     "StrippingSelD2kshhChi2NOMWPresLoose",
                                                     "StrippingSelKstarChi2NOMWPresLoose",
                                                     "StrippingSelPhiChi2NOMWPresLoose",
                                                     "StrippingBachelorChi2NOMWPresLoose"]

B2DXStrippingSelChi2NOMWPresLoose.DecayDescriptors  = ["[B+ -> D~0 K+]cc",
                                                       "[B+ -> D~0 pi+]cc",
                                                       "[B+ -> D0 K+]cc",
                                                       "[B+ -> D0 pi+]cc",
                                                       "[B0 -> D~0 K*(892)0]cc",
                                                       "[B0 -> D0 K*(892)0]cc",
                                                       "[B0 -> D~0 phi(1020)]cc",
                                                       "[B0 -> D0 phi(1020)]cc",
                                                       "[B0 -> D- pi+]cc",
                                                       "[B0 -> D- K+]cc"] 

B2DXStrippingSelChi2NOMWPresLoose.DaughtersCuts = { "D+" : Dcut , "K*(892)0" : Kstarcut ,  "D0" : Dcut , "phi(1020)": Phicut, "pi+" : Bachelorcut, "K+" : Bachelorcut}
B2DXStrippingSelChi2NOMWPresLoose.CombinationCut = Bcombcut
B2DXStrippingSelChi2NOMWPresLoose.MotherCut = Bcut

########################################################################
# Define the GEC on the number of tracks, needed in order to control
# the time for the combinatorics (especially once we add the 4-body D states)
B2DXStrippingNumTracksGECChi2NOMWPresLoose = VoidFilter('B2DXStrippingNumTracksGECChi2NOMWPresLoose'
                                                    , Code = "TrSOURCE('Rec/Track/Best') >> (TrSIZE < 240 )"
                                                    )
########################################################################
B2DXStrippingSequenceChi2NOMWPresLoose = GaudiSequencer("B2DXStrippingSequenceChi2NOMWPresLoose")
B2DXStrippingSequenceChi2NOMWPresLoose.Members += [SubKshortsChi2NOMWPresLoose]
B2DXStrippingSequenceChi2NOMWPresLoose.Members += [SubXChi2NOMWPresLoose]
B2DXStrippingSequenceChi2NOMWPresLoose.Members += [SubDChi2NOMWPresLoose]
B2DXStrippingSequenceChi2NOMWPresLoose.Members += [B2DXStrippingSelChi2NOMWPresLoose]
########################################################################

# Create the lines for this selection
line1 = StrippingLine('B2DXChi2NOMWPresLoose'
                      , prescale = 1.0
                      ,  algos = [ B2DXStrippingNumTracksGECChi2NOMWPresLoose, B2DXStrippingSequenceChi2NOMWPresLoose ]
                      , postscale = 1.0
                      )

