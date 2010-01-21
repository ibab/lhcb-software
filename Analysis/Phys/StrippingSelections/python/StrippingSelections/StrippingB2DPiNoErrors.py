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
StrippingSelKstar = CombineParticles("StrippingSelKstar")
StrippingSelKstar.InputLocations = ["StdNoPIDsPions","StdNoPIDsKaons"]
StrippingSelKstar.DecayDescriptors = ["[K*(892)0 -> K+ pi-]cc","[K*(892)0 -> pi+ pi-]cc"]
StrippingSelKstar.DaughtersCuts =  { "K+"        : Daughtercut,
                                     "pi+"        : Daughtercut }
StrippingSelKstar.CombinationCut = Dcombcut
StrippingSelKstar.MotherCut = KPhiMothercut
########################################################################
# the Phi
StrippingSelPhi = CombineParticles("StrippingSelPhi")
StrippingSelPhi.InputLocations = ["StdNoPIDsKaons"]
StrippingSelPhi.DecayDescriptor = "phi(1020) -> K+ K-"
StrippingSelPhi.DaughtersCuts =  { "K+"        : Daughtercut}
StrippingSelPhi.CombinationCut = Dcombcut
StrippingSelPhi.MotherCut = KPhiMothercut
########################################################################
#The bachelor
StrippingBachelor = FilterDesktop("StrippingBachelor")
StrippingBachelor.InputLocations  = [ "StdNoPIDsPions","StdNoPIDsKaons" ]
StrippingBachelor.Code = Bachelorcut
########################################################################
SubX = GaudiSequencer("SubX")
SubX.ModeOR = 1
SubX.IgnoreFilterPassed = True
SubX.Members += [StrippingSelKstar]
SubX.Members += [StrippingSelPhi]
SubX.Members += [StrippingBachelor]
#######################################################################
# All possible D decays starting from the D->hh
StrippingSelD0 = CombineParticles("StrippingSelD0")
StrippingSelD0.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD0.DecayDescriptors = ["[D0 -> K- pi+]cc","[D0 -> pi- pi+]cc","[D0 -> K- K+]cc","[D0 -> K+ pi-]cc"]
StrippingSelD0.DaughtersCuts =  { "K+"        : Daughtercut,"pi+"        : Daughtercut }
StrippingSelD0.CombinationCut = Dcombcut
StrippingSelD0.MotherCut = Dcut
#now the D ->hhh
StrippingSelD2hhh = CombineParticles("StrippingSelD2hhh")
StrippingSelD2hhh.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD2hhh.DecayDescriptors = ["[D+ -> K- pi+ pi+]cc","[D+ -> K- K+ pi+]cc","[D+ -> pi- pi+ pi+]cc","[D+ -> pi- pi+ K+]cc"]
StrippingSelD2hhh.DaughtersCuts =  { "K+"        : Daughtercut,"pi+"        : Daughtercut }
StrippingSelD2hhh.CombinationCut = Dcombcut
StrippingSelD2hhh.MotherCut = Dcut

# D->KPiPi0
StripSelD2KPiPi0 = CombineParticles("StripSelD2KPiPi0")
StripSelD2KPiPi0.InputLocations  = [ "StdLooseResolvedPi0","StdLooseMergedPi0","StdNoPIDsPions", "StdNoPIDsKaons"]
StripSelD2KPiPi0.DecayDescriptor = "[D0 -> K- pi+ pi0]cc"
StripSelD2KPiPi0.DaughtersCuts = { "K-"        : Daughtercut + "& (PT>400*MeV)",
                                   "pi+"        : Daughtercut + "& (PT>400*MeV)",
                                   "pi0": "(PT>800*MeV) & (P>4500*MeV)"}
StripSelD2KPiPi0.CombinationCut =  "(APT>2000*MeV)"
StripSelD2KPiPi0.MotherCut = Dcut+" & (BPVDIRA>0) & (P>3000*MeV)"

# D->hhhhh
StrippingSelD2hhhh = CombineParticles("StrippingSelD2hhhh")
StrippingSelD2hhhh.InputLocations = ["StdNoPIDsKaons", "StdNoPIDsPions"]
StrippingSelD2hhhh.DecayDescriptors  = ["[D0 -> K- pi+ pi- pi+]cc","[D0 -> K+ pi- pi- pi+]cc","[D0 -> K+ K- pi+ pi-]cc"]
StrippingSelD2hhhh.DaughtersCuts = { "K+"        : Daughtercut,"pi+"        : Daughtercut}
StrippingSelD2hhhh.CombinationCut =  Dcombcut
StrippingSelD2hhhh.MotherCut = Dcut+" & (BPVDIRA>0) & (P>3000*MeV)"

#Kshorts
LooseKsDD = CombineParticles("LooseKsDD")
LooseKsDD.InputLocations = [ "StdNoPIDsDownPions" ]
LooseKsDD.DecayDescriptor = "KS0 -> pi+ pi-"
LooseKsDD.DaughtersCuts = { "pi+" : Daughtercut }
LooseKsDD.CombinationCut = "(ADAMASS('KS0')<50*MeV)"
LooseKsDD.MotherCut = "(ADMASS('KS0')<30.0*MeV) & (VFASPF(VCHI2/VDOF)<16.0)& (BPVVD>1*mm)"
LooseKsLL = LooseKsDD.clone("LooseKsLL")
LooseKsLL.InputLocations = [ "StdNoPIDsPions" ]
SubKshorts = GaudiSequencer("SubKshorts")
SubKshorts.ModeOR = 1
SubKshorts.IgnoreFilterPassed = True
SubKshorts.Members += [LooseKsDD]
SubKshorts.Members += [LooseKsLL]

#D->kspipi
StrippingSelD2kshh = CombineParticles("StrippingSelD2kshh")
StrippingSelD2kshh.InputLocations = ["LooseKsDD", "LooseKsLL","StdNoPIDsPions","StdNoPIDsKaons"]
StrippingSelD2kshh.DecayDescriptors = ["[D0 -> KS0 pi+ pi-]cc","[D0 -> KS0 K+ K-]cc","[D0 -> KS0 K+ pi-]cc","[D0 -> KS0 pi+ K-]cc"]
StrippingSelD2kshh.DaughtersCuts = { "pi+" : Daughtercut,"K+" : Daughtercut, "KS0" : "ALL" }
StrippingSelD2kshh.CombinationCut = Dcombcut
StrippingSelD2kshh.MotherCut = Dcut

# D as a or of all possible Ds
SubD = GaudiSequencer("SubD")
SubD.ModeOR = 1
SubD.IgnoreFilterPassed = True
SubD.Members += [StrippingSelD0]
SubD.Members += [StrippingSelD2hhh]
SubD.Members += [StripSelD2KPiPi0]
SubD.Members += [StrippingSelD2hhhh]
SubD.Members += [StrippingSelD2kshh]
########################################################################
B2DXStrippingSel = CombineParticles("B2DXStrippingSel")
B2DXStrippingSel.InputLocations  = ["StrippingSelD0",
                                    "StrippingSelD2hhh",
                                    "StripSelD2KPiPi0",
                                    "StrippingSelD2hhhh",
                                    "StrippingSelD2kshh",
                                    "StrippingSelKstar",
                                    "StrippingSelPhi",
                                    "StrippingBachelor"]

B2DXStrippingSel.DecayDescriptors  = ["[B+ -> D~0 K+]cc",
                                      "[B+ -> D~0 pi+]cc",
                                      "[B+ -> D0 K+]cc",
                                      "[B+ -> D0 pi+]cc",
                                      "[B0 -> D~0 K*(892)0]cc",
                                      "[B0 -> D0 K*(892)0]cc",
                                      "[B0 -> D~0 phi(1020)]cc",
                                      "[B0 -> D0 phi(1020)]cc",
                                      "[B0 -> D- pi+]cc",
                                      "[B0 -> D- K+]cc"] 

B2DXStrippingSel.DaughtersCuts = { "D+" : Dcut , "K*(892)0" : Kstarcut ,  "D0" : Dcut , "phi(1020)": Phicut, "pi+" : Bachelorcut, "K+" : Bachelorcut}
B2DXStrippingSel.CombinationCut = Bcombcut
B2DXStrippingSel.MotherCut = Bcut
########################################################################
# Define the GEC on the number of tracks, needed in order to control
# the time for the combinatorics (especially once we add the 4-body D states)
B2DXStrippingNumTracksGEC = VoidFilter('B2DXStrippingNumTracksGEC'
                                  , Code = "TrSOURCE('Rec/Track/Best') >> (TrSIZE < 240 )"
                                  )
########################################################################
B2DXStrippingSequence = GaudiSequencer("B2DXStrippingSequence")
B2DXStrippingSequence.Members += [SubKshorts]
B2DXStrippingSequence.Members += [SubX]
B2DXStrippingSequence.Members += [SubD]
B2DXStrippingSequence.Members += [B2DXStrippingSel]
########################################################################
########################################################################
#Now we define a second sequence, for the signal box
#The signal box is in all the relevant masses: B, D, phi, and K*
#This is allowed since we are making all possible combinations in the decay descriptor
#(at least we think we are, if anything was forgotten it shouldn't have been...)
#For the B and the D the signal mass windows include both the B/Bs and D/Ds masses 
########################################################################
########################################################################
B2DXSignalFilter = FilterDesktop("B2DXSignalFilter")
B2DXSignalFilter.InputLocations  = [ "B2DXStrippingSel" ]
B2DXSignalFilter.Code = BSignalcombcut + "&" + "INTREE(((ABSID == 'D0') | (ABSID == 'D+')) & " + DSignalcut + ")" + "&" + "(INTREE((ABSID == 'K*(892)0') &" + KstarSignalcut + ") | INTREE((ABSID == 'phi(1020)') &" + PhiSignalcut + ") | INGENERATION(ABSID == 'K+',1) | INGENERATION(ABSID == 'pi+',1))"

B2DXSignalStrippingSequence = GaudiSequencer("B2DXSignalStrippingSequence")
B2DXSignalStrippingSequence.Members += [SubKshorts]
B2DXSignalStrippingSequence.Members += [SubX]
B2DXSignalStrippingSequence.Members += [SubD]
B2DXSignalStrippingSequence.Members += [B2DXStrippingSel]
B2DXSignalStrippingSequence.Members += [B2DXSignalFilter]
########################################################################

# Create the lines for this selection
line1 = StrippingLine('B2DX'
               , prescale = 1.0
               ,  algos = [ B2DXStrippingNumTracksGEC, B2DXStrippingSequence ]
               #,  algos = [ B2DXStrippingSequence ]
               , postscale = 0.05
               )

line2 = StrippingLine('B2DXSignal'
               , prescale = 1.
               #,  algos = [ B2DXSignalStrippingSequence ]
               ,  algos = [ B2DXStrippingNumTracksGEC, B2DXSignalStrippingSequence ]
               )
