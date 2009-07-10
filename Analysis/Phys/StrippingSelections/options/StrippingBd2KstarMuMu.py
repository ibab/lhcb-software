# Bd->KstarMuMu selection
#
# See RDWG talk, 08.07.2009

from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from Configurables import CombineParticles, FilterDesktop

# Make the DiMuon, Now in a common particle, StdLooseDiMuon
#
#Strip_Penguin_Jpsi2MuMu = CombineParticles ( 'Strip_Penguin_Jpsi2MuMu' )
#Strip_Penguin_Jpsi2MuMu.InputLocations = [ "StdLooseMuons" ]
#Strip_Penguin_Jpsi2MuMu.DecayDescriptor = "J/psi(1S) -> mu+ mu-" 
#
#Strip_Penguin_Jpsi2MuMu.CombinationCut = "ADAMASS('J/psi(1S)')<3000*MeV"
#Strip_Penguin_Jpsi2MuMu.MotherCut = "(VFASPF(VCHI2/VDOF)<64)"
#
####

####
# Make the K*. Now in a common particle StdLooseDetachedKstar
#Strip_Penguin_Kstar2KPi = CombineParticles ( 'Strip_Penguin_Kstar2KPi' )#
#
#Strip_Penguin_Kstar2KPi.InputLocations = [ "StdLooseKaons", "StdLoosePions" ]
#Strip_Penguin_Kstar2KPi.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc" 
#Strip_Penguin_Kstar2KPi.DaughtersCuts = { "pi+" : "(MIPCHI2DV(PRIMARY)>2.25)",
#                                    "K+" : "(MIPCHI2DV(PRIMARY)>2.25)" } 
#Strip_Penguin_Kstar2KPi.CombinationCut = "(ADAMASS('K*(892)0')<300*MeV)"
#Strip_Penguin_Kstar2KPi.MotherCut = "(VFASPF(VCHI2/VDOF)<100) & (MIPCHI2DV(PRIMARY)>1.5)"


#
# Make the Bd. This is the common loosest set of cuts
# 
Strip_loose_Bd2KstarMuMu = CombineParticles("Strip_loose_Bd2KstarMuMu")

Strip_loose_Bd2KstarMuMu.InputLocations = ["StdLooseDiMuon", "StdLooseDetachedKst2Kpi"]
Strip_loose_Bd2KstarMuMu.DecayDescriptor = "[B0 -> K*(892)0 J/psi(1S)]cc"
Strip_loose_Bd2KstarMuMu.DaughtersCuts = {
    'K*(892)0':  ("(BPVVDZ>-50*mm) & "+
                  "(INTREE((ABSID=='pi+') & ((PIDpi-PIDmu)>-10) & ((PIDpi-PIDK)>-10))) &"+
                  "(INTREE((ABSID=='K+') & ((PIDK-PIDmu)>-10) & ((PIDK-PIDpi)>-5)))" ),
    'J/psi(1S)' : "(2 == NINTREE(((PIDmu-PIDpi)>-10) & ((PIDmu-PIDK)>-10) & (MIPCHI2DV(PRIMARY)>0)) )"
    }
Strip_loose_Bd2KstarMuMu.CombinationCut = "(ADAMASS('B0')<300*MeV)"
Strip_loose_Bd2KstarMuMu.MotherCut=( "(MIPDV(PRIMARY)<0.08*mm) & (MIPCHI2DV(PRIMARY) < 150) & (PT>300*MeV) & (BPVDIRA>0.9998) & "+
                                     "(VFASPF(VCHI2/VDOF)<64) & (DMASS('B0')<250*MeV) & (DMASS('B0')>-150*MeV)"
                                    )


#
# Filter the Bd for the ~10Hz stripping selection
#

filter_Bd2KstarMuMu_10Hz = FilterDesktop("filter_Bd2KstarMuMu_10Hz")
filter_Bd2KstarMuMu_10Hz.InputLocations = ["Strip_loose_Bd2KstarMuMu"]
filter_Bd2KstarMuMu_10Hz.Code=( "(INTREE((ABSID=='K+') & (TRCHI2DOF < 36))) & "+
                                "(INTREE((ABSID=='pi+') & (TRCHI2DOF < 36))) & "+
                                "(2 == NINTREE((ABSID=='mu+') & (MIPCHI2DV(PRIMARY)>4) & (TRCHI2DOF < 16))) & "+
                                "(INTREE((ABSID=='K*(892)0') & (BPVVDCHI2>25) & (VFASPF(VCHI2/VDOF)<36))) & "+
                                "(PT>300*MeV) & (BPVDIRA>0.9998) & (BPVVDCHI2>36) & (VFASPF(VCHI2/VDOF)<36) "
                                )

# 
# Create StrippingLine with this selection.
#
Bd2KstarMuMu_10Hz_line = StrippingLine('Bd2KstarMuMu_10Hz'
               , prescale = 1
               , algos = [ Strip_loose_Bd2KstarMuMu, filter_Bd2KstarMuMu_10Hz ]
               , stream = 'BExclusive'
               )
#
# Filter the Bd for the ~2Hz stripping selection
#

filter_Bd2KstarMuMu_2Hz = FilterDesktop("filter_Bd2KstarMuMu_2Hz")
filter_Bd2KstarMuMu_2Hz.InputLocations = ["filter_Bd2KstarMuMu_10Hz"]
filter_Bd2KstarMuMu_2Hz.Code = ( "(INTREE((ABSID=='K+') & ((PIDK-PIDpi)>-5) & ((PIDK-PIDmu)>-5) & (TRCHI2DOF < 9))) & "+
                                 "(2 == NINTREE((ABSID=='mu+') & ((PIDmu-PIDpi)>-5) & (TRCHI2DOF < 9))) & "+
                                 "(INTREE((ABSID=='pi+') & (TRCHI2DOF < 16))) & "+
                                 "(INTREE((ABSID=='K*(892)0') & (BPVVDZ>-30*mm) & (VFASPF(VCHI2/VDOF)<16) & (PT>300*MeV))) & "+
                                 "(INTREE((ABSID=='J/psi(1S)') & (BPVVDCHI2>25) )) & "+
                                 "(BPVDIRA>0.9999) & (BPVVDCHI2>64) & (MIPCHI2DV(PRIMARY) < 36) & (VFASPF(VCHI2/VDOF)<16) "
                                 )

#
# Make the 2Hz Bd as a clone of 10Hz line
# 
Bd2KstarMuMu_2Hz_line = Bd2KstarMuMu_10Hz_line.clone("Bd2KstarMuMu_2Hz"
			, algos = [ Strip_loose_Bd2KstarMuMu, filter_Bd2KstarMuMu_10Hz, filter_Bd2KstarMuMu_2Hz ]
)

#
# Filter the Bd for the ~1Hz stripping selection
#

filter_Bd2KstarMuMu_1Hz = FilterDesktop("filter_Bd2KstarMuMu_1Hz")
filter_Bd2KstarMuMu_1Hz.InputLocations = ["Strip_loose_Bd2KstarMuMu"]
filter_Bd2KstarMuMu_1Hz.Code = ( "(INTREE((ABSID=='K+') & ((PIDK-PIDpi)>-5) & ((PIDK-PIDmu)>-5))) & "+
                                 "(2 == NINTREE((ABSID=='mu+') & ((PIDmu-PIDpi)>-5) & (MIPDV(PRIMARY) > 0.08*mm))) & "+
                                 "(INTREE((ABSID=='K*(892)0') & (BPVVDZ>-5*mm) & (PT>1000*MeV) & (MIPDV(PRIMARY) > 0.06*mm))) & "+
                                 "(BPVDIRA>0.99995) & (VFASPF(VMINVDDV(PRIMARY))>1*mm) & (MIPDV(PRIMARY) < 0.06*mm)"
                                 )
#
# Make the 1Hz Bd as a clone of 10Hz line
# 
Bd2KstarMuMu_1Hz_line = Bd2KstarMuMu_10Hz_line.clone("Bd2KstarMuMu_1Hz"
			, algos = [ Strip_loose_Bd2KstarMuMu, filter_Bd2KstarMuMu_1Hz ]
)

