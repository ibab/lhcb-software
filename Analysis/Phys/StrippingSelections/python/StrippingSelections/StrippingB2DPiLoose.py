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
# This is a loosed version of the B ->DX stripping to be used for early data
#
# A GEC is included to filter out extremely noisy events, it requires
# the size of the Rec/Track/Best container to be less than 240.
#
# The selection is tuned on the L0xHLT1 sample of minbias, achieving 
# a reduction factor of ~3/1000 for MC09 mbias data. 
# The signal efficiency for the various channels ranges between 75 and 90%.
#
# Authors: J. Nardulli
# 18-03-2010
#

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
ipcut = "0.05" #robust IP cut used for rate control

Dcut = "((M > 1768.*MeV) & (M < 2068.*MeV) & (VFASPF(VCHI2/VDOF)<12) & (BPVVDCHI2 > 4) & (BPVVD>1*mm) & (BPVVDR>0.2*mm)) "
DSignalcut = "((M > 1840.*MeV) & (M < 1895.*MeV)) | ((M > 1944.*MeV ) & (M < 1994.*MeV))"

Dptcut = "PT>1*GeV"
Dcombcut = "(A" + Dptcut + ")"   #Common pt cut for D, K*, phi, second version to use
                                 #before the vertexing  
KPhiMothercut = "((VFASPF(VCHI2/VDOF) < 12) & (MIPCHI2DV(PRIMARY) > 4.) & (MIPDV(PRIMARY) >"+ipcut+"*mm))"

Kstarcut = "(M > 0)"
KstarSignalcut = "(M > 0)" #Removed K* mass cuts for B->DKpi Dalitz analysis 
                           #funny coding for now, clean all this up 
                           #this is also why we cannot use any common Kstar 

Phicut = "(M > 0)"
PhiSignalcut = "(M > 0)"

Bcut = "((VFASPF(VCHI2/VDOF)<12)  & (BPVIPCHI2() < 49) & (BPVVDCHI2 > 4) & (BPVIP() < 0.07*mm) & (BPVVD > 1*mm) & (BPVDIRA > 0.9995))"
Bcombcut = "((AM > 4800.*MeV) & (AM < 5900.*MeV))"
BSignalcombcut = "((M > 5229.*MeV) & (M < 5329.*MeV)) | ((M > 5316.*MeV ) & (M < 5416.*MeV))"

Daughtercut = "((TRCHI2DOF<10) & (PT >250*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 4.) & (MIPDV(PRIMARY)>"+ipcut+"*mm))"
Bachelorcut = "((TRCHI2DOF<10) & (PT >350*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 4.) & (MIPDV(PRIMARY)>"+ipcut+"*mm))"
PhiDaughterCut = "( 2 == NINTREE( (ABSID=='K+') & (" + Daughtercut + ")))" 
########################################################################
########################################################################
#Now the various selections etc.
########################################################################
########################################################################
# the K*
StrippingSelKstarLoose = CombineParticles("StrippingSelKstarLoose")
StrippingSelKstarLoose.InputLocations = ["StdNoPIDsPions","StdNoPIDsKaons"]
StrippingSelKstarLoose.DecayDescriptors = ["[K*(892)0 -> K+ pi-]cc","[K*(892)0 -> pi+ pi-]cc"]
StrippingSelKstarLoose.DaughtersCuts =  { "K+"        : Daughtercut,
                                     "pi+"        : Daughtercut }
StrippingSelKstarLoose.CombinationCut = Dcombcut
StrippingSelKstarLoose.MotherCut = KPhiMothercut
########################################################################
# the Phi
StrippingSelPhiLoose = CombineParticles("StrippingSelPhiLoose")
StrippingSelPhiLoose.InputLocations = ["StdNoPIDsKaons"]
StrippingSelPhiLoose.DecayDescriptor = "phi(1020) -> K+ K-"
StrippingSelPhiLoose.DaughtersCuts =  { "K+"        : Daughtercut}
StrippingSelPhiLoose.CombinationCut = Dcombcut
StrippingSelPhiLoose.MotherCut = KPhiMothercut
########################################################################
#The bachelor
StrippingBachelorLoose = FilterDesktop("StrippingBachelorLoose")
StrippingBachelorLoose.InputLocations  = [ "StdNoPIDsPions","StdNoPIDsKaons" ]
StrippingBachelorLoose.Code = Bachelorcut
########################################################################
SubXLoose = GaudiSequencer("SubXLoose")
SubXLoose.ModeOR = 1
SubXLoose.IgnoreFilterPassed = True
SubXLoose.Members += [StrippingSelKstarLoose]
SubXLoose.Members += [StrippingSelPhiLoose]
SubXLoose.Members += [StrippingBachelorLoose]
#######################################################################
# All possible D decays starting from the D->hh
StrippingSelD0Loose = CombineParticles("StrippingSelD0Loose")
StrippingSelD0Loose.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD0Loose.DecayDescriptors = ["[D0 -> K- pi+]cc","[D0 -> pi- pi+]cc","[D0 -> K- K+]cc","[D0 -> K+ pi-]cc"]
StrippingSelD0Loose.DaughtersCuts =  { "K+"        : Daughtercut,"pi+"        : Daughtercut }
StrippingSelD0Loose.CombinationCut = Dcombcut
StrippingSelD0Loose.MotherCut = Dcut
#now the D ->hhh
StrippingSelD2hhhLoose = CombineParticles("StrippingSelD2hhhLoose")
StrippingSelD2hhhLoose.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD2hhhLoose.DecayDescriptors = ["[D+ -> K- pi+ pi+]cc","[D+ -> K- K+ pi+]cc","[D+ -> pi- pi+ pi+]cc","[D+ -> pi- pi+ K+]cc"]
StrippingSelD2hhhLoose.DaughtersCuts =  { "K+"        : Daughtercut,"pi+"        : Daughtercut }
StrippingSelD2hhhLoose.CombinationCut = Dcombcut
StrippingSelD2hhhLoose.MotherCut = Dcut

# D->KPiPi0
StripSelD2KPiPi0Loose = CombineParticles("StripSelD2KPiPi0Loose")
StripSelD2KPiPi0Loose.InputLocations  = [ "StdLooseResolvedPi0","StdLooseMergedPi0","StdNoPIDsPions", "StdNoPIDsKaons"]
StripSelD2KPiPi0Loose.DecayDescriptor = "[D0 -> K- pi+ pi0]cc"
StripSelD2KPiPi0Loose.DaughtersCuts = { "K-"        : Daughtercut + "& (PT>400*MeV)",
                                           "pi+"        : Daughtercut + "& (PT>400*MeV)",
                                           "pi0": "(PT>800*MeV) & (P>4500*MeV)"}
StripSelD2KPiPi0Loose.CombinationCut =  "(APT>2000*MeV)"
StripSelD2KPiPi0Loose.MotherCut = Dcut+" & (BPVDIRA>0) & (P>3000*MeV)"

# D->hhhhh
StrippingSelD2hhhhLoose = CombineParticles("StrippingSelD2hhhhLoose")
StrippingSelD2hhhhLoose.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD2hhhhLoose.DecayDescriptors  = ["[D0 -> K- pi+ pi- pi+]cc","[D0 -> K+ pi- pi- pi+]cc","[D0 -> K+ K- pi+ pi-]cc"]
StrippingSelD2hhhhLoose.DaughtersCuts = { "K+"        : Daughtercut+ "& (PT>400*MeV)","pi+"        : Daughtercut+ "& (PT>400*MeV)"}
StrippingSelD2hhhhLoose.CombinationCut =  Dcombcut
StrippingSelD2hhhhLoose.MotherCut = Dcut+" & (BPVDIRA>0) & (P>3000*MeV)"

#Kshorts
LooseKsDDLoose = CombineParticles("LooseKsDDLoose")
LooseKsDDLoose.InputLocations = [ "StdNoPIDsDownPions" ]
LooseKsDDLoose.DecayDescriptor = "KS0 -> pi+ pi-"
LooseKsDDLoose.DaughtersCuts = { "pi+" : Daughtercut }
LooseKsDDLoose.CombinationCut = "(ADAMASS('KS0')<50*MeV)"
LooseKsDDLoose.MotherCut = "(ADMASS('KS0')<30.0*MeV) & (VFASPF(VCHI2/VDOF)<16.0)& (BPVVD>1*mm)"
LooseKsLLLoose = LooseKsDDLoose.clone("LooseKsLLLoose")
LooseKsLLLoose.InputLocations = [ "StdNoPIDsPions" ]
SubKshortsLoose = GaudiSequencer("SubKshortsLoose")
SubKshortsLoose.ModeOR = 1
SubKshortsLoose.IgnoreFilterPassed = True
SubKshortsLoose.Members += [LooseKsDDLoose]
SubKshortsLoose.Members += [LooseKsLLLoose]

#D->kspipi
StrippingSelD2kshhLoose = CombineParticles("StrippingSelD2kshhLoose")
StrippingSelD2kshhLoose.InputLocations = ["LooseKsDDLoose", "LooseKsLLLoose","StdNoPIDsPions","StdNoPIDsKaons"]
StrippingSelD2kshhLoose.DecayDescriptors = ["[D0 -> KS0 pi+ pi-]cc","[D0 -> KS0 K+ K-]cc","[D0 -> KS0 K+ pi-]cc","[D0 -> KS0 pi+ K-]cc"]
StrippingSelD2kshhLoose.DaughtersCuts = { "pi+" : Daughtercut,"K+" : Daughtercut, "KS0" : "ALL" }
StrippingSelD2kshhLoose.CombinationCut = Dcombcut
StrippingSelD2kshhLoose.MotherCut = Dcut

# D as a or of all possible Ds
SubDLoose = GaudiSequencer("SubDLoose")
SubDLoose.ModeOR = 1
SubDLoose.IgnoreFilterPassed = True
SubDLoose.Members += [StrippingSelD0Loose]
SubDLoose.Members += [StrippingSelD2hhhLoose]
SubDLoose.Members += [StripSelD2KPiPi0Loose]
SubDLoose.Members += [StrippingSelD2hhhhLoose]
SubDLoose.Members += [StrippingSelD2kshhLoose]
########################################################################
B2DXStrippingSelLoose = CombineParticles("B2DXStrippingSelLoose")
B2DXStrippingSelLoose.InputLocations  = ["StrippingSelD0Loose",
                                         "StrippingSelD2hhhLoose",
                                         "StripSelD2KPiPi0Loose",
                                         "StrippingSelD2hhhhLoose",
                                         "StrippingSelD2kshhLoose",
                                         "StrippingSelKstarLoose",
                                         "StrippingSelPhiLoose",
                                         "StrippingBachelorLoose"]

B2DXStrippingSelLoose.DecayDescriptors  = ["[B+ -> D~0 K+]cc",
                                           "[B+ -> D~0 pi+]cc",
                                           "[B+ -> D0 K+]cc",
                                           "[B+ -> D0 pi+]cc",
                                           "[B0 -> D~0 K*(892)0]cc",
                                           "[B0 -> D0 K*(892)0]cc",
                                           "[B0 -> D~0 phi(1020)]cc",
                                           "[B0 -> D0 phi(1020)]cc",
                                           "[B0 -> D- pi+]cc",
                                           "[B0 -> D- K+]cc"] 

B2DXStrippingSelLoose.DaughtersCuts = { "D+" : Dcut , "K*(892)0" : Kstarcut ,  "D0" : Dcut , "phi(1020)": Phicut, "pi+" : Bachelorcut, "K+" : Bachelorcut}
B2DXStrippingSelLoose.CombinationCut = Bcombcut
B2DXStrippingSelLoose.MotherCut = Bcut
########################################################################
# Define the GEC on the number of tracks, needed in order to control
# the time for the combinatorics (especially once we add the 4-body D states)
B2DXStrippingNumTracksGECLoose = VoidFilter('B2DXStrippingNumTracksGECLoose'
                                               , Code = "TrSOURCE('Rec/Track/Best') >> (TrSIZE < 240 )"
                                               )
########################################################################
B2DXStrippingSequenceLoose = GaudiSequencer("B2DXStrippingSequenceLoose")
B2DXStrippingSequenceLoose.Members += [SubKshortsLoose]
B2DXStrippingSequenceLoose.Members += [SubXLoose]
B2DXStrippingSequenceLoose.Members += [SubDLoose]
B2DXStrippingSequenceLoose.Members += [B2DXStrippingSelLoose]
########################################################################
########################################################################
#Now we define a second sequence, for the signal box
#The signal box is in all the relevant masses: B, D, phi, and K*
#This is allowed since we are making all possible combinations in the decay descriptor
#(at least we think we are, if anything was forgotten it shouldn't have been...)
#For the B and the D the signal mass windows include both the B/Bs and D/Ds masses 
########################################################################
########################################################################
B2DXSignalFilterLoose = FilterDesktop("B2DXSignalFilterLoose")
B2DXSignalFilterLoose.InputLocations  = [ "B2DXStrippingSelLoose" ]
B2DXSignalFilterLoose.Code = BSignalcombcut + "&" + "INTREE(((ABSID == 'D0') | (ABSID == 'D+')) & " + DSignalcut + ")" + "&" + "(INTREE((ABSID == 'K*(892)0') &" + KstarSignalcut + ") | INTREE((ABSID == 'phi(1020)') &" + PhiSignalcut + ") | INGENERATION(ABSID == 'K+',1) | INGENERATION(ABSID == 'pi+',1))"

B2DXSignalStrippingSequenceLoose = GaudiSequencer("B2DXSignalStrippingSequenceLoose")
B2DXSignalStrippingSequenceLoose.Members += [SubKshortsLoose]
B2DXSignalStrippingSequenceLoose.Members += [SubXLoose]
B2DXSignalStrippingSequenceLoose.Members += [SubDLoose]
B2DXSignalStrippingSequenceLoose.Members += [B2DXStrippingSelLoose]
B2DXSignalStrippingSequenceLoose.Members += [B2DXSignalFilterLoose]
########################################################################

# Create the lines for this selection
line1 = StrippingLine('B2DXLoose'
                      , prescale = 1.0
                      ,  algos = [ B2DXStrippingNumTracksGECLoose, B2DXStrippingSequenceLoose ]
                      #,  algos = [ B2DXStrippingSequenceLoose ]
                      , postscale = 0.05
                      )

line2 = StrippingLine('B2DXSignalLoose'
                      , prescale = 1.
                      #,  algos = [ B2DXSignalStrippingSequenceLoose ]
                      ,  algos = [ B2DXStrippingNumTracksGECLoose, B2DXSignalStrippingSequenceLoose ]
                      )
