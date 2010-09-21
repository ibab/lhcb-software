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
D0KKpipi = CombineParticles("StripD0KKpipiForDstar2D0KKpipiRobust")
D0KKpipi.InputLocations = [ "StdLooseKaons","StdLoosePions" ]
D0KKpipi.DecayDescriptor = "D0 -> K+ K- pi+ pi-"
D0KKpipi.CombinationCut = "( ADAMASS('D0')<50*MeV) & (AMAXDOCA('')<0.12*mm)"
D0KKpipi.MotherCut = "(PT > 2500.0*MeV ) & (BPVDIRA > 0.9996) & (MIPDV(PRIMARY) < 0.05*mm ) & (BPVVD > 1.2*mm) & (  BPVVDZ > 0 )"
D0KKpipi.DaughtersCuts = {
    "K+" : "(PT > 450.0*MeV ) & (P > 1000.0*MeV) & (PIDK-PIDpi >-5.0)",
    "pi+" : "(PT > 350.0*MeV ) & (P > 1000.0*MeV) & (PIDpi-PIDK >-5.0)"
}


#Conjugate decay
from Configurables import ConjugateNeutralPID
Conjugate_D0KKpipi= ConjugateNeutralPID("StripD0barKKpipiForDstar2D0KKpipiRobust")
Conjugate_D0KKpipi.InputLocations = ["StripD0KKpipiForDstar2D0KKpipiRobust"]

# Reconstruct D*+ -> D0 pi+  (+cc)


Dstar_D0pi   = CombineParticles("StripDstar2D0KKpipiRobust");
Dstar_D0pi.DecayDescriptor   = "[D*(2010)+ -> D0 pi+]cc"
Dstar_D0pi.InputLocations = [ "StripD0KKpipiForDstar2D0KKpipiRobust", "StripD0barKKpipiForDstar2D0KKpipiRobust", "StdLoosePions" ]


# Set up delta mass cuts:

str_cutsDstarComb_MandDoca = "(ADAMASS('D*(2010)+')<50*MeV) & ( AMAXDOCA('')<0.15*mm )"
Dstar_D0pi.CombinationCut = str_cutsDstarComb_MandDoca
strDstarMotherCut_PT = '(PT > 2500*MeV)'
strDstarMotherCut_DM = '(MM - CHILD(MM,1) > 138*MeV) & (MM - CHILD(MM,1) < 154*MeV)'
strDstarMotherCut_SlpiIPandPT = '( CHILD(MIPDV(PRIMARY),2) < 0.3*mm) & ( CHILD(PT,2) > 250.*MeV)'



# Mother cut for DM signal window (tighter):
Dstar_D0pi.MotherCut = strDstarMotherCut_PT + ' & ' + strDstarMotherCut_DM + ' & ' + strDstarMotherCut_SlpiIPandPT



# Now create our StrippingLine object with this selection.

line1 = StrippingLine('Dstar2D0KKpipiRobust', prescale = 1.0, algos = [D0KKpipi, Conjugate_D0KKpipi, Dstar_D0pi ])
