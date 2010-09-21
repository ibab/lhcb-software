#
# B->D3Pi selection for stripping, based on B-->DPi Stripping
#
# This selection deliberately uses no RICH information, since the channels in
# the B->D3H family are kinematically identical to each other and are the dominant
# non-combinatoric backgrounds to each other.  
#
# A GEC is included to filter out extremely noisy events, it requires
# the size of the Rec/Track/Best container to be less than 240.
#
# The selection is tuned on the L0xHLT1 sample of minbias, achieving a rate
# of 10+-1 Hz for a L0xHLT1 output rate of 10kHz.
#
# Authors: S. Blusk, adapted from B-->Dh selection from J. Nardulli & V. Gligorov 
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
# Create the a1(1260) and K1(1270)
########################################################################

StdLooseA12PiPiPi = CombineParticles("StdLooseA12PiPiPi")
SeqStdLooseA12PiPiPi = GaudiSequencer("SeqStdLooseA12PiPiPi")
GaudiSequencer("SeqStdLooseA12PiPiPi").Members += [StdLooseA12PiPiPi ] 
SeqStdLooseA12PiPiPi.IgnoreFilterPassed = TRUE

StdLooseA12PiPiPi.InputLocations = [ "StdLoosePions"]
StdLooseA12PiPiPi.DecayDescriptor = "[a_1(1260)+ -> pi+ pi- pi+]cc" 

StdLooseA12PiPiPi.DaughtersCuts = { "pi+" : "((PT>250*MeV) & (P>2*GeV) & (MIPCHI2DV(PRIMARY)>4.0) & (TRCHI2DOF<10))"}
# need a large window to have the Ds* in as well.
StdLooseA12PiPiPi.CombinationCut = "(ADAMASS('a_1(1260)-')<400*MeV) & (APT>0.8*GeV)"
StdLooseA12PiPiPi.MotherCut = "(VFASPF(VCHI2/VDOF)<10) & (MIPCHI2DV(PRIMARY)>9)"


StdLooseK12KPiPi = CombineParticles("StdLooseK12KPiPi")
SeqStdLooseK12KPiPi = GaudiSequencer("SeqStdLooseK12KPiPi")
SeqStdLooseK12KPiPi.Members += [StdLooseK12KPiPi ] 
SeqStdLooseK12KPiPi.IgnoreFilterPassed = TRUE

StdLooseK12KPiPi.InputLocations = [ "StdLoosePions", "StdLooseKaons"]
StdLooseK12KPiPi.DecayDescriptor = "[K_1(1270)+ -> K+ pi- pi+]cc" 

StdLooseK12KPiPi.DaughtersCuts = { "pi+" : "((PT>250*MeV) & (P>2*GeV) & (MIPCHI2DV(PRIMARY)>4) & (TRCHI2DOF<10))",
                                   "K+" : "((PT>250*MeV) & (P>2*GeV) & (MIPCHI2DV(PRIMARY)>4) & (TRCHI2DOF<10))"}
# need a large window to have the Ds* in as well.
StdLooseK12KPiPi.CombinationCut = "(ADAMASS('K_1(1270)-')<400*MeV) & (APT>0.8*GeV)"
StdLooseK12KPiPi.MotherCut = "(VFASPF(VCHI2/VDOF)<10) & (MIPCHI2DV(PRIMARY)>9)"


########################################################################
#Define the shared cuts
ipcut = "0.080" #robust IP cut used for rate control

Dcut = "((M > 1868.*MeV) & (M < 2068.*MeV) & (VFASPF(VCHI2/VDOF)<10) & (BPVVDCHI2 > 36) & (BPVVD>2*mm) & (BPVVDR>0.2*mm)) "
DSignalcut = "((M > 1944.*MeV ) & (M < 1994.*MeV))"

Dptcut = "PT>1.0*GeV"
Dcombcut = "(A" + Dptcut + ")"   #Common pt cut for D

Bcut = "((VFASPF(VCHI2/VDOF)<10)  & (BPVIPCHI2() < 36) & (BPVVDCHI2 > 100) & (BPVDIRA > 0.9999) & (BPVIP() < 0.1*mm) & (BPVVD > 2*mm) & (PT>1.5*GeV))"
#Bcut = "((VFASPF(VCHI2/VDOF)<10)  & (BPVIPCHI2() < 36) & (BPVVDCHI2 > 100) & (BPVDIRA > 0.9999) & (BPVIP() < 0.1*mm) & (BPVVD > 2*mm))"
Bcombcut = "((AM > 4900.*MeV) & (AM < 5900.*MeV))"
BSignalcombcut = "((M > 5270.*MeV ) & (M < 5470.*MeV))"

Bachelorcut = "((PT > 800*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 4.) & (MIPDV(PRIMARY)>"+ipcut+"*mm))"

########################################################################
#The bachelor
StrippingBachelor3H = FilterDesktop("StrippingBachelor3H")
StrippingBachelor3H.InputLocations  = [ "StdLooseA12PiPiPi","StdLooseK12KPiPi" ]
#StrippingBachelor3H.InputLocations  = [ "StdLooseK12KPiPi" ]
StrippingBachelor3H.Code = Bachelorcut
########################################################################
Sub3H = GaudiSequencer("Sub3H")
Sub3H.ModeOR = 1
Sub3H.IgnoreFilterPassed = True
Sub3H.Members += [StrippingBachelor3H]
########################################################################
B2D3HStrippingSel = CombineParticles("B2D3HStrippingSel")
B2D3HStrippingSel.InputLocations  = [ "StdLooseDplus2KKPi",
                                     "StdLooseDplus2KPiPiOppSignPi",
                                     "StdLooseDplus2PiPiPi",
                                     "StrippingBachelor3H"]

B2D3HStrippingSel.DecayDescriptors  = ["[B0 -> D- a_1(1260)+]cc",
                                      "[B0 -> D- K_1(1270)+]cc"] 

B2D3HStrippingSel.DaughtersCuts = { "D+" : Dcut ,  "a_1(1260)+" : Bachelorcut, "K_1(1270)+" : Bachelorcut}
B2D3HStrippingSel.CombinationCut = Bcombcut
B2D3HStrippingSel.MotherCut = Bcut
########################################################################
# Define the GEC on the number of tracks, needed in order to control
# the time for the combinatorics (especially once we add the 4-body D states)
B2D3HStrippingNumTracksGEC = VoidFilter('B2D3HStrippingNumTracksGEC'
                                  , Code = "TrSOURCE('Rec/Track/Best') >> (TrSIZE < 240 )"
                                  )
########################################################################
B2D3HStrippingSequence = GaudiSequencer("B2D3HStrippingSequence")
B2D3HStrippingSequence.Members += [SeqStdLooseA12PiPiPi]
B2D3HStrippingSequence.Members += [SeqStdLooseK12KPiPi]
B2D3HStrippingSequence.Members += [Sub3H]
B2D3HStrippingSequence.Members += [B2D3HStrippingSel]
########################################################################
########################################################################
#Now we define a second sequence, for the signal box
#The signal box is in all the relevant masses: B and D
#This is allowed since we are making all possible combinations in the decay descriptor
#For the B and the D the signal mass windows include both the B/Bs and D/Ds masses 
########################################################################
########################################################################
B2D3HSignalFilter = FilterDesktop("B2D3HSignalFilter")
B2D3HSignalFilter.InputLocations  = [ "B2D3HStrippingSel" ]
B2D3HSignalFilter.Code = BSignalcombcut + "&" + "INTREE( (ABSID == 'D+') & " + DSignalcut + ")"

B2D3HSignalStrippingSequence = GaudiSequencer("B2D3HSignalStrippingSequence")
B2D3HSignalStrippingSequence.Members += [SeqStdLooseA12PiPiPi]
B2D3HSignalStrippingSequence.Members += [SeqStdLooseK12KPiPi]
B2D3HSignalStrippingSequence.Members += [Sub3H]
B2D3HSignalStrippingSequence.Members += [B2D3HStrippingSel]
B2D3HSignalStrippingSequence.Members += [B2D3HSignalFilter]
########################################################################


#
# Create the lines for this selection
#

line1 = StrippingLine('B2D3H'
               , prescale = 1.0
               ,  algos = [ B2D3HStrippingNumTracksGEC, B2D3HStrippingSequence ]
               , postscale = 0.05
               )


line2 = StrippingLine('B2D3HSignal'
               , prescale = 1.
               #,  algos = [ B2D3HSignalStrippingSequence ]
               ,  algos = [ B2D3HStrippingNumTracksGEC, B2D3HSignalStrippingSequence ]
               )

