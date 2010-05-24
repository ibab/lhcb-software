#
# @author B. VIAUD
# @date 2010-March-10
#
## #####################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember, bindMembers

from CommonParticles.StdLoosePions import StdLoosePions
from CommonParticles.StdLooseKaons import StdLooseKaons


# Make the D0->K+K-pi+pi-.
D0KKpipi = CombineParticles("StripD0KKpipiForDstar2D0KKpipiRegular")
D0KKpipi.InputLocations = [ "StdLooseKaons","StdLoosePions" ]
D0KKpipi.DecayDescriptor = "D0 -> K+ K- pi+ pi-"
D0KKpipi.CombinationCut = "( ADAMASS('D0')<50*MeV) & (AMAXDOCA('')<0.25*mm )"
D0KKpipi.MotherCut = "(PT > 2500.0*MeV ) & (BPVDIRA > 0.9996) & (MIPDV(PRIMARY) < 0.1*mm ) & (MIPCHI2DV(PRIMARY) < 25. ) & ( BPVVDCHI2 > 9.0) & ( BPVVD > 1.0*mm) & ( VFASPF(VCHI2/VDOF) < 9.) & (  BPVVDZ > 0 )"
D0KKpipi.DaughtersCuts = {
    "K+" : "(PT > 450.0*MeV ) & (P > 1000.0*MeV)  & (PIDK-PIDpi>-2.0)",
    "pi+" : "(PT > 300.0*MeV ) & (P > 1000.0*MeV) & (PIDpi-PIDK>-5.0)"
}


#Conjugate decay
from Configurables import ConjugateNeutralPID
Conjugate_D0KKpipi= ConjugateNeutralPID("StripD0barKKpipiForDstar2D0KKpipiRegular")
Conjugate_D0KKpipi.InputLocations = ["StripD0KKpipiForDstar2D0KKpipiRegular"]

# Reconstruct D*+ -> D0 pi+  (+cc)


Dstar_D0pi   = CombineParticles("StripDstar2D0KKpipiRegular");
Dstar_D0pi.DecayDescriptor   = "[D*(2010)+ -> D0 pi+]cc"
Dstar_D0pi.InputLocations = [ "StripD0KKpipiForDstar2D0KKpipiRegular", "StripD0barKKpipiForDstar2D0KKpipiRegular", "StdLoosePions" ]


# Set up delta mass cuts:

str_cutsDstarComb_MandDoca = "(ADAMASS('D*(2010)+')<50*MeV) & ( AMAXDOCA('')<0.5*mm )"
Dstar_D0pi.CombinationCut = str_cutsDstarComb_MandDoca
strDstarMotherCut_VCHI2 = '( VFASPF(VCHI2/VDOF) < 9.)'
strDstarMotherCut_PT = '(PT > 2500*MeV)'
strDstarMotherCut_DM = '(MM - CHILD(MM,1) > 138*MeV) & (MM - CHILD(MM,1) < 154*MeV)'
strDstarMotherCut_SlpiIPandPT = '( CHILD(MIPDV(PRIMARY),2) < 0.5*mm) & ( CHILD(MIPCHI2DV(PRIMARY),2) < 25) & ( CHILD(PT,2) > 250.*MeV)'


strDstarMotherCut_TOT1= strDstarMotherCut_PT + ' & ' + strDstarMotherCut_DM + ' & ' + strDstarMotherCut_SlpiIPandPT
strDstarMotherCut_TOT2= strDstarMotherCut_VCHI2 


# Mother cut for DM signal window (tighter):
Dstar_D0pi.MotherCut =  strDstarMotherCut_TOT1 +'&' + strDstarMotherCut_TOT2 



# Now create our StrippingLine object with this selection.

line1 = StrippingLine('Dstar2D0KKpipiRegular', prescale = 1.0, algos = [D0KKpipi, Conjugate_D0KKpipi, Dstar_D0pi ])
