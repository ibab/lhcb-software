

__author__ = 'Susan Haines'
__date__ = '20/08/2009'
__version__ = '$Revision: 1.2 $'

'''
Bu->D0(KSPiPi)K stripping selection using LoKi::Hybrid and python
configurables.
'''

from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from Configurables import FilterDesktop, CombineParticles
import GaudiKernel.SystemOfUnits as Units

###########################
# KS, long long daughters #
###########################
KSLLForBu2D0K_D02KSPiPi = FilterDesktop("StripKSLLForBu2D0K_D02KSPiPi")
KSLLForBu2D0K_D02KSPiPi.InputLocations = ["StdLooseKsLL"]
KSLLForBu2D0K_D02KSPiPi.Code = "(MINTREE( ('pi+'==ABSID), P) >2.*GeV)"\
                             "& (MINTREE( ('pi+'==ABSID), MIPDV(PRIMARY))>0.15)"\
                             "& (MAXTREE( ('pi+'==ABSID), TRCHI2DOF) <7.)"\
                             "& (MINTREE( ('pi+'==ABSID), PIDp) >-1000.)"\
                             "& (MINTREE( ('pi+'==ABSID), PIDK) >-1000.)"\
                             "& (VFASPF(VCHI2/VDOF)<17.5)"\
                             "& (BPVVD> 17.5)"\
                             "& (ADMASS('KS0') < 35.*MeV)"\
                             "& (2==NINGENERATION(('pi+'==ABSID)&((PIDp < -1e-10)|(PIDp > 1e-10)),1))"\
                             "& (2==NINGENERATION(('pi+'==ABSID)&((PIDK < -1e-10)|(PIDK > 1e-10)),1))"


#######################################
# KS, downstream downstream daughters #
#######################################
KSDDForBu2D0K_D02KSPiPi = FilterDesktop("StripKSDDForBu2D0K_D02KSPiPi")
KSDDForBu2D0K_D02KSPiPi.InputLocations = ["StdLooseKsDD"]
KSDDForBu2D0K_D02KSPiPi.Code = "(MINTREE( ('pi+'==ABSID), P) >2.*GeV)"\
                             "& (MINTREE( ('pi+'==ABSID), MIPDV(PRIMARY))>0.8)"\
                             "& (MAXTREE( ('pi+'==ABSID), TRCHI2DOF) <7.)"\
                             "& (MINTREE( ('pi+'==ABSID), PIDp) >-1000.)"\
                             "& (MINTREE( ('pi+'==ABSID), PIDK) >-1000.)"\
                             "& (VFASPF(VCHI2/VDOF)<17.5)"\
                             "& (BPVVD> 175.)"\
                             "& (ADMASS('KS0') < 64.*MeV)"\
                             "& (2==NINGENERATION(('pi+'==ABSID)&((PIDp < -1e-10)|(PIDp > 1e-10)),1))"\
                             "& (2==NINGENERATION(('pi+'==ABSID)&((PIDK < -1e-10)|(PIDK > 1e-10)),1))"


###########################
# D0, with KS LL daughter #
###########################
DForBu2D0K_D02KSPiPi_KSLL = CombineParticles("StripDForBu2D0K_D02KSPiPi_KSLL")
DForBu2D0K_D02KSPiPi_KSLL.DecayDescriptor = "[D0 -> KS0 pi+ pi-]cc"

DForBu2D0K_D02KSPiPi_KSLL.InputLocations =  ["StdLoosePions",
                                             "StripKSLLForBu2D0K_D02KSPiPi"
                                             ]

DForBu2D0K_D02KSPiPi_KSLL.DaughtersCuts  = {"pi+" : "(PT>350.*MeV) & ((PIDK <-1e-10)|(PIDK >1e-10)) & ((PIDp <-1e-10)|(PIDp > 1e-10)) & (PIDK > -1000.0) & (PIDp > -1000.0) & (TRCHI2DOF<7.)" 
                                           ,"KS0" : "(PT > 1.*GeV)"
                                           }
DForBu2D0K_D02KSPiPi_KSLL.CombinationCut =  "(ADAMASS('D0')<100.*MeV) & (APT>900.*MeV)" 
DForBu2D0K_D02KSPiPi_KSLL.MotherCut      =  "(ADMASS('D0')<80.*MeV) & (VFASPF(VCHI2/VDOF)<9.) & (PT>1.8*GeV) & (MIPDV(PRIMARY)>0.05)"



###########################
# D0, with KS DD daughter #
###########################
DForBu2D0K_D02KSPiPi_KSDD = CombineParticles("StripDForBu2D0K_D02KSPiPi_KSDD")
DForBu2D0K_D02KSPiPi_KSDD.DecayDescriptor = "[D0 -> KS0 pi+ pi-]cc"

DForBu2D0K_D02KSPiPi_KSDD.InputLocations =  ["StdLoosePions",
                                            "StripKSDDForBu2D0K_D02KSPiPi"
                                            ]

DForBu2D0K_D02KSPiPi_KSDD.DaughtersCuts  = {"pi+" : "(PT>350.*MeV) & ((PIDK <-1e-10)|(PIDK >1e-10)) & ((PIDp <-1e-10)|(PIDp > 1e-10)) & (PIDK > -1000.0) & (PIDp > -1000.0) & (TRCHI2DOF<7.)" 
                                           ,"KS0" : "(PT > 1.*GeV)"
                                           }
DForBu2D0K_D02KSPiPi_KSDD.CombinationCut =  "(ADAMASS('D0')<100.*MeV) & (APT>900.*MeV)" 
DForBu2D0K_D02KSPiPi_KSDD.MotherCut      =  "(ADMASS('D0')<80.*MeV) & (VFASPF(VCHI2/VDOF)<9.) & (PT>1.8*GeV) & (MIPDV(PRIMARY)>0.05)"


################
# B, LL KS     #
################
Bu2D0K_KSLL = CombineParticles("StripBu2D0K_KSLL")
Bu2D0K_KSLL.DecayDescriptor = "[B+ -> D~0 K+]cc"
Bu2D0K_KSLL.InputLocations = ["StripDForBu2D0K_D02KSPiPi_KSLL",
                              "StdLooseKaons"]


Bu2D0K_KSLL.DaughtersCuts = { "D0" : "ALL",
                              "K+" : "(PT>0.6*GeV) & (MIPDV(PRIMARY)>0.05) & ((PIDK < -1e-10)|(PIDK > 1e-10)) & ((PIDp < -1e-10)|(PIDp > 1e-10)) & (PIDK > -1000.0) & (PIDp > -1000.0)  & (TRCHI2DOF<7.)"
                            }
 
Bu2D0K_KSLL.CombinationCut =  "(ADAMASS('B+')<150.*MeV)"
Bu2D0K_KSLL.MotherCut = "(ADMASS('B+')<130.*MeV)  & (VFASPF(VCHI2/VDOF)<9.) & (BPVDIRA>0.99975) & (MIPDV(PRIMARY)< 0.1) & (BPVVD>1.7)"


################
# B, DD KS     #
################
Bu2D0K_KSDD = CombineParticles("StripBu2D0K_KSDD")
Bu2D0K_KSDD.DecayDescriptor = "[B+ -> D~0 K+]cc"
Bu2D0K_KSDD.InputLocations = ["StripDForBu2D0K_D02KSPiPi_KSDD",
                             "StdLooseKaons"]


Bu2D0K_KSDD.DaughtersCuts = { "D0" : "ALL",
                              "K+" : "(PT>0.6*GeV) & (MIPDV(PRIMARY)>0.05) & ((PIDK < -1e-10)|(PIDK > 1e-10)) & ((PIDp < -1e-10)|(PIDp > 1e-10)) & (PIDK > -1000.0) & (PIDp > -1000.0)  & (TRCHI2DOF<7.)"
                           }
 
Bu2D0K_KSDD.CombinationCut =  "(ADAMASS('B+')<160.*MeV)"
Bu2D0K_KSDD.MotherCut = "(ADMASS('B+')<145.*MeV)  & (VFASPF(VCHI2/VDOF)<9.) & (BPVDIRA>0.99975) & (MIPDV(PRIMARY)< 0.1) & (BPVVD>1.)"


############################################
# Create StrippingLine with this selection #
############################################
Bu2D0K_D02KSPiPi_KSLL_Line = StrippingLine('Bu2D0K_D02KSPiPi_KSLL'
               , prescale = 1
               , algos = [KSLLForBu2D0K_D02KSPiPi, DForBu2D0K_D02KSPiPi_KSLL, Bu2D0K_KSLL]
               , postscale = 1
               , stream = 'Hadron'
               )
Bu2D0K_D02KSPiPi_KSDD_Line = StrippingLine('Bu2D0K_D02KSPiPi_KSDD'
               , prescale = 1
               , algos = [KSDDForBu2D0K_D02KSPiPi, DForBu2D0K_D02KSPiPi_KSDD, Bu2D0K_KSDD]
               , postscale = 1
               , stream = 'Hadron'
               )
