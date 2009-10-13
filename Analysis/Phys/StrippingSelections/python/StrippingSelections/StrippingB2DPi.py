#
# B->DPi selection for stripping (consider as an example). 
# This is an example of a simple wrapper over user's own algorithm. 
# No StrippingMember functionality is used here. 
#
# Still no StrippingMember functionality now selects B->DX,
# where X=pi,K,K*,phi, and D=D+,D0,Ds. The allowed decay modes
# of the intermediates are K*->Kpi, phi->KK, D+->hhh (h=pi,K all combinations except KKK),
# Ds->hhh (h=pi,K all combinations except KKK), D0->hh (h=pi,K all combinations).
# The four body D0 decay modes are not in included for now but will be
# included eventually. The selection is based on a combination of loose chi2 cuts
# on the impact paramters and flight significances (wrt. the offline selections),
# and some additional "robust" cuts which correlate with what is used in the trigger.
# Note that since the HLT2 topological selection for these modes will use Chi2
# cuts on the impact parameters and flight distances, there is no real motivation
# for not including these cuts in the stripping, but they are nonetheless kept
# loose.
#
# This selection deliberately uses no RICH information, since the channels in
# the B->DX family are kinematically identical to each other and are the dominant
# non-combinatoric backgrounds to each other.  
#
# A GEC is included to filter out extremely noisy events, it requires
# the size of the Rec/Track/Best container to be less than 240.
#
# The selection is tuned on the L0xHLT1 sample of minbias, achieving a rate
# of 7+-0.7 Hz for a L0xHLT1 output rate of 2kHz. Approximately 15% of this rate is
# in the mass sidebands, with the remaining rate coming in the signal mass window.
# The rates for other scenarios can be obtained by scaling
# this number in an appropriate manner. Note that the expected signal rate after
# the full offline selection(s) for the B->DX family of channels is ~0.2 Hz. 
#
# Further rate control is provided by the "robust" IP cut, set by default to
# 50 microns (0.05mm) to correlate with what is done in the topological trigger.  
#
# The signal efficiency is tested on Bs->DsK and B0->D0K* channels. The combined
# signal efficiency is found to be ~90%, with half of the loss coming from the GEC
# and the other half from the "robust" cuts included to correlate with the trigger
# which are not used in the offline selection.     
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
ipcut = "0.05" #robust IP cut used for rate control

Dcut = "((M > 1768.*MeV) & (M < 2068.*MeV) & (BPVVDCHI2 > 36) & (BPVVD>1*mm) & (BPVVDR>0.2*mm)) "
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

Bcut = "((VFASPF(VCHI2/VDOF)<10)  & (BPVIPCHI2() < 36) & (BPVVDCHI2 > 4.) & (BPVDIRA > 0.9995))"
Bcombcut = "((AM > 4800.*MeV) & (AM < 5900.*MeV))"
BSignalcombcut = "((M > 5229.*MeV) & (M < 5329.*MeV)) | ((M > 5316.*MeV ) & (M < 5416.*MeV))"

Daughtercut = "((PT >250*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 4.) & (MIPDV(PRIMARY)>"+ipcut+"*mm))"
Bachelorcut = "((PT >400*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 4.) & (MIPDV(PRIMARY)>"+ipcut+"*mm))"
PhiDaughterCut = "( 2 == NINTREE( (ABSID=='K+') & (" + Daughtercut + ")))" 
########################################################################
########################################################################
#Now the various selections etc.
########################################################################
########################################################################
# the K*
StrippingSelKstar = CombineParticles("StrippingSelKstar")
StrippingSelKstar.InputLocations = ["StdNoPIDsPions","StdNoPIDsKaons"]
StrippingSelKstar.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc"
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
########################################################################
B2DXStrippingSel = CombineParticles("B2DXStrippingSel")
B2DXStrippingSel.InputLocations  = [ "StdLooseDplus2KKPi",
                                     "StdLooseDplus2KPiPi",
                                     "StdLooseDplus2PiPiPi",
                                     "StdLooseDplus2KPiPiOppSignPi",
                                     "StdLooseD02KPi",
                                     "StdLooseD02KK",
                                     "StdLooseD02PiPi",
                                     "StdLooseD02KPiDCS",
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
B2DXStrippingSequence.Members += [SubX]
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
B2DXSignalStrippingSequence.Members += [SubX]
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
