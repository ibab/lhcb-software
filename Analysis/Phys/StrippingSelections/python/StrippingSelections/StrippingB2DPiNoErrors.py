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
# The selection is based only on cuts on variables without their errors. 
# This selection deliberately uses no RICH information, since the channels in
# the B->DX family are kinematically identical to each other and are the dominant
# non-combinatoric backgrounds to each other.  
#
# A GEC is included to filter out extremely noisy events, it requires
# the size of the Rec/Track/Best container to be less than 240.
#
# The selection is tuned on the L0xHLT1 sample of minbias, achieving 
# a reduction factor of ~3/1000 for MC09 mbias data. 
# The signal efficiency for the various channels ranges between 75 and 90%.
#
# Further rate control is provided by the "robust" IP cut, set by default to
# 100 microns (0.1mm) to correlate with what is done in the topological trigger.  
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
ipcut = "0.1" #robust IP cut used for rate control

Dcut = "((M > 1768.*MeV) & (M < 2068.*MeV) & (VFASPF(VCHI2/VDOF)<10) & (BPVVD>2*mm) & (BPVVDR>0.2*mm)) "
DSignalcut = "((M > 1840.*MeV) & (M < 1895.*MeV)) | ((M > 1944.*MeV ) & (M < 1994.*MeV))"

Dptcut = "PT>1*GeV"
Dcombcut = "(A" + Dptcut + ")"   #Common pt cut for D, K*, phi, second version to use
                                 #before the vertexing  
KPhiMothercut = "((VFASPF(VCHI2/VDOF) < 12) & (MIPDV(PRIMARY) >"+ipcut+"*mm))"

Kstarcut = "(M > 0)"
KstarSignalcut = "(M > 0)" #Removed K* mass cuts for B->DKpi Dalitz analysis 
                           #funny coding for now, clean all this up 
                           #this is also why we cannot use any common Kstar 

Phicut = "(M > 0)"
PhiSignalcut = "(M > 0)"

Bcut = "((VFASPF(VCHI2/VDOF)<10)  & (BPVIP() < 0.06*mm) & (BPVVD > 2*mm) & (BPVDIRA > 0.9995))"
Bcombcut = "((AM > 4800.*MeV) & (AM < 5900.*MeV))"
BSignalcombcut = "((M > 5229.*MeV) & (M < 5329.*MeV)) | ((M > 5316.*MeV ) & (M < 5416.*MeV))"

Daughtercut = "((PT >250*MeV) & (P > 2*GeV) & (MIPDV(PRIMARY)>"+ipcut+"*mm))"
Bachelorcut = "((PT >400*MeV) & (P > 2*GeV) & (MIPDV(PRIMARY)>"+ipcut+"*mm))"
PhiDaughterCut = "( 2 == NINTREE( (ABSID=='K+') & (" + Daughtercut + ")))" 
########################################################################
########################################################################
#Now the various selections etc.
########################################################################
########################################################################
# the K*
StrippingSelKstarNoErrors = CombineParticles("StrippingSelKstarNoErrors")
StrippingSelKstarNoErrors.InputLocations = ["StdNoPIDsPions","StdNoPIDsKaons"]
StrippingSelKstarNoErrors.DecayDescriptors = ["[K*(892)0 -> K+ pi-]cc","[K*(892)0 -> pi+ pi-]cc"]
StrippingSelKstarNoErrors.DaughtersCuts =  { "K+"        : Daughtercut,
                                     "pi+"        : Daughtercut }
StrippingSelKstarNoErrors.CombinationCut = Dcombcut
StrippingSelKstarNoErrors.MotherCut = KPhiMothercut
########################################################################
# the Phi
StrippingSelPhiNoErrors = CombineParticles("StrippingSelPhiNoErrors")
StrippingSelPhiNoErrors.InputLocations = ["StdNoPIDsKaons"]
StrippingSelPhiNoErrors.DecayDescriptor = "phi(1020) -> K+ K-"
StrippingSelPhiNoErrors.DaughtersCuts =  { "K+"        : Daughtercut}
StrippingSelPhiNoErrors.CombinationCut = Dcombcut
StrippingSelPhiNoErrors.MotherCut = KPhiMothercut
########################################################################
#The bachelor
StrippingBachelorNoErrors = FilterDesktop("StrippingBachelorNoErrors")
StrippingBachelorNoErrors.InputLocations  = [ "StdNoPIDsPions","StdNoPIDsKaons" ]
StrippingBachelorNoErrors.Code = Bachelorcut
########################################################################
SubXNoErrors = GaudiSequencer("SubXNoErrors")
SubXNoErrors.ModeOR = 1
SubXNoErrors.IgnoreFilterPassed = True
SubXNoErrors.Members += [StrippingSelKstarNoErrors]
SubXNoErrors.Members += [StrippingSelPhiNoErrors]
SubXNoErrors.Members += [StrippingBachelorNoErrors]
#######################################################################
# All possible D decays starting from the D->hh
StrippingSelD0NoErrors = CombineParticles("StrippingSelD0NoErrors")
StrippingSelD0NoErrors.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD0NoErrors.DecayDescriptors = ["[D0 -> K- pi+]cc","[D0 -> pi- pi+]cc","[D0 -> K- K+]cc","[D0 -> K+ pi-]cc"]
StrippingSelD0NoErrors.DaughtersCuts =  { "K+"        : Daughtercut,"pi+"        : Daughtercut }
StrippingSelD0NoErrors.CombinationCut = Dcombcut
StrippingSelD0NoErrors.MotherCut = Dcut
#now the D ->hhh
StrippingSelD2hhhNoErrors = CombineParticles("StrippingSelD2hhhNoErrors")
StrippingSelD2hhhNoErrors.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD2hhhNoErrors.DecayDescriptors = ["[D+ -> K- pi+ pi+]cc","[D+ -> K- K+ pi+]cc","[D+ -> pi- pi+ pi+]cc","[D+ -> pi- pi+ K+]cc"]
StrippingSelD2hhhNoErrors.DaughtersCuts =  { "K+"        : Daughtercut,"pi+"        : Daughtercut }
StrippingSelD2hhhNoErrors.CombinationCut = Dcombcut
StrippingSelD2hhhNoErrors.MotherCut = Dcut

# D->KPiPi0
StripSelD2KPiPi0NoErrors = CombineParticles("StripSelD2KPiPi0NoErrors")
StripSelD2KPiPi0NoErrors.InputLocations  = [ "StdLooseResolvedPi0","StdLooseMergedPi0","StdNoPIDsPions", "StdNoPIDsKaons"]
StripSelD2KPiPi0NoErrors.DecayDescriptor = "[D0 -> K- pi+ pi0]cc"
StripSelD2KPiPi0NoErrors.DaughtersCuts = { "K-"        : Daughtercut + "& (PT>400*MeV)",
                                           "pi+"        : Daughtercut + "& (PT>400*MeV)",
                                           "pi0": "(PT>800*MeV) & (P>4500*MeV)"}
StripSelD2KPiPi0NoErrors.CombinationCut =  "(APT>2000*MeV)"
StripSelD2KPiPi0NoErrors.MotherCut = Dcut+" & (BPVDIRA>0) & (P>3000*MeV)"

# D->hhhhh
StrippingSelD2hhhhNoErrors = CombineParticles("StrippingSelD2hhhhNoErrors")
StrippingSelD2hhhhNoErrors.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD2hhhhNoErrors.DecayDescriptors  = ["[D0 -> K- pi+ pi- pi+]cc","[D0 -> K+ pi- pi- pi+]cc","[D0 -> K+ K- pi+ pi-]cc"]
StrippingSelD2hhhhNoErrors.DaughtersCuts = { "K+"        : Daughtercut+ "& (PT>400*MeV)","pi+"        : Daughtercut+ "& (PT>400*MeV)"}
StrippingSelD2hhhhNoErrors.CombinationCut =  Dcombcut
StrippingSelD2hhhhNoErrors.MotherCut = Dcut+" & (BPVDIRA>0) & (P>3000*MeV)"

#Kshorts
LooseKsDDNoErrors = CombineParticles("LooseKsDDNoErrors")
LooseKsDDNoErrors.InputLocations = [ "StdNoPIDsDownPions" ]
LooseKsDDNoErrors.DecayDescriptor = "KS0 -> pi+ pi-"
LooseKsDDNoErrors.DaughtersCuts = { "pi+" : Daughtercut }
LooseKsDDNoErrors.CombinationCut = "(ADAMASS('KS0')<50*MeV)"
LooseKsDDNoErrors.MotherCut = "(ADMASS('KS0')<30.0*MeV) & (VFASPF(VCHI2/VDOF)<16.0)& (BPVVD>1*mm)"
LooseKsLLNoErrors = LooseKsDDNoErrors.clone("LooseKsLLNoErrors")
LooseKsLLNoErrors.InputLocations = [ "StdNoPIDsPions" ]
SubKshortsNoErrors = GaudiSequencer("SubKshortsNoErrors")
SubKshortsNoErrors.ModeOR = 1
SubKshortsNoErrors.IgnoreFilterPassed = True
SubKshortsNoErrors.Members += [LooseKsDDNoErrors]
SubKshortsNoErrors.Members += [LooseKsLLNoErrors]

#D->kspipi
StrippingSelD2kshhNoErrors = CombineParticles("StrippingSelD2kshhNoErrors")
StrippingSelD2kshhNoErrors.InputLocations = ["LooseKsDDNoErrors", "LooseKsLLNoErrors","StdNoPIDsPions","StdNoPIDsKaons"]
StrippingSelD2kshhNoErrors.DecayDescriptors = ["[D0 -> KS0 pi+ pi-]cc","[D0 -> KS0 K+ K-]cc","[D0 -> KS0 K+ pi-]cc","[D0 -> KS0 pi+ K-]cc"]
StrippingSelD2kshhNoErrors.DaughtersCuts = { "pi+" : Daughtercut,"K+" : Daughtercut, "KS0" : "ALL" }
StrippingSelD2kshhNoErrors.CombinationCut = Dcombcut
StrippingSelD2kshhNoErrors.MotherCut = Dcut

# D as a or of all possible Ds
SubDNoErrors = GaudiSequencer("SubDNoErrors")
SubDNoErrors.ModeOR = 1
SubDNoErrors.IgnoreFilterPassed = True
SubDNoErrors.Members += [StrippingSelD0NoErrors]
SubDNoErrors.Members += [StrippingSelD2hhhNoErrors]
SubDNoErrors.Members += [StripSelD2KPiPi0NoErrors]
SubDNoErrors.Members += [StrippingSelD2hhhhNoErrors]
SubDNoErrors.Members += [StrippingSelD2kshhNoErrors]
########################################################################
B2DXStrippingSelNoErrors = CombineParticles("B2DXStrippingSelNoErrors")
B2DXStrippingSelNoErrors.InputLocations  = ["StrippingSelD0NoErrors",
                                            "StrippingSelD2hhhNoErrors",
                                            "StripSelD2KPiPi0NoErrors",
                                            "StrippingSelD2hhhhNoErrors",
                                            "StrippingSelD2kshhNoErrors",
                                            "StrippingSelKstarNoErrors",
                                            "StrippingSelPhiNoErrors",
                                            "StrippingBachelorNoErrors"]

B2DXStrippingSelNoErrors.DecayDescriptors  = ["[B+ -> D~0 K+]cc",
                                              "[B+ -> D~0 pi+]cc",
                                              "[B+ -> D0 K+]cc",
                                              "[B+ -> D0 pi+]cc",
                                              "[B0 -> D~0 K*(892)0]cc",
                                              "[B0 -> D0 K*(892)0]cc",
                                              "[B0 -> D~0 phi(1020)]cc",
                                              "[B0 -> D0 phi(1020)]cc",
                                              "[B0 -> D- pi+]cc",
                                              "[B0 -> D- K+]cc"] 

B2DXStrippingSelNoErrors.DaughtersCuts = { "D+" : Dcut , "K*(892)0" : Kstarcut ,  "D0" : Dcut , "phi(1020)": Phicut, "pi+" : Bachelorcut, "K+" : Bachelorcut}
B2DXStrippingSelNoErrors.CombinationCut = Bcombcut
B2DXStrippingSelNoErrors.MotherCut = Bcut
########################################################################
# Define the GEC on the number of tracks, needed in order to control
# the time for the combinatorics (especially once we add the 4-body D states)
B2DXStrippingNumTracksGECNoErrors = VoidFilter('B2DXStrippingNumTracksGECNoErrors'
                                               , Code = "TrSOURCE('Rec/Track/Best') >> (TrSIZE < 240 )"
                                               )
########################################################################
B2DXStrippingSequenceNoErrors = GaudiSequencer("B2DXStrippingSequenceNoErrors")
B2DXStrippingSequenceNoErrors.Members += [SubKshortsNoErrors]
B2DXStrippingSequenceNoErrors.Members += [SubXNoErrors]
B2DXStrippingSequenceNoErrors.Members += [SubDNoErrors]
B2DXStrippingSequenceNoErrors.Members += [B2DXStrippingSelNoErrors]
########################################################################
########################################################################
#Now we define a second sequence, for the signal box
#The signal box is in all the relevant masses: B, D, phi, and K*
#This is allowed since we are making all possible combinations in the decay descriptor
#(at least we think we are, if anything was forgotten it shouldn't have been...)
#For the B and the D the signal mass windows include both the B/Bs and D/Ds masses 
########################################################################
########################################################################
B2DXSignalFilterNoErrors = FilterDesktop("B2DXSignalFilterNoErrors")
B2DXSignalFilterNoErrors.InputLocations  = [ "B2DXStrippingSelNoErrors" ]
B2DXSignalFilterNoErrors.Code = BSignalcombcut + "&" + "INTREE(((ABSID == 'D0') | (ABSID == 'D+')) & " + DSignalcut + ")" + "&" + "(INTREE((ABSID == 'K*(892)0') &" + KstarSignalcut + ") | INTREE((ABSID == 'phi(1020)') &" + PhiSignalcut + ") | INGENERATION(ABSID == 'K+',1) | INGENERATION(ABSID == 'pi+',1))"

B2DXSignalStrippingSequenceNoErrors = GaudiSequencer("B2DXSignalStrippingSequenceNoErrors")
B2DXSignalStrippingSequenceNoErrors.Members += [SubKshortsNoErrors]
B2DXSignalStrippingSequenceNoErrors.Members += [SubXNoErrors]
B2DXSignalStrippingSequenceNoErrors.Members += [SubDNoErrors]
B2DXSignalStrippingSequenceNoErrors.Members += [B2DXStrippingSelNoErrors]
B2DXSignalStrippingSequenceNoErrors.Members += [B2DXSignalFilterNoErrors]
########################################################################

# Create the lines for this selection
line1 = StrippingLine('B2DXNoErrors'
                      , prescale = 1.0
                      ,  algos = [ B2DXStrippingNumTracksGECNoErrors, B2DXStrippingSequenceNoErrors ]
                      #,  algos = [ B2DXStrippingSequenceNoErrors ]
                      , postscale = 0.05
                      )

line2 = StrippingLine('B2DXSignalNoErrors'
                      , prescale = 1.
                      #,  algos = [ B2DXSignalStrippingSequenceNoErrors ]
                      ,  algos = [ B2DXStrippingNumTracksGECNoErrors, B2DXSignalStrippingSequenceNoErrors ]
                      )
