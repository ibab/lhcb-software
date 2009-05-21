# B->JpsiKstar selection
# 

from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from Configurables import CombineParticles, FilterDesktop

#
# Make the Bd. This is a ~10Hz stripping selection
# 
Strip_10Hz_Bd2KstarMuMu = CombineParticles("Strip_10Hz_Bd2KstarMuMu")

# Take Jpsi and Kstar from CommonParticles. 
Strip_10Hz_Bd2KstarMuMu.InputLocations = ["StdLooseJpsi2MuMu", "StdLooseKstar2KPi"]
Strip_10Hz_Bd2KstarMuMu.DecayDescriptor = "[B0 -> K*(892)0 J/psi(1S)]cc"
Strip_10Hz_Bd2KstarMuMu.DaughtersCuts = {
    'K*(892)0': "(BPVVDZ>-50) & (BPVVDCHI2>25) & (VFASPF(VCHI2/VDOF)<36) & (ADMASS('K*(892)0')<300*MeV) & (2 == NINTREE((MIPCHI2DV(PRIMARY)>2.25) & (TRCHI2DOF < 36))) & (INTREE((ABSID=='pi+') & ((PIDpi-PIDmu)>-10) & ((PIDpi-PIDK)>-10))) & (INTREE((ABSID=='K+') & ((PIDK-PIDmu)>-10) & ((PIDK-PIDpi)>-5)))",
    'J/psi(1S)' : "(VFASPF(VCHI2/VDOF)<64) & (2 == NINTREE((MIPCHI2DV(PRIMARY)>4) & (TRCHI2DOF < 16) & ((PIDmu-PIDpi)>-10) & ((PIDmu-PIDK)>-10)) )"
    }
Strip_10Hz_Bd2KstarMuMu.CombinationCut = "(ADAMASS('B0')<300*MeV)"
Strip_10Hz_Bd2KstarMuMu.MotherCut="(MIPDV(PRIMARY)<0.08) & (PT>300*MeV) & (BPVDIRA>0.9998) & (BPVVDCHI2>36) & (VFASPF(VCHI2/VDOF)<36) & (DMASS('B0')<250*MeV) & (DMASS('B0')>-150*MeV)"

# 
# Create StrippingLine with this selection. This just takes CombineParticles as an algorithm
#
Bd2KstarMuMu_10Hz_line = StrippingLine('Bd2KstarMuMu_10Hz'
               , prescale = 1
               , algos = [ Strip_10Hz_Bd2KstarMuMu ]
               , stream = 'BExclusive'
               )

filter_2hz = StrippingMember (FilterDesktop
			      , "Filter"
		    	      , InputLocations = ["Strip_10Hz_Bd2KstarMuMu"]

#                               Syntax error in this code, just use "ALL" for now
#		    	      , Code = "(INTREE((ABSID=='K+') & ((PIDK-PIDpi))>-5) & ((PIDK-PIDmu))>-5) & (TRCHI2DOF < 9)) & (2 == NINTREE((ABSID=='mu+') & ((PIDmu-PIDpi))>-5) & (TRCHI2DOF < 9)) & (INTREE((ABSID=='pi+') & (TRCHI2DOF < 16))) & (INTREE((ABSID=='K*(892)0') & (BPVVDZ>-30) & (VFASPF(VCHI2/VDOF)<16) & (PT>300*MeV))) & (INTREE((ABSID=='J/psi(1S)') & (BPVVDCHI2>25) )) & (BPVDIRA>0.9999) & (BPVVDCHI2>64) & (MIPCHI2DV(PRIMARY) < 36) & (VFASPF(VCHI2/VDOF)<16) "
		    	      , Code = "ALL"
		             )

#
# Make the 2Hz Bd as a clone of 10Hz line
# 
Bd2KstarMuMu_2Hz_line = Bd2KstarMuMu_10Hz_line.clone("Bd2KstarMuMu_2Hz"
			, algos = [ Strip_10Hz_Bd2KstarMuMu, filter_2hz ]
)

#
# TODO: Make both 10Hz and 2Hz lines from the same combinatorics with different FilterDesktops. 
#
