# B->JpsiKstar selection
# 

from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from Configurables import CombineParticles, FilterDesktop

Strip_Penguin_Jpsi2MuMu = CombineParticles ( 'Strip_Penguin_Jpsi2MuMu' )
Strip_Penguin_Jpsi2MuMu.InputLocations = [ "StdLooseMuons" ]
Strip_Penguin_Jpsi2MuMu.DecayDescriptor = "J/psi(1S) -> mu+ mu-" 

Strip_Penguin_Jpsi2MuMu.CombinationCut = "ADAMASS('J/psi(1S)')<3000*MeV"
Strip_Penguin_Jpsi2MuMu.MotherCut = "(VFASPF(VCHI2/VDOF)<64)"
#
####

####
# Make the K*... should be a common particle
Strip_Penguin_Kstar2KPi = CombineParticles ( 'Strip_Penguin_Kstar2KPi' )

Strip_Penguin_Kstar2KPi.InputLocations = [ "StdLooseKaons", "StdLoosePions" ]
Strip_Penguin_Kstar2KPi.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc" 
Strip_Penguin_Kstar2KPi.DaughtersCuts = { "pi+" : "(MIPCHI2DV(PRIMARY)>2.25)",
                                    "K+" : "(MIPCHI2DV(PRIMARY)>2.25)" } 
Strip_Penguin_Kstar2KPi.CombinationCut = "(ADAMASS('K*(892)0')<300*MeV)"
Strip_Penguin_Kstar2KPi.MotherCut = "(VFASPF(VCHI2/VDOF)<100) & (MIPCHI2DV(PRIMARY)>1.5)"


#
# Make the Bd. This is a ~10Hz stripping selection
# 
Strip_10Hz_Bd2KstarMuMu = CombineParticles("Strip_10Hz_Bd2KstarMuMu")

Strip_10Hz_Bd2KstarMuMu.InputLocations = ["Strip_Penguin_Jpsi2MuMu", "Strip_Penguin_Kstar2KPi"]
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
               , algos = [ Strip_Penguin_Jpsi2MuMu, Strip_Penguin_Kstar2KPi, Strip_10Hz_Bd2KstarMuMu ]
               , stream = 'BExclusive'
               )

filter_2hz = StrippingMember (FilterDesktop
			      , "Filter"
		    	      , InputLocations = ["Strip_10Hz_Bd2KstarMuMu"]

#                             Parse error in this original code
#		    	      , Code = "(INTREE((ABSID=='K+') & ((PIDK-PIDpi))>-5) & ((PIDK-PIDmu))>-5) & (TRCHI2DOF < 9)) & (2 == NINTREE((ABSID=='mu+') & ((PIDmu-PIDpi))>-5) & (TRCHI2DOF < 9)) & (INTREE((ABSID=='pi+') & (TRCHI2DOF < 16))) & (INTREE((ABSID=='K*(892)0') & (BPVVDZ>-30) & (VFASPF(VCHI2/VDOF)<16) & (PT>300*MeV))) & (INTREE((ABSID=='J/psi(1S)') & (BPVVDCHI2>25) )) & (BPVDIRA>0.9999) & (BPVVDCHI2>64) & (MIPCHI2DV(PRIMARY) < 36) & (VFASPF(VCHI2/VDOF)<16) "
		    	      , Code = 
		    	    		"(INTREE((ABSID=='K+') & ((PIDK-PIDpi)>-5) & ((PIDK-PIDmu)>-5) & (TRCHI2DOF < 9))) & "+
		    	        	"(2 == NINTREE((ABSID=='mu+') & ((PIDmu-PIDpi)>-5) & (TRCHI2DOF < 9))) & "+
		    	                "(INTREE((ABSID=='pi+') & (TRCHI2DOF < 16))) & "+
		    	                "(INTREE((ABSID=='K*(892)0') & (BPVVDZ>-30) & (VFASPF(VCHI2/VDOF)<16) & (PT>300*MeV))) & "+
		    	                "(INTREE((ABSID=='J/psi(1S)') & (BPVVDCHI2>25) )) & "+
		    	                "(BPVDIRA>0.9999) & (BPVVDCHI2>64) & (MIPCHI2DV(PRIMARY) < 36) & (VFASPF(VCHI2/VDOF)<16) "
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
