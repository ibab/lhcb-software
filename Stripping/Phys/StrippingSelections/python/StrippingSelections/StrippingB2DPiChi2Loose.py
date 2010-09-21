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
# a reduction factor of 2/100 for MC09 mbias data. 
# The signal efficiency for the various channels ranges between 80 and 90%.
# 
# Will further update the selection in stages.
#
# Authors: J. Nardulli & V. Gligorov 

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
Dcut = "((M > 1768.*MeV) & (M < 2068.*MeV) & (VFASPF(VCHI2/VDOF)<12) & (BPVVDCHI2 > 4)) "
DSignalcut = "((M > 1840.*MeV) & (M < 1895.*MeV)) | ((M > 1944.*MeV ) & (M < 1994.*MeV))"

Dptcut = "PT>1*GeV"
Dcombcut = "(A" + Dptcut + ")"   #Common pt cut for D, K*, phi, second version to use
                                 #before the vertexing  
KPhiMothercut = "((VFASPF(VCHI2/VDOF) < 12) & (MIPCHI2DV(PRIMARY) > 4.))"

Kstarcut = "(M > 0)"
KstarSignalcut = "(M > 0)" #Removed K* mass cuts for B->DKpi Dalitz analysis 
                           #funny coding for now, clean all this up 
                           #this is also why we cannot use any common Kstar 

Phicut = "(M > 0)"
PhiSignalcut = "(M > 0)"

Bcut = "((VFASPF(VCHI2/VDOF)<12)  & (BPVIPCHI2() < 49) & (BPVVDCHI2 > 4)  & (BPVDIRA > 0.9995))"
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
StrippingSelKstarChi2Loose = CombineParticles("StrippingSelKstarChi2Loose")
StrippingSelKstarChi2Loose.InputLocations = ["StdNoPIDsPions","StdNoPIDsKaons"]
StrippingSelKstarChi2Loose.DecayDescriptors = ["[K*(892)0 -> K+ pi-]cc","[K*(892)0 -> pi+ pi-]cc"]
StrippingSelKstarChi2Loose.DaughtersCuts =  { "K+"        : Daughtercut,
                                         "pi+"        : Daughtercut }
StrippingSelKstarChi2Loose.CombinationCut = Dcombcut
StrippingSelKstarChi2Loose.MotherCut = KPhiMothercut
########################################################################
# the Phi
StrippingSelPhiChi2Loose = CombineParticles("StrippingSelPhiChi2Loose")
StrippingSelPhiChi2Loose.InputLocations = ["StdNoPIDsKaons"]
StrippingSelPhiChi2Loose.DecayDescriptor = "phi(1020) -> K+ K-"
StrippingSelPhiChi2Loose.DaughtersCuts =  { "K+"        : Daughtercut}
StrippingSelPhiChi2Loose.CombinationCut = Dcombcut
StrippingSelPhiChi2Loose.MotherCut = KPhiMothercut
########################################################################
#The bachelor
StrippingBachelorChi2Loose = FilterDesktop("StrippingBachelorChi2Loose")
StrippingBachelorChi2Loose.InputLocations  = [ "StdNoPIDsPions","StdNoPIDsKaons" ]
StrippingBachelorChi2Loose.Code = Bachelorcut
########################################################################
SubXChi2Loose = GaudiSequencer("SubXChi2Loose")
SubXChi2Loose.ModeOR = 1
SubXChi2Loose.IgnoreFilterPassed = True
SubXChi2Loose.Members += [StrippingSelKstarChi2Loose]
SubXChi2Loose.Members += [StrippingSelPhiChi2Loose]
SubXChi2Loose.Members += [StrippingBachelorChi2Loose]
#######################################################################
# All possible D decays starting from the D->hh
StrippingSelD0Chi2Loose = CombineParticles("StrippingSelD0Chi2Loose")
StrippingSelD0Chi2Loose.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD0Chi2Loose.DecayDescriptors = ["[D0 -> K- pi+]cc","[D0 -> pi- pi+]cc","[D0 -> K- K+]cc","[D0 -> K+ pi-]cc"]
StrippingSelD0Chi2Loose.DaughtersCuts =  { "K+"        : Daughtercut,"pi+"        : Daughtercut }
StrippingSelD0Chi2Loose.CombinationCut = Dcombcut
StrippingSelD0Chi2Loose.MotherCut = Dcut
#now the D ->hhh
StrippingSelD2hhhChi2Loose = CombineParticles("StrippingSelD2hhhChi2Loose")
StrippingSelD2hhhChi2Loose.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD2hhhChi2Loose.DecayDescriptors = ["[D+ -> K- pi+ pi+]cc","[D+ -> K- K+ pi+]cc","[D+ -> pi- pi+ pi+]cc","[D+ -> pi- pi+ K+]cc"]
StrippingSelD2hhhChi2Loose.DaughtersCuts =  { "K+"        : Daughtercut,"pi+"        : Daughtercut }
StrippingSelD2hhhChi2Loose.CombinationCut = Dcombcut
StrippingSelD2hhhChi2Loose.MotherCut = Dcut

# D->KPiPi0
StripSelD2KPiPi0Chi2Loose = CombineParticles("StripSelD2KPiPi0Chi2Loose")
StripSelD2KPiPi0Chi2Loose.InputLocations  = [ "StdLooseResolvedPi0","StdLooseMergedPi0","StdNoPIDsPions", "StdNoPIDsKaons"]
StripSelD2KPiPi0Chi2Loose.DecayDescriptor = "[D0 -> K- pi+ pi0]cc"
StripSelD2KPiPi0Chi2Loose.DaughtersCuts = { "K-"        : Daughtercut + "& (PT>400*MeV)",
                                   "pi+"        : Daughtercut + "& (PT>400*MeV)",
                                   "pi0": "(PT>800*MeV) & (P>4500*MeV)"}
StripSelD2KPiPi0Chi2Loose.CombinationCut =  "(APT>2000*MeV)"
StripSelD2KPiPi0Chi2Loose.MotherCut = Dcut+" & (BPVDIRA>0) & (P>3000*MeV)"

# D->hhhhh
StrippingSelD2hhhhChi2Loose = CombineParticles("StrippingSelD2hhhhChi2Loose")
StrippingSelD2hhhhChi2Loose.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD2hhhhChi2Loose.DecayDescriptors  = ["[D0 -> K- pi+ pi- pi+]cc","[D0 -> K+ pi- pi- pi+]cc","[D0 -> K+ K- pi+ pi-]cc"]
StrippingSelD2hhhhChi2Loose.DaughtersCuts = { "K+"        : Daughtercut+ "& (PT>400*MeV)","pi+"        : Daughtercut+ "& (PT>400*MeV)"}
StrippingSelD2hhhhChi2Loose.CombinationCut =  Dcombcut
StrippingSelD2hhhhChi2Loose.MotherCut = Dcut+" & (BPVDIRA>0) & (P>3000*MeV)"

#Kshorts
KsDDChi2Loose = CombineParticles("KsDDChi2Loose")
KsDDChi2Loose.InputLocations = [ "StdNoPIDsDownPions" ]
KsDDChi2Loose.DecayDescriptor = "KS0 -> pi+ pi-"
KsDDChi2Loose.DaughtersCuts = { "pi+" : Daughtercut }
KsDDChi2Loose.CombinationCut = "(ADAMASS('KS0')<50*MeV)"
KsDDChi2Loose.MotherCut = "(ADMASS('KS0')<30.0*MeV) & (VFASPF(VCHI2/VDOF)<16.0)"
KsLLChi2Loose = KsDDChi2Loose.clone("KsLLChi2Loose")
KsLLChi2Loose.InputLocations = [ "StdNoPIDsPions" ]
SubKshortsChi2Loose = GaudiSequencer("SubKshortsChi2Loose")
SubKshortsChi2Loose.ModeOR = 1
SubKshortsChi2Loose.IgnoreFilterPassed = True
SubKshortsChi2Loose.Members += [KsDDChi2Loose]
SubKshortsChi2Loose.Members += [KsLLChi2Loose]

#D->kspipi
StrippingSelD2kshhChi2Loose = CombineParticles("StrippingSelD2kshhChi2Loose")
StrippingSelD2kshhChi2Loose.InputLocations = ["KsDDChi2Loose", "KsLLChi2Loose","StdNoPIDsPions","StdNoPIDsKaons"]
StrippingSelD2kshhChi2Loose.DecayDescriptors = ["[D0 -> KS0 pi+ pi-]cc","[D0 -> KS0 K+ K-]cc","[D0 -> KS0 K+ pi-]cc","[D0 -> KS0 pi+ K-]cc"]
StrippingSelD2kshhChi2Loose.DaughtersCuts = { "pi+" : Daughtercut,"K+" : Daughtercut, "KS0" : "ALL" }
StrippingSelD2kshhChi2Loose.CombinationCut = Dcombcut
StrippingSelD2kshhChi2Loose.MotherCut = Dcut

# D as a or of all possible Ds
SubDChi2Loose = GaudiSequencer("SubDChi2Loose")
SubDChi2Loose.ModeOR = 1
SubDChi2Loose.IgnoreFilterPassed = True
SubDChi2Loose.Members += [StrippingSelD0Chi2Loose]
SubDChi2Loose.Members += [StrippingSelD2hhhChi2Loose]
SubDChi2Loose.Members += [StripSelD2KPiPi0Chi2Loose]
SubDChi2Loose.Members += [StrippingSelD2hhhhChi2Loose]
SubDChi2Loose.Members += [StrippingSelD2kshhChi2Loose]
########################################################################
B2DXStrippingSelChi2Loose = CombineParticles("B2DXStrippingSelChi2Loose")
B2DXStrippingSelChi2Loose.InputLocations  = ["StrippingSelD0Chi2Loose",
                                        "StrippingSelD2hhhChi2Loose",
                                        "StripSelD2KPiPi0Chi2Loose",
                                        "StrippingSelD2hhhhChi2Loose",
                                        "StrippingSelD2kshhChi2Loose",
                                        "StrippingSelKstarChi2Loose",
                                        "StrippingSelPhiChi2Loose",
                                        "StrippingBachelorChi2Loose"]

B2DXStrippingSelChi2Loose.DecayDescriptors  = ["[B+ -> D~0 K+]cc",
                                          "[B+ -> D~0 pi+]cc",
                                          "[B+ -> D0 K+]cc",
                                          "[B+ -> D0 pi+]cc",
                                          "[B0 -> D~0 K*(892)0]cc",
                                          "[B0 -> D0 K*(892)0]cc",
                                          "[B0 -> D~0 phi(1020)]cc",
                                          "[B0 -> D0 phi(1020)]cc",
                                          "[B0 -> D- pi+]cc",
                                          "[B0 -> D- K+]cc"] 

B2DXStrippingSelChi2Loose.DaughtersCuts = { "D+" : Dcut , "K*(892)0" : Kstarcut ,  "D0" : Dcut , "phi(1020)": Phicut, "pi+" : Bachelorcut, "K+" : Bachelorcut}
B2DXStrippingSelChi2Loose.CombinationCut = Bcombcut
B2DXStrippingSelChi2Loose.MotherCut = Bcut
########################################################################
# Define the GEC on the number of tracks, needed in order to control
# the time for the combinatorics (especially once we add the 4-body D states)
B2DXStrippingNumTracksGECChi2Loose = VoidFilter('B2DXStrippingNumTracksGECChi2Loose'
                                           , Code = "TrSOURCE('Rec/Track/Best') >> (TrSIZE < 240 )"
                                           )
########################################################################
B2DXStrippingSequenceChi2Loose = GaudiSequencer("B2DXStrippingSequenceChi2Loose")
B2DXStrippingSequenceChi2Loose.Members += [SubKshortsChi2Loose]
B2DXStrippingSequenceChi2Loose.Members += [SubXChi2Loose]
B2DXStrippingSequenceChi2Loose.Members += [SubDChi2Loose]
B2DXStrippingSequenceChi2Loose.Members += [B2DXStrippingSelChi2Loose]
########################################################################
########################################################################
#Now we define a second sequence, for the signal box
#The signal box is in all the relevant masses: B, D, phi, and K*
#This is allowed since we are making all possible combinations in the decay descriptor
#(at least we think we are, if anything was forgotten it shouldn't have been...)
#For the B and the D the signal mass windows include both the B/Bs and D/Ds masses 
########################################################################
########################################################################
B2DXSignalFilterChi2Loose = FilterDesktop("B2DXSignalFilterChi2Loose")
B2DXSignalFilterChi2Loose.InputLocations  = [ "B2DXStrippingSelChi2Loose" ]
B2DXSignalFilterChi2Loose.Code = BSignalcombcut + "&" + "INTREE(((ABSID == 'D0') | (ABSID == 'D+')) & " + DSignalcut + ")" + "&" + "(INTREE((ABSID == 'K*(892)0') &" + KstarSignalcut + ") | INTREE((ABSID == 'phi(1020)') &" + PhiSignalcut + ") | INGENERATION(ABSID == 'K+',1) | INGENERATION(ABSID == 'pi+',1))"

B2DXSignalStrippingSequenceChi2Loose = GaudiSequencer("B2DXSignalStrippingSequenceChi2Loose")
B2DXSignalStrippingSequenceChi2Loose.Members += [SubKshortsChi2Loose]
B2DXSignalStrippingSequenceChi2Loose.Members += [SubXChi2Loose]
B2DXSignalStrippingSequenceChi2Loose.Members += [SubDChi2Loose]
B2DXSignalStrippingSequenceChi2Loose.Members += [B2DXStrippingSelChi2Loose]
B2DXSignalStrippingSequenceChi2Loose.Members += [B2DXSignalFilterChi2Loose]
########################################################################

# Create the lines for this selection
line1 = StrippingLine('B2DXChi2Loose'
               , prescale = 1.0
               ,  algos = [ B2DXStrippingNumTracksGECChi2Loose, B2DXStrippingSequenceChi2Loose ]
               #,  algos = [ B2DXStrippingSequence ]
               , postscale = 0.05
               )

line2 = StrippingLine('B2DXChi2SignalLoose'
               , prescale = 1.
               #,  algos = [ B2DXSignalStrippingSequence ]
               ,  algos = [ B2DXStrippingNumTracksGECChi2Loose, B2DXSignalStrippingSequenceChi2Loose ]
               )
