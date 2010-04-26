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
# a reduction factor of 5/1000 for MC09 mbias data. 
# The signal efficiency for the various channels ranges between 80 and 95%.
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
Dcut = "((M > 1768.*MeV) & (M < 2068.*MeV) & (VFASPF(VCHI2/VDOF)<10) & (BPVVDCHI2 > 36)) "
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

Bcut = "((VFASPF(VCHI2/VDOF)<10)  & (BPVIPCHI2() < 36) & (BPVVDCHI2 > 144)  & (BPVDIRA > 0.9995))"
Bcombcut = "((AM > 4800.*MeV) & (AM < 5900.*MeV))"
BSignalcombcut = "((M > 5229.*MeV) & (M < 5329.*MeV)) | ((M > 5316.*MeV ) & (M < 5416.*MeV))"

Daughtercut = "((TRCHI2DOF<10) & (PT >250*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 4.))"
Bachelorcut = "((TRCHI2DOF<10) & (PT >400*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 4.))"
PhiDaughterCut = "( 2 == NINTREE( (ABSID=='K+') & (" + Daughtercut + ")))" 
########################################################################
########################################################################
#Now the various selections etc.
########################################################################
########################################################################
# the K*
StrippingSelKstarChi2 = CombineParticles("StrippingSelKstarChi2")
StrippingSelKstarChi2.InputLocations = ["StdNoPIDsPions","StdNoPIDsKaons"]
StrippingSelKstarChi2.DecayDescriptors = ["[K*(892)0 -> K+ pi-]cc","[K*(892)0 -> pi+ pi-]cc"]
StrippingSelKstarChi2.DaughtersCuts =  { "K+"        : Daughtercut,
                                         "pi+"        : Daughtercut }
StrippingSelKstarChi2.CombinationCut = Dcombcut
StrippingSelKstarChi2.MotherCut = KPhiMothercut
########################################################################
# the Phi
StrippingSelPhiChi2 = CombineParticles("StrippingSelPhiChi2")
StrippingSelPhiChi2.InputLocations = ["StdNoPIDsKaons"]
StrippingSelPhiChi2.DecayDescriptor = "phi(1020) -> K+ K-"
StrippingSelPhiChi2.DaughtersCuts =  { "K+"        : Daughtercut}
StrippingSelPhiChi2.CombinationCut = Dcombcut
StrippingSelPhiChi2.MotherCut = KPhiMothercut
########################################################################
#The bachelor
StrippingBachelorChi2 = FilterDesktop("StrippingBachelorChi2")
StrippingBachelorChi2.InputLocations  = [ "StdNoPIDsPions","StdNoPIDsKaons" ]
StrippingBachelorChi2.Code = Bachelorcut
########################################################################
SubXChi2 = GaudiSequencer("SubXChi2")
SubXChi2.ModeOR = 1
SubXChi2.IgnoreFilterPassed = True
SubXChi2.Members += [StrippingSelKstarChi2]
SubXChi2.Members += [StrippingSelPhiChi2]
SubXChi2.Members += [StrippingBachelorChi2]
#######################################################################
# All possible D decays starting from the D->hh
StrippingSelD0Chi2 = CombineParticles("StrippingSelD0Chi2")
StrippingSelD0Chi2.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD0Chi2.DecayDescriptors = ["[D0 -> K- pi+]cc","[D0 -> pi- pi+]cc","[D0 -> K- K+]cc","[D0 -> K+ pi-]cc"]
StrippingSelD0Chi2.DaughtersCuts =  { "K+"        : Daughtercut,"pi+"        : Daughtercut }
StrippingSelD0Chi2.CombinationCut = Dcombcut
StrippingSelD0Chi2.MotherCut = Dcut
#now the D ->hhh
StrippingSelD2hhhChi2 = CombineParticles("StrippingSelD2hhhChi2")
StrippingSelD2hhhChi2.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD2hhhChi2.DecayDescriptors = ["[D+ -> K- pi+ pi+]cc","[D+ -> K- K+ pi+]cc","[D+ -> pi- pi+ pi+]cc","[D+ -> pi- pi+ K+]cc"]
StrippingSelD2hhhChi2.DaughtersCuts =  { "K+"        : Daughtercut,"pi+"        : Daughtercut }
StrippingSelD2hhhChi2.CombinationCut = Dcombcut
StrippingSelD2hhhChi2.MotherCut = Dcut

# D->KPiPi0
StripSelD2KPiPi0Chi2 = CombineParticles("StripSelD2KPiPi0Chi2")
StripSelD2KPiPi0Chi2.InputLocations  = [ "StdLooseResolvedPi0","StdLooseMergedPi0","StdNoPIDsPions", "StdNoPIDsKaons"]
StripSelD2KPiPi0Chi2.DecayDescriptor = "[D0 -> K- pi+ pi0]cc"
StripSelD2KPiPi0Chi2.DaughtersCuts = { "K-"        : Daughtercut + "& (PT>400*MeV)",
                                       "pi+"        : Daughtercut + "& (PT>400*MeV)",
                                       "pi0": "(PT>800*MeV) & (P>4500*MeV)"}
StripSelD2KPiPi0Chi2.CombinationCut =  "(APT>2000*MeV)"
StripSelD2KPiPi0Chi2.MotherCut = Dcut+" & (BPVDIRA>0) & (P>3000*MeV)"

# D->hhhhh
StrippingSelD2hhhhChi2 = CombineParticles("StrippingSelD2hhhhChi2")
StrippingSelD2hhhhChi2.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD2hhhhChi2.DecayDescriptors  = ["[D0 -> K- pi+ pi- pi+]cc","[D0 -> K+ pi- pi- pi+]cc","[D0 -> K+ K- pi+ pi-]cc"]
StrippingSelD2hhhhChi2.DaughtersCuts = { "K+"        : Daughtercut+ "& (PT>400*MeV)","pi+"        : Daughtercut+ "& (PT>400*MeV)"}
StrippingSelD2hhhhChi2.CombinationCut =  Dcombcut
StrippingSelD2hhhhChi2.MotherCut = Dcut+" & (BPVDIRA>0) & (P>3000*MeV)"

#Kshorts
KsDDChi2 = CombineParticles("KsDDChi2")
KsDDChi2.InputLocations = [ "StdNoPIDsDownPions" ]
KsDDChi2.DecayDescriptor = "KS0 -> pi+ pi-"
KsDDChi2.DaughtersCuts = { "pi+" : Daughtercut }
KsDDChi2.CombinationCut = "(ADAMASS('KS0')<50*MeV)"
KsDDChi2.MotherCut = "(ADMASS('KS0')<30.0*MeV) & (VFASPF(VCHI2/VDOF)<16.0)"
KsLLChi2 = KsDDChi2.clone("KsLLChi2")
KsLLChi2.InputLocations = [ "StdNoPIDsPions" ]
SubKshortsChi2 = GaudiSequencer("SubKshortsChi2")
SubKshortsChi2.ModeOR = 1
SubKshortsChi2.IgnoreFilterPassed = True
SubKshortsChi2.Members += [KsDDChi2]
SubKshortsChi2.Members += [KsLLChi2]

#D->kspipi
StrippingSelD2kshhChi2 = CombineParticles("StrippingSelD2kshhChi2")
StrippingSelD2kshhChi2.InputLocations = ["KsDDChi2", "KsLLChi2","StdNoPIDsPions","StdNoPIDsKaons"]
StrippingSelD2kshhChi2.DecayDescriptors = ["[D0 -> KS0 pi+ pi-]cc","[D0 -> KS0 K+ K-]cc","[D0 -> KS0 K+ pi-]cc","[D0 -> KS0 pi+ K-]cc"]
StrippingSelD2kshhChi2.DaughtersCuts = { "pi+" : Daughtercut,"K+" : Daughtercut, "KS0" : "ALL" }
StrippingSelD2kshhChi2.CombinationCut = Dcombcut
StrippingSelD2kshhChi2.MotherCut = Dcut

# D as a or of all possible Ds
SubDChi2 = GaudiSequencer("SubDChi2")
SubDChi2.ModeOR = 1
SubDChi2.IgnoreFilterPassed = True
SubDChi2.Members += [StrippingSelD0Chi2]
SubDChi2.Members += [StrippingSelD2hhhChi2]
SubDChi2.Members += [StripSelD2KPiPi0Chi2]
SubDChi2.Members += [StrippingSelD2hhhhChi2]
SubDChi2.Members += [StrippingSelD2kshhChi2]
########################################################################
B2DXStrippingSelChi2 = CombineParticles("B2DXStrippingSelChi2")
B2DXStrippingSelChi2.InputLocations  = ["StrippingSelD0Chi2",
                                        "StrippingSelD2hhhChi2",
                                        "StripSelD2KPiPi0Chi2",
                                        "StrippingSelD2hhhhChi2",
                                        "StrippingSelD2kshhChi2",
                                        "StrippingSelKstarChi2",
                                        "StrippingSelPhiChi2",
                                        "StrippingBachelorChi2"]

B2DXStrippingSelChi2.DecayDescriptors  = ["[B+ -> D~0 K+]cc",
                                          "[B+ -> D~0 pi+]cc",
                                          "[B+ -> D0 K+]cc",
                                          "[B+ -> D0 pi+]cc",
                                          "[B0 -> D~0 K*(892)0]cc",
                                          "[B0 -> D0 K*(892)0]cc",
                                          "[B0 -> D~0 phi(1020)]cc",
                                          "[B0 -> D0 phi(1020)]cc",
                                          "[B0 -> D- pi+]cc",
                                          "[B0 -> D- K+]cc"] 

B2DXStrippingSelChi2.DaughtersCuts = { "D+" : Dcut , "K*(892)0" : Kstarcut ,  "D0" : Dcut , "phi(1020)": Phicut, "pi+" : Bachelorcut, "K+" : Bachelorcut}
B2DXStrippingSelChi2.CombinationCut = Bcombcut
B2DXStrippingSelChi2.MotherCut = Bcut
########################################################################
# Define the GEC on the number of tracks, needed in order to control
# the time for the combinatorics (especially once we add the 4-body D states)
B2DXStrippingNumTracksGECChi2 = VoidFilter('B2DXStrippingNumTracksGECChi2'
                                           , Code = "TrSOURCE('Rec/Track/Best') >> (TrSIZE < 240 )"
                                           )
########################################################################
B2DXStrippingSequenceChi2 = GaudiSequencer("B2DXStrippingSequenceChi2")
B2DXStrippingSequenceChi2.Members += [SubKshortsChi2]
B2DXStrippingSequenceChi2.Members += [SubXChi2]
B2DXStrippingSequenceChi2.Members += [SubDChi2]
B2DXStrippingSequenceChi2.Members += [B2DXStrippingSelChi2]
########################################################################
########################################################################
#Now we define a second sequence, for the signal box
#The signal box is in all the relevant masses: B, D, phi, and K*
#This is allowed since we are making all possible combinations in the decay descriptor
#(at least we think we are, if anything was forgotten it shouldn't have been...)
#For the B and the D the signal mass windows include both the B/Bs and D/Ds masses 
########################################################################
########################################################################
B2DXSignalFilterChi2 = FilterDesktop("B2DXSignalFilterChi2")
B2DXSignalFilterChi2.InputLocations  = [ "B2DXStrippingSelChi2" ]
B2DXSignalFilterChi2.Code = BSignalcombcut + "&" + "INTREE(((ABSID == 'D0') | (ABSID == 'D+')) & " + DSignalcut + ")" + "&" + "(INTREE((ABSID == 'K*(892)0') &" + KstarSignalcut + ") | INTREE((ABSID == 'phi(1020)') &" + PhiSignalcut + ") | INGENERATION(ABSID == 'K+',1) | INGENERATION(ABSID == 'pi+',1))"

B2DXSignalStrippingSequenceChi2 = GaudiSequencer("B2DXSignalStrippingSequenceChi2")
B2DXSignalStrippingSequenceChi2.Members += [SubKshortsChi2]
B2DXSignalStrippingSequenceChi2.Members += [SubXChi2]
B2DXSignalStrippingSequenceChi2.Members += [SubDChi2]
B2DXSignalStrippingSequenceChi2.Members += [B2DXStrippingSelChi2]
B2DXSignalStrippingSequenceChi2.Members += [B2DXSignalFilterChi2]
########################################################################

# Create the lines for this selection
line1 = StrippingLine('B2DXChi2'
               , prescale = 1.0
               ,  algos = [ B2DXStrippingNumTracksGECChi2, B2DXStrippingSequenceChi2 ]
               #,  algos = [ B2DXStrippingSequence ]
               , postscale = 0.05
               )

line2 = StrippingLine('B2DXChi2Signal'
               , prescale = 1.
               #,  algos = [ B2DXSignalStrippingSequence ]
               ,  algos = [ B2DXStrippingNumTracksGECChi2, B2DXSignalStrippingSequenceChi2 ]
               )
