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
# a reduction factor of 6/1000 for MC09 mbias data. 
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
Dcut = "((M > 1768.*MeV) & (M < 2068.*MeV) & (VFASPF(VCHI2/VDOF)<12) & (BPVVDCHI2 > 169)) "

Dptcut = "PT>1*GeV"
Dcombcut = "(A" + Dptcut + ")"   #Common pt cut for D, K*, phi, second version to use
                                 #before the vertexing  
KPhiMothercut = "((VFASPF(VCHI2/VDOF) < 12) & (MIPCHI2DV(PRIMARY) > 4.))"

Kstarcut = "(M > 0)"
Phicut = "(M > 0)"

Bcut = "((VFASPF(VCHI2/VDOF)<12)  & (BPVIPCHI2() < 49) & (BPVVDCHI2 > 225)  & (BPVDIRA > 0.9995))"
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
StrippingSelKstarChi2NOMWPres = CombineParticles("StrippingSelKstarChi2NOMWPres")
StrippingSelKstarChi2NOMWPres.InputLocations = ["StdNoPIDsPions","StdNoPIDsKaons"]
StrippingSelKstarChi2NOMWPres.DecayDescriptors = ["[K*(892)0 -> K+ pi-]cc","[K*(892)0 -> pi+ pi-]cc"]
StrippingSelKstarChi2NOMWPres.DaughtersCuts =  { "K+"        : Daughtercut,
                                             "pi+"        : Daughtercut }
StrippingSelKstarChi2NOMWPres.CombinationCut = Dcombcut
StrippingSelKstarChi2NOMWPres.MotherCut = KPhiMothercut
########################################################################
# the Phi
StrippingSelPhiChi2NOMWPres = CombineParticles("StrippingSelPhiChi2NOMWPres")
StrippingSelPhiChi2NOMWPres.InputLocations = ["StdNoPIDsKaons"]
StrippingSelPhiChi2NOMWPres.DecayDescriptor = "phi(1020) -> K+ K-"
StrippingSelPhiChi2NOMWPres.DaughtersCuts =  { "K+"        : Daughtercut}
StrippingSelPhiChi2NOMWPres.CombinationCut = Dcombcut
StrippingSelPhiChi2NOMWPres.MotherCut = KPhiMothercut
########################################################################
#The bachelor
StrippingBachelorChi2NOMWPres = FilterDesktop("StrippingBachelorChi2NOMWPres")
StrippingBachelorChi2NOMWPres.InputLocations  = [ "StdNoPIDsPions","StdNoPIDsKaons" ]
StrippingBachelorChi2NOMWPres.Code = Bachelorcut
########################################################################
SubXChi2NOMWPres = GaudiSequencer("SubXChi2NOMWPres")
SubXChi2NOMWPres.ModeOR = 1
SubXChi2NOMWPres.IgnoreFilterPassed = True
SubXChi2NOMWPres.Members += [StrippingSelKstarChi2NOMWPres]
SubXChi2NOMWPres.Members += [StrippingSelPhiChi2NOMWPres]
SubXChi2NOMWPres.Members += [StrippingBachelorChi2NOMWPres]
#######################################################################
# All possible D decays starting from the D->hh
StrippingSelD0Chi2NOMWPres = CombineParticles("StrippingSelD0Chi2NOMWPres")
StrippingSelD0Chi2NOMWPres.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD0Chi2NOMWPres.DecayDescriptors = ["[D0 -> K- pi+]cc","[D0 -> pi- pi+]cc","[D0 -> K- K+]cc","[D0 -> K+ pi-]cc"]
StrippingSelD0Chi2NOMWPres.DaughtersCuts =  { "K+"        : Daughtercut,"pi+"        : Daughtercut }
StrippingSelD0Chi2NOMWPres.CombinationCut = Dcombcut
StrippingSelD0Chi2NOMWPres.MotherCut = Dcut
#now the D ->hhh
StrippingSelD2hhhChi2NOMWPres = CombineParticles("StrippingSelD2hhhChi2NOMWPres")
StrippingSelD2hhhChi2NOMWPres.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD2hhhChi2NOMWPres.DecayDescriptors = ["[D+ -> K- pi+ pi+]cc","[D+ -> K- K+ pi+]cc","[D+ -> pi- pi+ pi+]cc","[D+ -> pi- pi+ K+]cc"]
StrippingSelD2hhhChi2NOMWPres.DaughtersCuts =  { "K+"        : Daughtercut,"pi+"        : Daughtercut }
StrippingSelD2hhhChi2NOMWPres.CombinationCut = Dcombcut
StrippingSelD2hhhChi2NOMWPres.MotherCut = Dcut

# D->KPiPi0
StripSelD2KPiPi0Chi2NOMWPres = CombineParticles("StripSelD2KPiPi0Chi2NOMWPres")
StripSelD2KPiPi0Chi2NOMWPres.InputLocations  = [ "StdResolvedPi0","StdMergedPi0","StdNoPIDsPions", "StdNoPIDsKaons"]
StripSelD2KPiPi0Chi2NOMWPres.DecayDescriptor = "[D0 -> K- pi+ pi0]cc"
StripSelD2KPiPi0Chi2NOMWPres.DaughtersCuts = { "K-"        : Daughtercut + "& (PT>400*MeV)",
                                               "pi+"        : Daughtercut + "& (PT>400*MeV)",
                                               "pi0": "(PT>800*MeV) & (P>4500*MeV)"}
StripSelD2KPiPi0Chi2NOMWPres.CombinationCut =  "(APT>2000*MeV)"
StripSelD2KPiPi0Chi2NOMWPres.MotherCut = Dcut+" & (BPVDIRA>0) & (P>3000*MeV)"

# D->hhhhh
StrippingSelD2hhhhChi2NOMWPres = CombineParticles("StrippingSelD2hhhhChi2NOMWPres")
StrippingSelD2hhhhChi2NOMWPres.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD2hhhhChi2NOMWPres.DecayDescriptors  = ["[D0 -> K- pi+ pi- pi+]cc","[D0 -> K+ pi- pi- pi+]cc","[D0 -> K+ K- pi+ pi-]cc"]
StrippingSelD2hhhhChi2NOMWPres.DaughtersCuts = { "K+"        : Daughtercut+ "& (PT>400*MeV)","pi+"        : Daughtercut+ "& (PT>400*MeV)"}
StrippingSelD2hhhhChi2NOMWPres.CombinationCut =  Dcombcut
StrippingSelD2hhhhChi2NOMWPres.MotherCut = Dcut+" & (BPVDIRA>0) & (P>3000*MeV)"

#Kshorts
KsDDChi2NOMWPres = CombineParticles("KsDDChi2NOMWPres")
KsDDChi2NOMWPres.InputLocations = [ "StdNoPIDsDownPions" ]
KsDDChi2NOMWPres.DecayDescriptor = "KS0 -> pi+ pi-"
KsDDChi2NOMWPres.DaughtersCuts = { "pi+" : Daughtercut }
KsDDChi2NOMWPres.CombinationCut = "(ADAMASS('KS0')<50*MeV)"
KsDDChi2NOMWPres.MotherCut = "(ADMASS('KS0')<30.0*MeV) & (VFASPF(VCHI2/VDOF)<16.0)"
KsLLChi2NOMWPres = KsDDChi2NOMWPres.clone("KsLLChi2NOMWPres")
KsLLChi2NOMWPres.InputLocations = [ "StdNoPIDsPions" ]
SubKshortsChi2NOMWPres = GaudiSequencer("SubKshortsChi2NOMWPres")
SubKshortsChi2NOMWPres.ModeOR = 1
SubKshortsChi2NOMWPres.IgnoreFilterPassed = True
SubKshortsChi2NOMWPres.Members += [KsDDChi2NOMWPres]
SubKshortsChi2NOMWPres.Members += [KsLLChi2NOMWPres]

#D->kspipi
StrippingSelD2kshhChi2NOMWPres = CombineParticles("StrippingSelD2kshhChi2NOMWPres")
StrippingSelD2kshhChi2NOMWPres.InputLocations = ["KsDDChi2NOMWPres", "KsLLChi2NOMWPres","StdNoPIDsPions","StdNoPIDsKaons"]
StrippingSelD2kshhChi2NOMWPres.DecayDescriptors = ["[D0 -> KS0 pi+ pi-]cc","[D0 -> KS0 K+ K-]cc","[D0 -> KS0 K+ pi-]cc","[D0 -> KS0 pi+ K-]cc"]
StrippingSelD2kshhChi2NOMWPres.DaughtersCuts = { "pi+" : Daughtercut,"K+" : Daughtercut, "KS0" : "ALL" }
StrippingSelD2kshhChi2NOMWPres.CombinationCut = Dcombcut
StrippingSelD2kshhChi2NOMWPres.MotherCut = Dcut

# D as a or of all possible Ds
SubDChi2NOMWPres = GaudiSequencer("SubDChi2NOMWPres")
SubDChi2NOMWPres.ModeOR = 1
SubDChi2NOMWPres.IgnoreFilterPassed = True
SubDChi2NOMWPres.Members += [StrippingSelD0Chi2NOMWPres]
SubDChi2NOMWPres.Members += [StrippingSelD2hhhChi2NOMWPres]
SubDChi2NOMWPres.Members += [StripSelD2KPiPi0Chi2NOMWPres]
SubDChi2NOMWPres.Members += [StrippingSelD2hhhhChi2NOMWPres]
SubDChi2NOMWPres.Members += [StrippingSelD2kshhChi2NOMWPres]
########################################################################
B2DXStrippingSelChi2NOMWPres = CombineParticles("B2DXStrippingSelChi2NOMWPres")
B2DXStrippingSelChi2NOMWPres.InputLocations  = ["StrippingSelD0Chi2NOMWPres",
                                                "StrippingSelD2hhhChi2NOMWPres",
                                                "StripSelD2KPiPi0Chi2NOMWPres",
                                                "StrippingSelD2hhhhChi2NOMWPres",
                                                "StrippingSelD2kshhChi2NOMWPres",
                                                "StrippingSelKstarChi2NOMWPres",
                                                "StrippingSelPhiChi2NOMWPres",
                                                "StrippingBachelorChi2NOMWPres"]

B2DXStrippingSelChi2NOMWPres.DecayDescriptors  = ["[B+ -> D~0 K+]cc",
                                                  "[B+ -> D~0 pi+]cc",
                                                  "[B+ -> D0 K+]cc",
                                                  "[B+ -> D0 pi+]cc",
                                                  "[B0 -> D~0 K*(892)0]cc",
                                                  "[B0 -> D0 K*(892)0]cc",
                                                  "[B0 -> D~0 phi(1020)]cc",
                                                  "[B0 -> D0 phi(1020)]cc",
                                                  "[B0 -> D- pi+]cc",
                                                  "[B0 -> D- K+]cc"] 

B2DXStrippingSelChi2NOMWPres.DaughtersCuts = { "D+" : Dcut , "K*(892)0" : Kstarcut ,  "D0" : Dcut , "phi(1020)": Phicut, "pi+" : Bachelorcut, "K+" : Bachelorcut}
B2DXStrippingSelChi2NOMWPres.CombinationCut = Bcombcut
B2DXStrippingSelChi2NOMWPres.MotherCut = Bcut

########################################################################
# Define the GEC on the number of tracks, needed in order to control
# the time for the combinatorics (especially once we add the 4-body D states)
B2DXStrippingNumTracksGECChi2NOMWPres = VoidFilter('B2DXStrippingNumTracksGECChi2NOMWPres'
                                                    , Code = "TrSOURCE('Rec/Track/Best') >> (TrSIZE < 240 )"
                                                    )
########################################################################
B2DXStrippingSequenceChi2NOMWPres = GaudiSequencer("B2DXStrippingSequenceChi2NOMWPres")
B2DXStrippingSequenceChi2NOMWPres.Members += [SubKshortsChi2NOMWPres]
B2DXStrippingSequenceChi2NOMWPres.Members += [SubXChi2NOMWPres]
B2DXStrippingSequenceChi2NOMWPres.Members += [SubDChi2NOMWPres]
B2DXStrippingSequenceChi2NOMWPres.Members += [B2DXStrippingSelChi2NOMWPres]
########################################################################

# Create the lines for this selection
line1 = StrippingLine('B2DXChi2NOMWPres'
                      , prescale = 1.0
                      ,  algos = [ B2DXStrippingNumTracksGECChi2NOMWPres, B2DXStrippingSequenceChi2NOMWPres ]
                      , postscale = 1.0
                      )

