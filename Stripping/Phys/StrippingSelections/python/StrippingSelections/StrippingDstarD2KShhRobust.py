## #####################################################################
# $Id: StrippingDstarD2KShhRobust.py,v 1.1 2009-11-30 21:19:31 mcharles Exp $
#
# A robust stripping selection for D*+ -> D0(Ks h+ h-) pi+
# See the non-robust version (StrippingDstarD2KShh.py) for more info.
#
# This selection uses the LoKi functor CHILDIP, which requires
# Phys/LoKiPhys v8r11 or newer.
#
# @author Mat Charles
# @date 2009-November-13
#
## #####################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember, bindMembers

from CommonParticles.StdNoPIDsPions import StdNoPIDsPions
from CommonParticles.StdNoPIDsKaons import StdNoPIDsKaons

# Make the Ks.

KSLL = CombineParticles("StripKSLLForDstarD2KShhRobust")
KSLL.InputLocations = [ "StdNoPIDsPions" ]
KSLL.DecayDescriptor = "KS0 -> pi+ pi-"
KSLL.CombinationCut = "ADAMASS('KS0')<250*MeV"
KSLL.MotherCut = "(BPVVDZ>-1000.0) & (BPVVDZ<650.0) & (ADMASS('KS0')<11.4*MeV) & (BPVDIRA > 0.9997) & (CHILDIP(1) < 0.5*mm) & (CHILDIP(2) < 0.5*mm) & (BPVVD > 15.0*mm)"
KSLL.DaughtersCuts = {
    "pi+" : "(MIPDV(PRIMARY)>0.5) & (PT > 420.0*(1.0-0.5*MIPDV(PRIMARY))) & (P > 2000.0)",
    "pi-" : "(MIPDV(PRIMARY)>0.5) & (PT > 420.0*(1.0-0.5*MIPDV(PRIMARY))) & (P > 2000.0)"
}

KSDD = CombineParticles("StripKSDDForDstarD2KShhRobust")
KSDD.InputLocations = [ "StdNoPIDsDownPions" ]
KSDD.DecayDescriptor = "KS0 -> pi+ pi-"
KSDD.CombinationCut = "ADAMASS('KS0')<500*MeV"
KSDD.MotherCut = "(BPVVDZ>0.0) & (BPVVDZ<2300.0) & (ADMASS('KS0')<24.9*MeV) & (BPVDIRA > 0.99995) & (CHILDIP(1) < 6.0*mm) & (CHILDIP(2) < 6.0*mm) & (BPVVD > 200.0*mm)"
KSDD.DaughtersCuts = {
    "pi+" : "(MIPDV(PRIMARY)>0.5) & (P>25000-5000*MIPDV(PRIMARY)) & (P>100000-50000*MIPDV(PRIMARY))",
    "pi-" : "(MIPDV(PRIMARY)>0.5) & (P>25000-5000*MIPDV(PRIMARY)) & (P>100000-50000*MIPDV(PRIMARY))"
}

# Make the D0 -> KS h+ h- candidates.

D2KSpp_LL = CombineParticles("StripD2KSppLLForDstarD2KShhRobustSB")
D2KSKp_LL = CombineParticles("StripD2KSKpLLForDstarD2KShhRobustSB")
D2KSKK_LL = CombineParticles("StripD2KSKKLLForDstarD2KShhRobustSB")
D2KSpp_DD = CombineParticles("StripD2KSppDDForDstarD2KShhRobustSB")
D2KSKp_DD = CombineParticles("StripD2KSKpDDForDstarD2KShhRobustSB")
D2KSKK_DD = CombineParticles("StripD2KSKKDDForDstarD2KShhRobustSB")

D2KSpp_LL.DecayDescriptor  = "D0 -> KS0 pi+ pi-"
D2KSKp_LL.DecayDescriptors = ["D0 -> KS0 K+ pi-", "D0 -> KS0 K- pi+"]
D2KSKK_LL.DecayDescriptor  = "D0 -> KS0 K+ K-"
D2KSpp_DD.DecayDescriptor  = "D0 -> KS0 pi+ pi-"
D2KSKp_DD.DecayDescriptors = ["D0 -> KS0 K+ pi-", "D0 -> KS0 K- pi+"]
D2KSKK_DD.DecayDescriptor  = "D0 -> KS0 K+ K-"

D2KSpp_LL.InputLocations = [ "StripKSLLForDstarD2KShhRobust", "StdNoPIDsPions" ]
D2KSKp_LL.InputLocations = [ "StripKSLLForDstarD2KShhRobust", "StdNoPIDsKaons", "StdNoPIDsPions" ]
D2KSKK_LL.InputLocations = [ "StripKSLLForDstarD2KShhRobust", "StdNoPIDsKaons" ]
D2KSpp_DD.InputLocations = [ "StripKSDDForDstarD2KShhRobust", "StdNoPIDsPions" ]
D2KSKp_DD.InputLocations = [ "StripKSDDForDstarD2KShhRobust", "StdNoPIDsKaons", "StdNoPIDsPions" ]
D2KSKK_DD.InputLocations = [ "StripKSDDForDstarD2KShhRobust", "StdNoPIDsKaons" ]

str_dauCuts = '(P>1500.0*MeV)'
D2KSpp_LL.DaughtersCuts = { "pi+" : str_dauCuts }
D2KSKp_LL.DaughtersCuts = { "pi+" : str_dauCuts, "K+" : str_dauCuts }
D2KSKK_LL.DaughtersCuts = { "K+"  : str_dauCuts }
D2KSpp_DD.DaughtersCuts = { "pi+" : str_dauCuts }
D2KSKp_DD.DaughtersCuts = { "pi+" : str_dauCuts, "K+" : str_dauCuts }
D2KSKK_DD.DaughtersCuts = { "K+"  : str_dauCuts }

strCombMassCutLL = "(ADAMASS('D0')<100*MeV)"
strCombMassCutDD = "(ADAMASS('D0')<200*MeV)"
strDaughterIPCut_Tight = "((AHASCHILD((ABSID=='pi+') & (MIPDV(PRIMARY)>0.1*mm))) | (AHASCHILD((ABSID=='K+') & (MIPDV(PRIMARY)>0.1*mm))))"
strDaughterIPCut_Loose = "(AHASCHILD((ABSID=='K+') & (MIPDV(PRIMARY)>0.01*mm)))"
D2KSpp_LL.CombinationCut = strCombMassCutLL + " & " + strDaughterIPCut_Tight
D2KSKp_LL.CombinationCut = strCombMassCutLL + " & " + strDaughterIPCut_Tight
D2KSKK_LL.CombinationCut = strCombMassCutLL + " & " + strDaughterIPCut_Loose
D2KSpp_DD.CombinationCut = strCombMassCutDD + " & " + strDaughterIPCut_Tight
D2KSKp_DD.CombinationCut = strCombMassCutDD + " & " + strDaughterIPCut_Tight
D2KSKK_DD.CombinationCut = strCombMassCutDD + " & " + strDaughterIPCut_Loose

# Signal window:
str_cutppLL = '27.5*MeV'
str_cutKpLL = '23.0*MeV'
str_cutKKLL = '14.3*MeV'
str_cutppDD = '35.0*MeV'
str_cutKpDD = '30.0*MeV'
str_cutKKDD = '18.0*MeV'
str_massCutD2KSpp_LL = "(ADMASS('D0')<" + str_cutppLL + ")"
str_massCutD2KSKp_LL = "(ADMASS('D0')<" + str_cutKpLL + ")"
str_massCutD2KSKK_LL = "(ADMASS('D0')<" + str_cutKKLL + ")"
str_massCutD2KSpp_DD = "(ADMASS('D0')<" + str_cutppDD + ")"
str_massCutD2KSKp_DD = "(ADMASS('D0')<" + str_cutKpDD + ")"
str_massCutD2KSKK_DD = "(ADMASS('D0')<" + str_cutKKDD + ")"
# Sideband:
str_cutppLLSB = "82.5*MeV"
str_cutKpLLSB = "69*MeV"
str_cutKKLLSB = "42.9*MeV"
str_cutppDDSB = "105.0*MeV"
str_cutKpDDSB = "90*MeV"
str_cutKKDDSB = "54.0*MeV"
str_massCutD2KSpp_LLSB = "(ADMASS('D0')<" + str_cutppLLSB + ")"
str_massCutD2KSKp_LLSB = "(ADMASS('D0')<" + str_cutKpLLSB + ")"
str_massCutD2KSKK_LLSB = "(ADMASS('D0')<" + str_cutKKLLSB + ")"
str_massCutD2KSpp_DDSB = "(ADMASS('D0')<" + str_cutppDDSB + ")"
str_massCutD2KSKp_DDSB = "(ADMASS('D0')<" + str_cutKpDDSB + ")"
str_massCutD2KSKK_DDSB = "(ADMASS('D0')<" + str_cutKKDDSB + ")"
# Also define a slightly narrower version of the signal window.
# The idea here is that we will have a small overlap between the
# mass region accepted by the signal lines and the sideband lines.
# For example, if the signal window is
#     (m - m_nominal) < X
#     (DM - DM_nominal) < Y
# then one part of the sideband region will be
#     (m - m_nominal) > X - 2 MeV
#     (DM - DM_nominal) < Y
str_cutppLL_truncated = '25.5*MeV'
str_cutKpLL_truncated = '21.0*MeV'
str_cutKKLL_truncated = '12.3*MeV'
str_cutppDD_truncated = '33.0*MeV'
str_cutKpDD_truncated = '28.0*MeV'
str_cutKKDD_truncated = '16.0*MeV'

# Assemble mother cut:
str_DMotherCut_base = '(BPVVDZ<7000.0*mm) & (BPVDIRA > 0.995) & (PT>500.0*MeV)'
str_DMotherCut_IPLL_Tight = "(CHILDIP(1) < 0.3*mm) & (CHILDIP(2) < 0.3*mm) & (CHILDIP(3) < 0.3*mm) & (BPVVD > 1.0*mm)"
str_DMotherCut_IPDD_Tight = "(CHILDIP(1) < 4.0*mm) & (CHILDIP(2) < 0.3*mm) & (CHILDIP(3) < 0.3*mm) & (BPVVD > 1.0*mm)"
str_DMotherCut_IPLL_Loose = "(CHILDIP(1) < 0.4*mm) & (CHILDIP(2) < 0.3*mm) & (CHILDIP(3) < 0.3*mm) & (BPVVD > 0.3*mm)"
str_DMotherCut_IPDD_Loose = "(CHILDIP(1) < 5.0*mm) & (CHILDIP(2) < 0.3*mm) & (CHILDIP(3) < 0.3*mm) & (BPVVD > 0.3*mm)"

# MotherCut uses loose mass cut only
D2KSpp_LL.MotherCut = str_DMotherCut_base + ' & ' + str_DMotherCut_IPLL_Tight + ' & ' + str_massCutD2KSpp_LLSB
D2KSKp_LL.MotherCut = str_DMotherCut_base + ' & ' + str_DMotherCut_IPLL_Tight + ' & ' + str_massCutD2KSKp_LLSB
D2KSKK_LL.MotherCut = str_DMotherCut_base + ' & ' + str_DMotherCut_IPLL_Loose + ' & ' + str_massCutD2KSKK_LLSB
D2KSpp_DD.MotherCut = str_DMotherCut_base + ' & ' + str_DMotherCut_IPDD_Tight + ' & ' + str_massCutD2KSpp_DDSB
D2KSKp_DD.MotherCut = str_DMotherCut_base + ' & ' + str_DMotherCut_IPDD_Tight + ' & ' + str_massCutD2KSKp_DDSB
D2KSKK_DD.MotherCut = str_DMotherCut_base + ' & ' + str_DMotherCut_IPDD_Loose + ' & ' + str_massCutD2KSKK_DDSB

# Apply a tighter filter to isolate the signal window:
FilterD2KSpp_LL = FilterDesktop("StripD2KSppLLForDstarD2KShhRobust", InputLocations = ["StripD2KSppLLForDstarD2KShhRobustSB"], Code = str_massCutD2KSpp_LL)
FilterD2KSKp_LL = FilterDesktop("StripD2KSKpLLForDstarD2KShhRobust", InputLocations = ["StripD2KSKpLLForDstarD2KShhRobustSB"], Code = str_massCutD2KSKp_LL)
FilterD2KSKK_LL = FilterDesktop("StripD2KSKKLLForDstarD2KShhRobust", InputLocations = ["StripD2KSKKLLForDstarD2KShhRobustSB"], Code = str_massCutD2KSKK_LL)
FilterD2KSpp_DD = FilterDesktop("StripD2KSppDDForDstarD2KShhRobust", InputLocations = ["StripD2KSppDDForDstarD2KShhRobustSB"], Code = str_massCutD2KSpp_DD)
FilterD2KSKp_DD = FilterDesktop("StripD2KSKpDDForDstarD2KShhRobust", InputLocations = ["StripD2KSKpDDForDstarD2KShhRobustSB"], Code = str_massCutD2KSKp_DD)
FilterD2KSKK_DD = FilterDesktop("StripD2KSKKDDForDstarD2KShhRobust", InputLocations = ["StripD2KSKKDDForDstarD2KShhRobustSB"], Code = str_massCutD2KSKK_DD)

# Clone the D0 candidates to make D0bar candidates.

from Configurables import ConjugateNeutralPID
# Signal box:
Conjugate_D2KSpp_LL = ConjugateNeutralPID("StripDbar2KSppLLForDstarD2KShhRobust")
Conjugate_D2KSKp_LL = ConjugateNeutralPID("StripDbar2KSKpLLForDstarD2KShhRobust")
Conjugate_D2KSKK_LL = ConjugateNeutralPID("StripDbar2KSKKLLForDstarD2KShhRobust")
Conjugate_D2KSpp_DD = ConjugateNeutralPID("StripDbar2KSppDDForDstarD2KShhRobust")
Conjugate_D2KSKp_DD = ConjugateNeutralPID("StripDbar2KSKpDDForDstarD2KShhRobust")
Conjugate_D2KSKK_DD = ConjugateNeutralPID("StripDbar2KSKKDDForDstarD2KShhRobust")
# Wide mass window (sideband):
Conjugate_D2KSpp_LLSB = ConjugateNeutralPID("StripDbar2KSppLLForDstarD2KShhRobustSB")
Conjugate_D2KSKp_LLSB = ConjugateNeutralPID("StripDbar2KSKpLLForDstarD2KShhRobustSB")
Conjugate_D2KSKK_LLSB = ConjugateNeutralPID("StripDbar2KSKKLLForDstarD2KShhRobustSB")
Conjugate_D2KSpp_DDSB = ConjugateNeutralPID("StripDbar2KSppDDForDstarD2KShhRobustSB")
Conjugate_D2KSKp_DDSB = ConjugateNeutralPID("StripDbar2KSKpDDForDstarD2KShhRobustSB")
Conjugate_D2KSKK_DDSB = ConjugateNeutralPID("StripDbar2KSKKDDForDstarD2KShhRobustSB")
# Signal box:
Conjugate_D2KSpp_LL.InputLocations = [ "StripD2KSppLLForDstarD2KShhRobust" ]
Conjugate_D2KSKp_LL.InputLocations = [ "StripD2KSKpLLForDstarD2KShhRobust" ]
Conjugate_D2KSKK_LL.InputLocations = [ "StripD2KSKKLLForDstarD2KShhRobust" ]
Conjugate_D2KSpp_DD.InputLocations = [ "StripD2KSppDDForDstarD2KShhRobust" ]
Conjugate_D2KSKp_DD.InputLocations = [ "StripD2KSKpDDForDstarD2KShhRobust" ]
Conjugate_D2KSKK_DD.InputLocations = [ "StripD2KSKKDDForDstarD2KShhRobust" ]
# Wide mass window (sideband):
Conjugate_D2KSpp_LLSB.InputLocations = [ "StripD2KSppLLForDstarD2KShhRobustSB" ]
Conjugate_D2KSKp_LLSB.InputLocations = [ "StripD2KSKpLLForDstarD2KShhRobustSB" ]
Conjugate_D2KSKK_LLSB.InputLocations = [ "StripD2KSKKLLForDstarD2KShhRobustSB" ]
Conjugate_D2KSpp_DDSB.InputLocations = [ "StripD2KSppDDForDstarD2KShhRobustSB" ]
Conjugate_D2KSKp_DDSB.InputLocations = [ "StripD2KSKpDDForDstarD2KShhRobustSB" ]
Conjugate_D2KSKK_DDSB.InputLocations = [ "StripD2KSKKDDForDstarD2KShhRobustSB" ]

# Reconstruct D*+ -> D0 pi+  (+cc)

# Signal box:
Dstar_KSpp_LL   = CombineParticles("StripDstar2D2KSppLLForDstarD2KShhRobust");
Dstar_KSKp_LL   = CombineParticles("StripDstar2D2KSKpLLForDstarD2KShhRobust");
Dstar_KSKK_LL   = CombineParticles("StripDstar2D2KSKKLLForDstarD2KShhRobust");
Dstar_KSpp_DD   = CombineParticles("StripDstar2D2KSppDDForDstarD2KShhRobust");
Dstar_KSKp_DD   = CombineParticles("StripDstar2D2KSKpDDForDstarD2KShhRobust");
Dstar_KSKK_DD   = CombineParticles("StripDstar2D2KSKKDDForDstarD2KShhRobust");
# Wide mass windows (sidebands):
Dstar_KSpp_LLSB = CombineParticles("StripDstar2D2KSppLLForDstarD2KShhRobustSB");
Dstar_KSKp_LLSB = CombineParticles("StripDstar2D2KSKpLLForDstarD2KShhRobustSB");
Dstar_KSKK_LLSB = CombineParticles("StripDstar2D2KSKKLLForDstarD2KShhRobustSB");
Dstar_KSpp_DDSB = CombineParticles("StripDstar2D2KSppDDForDstarD2KShhRobustSB");
Dstar_KSKp_DDSB = CombineParticles("StripDstar2D2KSKpDDForDstarD2KShhRobustSB");
Dstar_KSKK_DDSB = CombineParticles("StripDstar2D2KSKKDDForDstarD2KShhRobustSB");

# Signal box:
Dstar_KSpp_LL.DecayDescriptor   = "[D*(2010)+ -> D0 pi+]cc"
Dstar_KSKp_LL.DecayDescriptor   = "[D*(2010)+ -> D0 pi+]cc"
Dstar_KSKK_LL.DecayDescriptor   = "[D*(2010)+ -> D0 pi+]cc"
Dstar_KSpp_DD.DecayDescriptor   = "[D*(2010)+ -> D0 pi+]cc"
Dstar_KSKp_DD.DecayDescriptor   = "[D*(2010)+ -> D0 pi+]cc"
Dstar_KSKK_DD.DecayDescriptor   = "[D*(2010)+ -> D0 pi+]cc"
# Wide mass windows (sidebands):
Dstar_KSpp_LLSB.DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc"
Dstar_KSKp_LLSB.DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc"
Dstar_KSKK_LLSB.DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc"
Dstar_KSpp_DDSB.DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc"
Dstar_KSKp_DDSB.DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc"
Dstar_KSKK_DDSB.DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc"

# Signal box:
Dstar_KSpp_LL.InputLocations = [ "StripD2KSppLLForDstarD2KShhRobust", "StripDbar2KSppLLForDstarD2KShhRobust", "StdNoPIDsPions" ]
Dstar_KSKp_LL.InputLocations = [ "StripD2KSKpLLForDstarD2KShhRobust", "StripDbar2KSKpLLForDstarD2KShhRobust", "StdNoPIDsPions" ]
Dstar_KSKK_LL.InputLocations = [ "StripD2KSKKLLForDstarD2KShhRobust", "StripDbar2KSKKLLForDstarD2KShhRobust", "StdNoPIDsPions" ]
Dstar_KSpp_DD.InputLocations = [ "StripD2KSppDDForDstarD2KShhRobust", "StripDbar2KSppDDForDstarD2KShhRobust", "StdNoPIDsPions" ]
Dstar_KSKp_DD.InputLocations = [ "StripD2KSKpDDForDstarD2KShhRobust", "StripDbar2KSKpDDForDstarD2KShhRobust", "StdNoPIDsPions" ]
Dstar_KSKK_DD.InputLocations = [ "StripD2KSKKDDForDstarD2KShhRobust", "StripDbar2KSKKDDForDstarD2KShhRobust", "StdNoPIDsPions" ]
# Wide mass windows (sidebands):
Dstar_KSpp_LLSB.InputLocations = [ "StripD2KSppLLForDstarD2KShhRobustSB", "StripDbar2KSppLLForDstarD2KShhRobustSB", "StdNoPIDsPions" ]
Dstar_KSKp_LLSB.InputLocations = [ "StripD2KSKpLLForDstarD2KShhRobustSB", "StripDbar2KSKpLLForDstarD2KShhRobustSB", "StdNoPIDsPions" ]
Dstar_KSKK_LLSB.InputLocations = [ "StripD2KSKKLLForDstarD2KShhRobustSB", "StripDbar2KSKKLLForDstarD2KShhRobustSB", "StdNoPIDsPions" ]
Dstar_KSpp_DDSB.InputLocations = [ "StripD2KSppDDForDstarD2KShhRobustSB", "StripDbar2KSppDDForDstarD2KShhRobustSB", "StdNoPIDsPions" ]
Dstar_KSKp_DDSB.InputLocations = [ "StripD2KSKpDDForDstarD2KShhRobustSB", "StripDbar2KSKpDDForDstarD2KShhRobustSB", "StdNoPIDsPions" ]
Dstar_KSKK_DDSB.InputLocations = [ "StripD2KSKKDDForDstarD2KShhRobustSB", "StripDbar2KSKKDDForDstarD2KShhRobustSB", "StdNoPIDsPions" ]

# Set up delta mass cuts:

str_cutsDstarCombLL = "ADAMASS('D*(2010)+')<150*MeV"
str_cutsDstarCombDD = "ADAMASS('D*(2010)+')<200*MeV"
# Signal box
Dstar_KSpp_LL.CombinationCut = str_cutsDstarCombLL
Dstar_KSKp_LL.CombinationCut = str_cutsDstarCombLL
Dstar_KSKK_LL.CombinationCut = str_cutsDstarCombLL
Dstar_KSpp_DD.CombinationCut = str_cutsDstarCombDD
Dstar_KSKp_DD.CombinationCut = str_cutsDstarCombDD
Dstar_KSKK_DD.CombinationCut = str_cutsDstarCombDD
# Wide mass windows (sidebands):
Dstar_KSpp_LLSB.CombinationCut = str_cutsDstarCombLL
Dstar_KSKp_LLSB.CombinationCut = str_cutsDstarCombLL
Dstar_KSKK_LLSB.CombinationCut = str_cutsDstarCombLL
Dstar_KSpp_DDSB.CombinationCut = str_cutsDstarCombDD
Dstar_KSKp_DDSB.CombinationCut = str_cutsDstarCombDD
Dstar_KSKK_DDSB.CombinationCut = str_cutsDstarCombDD

# Base cuts (applied to both signal box and sidebands):
strDstarMotherCut_pp = '(PT > 3400*MeV)'
strDstarMotherCut_Kp = '(PT > 3400*MeV)'
strDstarMotherCut_KK = '(PT > 3200*MeV)'
strDstarMotherCut_LL = '(CHILDIP(1) < 0.02*mm) & (CHILDIP(2) < 0.5*mm)'
strDstarMotherCut_DD = '(CHILDIP(1) < 0.05*mm) & (CHILDIP(2) < 0.5*mm)'

# DM signal window:
str_cutsDstarMoth = '(MM - CHILD(MM,1) - CHILD(MM,2) > 3.35*MeV) & (MM - CHILD(MM,1) - CHILD(MM,2) < 8.35*MeV)'
# Mother cut for DM signal window (tighter):
Dstar_KSpp_LL.MotherCut = strDstarMotherCut_pp + ' & ' + strDstarMotherCut_LL + ' & ' + str_cutsDstarMoth
Dstar_KSKp_LL.MotherCut = strDstarMotherCut_Kp + ' & ' + strDstarMotherCut_LL + ' & ' + str_cutsDstarMoth
Dstar_KSKK_LL.MotherCut = strDstarMotherCut_KK + ' & ' + strDstarMotherCut_LL + ' & ' + str_cutsDstarMoth
Dstar_KSpp_DD.MotherCut = strDstarMotherCut_pp + ' & ' + strDstarMotherCut_DD + ' & ' + str_cutsDstarMoth
Dstar_KSKp_DD.MotherCut = strDstarMotherCut_Kp + ' & ' + strDstarMotherCut_DD + ' & ' + str_cutsDstarMoth
Dstar_KSKK_DD.MotherCut = strDstarMotherCut_KK + ' & ' + strDstarMotherCut_DD + ' & ' + str_cutsDstarMoth

# DM signal window + sidebands:
str_cutsDstar_DM_wide = '(MM - CHILD(MM,1) - CHILD(MM,2) > 0.0*MeV) & (MM - CHILD(MM,1) - CHILD(MM,2) < 13.35*MeV)'
# DM sidebands only:
str_cutsDstar_DM_lowerSB = '(MM - CHILD(MM,1) - CHILD(MM,2) > 0.0*MeV) & (MM - CHILD(MM,1) - CHILD(MM,2) < 3.35*MeV)'
str_cutsDstar_DM_upperSB = '(MM - CHILD(MM,1) - CHILD(MM,2) > 8.35*MeV) & (MM - CHILD(MM,1) - CHILD(MM,2) < 13.35*MeV)'
str_cutsDstar_DM_bothSB = "(" + str_cutsDstar_DM_lowerSB + ") | (" + str_cutsDstar_DM_upperSB + ")"

# M sidebands only (complicated, and uses truncated signal window to allow for overlap):
str_cutsDstarD2KSpp_LL_M_sidebands = "(CHILD(ADMASS('D0'),1) > " + str_cutppLL_truncated + ") & (CHILD(ADMASS('D0'),1) < " + str_cutppLLSB + ")"
str_cutsDstarD2KSKp_LL_M_sidebands = "(CHILD(ADMASS('D0'),1) > " + str_cutKpLL_truncated + ") & (CHILD(ADMASS('D0'),1) < " + str_cutKpLLSB + ")"
str_cutsDstarD2KSKK_LL_M_sidebands = "(CHILD(ADMASS('D0'),1) > " + str_cutKKLL_truncated + ") & (CHILD(ADMASS('D0'),1) < " + str_cutKKLLSB + ")"
str_cutsDstarD2KSpp_DD_M_sidebands = "(CHILD(ADMASS('D0'),1) > " + str_cutppDD_truncated + ") & (CHILD(ADMASS('D0'),1) < " + str_cutppDDSB + ")"
str_cutsDstarD2KSKp_DD_M_sidebands = "(CHILD(ADMASS('D0'),1) > " + str_cutKpDD_truncated + ") & (CHILD(ADMASS('D0'),1) < " + str_cutKpDDSB + ")"
str_cutsDstarD2KSKK_DD_M_sidebands = "(CHILD(ADMASS('D0'),1) > " + str_cutKKDD_truncated + ") & (CHILD(ADMASS('D0'),1) < " + str_cutKKDDSB + ")"

# Combined cuts for 2D sidebands excluding signal box: [wide M & DM sidebands] + [M sidebands & DM signalbox]
# Wide M cut is implicit -- don't need it explicitly
str_cutsDstarMothSB_ppLL ="(" + str_cutsDstar_DM_bothSB + ") | ((" + str_cutsDstarMoth + ") & (" + str_cutsDstarD2KSpp_LL_M_sidebands + "))"
str_cutsDstarMothSB_KpLL ="(" + str_cutsDstar_DM_bothSB + ") | ((" + str_cutsDstarMoth + ") & (" + str_cutsDstarD2KSKp_LL_M_sidebands + "))"
str_cutsDstarMothSB_KKLL ="(" + str_cutsDstar_DM_bothSB + ") | ((" + str_cutsDstarMoth + ") & (" + str_cutsDstarD2KSKK_LL_M_sidebands + "))"
str_cutsDstarMothSB_ppDD ="(" + str_cutsDstar_DM_bothSB + ") | ((" + str_cutsDstarMoth + ") & (" + str_cutsDstarD2KSpp_DD_M_sidebands + "))"
str_cutsDstarMothSB_KpDD ="(" + str_cutsDstar_DM_bothSB + ") | ((" + str_cutsDstarMoth + ") & (" + str_cutsDstarD2KSKp_DD_M_sidebands + "))"
str_cutsDstarMothSB_KKDD ="(" + str_cutsDstar_DM_bothSB + ") | ((" + str_cutsDstarMoth + ") & (" + str_cutsDstarD2KSKK_DD_M_sidebands + "))"

# Mother cut for sidebands:
Dstar_KSpp_LLSB.MotherCut = strDstarMotherCut_pp + ' & ' + strDstarMotherCut_LL + ' & (' + str_cutsDstarMothSB_ppLL + ')'
Dstar_KSKp_LLSB.MotherCut = strDstarMotherCut_Kp + ' & ' + strDstarMotherCut_LL + ' & (' + str_cutsDstarMothSB_KpLL + ')'
Dstar_KSKK_LLSB.MotherCut = strDstarMotherCut_KK + ' & ' + strDstarMotherCut_LL + ' & (' + str_cutsDstarMothSB_KKLL + ')'
Dstar_KSpp_DDSB.MotherCut = strDstarMotherCut_pp + ' & ' + strDstarMotherCut_DD + ' & (' + str_cutsDstarMothSB_ppDD + ')'
Dstar_KSKp_DDSB.MotherCut = strDstarMotherCut_Kp + ' & ' + strDstarMotherCut_DD + ' & (' + str_cutsDstarMothSB_KpDD + ')'
Dstar_KSKK_DDSB.MotherCut = strDstarMotherCut_KK + ' & ' + strDstarMotherCut_DD + ' & (' + str_cutsDstarMothSB_KKDD + ')'

# Now create a set of StrippingLine objects with this selection.
# The signal lives here -- please don't prescale.

line1 = StrippingLine('DstarD2KSPiPi_KSLLRobust', prescale = 1.0, algos = [KSLL, D2KSpp_LL, FilterD2KSpp_LL, Conjugate_D2KSpp_LL, Dstar_KSpp_LL])
line2 = StrippingLine('DstarD2KSKPi_KSLLRobust' , prescale = 1.0, algos = [KSLL, D2KSKp_LL, FilterD2KSKp_LL, Conjugate_D2KSKp_LL, Dstar_KSKp_LL])
line3 = StrippingLine('DstarD2KSKK_KSLLRobust'  , prescale = 1.0, algos = [KSLL, D2KSKK_LL, FilterD2KSKK_LL, Conjugate_D2KSKK_LL, Dstar_KSKK_LL])
line4 = StrippingLine('DstarD2KSPiPi_KSDDRobust', prescale = 1.0, algos = [KSDD, D2KSpp_DD, FilterD2KSpp_DD, Conjugate_D2KSpp_DD, Dstar_KSpp_DD])
line5 = StrippingLine('DstarD2KSKPi_KSDDRobust' , prescale = 1.0, algos = [KSDD, D2KSKp_DD, FilterD2KSKp_DD, Conjugate_D2KSKp_DD, Dstar_KSKp_DD])
line6 = StrippingLine('DstarD2KSKK_KSDDRobust'  , prescale = 1.0, algos = [KSDD, D2KSKK_DD, FilterD2KSKK_DD, Conjugate_D2KSKK_DD, Dstar_KSKK_DD])

# Equivalent sideband stripping lines. These can be prescaled safely.
# I've set the prescale to 0.2 for now -- this can be tuned depending
# on how much bandwidth is available, but is probably a reasonable
# starting point.

line7  = StrippingLine('DstarD2KSPiPi_KSLLRobust_SB', prescale = 0.2, algos = [KSLL, D2KSpp_LL, Conjugate_D2KSpp_LLSB, Dstar_KSpp_LLSB])
line8  = StrippingLine('DstarD2KSKPi_KSLLRobust_SB' , prescale = 0.2, algos = [KSLL, D2KSKp_LL, Conjugate_D2KSKp_LLSB, Dstar_KSKp_LLSB])
line9  = StrippingLine('DstarD2KSKK_KSLLRobust_SB'  , prescale = 0.2, algos = [KSLL, D2KSKK_LL, Conjugate_D2KSKK_LLSB, Dstar_KSKK_LLSB])
line10 = StrippingLine('DstarD2KSPiPi_KSDDRobust_SB', prescale = 0.2, algos = [KSDD, D2KSpp_DD, Conjugate_D2KSpp_DDSB, Dstar_KSpp_DDSB])
line11 = StrippingLine('DstarD2KSKPi_KSDDRobust_SB' , prescale = 0.2, algos = [KSDD, D2KSKp_DD, Conjugate_D2KSKp_DDSB, Dstar_KSKp_DDSB])
line12 = StrippingLine('DstarD2KSKK_KSDDRobust_SB'  , prescale = 0.2, algos = [KSDD, D2KSKK_DD, Conjugate_D2KSKK_DDSB, Dstar_KSKK_DDSB])
