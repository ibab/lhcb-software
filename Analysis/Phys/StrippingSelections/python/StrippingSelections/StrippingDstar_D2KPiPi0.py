#######################################################################
#
# Options for pre-selecting prompt D*->D(KPiPi0)Pi events, favoured and suppressed mode
#
# @author Andrea Contu, University of Oxford
# @date 2010-01-22
#
########################################################################                   
__author__ = 'Andrea Contu'
__date__ = '05/09/2010'
__version__ = '$Revision: 1.4 $'

from Configurables import GaudiSequencer, CombineParticles
from StrippingConf.StrippingLine import StrippingLine

StrippingDstar_D2KPiPi0seq=GaudiSequencer("StrippingDstar_D2KPiPi0seq")
StrippingDstar_D2KPiPi0seqRS=GaudiSequencer("StrippingDstar_D2KPiPi0seqRS")
StrippingDstar_D2KPiPi0seqWS=GaudiSequencer("StrippingDstar_D2KPiPi0seqWS")

#D0 Cuts
str_D0_ChargedDau="(PT>500*MeV) & (P>5*GeV) & (MIPCHI2DV(PRIMARY)>4) & (TRCHI2DOF<5)"
str_D0_Pi0="(PT>800*MeV) & (P>5*GeV)"
str_D0_CombCuts="(ADAMASS('D0')<200*MeV) & (ADOCA(1,2) < 1.5) & (AHASCHILD(((ABSID == 'K+') | (ABSID == 'pi+')) & (MIPCHI2DV(PRIMARY)>40)))"
str_D0_MotherCuts="(BPVDIRA>0.9) & (PT>2.5*GeV) & (VFASPF(VCHI2/VDOF)<10) & (BPVVDCHI2>36)"
str_D0_DecayDescriptor_RS="[D0 -> K- pi+ pi0]cc"
str_D0_Sidebands="200*MeV"

#D* Cuts
str_DStar_SlowPiCuts="(PT>170*MeV) & (BPVIP()<0.5*mm)"
str_DStar_CombCuts="(ADAMASS('D*(2010)+')<200*MeV)"
str_DStar_MotherCuts="(PT>3*GeV) & ((M-M2)>(145.5-10)*MeV) & ((M-M2)<(145.5+20)*MeV)  & (VFASPF(VCHI2/VDOF)<10) & (BPVIPCHI2()<100)"

#make D0 RS
D02KPiPi0_RS = CombineParticles ( "D02KPiPi0_RS" )
D02KPiPi0_RS.InputLocations = [ "StdLooseKaons", "StdLoosePions", "StdLooseMergedPi0", "StdLooseResolvedPi0" ]
D02KPiPi0_RS.DecayDescriptor = str_D0_DecayDescriptor_RS
D02KPiPi0_RS.DaughtersCuts = { "K-" : str_D0_ChargedDau+' & ((PPINFO(LHCb.ProtoParticle.RichDLLk,-1000))>0)',
                                    "pi+" : str_D0_ChargedDau+' & ((PPINFO(LHCb.ProtoParticle.RichDLLk,-1000))<0)',
                                    "pi0" : str_D0_Pi0}
D02KPiPi0_RS.CombinationCut = str_D0_CombCuts
D02KPiPi0_RS.MotherCut = str_D0_MotherCuts
StrippingDstar_D2KPiPi0seq.Members.append(D02KPiPi0_RS)

from Configurables import ConjugateNeutralPID
#make D0 WS
D02KPiPi0_WS=ConjugateNeutralPID("D02KPiPi0_WS")
D02KPiPi0_WS.InputLocations = [ "D02KPiPi0_RS" ]
StrippingDstar_D2KPiPi0seq.Members.append(D02KPiPi0_WS)

StrippingDstar_D2KPiPi0seqRS.Members.append(StrippingDstar_D2KPiPi0seq)
StrippingDstar_D2KPiPi0seqWS.Members.append(StrippingDstar_D2KPiPi0seq)
#make D* RS
Strip_DStar2D0Pi_D02KPiPi0_RS = CombineParticles ( "DStar2D0Pi_D02KPiPi0_RS" )
Strip_DStar2D0Pi_D02KPiPi0_RS.InputLocations = [ "D02KPiPi0_RS", "StdNoPIDsPions" ]
Strip_DStar2D0Pi_D02KPiPi0_RS.DecayDescriptor = "[D*(2010)+ -> pi+ D0]cc"
Strip_DStar2D0Pi_D02KPiPi0_RS.DaughtersCuts = { "D0" : "(ALL)",
                                    "pi+" : str_DStar_SlowPiCuts}
Strip_DStar2D0Pi_D02KPiPi0_RS.CombinationCut = str_DStar_CombCuts
Strip_DStar2D0Pi_D02KPiPi0_RS.MotherCut = str_DStar_MotherCuts
StrippingDstar_D2KPiPi0seqRS.Members.append(Strip_DStar2D0Pi_D02KPiPi0_RS)

#make D* WS
Strip_DStar2D0Pi_D02KPiPi0_WS = CombineParticles ( "DStar2D0Pi_D02KPiPi0_WS" )
Strip_DStar2D0Pi_D02KPiPi0_WS.InputLocations = [ "D02KPiPi0_WS", "StdNoPIDsPions" ]
Strip_DStar2D0Pi_D02KPiPi0_WS.DecayDescriptor = "[D*(2010)+ -> pi+ D0]cc"
Strip_DStar2D0Pi_D02KPiPi0_WS.DaughtersCuts = { "D0" : "(ALL)",
                                    "pi+" : str_DStar_SlowPiCuts}
Strip_DStar2D0Pi_D02KPiPi0_WS.CombinationCut = str_DStar_CombCuts
Strip_DStar2D0Pi_D02KPiPi0_WS.MotherCut = str_DStar_MotherCuts

StrippingDstar_D2KPiPi0seqWS.Members.append(Strip_DStar2D0Pi_D02KPiPi0_WS)

# Create 2 distinct lines for WS/RS
lineRS = StrippingLine('Dstar_D2KPiPi0RS_Line'
               , prescale = 0.5
               ,  algos = [ StrippingDstar_D2KPiPi0seqRS ]
               )
							 
lineWS = StrippingLine('Dstar_D2KPiPi0WS_Line'
               , prescale = 0.0005  #if we really need to prescale, prescale this one first
               ,  algos = [ StrippingDstar_D2KPiPi0seqWS ]
               )
