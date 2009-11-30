## #####################################################################
# $Id: StrippingDstarD2KShh.py,v 1.1 2009-11-30 21:19:31 mcharles Exp $
#
# A general stripping selection for D*+ -> D0(Ks h+ h-) pi+
#
# This includes stripping selections for several related decay modes:
#   D*+ -> D0 pi+, D0 -> KS pi+ pi-
#   D*+ -> D0 pi+, D0 -> KS K+ pi- (or KS K- pi+)
#   D*+ -> D0 pi+, D0 -> KS K+ K-
# and includes variants where the KS is reconstructed as long+long or
# as downstream+downstream. The cuts are in general different from
# mode to mode, and between KS reconstructed as LL or DD.
#
# The selection also includes a sideband variant which has bigger
# mass windows and a more severe prescale. Except for the mass cuts
# (mass of D0, mass difference between D*+ and D0), all cuts are the
# same for the signal and sideband variants by construction. We do
# not use the StrippingMember technology for this, though, because
# StrippingMembers can only be FilterDesktop or CombineParticles
# modules, whereas we need to have a ConjugateNeutralPID module
# in the middle of the sequence. This cannot take the output of
# a StrippingMember as its input or else the code will break.
# 
# (We could in principle use non-StrippingMember modules up to
# that point, and then StrippingMember modules afterwards. But
# I think it's more confusing/dangerous to have the mass window
# in one variable controlled by a hard-coded cut and the mass
# window in another variable controlled by a StrippingLine knob...)
#
# @author Mat Charles <m.charles1@physics.ox.ac.uk>
# @date 2009-11-26
#
## #####################################################################

from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember, bindMembers

from CommonParticles.StdNoPIDsPions import StdNoPIDsPions
from CommonParticles.StdNoPIDsKaons import StdNoPIDsKaons

# Make the Ks. For examples on how to put in cuts, see StrippingBu2D0K_D02KSPiPi.py

KSLL = FilterDesktop("StripKSLLForDstarD2KShh")
KSLL.InputLocations = ["StdLooseKsLL"]
KSLL.Code = "(BPVVDZ>-1000.0*mm) & (BPVVDZ<650.0*mm) & (BPVDIRA > 0.9997) & (ADMASS('KS0')<11.4*MeV) & (BPVVDCHI2>100)"

KSDD = FilterDesktop("StripKSDDForDstarD2KShh")
KSDD.InputLocations = ["StdLooseKsDD"]
KSDD.Code = "(BPVVDZ>0.0*mm) & (BPVVDZ<2300.0*mm) & (BPVDIRA > 0.99995) & (ADMASS('KS0')<24.9*MeV) & (BPVVDCHI2>100)"

# Make the D0 -> KS h+ h- candidates.
# There are several permutations here, since each h can be K or pi.
# Also, we keep DD and LL separate since their mass resolutions are
# pretty different.
#
# Note that there is an inherent ambiguity in the flavour of the D.
# We force the particle to be reconstructed as D0 (not D0bar) in each case.
# This happens automatically for self-conjugate modes (e.g. KS pi+ pi-)
# needs a little care for (KS K+ pi-) vs (KS K- pi+).

# Selection here is marked as "SB" (sidebands) since it'll use a wide
# mass window that includes signal box + sidebands. We'll make a second
# pass below to tighten it.

D2KSpp_LL = CombineParticles("StripD2KSppLLForDstarD2KShhSB")
D2KSKp_LL = CombineParticles("StripD2KSKpLLForDstarD2KShhSB")
D2KSKK_LL = CombineParticles("StripD2KSKKLLForDstarD2KShhSB")
D2KSpp_DD = CombineParticles("StripD2KSppDDForDstarD2KShhSB")
D2KSKp_DD = CombineParticles("StripD2KSKpDDForDstarD2KShhSB")
D2KSKK_DD = CombineParticles("StripD2KSKKDDForDstarD2KShhSB")

D2KSpp_LL.DecayDescriptor  = "D0 -> KS0 pi+ pi-"
D2KSKp_LL.DecayDescriptors = ["D0 -> KS0 K+ pi-", "D0 -> KS0 K- pi+"]
D2KSKK_LL.DecayDescriptor  = "D0 -> KS0 K+ K-"
D2KSpp_DD.DecayDescriptor  = "D0 -> KS0 pi+ pi-"
D2KSKp_DD.DecayDescriptors = ["D0 -> KS0 K+ pi-", "D0 -> KS0 K- pi+"]
D2KSKK_DD.DecayDescriptor  = "D0 -> KS0 K+ K-"

D2KSpp_LL.InputLocations = [ "StripKSLLForDstarD2KShh", "StdLoosePions" ]
D2KSKp_LL.InputLocations = [ "StripKSLLForDstarD2KShh", "StdLooseKaons", "StdLoosePions" ]
D2KSKK_LL.InputLocations = [ "StripKSLLForDstarD2KShh", "StdLooseKaons" ]
D2KSpp_DD.InputLocations = [ "StripKSDDForDstarD2KShh", "StdLoosePions" ]
D2KSKp_DD.InputLocations = [ "StripKSDDForDstarD2KShh", "StdLooseKaons", "StdLoosePions" ]
D2KSKK_DD.InputLocations = [ "StripKSDDForDstarD2KShh", "StdLooseKaons" ]

str_dauCutsPi = '(P>1500.0) & (PIDe-PIDpi<15.0) & (PIDp-PIDpi<20.0) & (PIDK -PIDpi<10.0) & (TRPCHI2>0.00001)'
str_dauCutsK  = '(P>1500.0) & (PIDe-PIDK <15.0) & (PIDp-PIDK <20.0) & (PIDpi-PIDK <10.0)'
D2KSpp_LL.DaughtersCuts = { "pi+" : str_dauCutsPi }
D2KSKp_LL.DaughtersCuts = { "pi+" : str_dauCutsPi, "K+" : str_dauCutsK }
D2KSKK_LL.DaughtersCuts = { "K+"  : str_dauCutsK }
D2KSpp_DD.DaughtersCuts = { "pi+" : str_dauCutsPi }
D2KSKp_DD.DaughtersCuts = { "pi+" : str_dauCutsPi, "K+" : str_dauCutsK }
D2KSKK_DD.DaughtersCuts = { "K+"  : str_dauCutsK }

strDaughterIPCHI2Cut_p = "(AHASCHILD((ABSID=='pi+') & (MIPCHI2DV(PRIMARY)>44.0)))"
strDaughterIPCHI2Cut_k = "(AHASCHILD((ABSID=='K+') & (MIPCHI2DV(PRIMARY)>14.0)))"
strDaughterIPCHI2Cut_kp = "((AHASCHILD((ABSID=='pi+') & (MIPCHI2DV(PRIMARY)>20.0))) | (AHASCHILD((ABSID=='K+') & (MIPCHI2DV(PRIMARY)>20.0))))"
strCombMassCutLL = "(ADAMASS('D0')<100*MeV)"
strCombMassCutDD = "(ADAMASS('D0')<200*MeV)"
D2KSpp_LL.CombinationCut = strCombMassCutLL + " & " + strDaughterIPCHI2Cut_p
D2KSKp_LL.CombinationCut = strCombMassCutLL + " & " + strDaughterIPCHI2Cut_kp
D2KSKK_LL.CombinationCut = strCombMassCutLL + " & " + strDaughterIPCHI2Cut_k
D2KSpp_DD.CombinationCut = strCombMassCutDD + " & " + strDaughterIPCHI2Cut_p
D2KSKp_DD.CombinationCut = strCombMassCutDD + " & " + strDaughterIPCHI2Cut_kp
D2KSKK_DD.CombinationCut = strCombMassCutDD + " & " + strDaughterIPCHI2Cut_k

# Mass cut for signal is roughly +- 2.5 sigma:
#   * sigma for D0 -> KS(LL) pi+ pi-  ~ 11 MeV
#   * sigma for D0 -> KS(DD) pi+ pi-  ~ 14 MeV
#   * sigma for D0 -> KS(LL) K+ K-    ~ 5.7 MeV
#   * sigma for D0 -> KS(DD) K+ K-    ~ 7.2 MeV
# I don't have resolutions for the (KS K pi) modes, so they're guesses.
#
# We will also make a larger sideband window which is roughly +- 7.5 sigma
# and prescale it. To keep combinatorics down, we will:
#   1) Make the sideband (i.e. looser) mass cut in CombineParticles as a MotherCut
#   2) Immediately apply a FilterDesktop with the signal box (i.e. tighter) mass cut
#   3) Come back and clone the StrippingLine later, replacing the FilterDesktop cut with the sideband (looser) mass cut.
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


# MotherCut uses loose mass cut only, as described above.
str_DMotherCut_base  = '(BPVVDZ<7000.0*mm) & (BPVDIRA > 0.995)'
str_DMotherCutpp = str_DMotherCut_base + ' & (VFASPF(VCHI2)<13.0)'
str_DMotherCutKp = str_DMotherCut_base + ' & (VFASPF(VCHI2)<15.0)'
str_DMotherCutKK = str_DMotherCut_base + ' & (VFASPF(VCHI2)<21.0)'
D2KSpp_LL.MotherCut = str_DMotherCutpp + ' & ' + str_massCutD2KSpp_LLSB
D2KSKp_LL.MotherCut = str_DMotherCutKp + ' & ' + str_massCutD2KSKp_LLSB
D2KSKK_LL.MotherCut = str_DMotherCutKK + ' & ' + str_massCutD2KSKK_LLSB
D2KSpp_DD.MotherCut = str_DMotherCutpp + ' & ' + str_massCutD2KSpp_DDSB
D2KSKp_DD.MotherCut = str_DMotherCutKp + ' & ' + str_massCutD2KSKp_DDSB
D2KSKK_DD.MotherCut = str_DMotherCutKK + ' & ' + str_massCutD2KSKK_DDSB

# Apply a tighter filter to isolate the signal window:
FilterD2KSpp_LL   = FilterDesktop("StripD2KSppLLForDstarD2KShh",   InputLocations = ["StripD2KSppLLForDstarD2KShhSB"], Code = str_massCutD2KSpp_LL)
FilterD2KSKp_LL   = FilterDesktop("StripD2KSKpLLForDstarD2KShh",   InputLocations = ["StripD2KSKpLLForDstarD2KShhSB"], Code = str_massCutD2KSKp_LL)
FilterD2KSKK_LL   = FilterDesktop("StripD2KSKKLLForDstarD2KShh",   InputLocations = ["StripD2KSKKLLForDstarD2KShhSB"], Code = str_massCutD2KSKK_LL)
FilterD2KSpp_DD   = FilterDesktop("StripD2KSppDDForDstarD2KShh",   InputLocations = ["StripD2KSppDDForDstarD2KShhSB"], Code = str_massCutD2KSpp_DD)
FilterD2KSKp_DD   = FilterDesktop("StripD2KSKpDDForDstarD2KShh",   InputLocations = ["StripD2KSKpDDForDstarD2KShhSB"], Code = str_massCutD2KSKp_DD)
FilterD2KSKK_DD   = FilterDesktop("StripD2KSKKDDForDstarD2KShh",   InputLocations = ["StripD2KSKKDDForDstarD2KShhSB"], Code = str_massCutD2KSKK_DD)

# Clone the D0 candidates to make D0bar candidates.
# This is needed so that we can later make either of
#    D*+ -> D0 pi+
#    D*- -> D0bar pi-
# from the SAME D0 -> KS pi+ pi- (or whatever) combination.

from Configurables import ConjugateNeutralPID
# Signal box:
Conjugate_D2KSpp_LL = ConjugateNeutralPID("StripDbar2KSppLLForDstarD2KShh")
Conjugate_D2KSKp_LL = ConjugateNeutralPID("StripDbar2KSKpLLForDstarD2KShh")
Conjugate_D2KSKK_LL = ConjugateNeutralPID("StripDbar2KSKKLLForDstarD2KShh")
Conjugate_D2KSpp_DD = ConjugateNeutralPID("StripDbar2KSppDDForDstarD2KShh")
Conjugate_D2KSKp_DD = ConjugateNeutralPID("StripDbar2KSKpDDForDstarD2KShh")
Conjugate_D2KSKK_DD = ConjugateNeutralPID("StripDbar2KSKKDDForDstarD2KShh")
# Wide mass window (sideband):
Conjugate_D2KSpp_LLSB = ConjugateNeutralPID("StripDbar2KSppLLForDstarD2KShhSB")
Conjugate_D2KSKp_LLSB = ConjugateNeutralPID("StripDbar2KSKpLLForDstarD2KShhSB")
Conjugate_D2KSKK_LLSB = ConjugateNeutralPID("StripDbar2KSKKLLForDstarD2KShhSB")
Conjugate_D2KSpp_DDSB = ConjugateNeutralPID("StripDbar2KSppDDForDstarD2KShhSB")
Conjugate_D2KSKp_DDSB = ConjugateNeutralPID("StripDbar2KSKpDDForDstarD2KShhSB")
Conjugate_D2KSKK_DDSB = ConjugateNeutralPID("StripDbar2KSKKDDForDstarD2KShhSB")
# Signal box:
Conjugate_D2KSpp_LL.InputLocations = [ "StripD2KSppLLForDstarD2KShh" ]
Conjugate_D2KSKp_LL.InputLocations = [ "StripD2KSKpLLForDstarD2KShh" ]
Conjugate_D2KSKK_LL.InputLocations = [ "StripD2KSKKLLForDstarD2KShh" ]
Conjugate_D2KSpp_DD.InputLocations = [ "StripD2KSppDDForDstarD2KShh" ]
Conjugate_D2KSKp_DD.InputLocations = [ "StripD2KSKpDDForDstarD2KShh" ]
Conjugate_D2KSKK_DD.InputLocations = [ "StripD2KSKKDDForDstarD2KShh" ]
# Wide mass window (sideband):
Conjugate_D2KSpp_LLSB.InputLocations = [ "StripD2KSppLLForDstarD2KShhSB" ]
Conjugate_D2KSKp_LLSB.InputLocations = [ "StripD2KSKpLLForDstarD2KShhSB" ]
Conjugate_D2KSKK_LLSB.InputLocations = [ "StripD2KSKKLLForDstarD2KShhSB" ]
Conjugate_D2KSpp_DDSB.InputLocations = [ "StripD2KSppDDForDstarD2KShhSB" ]
Conjugate_D2KSKp_DDSB.InputLocations = [ "StripD2KSKpDDForDstarD2KShhSB" ]
Conjugate_D2KSKK_DDSB.InputLocations = [ "StripD2KSKKDDForDstarD2KShhSB" ]

# Reconstruct D*+ -> D0 pi+  (+cc)

# Signal box:
Dstar_KSpp_LL   = CombineParticles("StripDstar2D2KSppLLForDstarD2KShh");
Dstar_KSKp_LL   = CombineParticles("StripDstar2D2KSKpLLForDstarD2KShh");
Dstar_KSKK_LL   = CombineParticles("StripDstar2D2KSKKLLForDstarD2KShh");
Dstar_KSpp_DD   = CombineParticles("StripDstar2D2KSppDDForDstarD2KShh");
Dstar_KSKp_DD   = CombineParticles("StripDstar2D2KSKpDDForDstarD2KShh");
Dstar_KSKK_DD   = CombineParticles("StripDstar2D2KSKKDDForDstarD2KShh");
# Wide mass windows (sidebands):
Dstar_KSpp_LLSB = CombineParticles("StripDstar2D2KSppLLForDstarD2KShhSB");
Dstar_KSKp_LLSB = CombineParticles("StripDstar2D2KSKpLLForDstarD2KShhSB");
Dstar_KSKK_LLSB = CombineParticles("StripDstar2D2KSKKLLForDstarD2KShhSB");
Dstar_KSpp_DDSB = CombineParticles("StripDstar2D2KSppDDForDstarD2KShhSB");
Dstar_KSKp_DDSB = CombineParticles("StripDstar2D2KSKpDDForDstarD2KShhSB");
Dstar_KSKK_DDSB = CombineParticles("StripDstar2D2KSKKDDForDstarD2KShhSB");

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
Dstar_KSpp_LL.InputLocations = [ "StripD2KSppLLForDstarD2KShh", "StripDbar2KSppLLForDstarD2KShh", "StdLoosePions" ]
Dstar_KSKp_LL.InputLocations = [ "StripD2KSKpLLForDstarD2KShh", "StripDbar2KSKpLLForDstarD2KShh", "StdLoosePions" ]
Dstar_KSKK_LL.InputLocations = [ "StripD2KSKKLLForDstarD2KShh", "StripDbar2KSKKLLForDstarD2KShh", "StdLoosePions" ]
Dstar_KSpp_DD.InputLocations = [ "StripD2KSppDDForDstarD2KShh", "StripDbar2KSppDDForDstarD2KShh", "StdLoosePions" ]
Dstar_KSKp_DD.InputLocations = [ "StripD2KSKpDDForDstarD2KShh", "StripDbar2KSKpDDForDstarD2KShh", "StdLoosePions" ]
Dstar_KSKK_DD.InputLocations = [ "StripD2KSKKDDForDstarD2KShh", "StripDbar2KSKKDDForDstarD2KShh", "StdLoosePions" ]
# Wide mass windows (sidebands):
Dstar_KSpp_LLSB.InputLocations = [ "StripD2KSppLLForDstarD2KShhSB", "StripDbar2KSppLLForDstarD2KShhSB", "StdLoosePions" ]
Dstar_KSKp_LLSB.InputLocations = [ "StripD2KSKpLLForDstarD2KShhSB", "StripDbar2KSKpLLForDstarD2KShhSB", "StdLoosePions" ]
Dstar_KSKK_LLSB.InputLocations = [ "StripD2KSKKLLForDstarD2KShhSB", "StripDbar2KSKKLLForDstarD2KShhSB", "StdLoosePions" ]
Dstar_KSpp_DDSB.InputLocations = [ "StripD2KSppDDForDstarD2KShhSB", "StripDbar2KSppDDForDstarD2KShhSB", "StdLoosePions" ]
Dstar_KSKp_DDSB.InputLocations = [ "StripD2KSKpDDForDstarD2KShhSB", "StripDbar2KSKpDDForDstarD2KShhSB", "StdLoosePions" ]
Dstar_KSKK_DDSB.InputLocations = [ "StripD2KSKKDDForDstarD2KShhSB", "StripDbar2KSKKDDForDstarD2KShhSB", "StdLoosePions" ]

# Leave off pointing cut to retain secondaries. If you want to add it back, it's:  (MIPCHI2DV(PRIMARY)<25.0)
str_cutsDauSoftPi = '(PIDe-PIDpi<5.0)'
# Signal box
Dstar_KSpp_LL.DaughtersCuts = { "pi+" : str_cutsDauSoftPi }
Dstar_KSKp_LL.DaughtersCuts = { "pi+" : str_cutsDauSoftPi }
Dstar_KSKK_LL.DaughtersCuts = { "pi+" : str_cutsDauSoftPi }
Dstar_KSpp_DD.DaughtersCuts = { "pi+" : str_cutsDauSoftPi }
Dstar_KSKp_DD.DaughtersCuts = { "pi+" : str_cutsDauSoftPi }
Dstar_KSKK_DD.DaughtersCuts = { "pi+" : str_cutsDauSoftPi }
# Wide mass windows (sidebands):
Dstar_KSpp_LLSB.DaughtersCuts = { "pi+" : str_cutsDauSoftPi }
Dstar_KSKp_LLSB.DaughtersCuts = { "pi+" : str_cutsDauSoftPi }
Dstar_KSKK_LLSB.DaughtersCuts = { "pi+" : str_cutsDauSoftPi }
Dstar_KSpp_DDSB.DaughtersCuts = { "pi+" : str_cutsDauSoftPi }
Dstar_KSKp_DDSB.DaughtersCuts = { "pi+" : str_cutsDauSoftPi }
Dstar_KSKK_DDSB.DaughtersCuts = { "pi+" : str_cutsDauSoftPi }

# Take care when making the "combination" cut -- what we really want to cut on
# are m(D0) and DM=m(D*)-m(D0)-m(pi). If the combination cut is too tight it slices out
# diagonal regions in the m(D0) vs DM plane. We take this as roughly
#   (sideband window for M) + (sideband window for DM)
# plus a healthy bit of wiggle room.
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

# The DM signal box is roughly consistent between the different modes (between 0.75 and 1.0 MeV)
# and is centred on 5.85 - 5.95 MeV. (The nominal/PDG value is 5.85 +- 0.01 MeV.)
# For simplicity we take a common cut of 2.5 MeV about 5.85 MeV for the signal box, i.e. (3.35, 8.35).
# This is the same for all modes.
#
# We will also make a larger sideband box (and plan to prescale it). We won't take a completely
# symmmetric window because DM has a lower bound at zero. Instead, we'll use 
# (0, 5.85+7.5) = (0, 13.35) MeV.
#
# We want to exclude the signal box for the sideband selector, which is rather ugly in
# LOKI code because of the absence of negation. We do it like:
#   [m(D0) wide + DM sidebands excluding DM signal box] + [m(D0) sidebands only + DM signal box] 
#

# Base cuts (applied to both signal box and sidebands):
strDstarMotherCutBasepp = '(PT > 3400*MeV) & (VFASPF(VCHI2/VDOF)<20.0)'
strDstarMotherCutBaseKp = '(PT > 3400*MeV) & (VFASPF(VCHI2/VDOF)<20.0)'
strDstarMotherCutBaseKK = '(PT > 3200*MeV) & (VFASPF(VCHI2/VDOF)<60.0)'
# DM signal window:
str_cutsDstarMoth = '(MM - CHILD(MM,1) - CHILD(MM,2) > 3.35*MeV) & (MM - CHILD(MM,1) - CHILD(MM,2) < 8.35*MeV)'
# Mother cut for DM signal window (tighter):
Dstar_KSpp_LL.MotherCut   = strDstarMotherCutBasepp + ' & ' + str_cutsDstarMoth
Dstar_KSKp_LL.MotherCut   = strDstarMotherCutBaseKp + ' & ' + str_cutsDstarMoth
Dstar_KSKK_LL.MotherCut   = strDstarMotherCutBaseKK + ' & ' + str_cutsDstarMoth
Dstar_KSpp_DD.MotherCut   = strDstarMotherCutBasepp + ' & ' + str_cutsDstarMoth
Dstar_KSKp_DD.MotherCut   = strDstarMotherCutBaseKp + ' & ' + str_cutsDstarMoth
Dstar_KSKK_DD.MotherCut   = strDstarMotherCutBaseKK + ' & ' + str_cutsDstarMoth

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
Dstar_KSpp_LLSB.MotherCut = strDstarMotherCutBasepp + ' & (' + str_cutsDstarMothSB_ppLL + ')'
Dstar_KSKp_LLSB.MotherCut = strDstarMotherCutBaseKp + ' & (' + str_cutsDstarMothSB_KpLL + ')'
Dstar_KSKK_LLSB.MotherCut = strDstarMotherCutBaseKK + ' & (' + str_cutsDstarMothSB_KKLL + ')'
Dstar_KSpp_DDSB.MotherCut = strDstarMotherCutBasepp + ' & (' + str_cutsDstarMothSB_ppDD + ')'
Dstar_KSKp_DDSB.MotherCut = strDstarMotherCutBaseKp + ' & (' + str_cutsDstarMothSB_KpDD + ')'
Dstar_KSKK_DDSB.MotherCut = strDstarMotherCutBaseKK + ' & (' + str_cutsDstarMothSB_KKDD + ')'

# Now create a set of StrippingLine objects with this selection.
# These lines are signal-dominated -- please don't prescale them.

line1 = StrippingLine('DstarD2KSPiPi_KSLL', prescale = 1.0, algos = [KSLL, D2KSpp_LL, FilterD2KSpp_LL, Conjugate_D2KSpp_LL, Dstar_KSpp_LL])
line2 = StrippingLine('DstarD2KSKPi_KSLL' , prescale = 1.0, algos = [KSLL, D2KSKp_LL, FilterD2KSKp_LL, Conjugate_D2KSKp_LL, Dstar_KSKp_LL])
line3 = StrippingLine('DstarD2KSKK_KSLL'  , prescale = 1.0, algos = [KSLL, D2KSKK_LL, FilterD2KSKK_LL, Conjugate_D2KSKK_LL, Dstar_KSKK_LL])
line4 = StrippingLine('DstarD2KSPiPi_KSDD', prescale = 1.0, algos = [KSDD, D2KSpp_DD, FilterD2KSpp_DD, Conjugate_D2KSpp_DD, Dstar_KSpp_DD])
line5 = StrippingLine('DstarD2KSKPi_KSDD' , prescale = 1.0, algos = [KSDD, D2KSKp_DD, FilterD2KSKp_DD, Conjugate_D2KSKp_DD, Dstar_KSKp_DD])
line6 = StrippingLine('DstarD2KSKK_KSDD'  , prescale = 1.0, algos = [KSDD, D2KSKK_DD, FilterD2KSKK_DD, Conjugate_D2KSKK_DD, Dstar_KSKK_DD])

# Equivalent sideband stripping lines. These can be prescaled safely.
# I've set the prescale to 0.2 for now -- this can be tuned depending
# on how much bandwidth is available, but is probably a reasonable
# starting point.

line7  = StrippingLine('DstarD2KSPiPi_KSLL_SB', prescale = 0.2, algos = [KSLL, D2KSpp_LL, Conjugate_D2KSpp_LLSB, Dstar_KSpp_LLSB])
line8  = StrippingLine('DstarD2KSKPi_KSLL_SB' , prescale = 0.2, algos = [KSLL, D2KSKp_LL, Conjugate_D2KSKp_LLSB, Dstar_KSKp_LLSB])
line9  = StrippingLine('DstarD2KSKK_KSLL_SB'  , prescale = 0.2, algos = [KSLL, D2KSKK_LL, Conjugate_D2KSKK_LLSB, Dstar_KSKK_LLSB])
line10 = StrippingLine('DstarD2KSPiPi_KSDD_SB', prescale = 0.2, algos = [KSDD, D2KSpp_DD, Conjugate_D2KSpp_DDSB, Dstar_KSpp_DDSB])
line11 = StrippingLine('DstarD2KSKPi_KSDD_SB' , prescale = 0.2, algos = [KSDD, D2KSKp_DD, Conjugate_D2KSKp_DDSB, Dstar_KSKp_DDSB])
line12 = StrippingLine('DstarD2KSKK_KSDD_SB'  , prescale = 0.2, algos = [KSDD, D2KSKK_DD, Conjugate_D2KSKK_DDSB, Dstar_KSKK_DDSB])
