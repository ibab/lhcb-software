## #####################################################################
# A stripping selection for muon identification calibration trough
# the study of J/psi(1S) -> mu+ mu- and Lambda to p pi decays
#
# @authors S.Furcas, G.Lanfranchi, M.Palutan, A.Sarti 
# @date 2009-July-30
# 
## #####################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember

# Create Lambda_dd -> ppi candidates
## ############################################################
LambdaDDCombine = StrippingMember( CombineParticles
                                   , 'CombineDD'
                                   , InputLocations = [ "StdNoPIDsDownPions","StdNoPIDsDownProtons" ]
                                   , DecayDescriptor = "[Lambda0 -> p+ pi-]cc"
                                   , DaughtersCuts = { "p+"  :  "(P>3000*MeV) & (PT>100*MeV) & (TRCHI2DOF<2) & (ISDOWN) & (MIPCHI2DV(PRIMARY)>9)",
                                                       "pi-" :  "(P>3000*MeV) & (PT>100*MeV) & (TRCHI2DOF<2) & (ISDOWN) & (MIPCHI2DV(PRIMARY)>9)" }
                                   , CombinationCut = "(ADAMASS('Lambda0')<20*MeV)"
                                   , MotherCut = " (500<BPVVDZ) & (2200>BPVVDZ) & (BPVDIRA>0.999999)  & ( (CHILDCUT((PT>0.4*GeV),1)) | (CHILDCUT((PT>0.4*GeV),2)) ) & " +
                                   " ( ADWM( 'KS0' , WM( 'pi+' , 'pi-') ) > 20*MeV )"
                                   )

# Define the line
# ############################################################
line_lambda_dd = StrippingLine('LambdaDDNoPID'
                        , prescale = 0.004
                        , algos = [ LambdaDDCombine ]
                    )


# Create Lambda_LL -> ppi candidates
## ############################################################
LambdaLLCombine = StrippingMember( CombineParticles
                                   , 'CombineLL'
                                   , InputLocations = [ "StdNoPIDsPions","StdNoPIDsProtons" ]
                                   , DecayDescriptor = "[Lambda0 -> p+ pi-]cc"
                                   , DaughtersCuts = { "p+"  :  "(P>3000*MeV) & (PT>100*MeV) & (TRCHI2DOF<2) & (ISLONG) & (MIPCHI2DV(PRIMARY)>9) ",
                                                       "pi-" :  "(P>3000*MeV) & (PT>100*MeV) & (TRCHI2DOF<2) & (ISLONG) & (MIPCHI2DV(PRIMARY)>9) " }
                                   , CombinationCut = "(ADAMASS('Lambda0')<20*MeV)"
                                   , MotherCut = "(50<BPVVDZ) & (600>BPVVDZ) & (BPVDIRA>0.9999995)  & ( CHILDCUT((PT>0.2*GeV),1) | CHILDCUT((PT>0.2*GeV),2) ) & " +
                                   " ( ADWM( 'KS0' , WM( 'pi+' , 'pi-') ) > 20*MeV )"
                                   )

# Define the line
# ############################################################
line_lambda_ll = StrippingLine('LambdaLLNoPID'
                        , prescale = 0.02
                        , algos = [ LambdaLLCombine ]
                    )
