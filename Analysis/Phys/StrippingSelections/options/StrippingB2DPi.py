#
# B->DPi selection for stripping (consider as an example). 
# This is an example of a simple wrapper over user's own algorithm. 
# No StrippingMember functionality is used here. 
#
# Still no StrippingMember functionality but being updated to do B->DX.
# Will further update in stages.
# Authors: J. Nardulli & V. Gligorov 

from os import environ
from Gaudi.Configuration import *
from Configurables import DecayTreeTuple
from Configurables import CombineParticles, FilterDesktop
from Configurables import TupleToolMCBackgroundInfo, TupleToolMCTruth, BackgroundCategory
from StrippingConf.StrippingLine import StrippingLine

########################################################################
#Define the shared cuts
Dcut = "((M > 1768.*MeV) & (M < 2068.*MeV) & (BPVVDCHI2 > 36)) "
DSignalcut = "((M > 1840.*MeV) & (M < 1895.*MeV)) | ((M > 1944.*MeV ) & (M < 1994.*MeV))"
Dcombcut = "((APT > 1*GeV))"

KPhiMothercut = "((VFASPF(VCHI2/VDOF) < 12) & (MIPCHI2DV(PRIMARY) > 4.))"
Kstarcut = "(1==1)"
KstarSignalcut = "(1==1)" #Removed K* mass cuts for B->DKpi Dalitz analysis 
                          #funny coding for now, clean all this up 
Phicut = "(ADMASS('phi(1020)') < 100*MeV)"
PhiSignalcut = "(ADMASS('phi(1020)') < 12*MeV)"

Bcut = "((VFASPF(VCHI2/VDOF)<10)  & (BPVIPCHI2() < 36) & (BPVVDCHI2 > 4.) & (BPVDIRA > 0.9995) & (D2DVVDSIGN(1)>-3.0*mm))";
Bcombcut = "((AM > 4800.*MeV) & (AM < 5900.*MeV))"
BSignalcombcut = "((M > 5229.*MeV) & (M < 5329.*MeV)) | ((M > 5316.*MeV ) & (M < 5416.*MeV))"

Daughtercut = "((TRCHI2DOF<10) & (PT >250*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 4.))"
Bachelorcut = "((TRCHI2DOF<10) & (PT >350*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 4.))"
########################################################################
########################################################################
#Now the various selections etc.
########################################################################
########################################################################
# the K*
StrippingSelKstar = CombineParticles("StrippingSelKstar")
StrippingSelKstar.InputLocations = ["StdNoPIDsPions","StdNoPIDsKaons"]
StrippingSelKstar.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc"
StrippingSelKstar.DaughtersCuts =  { "K+"        : Daughtercut,"pi+"        : Daughtercut }
StrippingSelKstar.CombinationCut = Dcombcut
StrippingSelKstar.MotherCut = KPhiMothercut
########################################################################
# the Phi
StrippingSelPhi = CombineParticles("StrippingSelPhi")
StrippingSelPhi.InputLocations = ["StdNoPIDsKaons"]
StrippingSelPhi.DecayDescriptor = "[phi(1020) -> K+ K-]cc"
StrippingSelPhi.DaughtersCuts =  { "K+"        : Daughtercut }
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
line = StrippingLine('B2DX'
               , prescale = 0.01
               ,  algos = [ B2DXStrippingSequence ]
               ,  stream = 'BExclusive'
               )

line = StrippingLine('B2DXSignal'
               , prescale = 1.
               ,  algos = [ B2DXSignalStrippingSequence ]
               ,  stream = 'BExclusive'
               )
