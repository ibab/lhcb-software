# The stripping selection for inclusive b->DMuX.
# The D includes D0 -> K-pi+, D+ -> K-pi+pi+, Ds+ -> K+K-pi+ and Lambda_c+ -> p+K-pi+.
# The stripping has two lines, one called robust cut (RB) and the other called nominal cut.
# The robust cut has retention rate 1*10^{-3} w.r.t. L0*HLT1, and the later 1.6*10^{-4}. 
# The robust cut requires very loose PID DLL(no RICH requirement), and no MIPCHI2/BPVVDCHI2
# but absolute MIP&BPVVD for D's. The nominal cut bases on the robust cut with tighter DLL, 
# and additional MIPCHI2& BPVVDCHI2 for D's.
# author: Liming Zhang (lzhang@phy.syr.edu)
# date: Dec/06/2009.

from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import  PhysDesktop, CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember

#the robust cut
RobustDaughtercut = "(PT > 350*MeV) & (P > 2.0*GeV) & (MIPDV(PRIMARY)>0.05*mm) & (TRCHI2DOF < 8.0)"
RobustDcut = "(BPVVD > 2.5*mm)"
SelKaon_forDMuXRB = FilterDesktop('SelKaon_forDMuXRB')
SelKaon_forDMuXRB.InputLocations = ['StdNoPIDsKaons']
SelKaon_forDMuXRB.Code = RobustDaughtercut + "& (PIDK>-5) "

SelPion_forDMuXRB = FilterDesktop('SelPion_forDMuXRB')
SelPion_forDMuXRB.InputLocations = ['StdNoPIDsPions']
SelPion_forDMuXRB.Code = RobustDaughtercut + "& (PIDK<10) "

D02KPi_forDMuXRB = CombineParticles( 'D02KPi_forDMuXRB' )
D02KPi_forDMuXRB.DecayDescriptor = '[D0 -> K- pi+]cc'
D02KPi_forDMuXRB.InputLocations = [ 'SelKaon_forDMuXRB', 'SelPion_forDMuXRB' ]
D02KPi_forDMuXRB.CombinationCut = "(ADAMASS('D0')<200*MeV) "
D02KPi_forDMuXRB.MotherCut = RobustDcut + " & (ADMASS('D0')<120*MeV) & (VFASPF(VCHI2/VDOF)<7.17) & (CHILD(PT,1)+CHILD(PT,2)>1700*MeV) "

Dp2KPiPi_forDMuXRB = CombineParticles( 'Dp2KPiPi_forDMuXRB' )
Dp2KPiPi_forDMuXRB.DecayDescriptor = '[D+ -> K- pi+ pi+]cc'
Dp2KPiPi_forDMuXRB.InputLocations = [ 'SelKaon_forDMuXRB', 'SelPion_forDMuXRB' ]
Dp2KPiPi_forDMuXRB.CombinationCut = "(ADAMASS('D+')<200*MeV)  "
Dp2KPiPi_forDMuXRB.MotherCut = RobustDcut + " & (ADMASS('D+')<120*MeV)  & (VFASPF(VCHI2/VDOF)<4.0) & (CHILD(PT,1)+CHILD(PT,2)+CHILD(PT,3)>1800*MeV)"

Ds2KKPi_forDMuXRB = CombineParticles( 'Ds2KKPi_forDMuXRB' )
Ds2KKPi_forDMuXRB.DecayDescriptor = '[D_s+ -> K- K+ pi+]cc'
Ds2KKPi_forDMuXRB.InputLocations = [ 'SelKaon_forDMuXRB', 'SelPion_forDMuXRB' ]
Ds2KKPi_forDMuXRB.CombinationCut = "(ADAMASS('D_s+')<250*MeV) "
Ds2KKPi_forDMuXRB.MotherCut = RobustDcut + " & (DMASS('D_s+')<120*MeV) & (DMASS('D_s+')>-220*MeV)  & (VFASPF(VCHI2/VDOF)<4.0) & (CHILD(PT,1)+CHILD(PT,2)+CHILD(PT,3)>1800*MeV) "

Lc2PKPi_forDMuXRB = CombineParticles( 'Lc2PKPi_forDMuXRB' )
Lc2PKPi_forDMuXRB.DecayDescriptor = '[Lambda_c+ -> K- p+ pi+]cc'
Lc2PKPi_forDMuXRB.DaughtersCuts = { "p+" : RobustDaughtercut + "& (PIDp>-5) " }
Lc2PKPi_forDMuXRB.InputLocations = [ 'SelKaon_forDMuXRB', 'SelPion_forDMuXRB', 'StdNoPIDsProtons']
Lc2PKPi_forDMuXRB.CombinationCut = "(ADAMASS('Lambda_c+')<200*MeV)"
Lc2PKPi_forDMuXRB.MotherCut =  RobustDcut + " & (ADMASS('Lambda_c+')<120*MeV)  & (VFASPF(VCHI2/VDOF)<4.0) & (CHILD(PT,1)+CHILD(PT,2)+CHILD(PT,3)>1800*MeV) "

SeqD_forDMuXRB = GaudiSequencer( 'SeqD_forDMuXRB' )
SeqD_forDMuXRB.ModeOR = 1
SeqD_forDMuXRB.IgnoreFilterPassed = True
SeqD_forDMuXRB.Members += [ D02KPi_forDMuXRB, Dp2KPiPi_forDMuXRB, Ds2KKPi_forDMuXRB, Lc2PKPi_forDMuXRB ]

Selb2DMuXRB = CombineParticles( 'Selb2DMuXRB' )
Selb2DMuXRB.DecayDescriptors = ['[Lambda_b0 -> Lambda_c+ mu-]cc', '[B_s0 -> D_s- mu+]cc', '[B0 -> D- mu+]cc', '[B- -> D0 mu-]cc']
Selb2DMuXRB.DaughtersCuts = {'mu+': RobustDaughtercut + " & (PT>700 * MeV) & (HASMUON) & (PIDmu>-5)"}
Selb2DMuXRB.InputLocations = [ 'D02KPi_forDMuXRB', 'Dp2KPiPi_forDMuXRB', 'Ds2KKPi_forDMuXRB', 'Lc2PKPi_forDMuXRB', 'StdLooseMuons' ]
Selb2DMuXRB.CombinationCut = "(AM<6.5*GeV) & (AM>2*GeV)"
Selb2DMuXRB.MotherCut = "(MM<6.0*GeV) & (MM>2.7*GeV) & (VFASPF(VCHI2/VDOF)<10.0) & (BPVDIRA>0.998) & (MINTREE(((ABSID=='D+') | (ABSID=='D0') | (ABSID=='D_s+') | (ABSID=='Lambda_c+')) , VFASPF(VZ))-VFASPF(VZ) > -1.0*mm )"
Selb2DMuXRB.ReFitPVs = True

#the nominal cut
Daughtercut = "(PT > 350*MeV) & (P > 2.0*GeV) & (MIPDV(PRIMARY)>0.05*mm) & (TRCHI2DOF < 8.0) & (MIPCHI2DV(PRIMARY)>6.25)"
Dcut = "(BPVVD > 2.5*mm)  & (BPVVDCHI2 > 49)"
SelKaon_forDMuX = FilterDesktop('SelKaon_forDMuX')
SelKaon_forDMuX.InputLocations = ['StdLooseKaons']
SelKaon_forDMuX.Code = Daughtercut + "& (PIDK>1e-10) "

SelPion_forDMuX = FilterDesktop('SelPion_forDMuX')
SelPion_forDMuX.InputLocations = ['StdLoosePions']
SelPion_forDMuX.Code = Daughtercut + "& (PIDK<10) "

D02KPi_forDMuX = CombineParticles( 'D02KPi_forDMuX' )
D02KPi_forDMuX.DecayDescriptor = '[D0 -> K- pi+]cc'
D02KPi_forDMuX.InputLocations = [ 'SelKaon_forDMuX', 'SelPion_forDMuX' ]
D02KPi_forDMuX.DaughtersCuts = { "K+" : " PIDK>3 " }
D02KPi_forDMuX.CombinationCut = "(ADAMASS('D0')<200*MeV) "
D02KPi_forDMuX.MotherCut = Dcut + " & (ADMASS('D0')<120*MeV) & (VFASPF(VCHI2/VDOF)<7.17) & (CHILD(PT,1)+CHILD(PT,2)>1700*MeV) "

Dp2KPiPi_forDMuX = CombineParticles( 'Dp2KPiPi_forDMuX' )
Dp2KPiPi_forDMuX.DecayDescriptor = '[D+ -> K- pi+ pi+]cc'
Dp2KPiPi_forDMuX.InputLocations = [ 'SelKaon_forDMuX', 'SelPion_forDMuX' ]
Dp2KPiPi_forDMuX.DaughtersCuts = { "K+" : " PIDK>3 " }
Dp2KPiPi_forDMuX.CombinationCut = "(ADAMASS('D+')<200*MeV)  "
Dp2KPiPi_forDMuX.MotherCut = Dcut + " & (ADMASS('D+')<120*MeV) & (VFASPF(VCHI2/VDOF)<4.0) & (CHILD(PT,1)+CHILD(PT,2)+CHILD(PT,3)>1800*MeV)"

Ds2KKPi_forDMuX = CombineParticles( 'Ds2KKPi_forDMuX' )
Ds2KKPi_forDMuX.DecayDescriptor = '[D_s+ -> K- K+ pi+]cc'
Ds2KKPi_forDMuX.InputLocations = [ 'SelKaon_forDMuX', 'SelPion_forDMuX' ]
Ds2KKPi_forDMuX.DaughtersCuts = { "K+" : " PIDK>3 " }
Ds2KKPi_forDMuX.CombinationCut = "(ADAMASS('D_s+')<250*MeV) "
Ds2KKPi_forDMuX.MotherCut = Dcut + " & (DMASS('D_s+')<120*MeV) & (DMASS('D_s+')>-220*MeV)  & (VFASPF(VCHI2/VDOF)<4.0) & (CHILD(PT,1)+CHILD(PT,2)+CHILD(PT,3)>1800*MeV) "

Lc2PKPi_forDMuX = CombineParticles( 'Lc2PKPi_forDMuX' )
Lc2PKPi_forDMuX.DecayDescriptor = '[Lambda_c+ -> K- p+ pi+]cc'
Lc2PKPi_forDMuX.DaughtersCuts = { "p+" : Daughtercut + "& (PIDp>10) & ((PIDp-PIDK)>3)"}
Lc2PKPi_forDMuX.InputLocations = [ 'SelKaon_forDMuX', 'SelPion_forDMuX', 'StdLooseProtons']
Lc2PKPi_forDMuX.CombinationCut = "(ADAMASS('Lambda_c+')<200*MeV)"
Lc2PKPi_forDMuX.MotherCut = Dcut + " & (ADMASS('Lambda_c+')<120*MeV) & (VFASPF(VCHI2/VDOF)<4.0) & (CHILD(PT,1)+CHILD(PT,2)+CHILD(PT,3)>1800*MeV) "

SeqD_forDMuX = GaudiSequencer( 'SeqD_forDMuX' )
SeqD_forDMuX.ModeOR = 1
SeqD_forDMuX.IgnoreFilterPassed = True
SeqD_forDMuX.Members += [ D02KPi_forDMuX, Dp2KPiPi_forDMuX, Ds2KKPi_forDMuX, Lc2PKPi_forDMuX ]

Selb2DMuX = CombineParticles( 'Selb2DMuX' )
Selb2DMuX.DecayDescriptors = ['[Lambda_b0 -> Lambda_c+ mu-]cc', '[B_s0 -> D_s- mu+]cc', '[B0 -> D- mu+]cc', '[B- -> D0 mu-]cc']
Selb2DMuX.DaughtersCuts = {'mu+': Daughtercut + " & (PT>700 * MeV) & (HASMUON) & (PIDmu>-5) "}
Selb2DMuX.InputLocations = [ 'D02KPi_forDMuX', 'Dp2KPiPi_forDMuX', 'Ds2KKPi_forDMuX', 'Lc2PKPi_forDMuX', 'StdLooseMuons' ]
Selb2DMuX.CombinationCut = "(AM<6.5*GeV) & (AM>2*GeV)"
Selb2DMuX.MotherCut = "(MM<6.0*GeV) & (MM>2.7*GeV) & (VFASPF(VCHI2/VDOF)<10.0) & (BPVDIRA>0.998) & (MINTREE(((ABSID=='D+') | (ABSID=='D0') | (ABSID=='D_s+') | (ABSID=='Lambda_c+')) , VFASPF(VZ))-VFASPF(VZ) > -1.0*mm )"
Selb2DMuX.ReFitPVs = True

############################################
# Create StrippingLine with this selection #
############################################

line = StrippingLine('b2DMuX'
                     , prescale = 1.0
                     , algos = [ SelKaon_forDMuX, SelPion_forDMuX, SeqD_forDMuX, Selb2DMuX ]
                     )

line1 = StrippingLine('b2DMuXRB'
                     , prescale = 1.0
                     , algos = [ SelKaon_forDMuXRB, SelPion_forDMuXRB, SeqD_forDMuXRB, Selb2DMuXRB ]
                     )

