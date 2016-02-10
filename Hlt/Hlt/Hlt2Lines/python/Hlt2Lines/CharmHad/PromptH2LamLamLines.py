## a set of Hlt2 lines designed to search for H-dibaryon decay to
## pairs of Lambda0's.  In this scenario, the H is a resonance above threshold,
## so should decay "promptly".
##
## measuring how often two Lambda0's are nearby in phase space provides
## a good denominator in terms of nuclear coupling strengths.
##
## the best current limits (160209) come by Belle:  PRL 110, 222002 (2013).
##
__author__  = "Mike SOKOLOFF msokolof@cern.ch"

from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class PromptH2LamLamLines():
    def localcuts(self):
        return {
                 # The combiner for the CPV lines, separate for 2LL, LLDD, and 2DD
                 'H2LamLam_LL' : {
                                 'Lam_ALL_PT_MIN'           :  1000.0 * MeV,
                                 'Lam_ALL_MIPCHI2DV_MAX'    :  16.0,
                                 'BPVVDCHI2_MAX'            :  16.0,
                                 'BPVLTIME_MIN'             :  -0.1 * picosecond,
                                 'BPVLTIME_MAX'             :  0.1 * picosecond,
                                 'ASUMPT_MIN'               :  2000 * MeV,
                                 'AM_MAX'                   :  2310 * MeV,
                                 'Mass_M_MIN'               :  1789.0 * MeV,
                                 'Mass_M_MAX'               :  2300.0 * MeV,
                                },
                  'H2LamLam_DD' : {
                                 'Lam_ALL_PT_MIN'           :  1000.0 * MeV,
                                 'Lam_ALL_MIPCHI2DV_MAX'    :  16.0,
                                 'BPVVDCHI2_MAX'            :  16.0,
                                 'BPVLTIME_MIN'             :  0.2 * picosecond,
                                 'BPVLTIME_MAX'             :  0.2 * picosecond,
                                 'ASUMPT_MIN'               :  2000 * MeV,
                                 'AM_MAX'                   :  2310 * MeV,
                                 'Mass_M_MIN'               :  1789.0 * MeV,
                                 'Mass_M_MAX'               :  2300.0 * MeV,
                                }, 
                }
    
    def locallines(self):
        from Stages import MassFilter, H2LambdaLambdaCombiner
        from Stages import CharmHadSharedSecondaryLambdaDD, CharmHadSharedSecondaryLambdaLL

        ## If either Lambda is DD,  use the DD dictionary, otherwise use LL.

## First, let's do the H-dibaryon candidates

        H2LamLam_2LLComb = H2LambdaLambdaCombiner( 'Comb'
                , decay = "[D0 -> Lambda0 Lambda0]cc"
                , inputs = [ CharmHadSharedSecondaryLambdaLL ]
                , nickname = 'H2LamLam_LL' )

        H2LamLam_2LL = MassFilter('H2LamLam_LL', inputs=[ H2LamLam_2LLComb ])


        H2LamLam_LLDDComb = H2LambdaLambdaCombiner( 'Comb'
                , decay = "[D0 -> Lambda0 Lambda0]cc"
                , inputs = [ CharmHadSharedSecondaryLambdaLL, CharmHadSharedSecondaryLambdaDD ]
                , lldd = True, nickname = 'H2LamLam_DD')

        H2LamLam_LLDD = MassFilter('H2LamLam_DD', inputs = [ H2LamLam_LLDDComb ])


        H2LamLam_2DDComb = H2LambdaLambdaCombiner( 'Comb'
                , decay = "[D0 -> Lambda0 Lambda0]cc"
                , inputs = [ CharmHadSharedSecondaryLambdaDD ]
                , nickname = 'H2LamLam_DD' )

        H2LamLam_2DD = MassFilter('H2LamLam_DD', inputs = [H2LamLam_2DDComb])


##  next, look at the corresponding Lambda,anti-Lambda candidates

        H2LamLamBar_2LLComb = H2LambdaLambdaCombiner( 'Comb'
                , decay = "D0 -> Lambda0 Lambda~0"
                , inputs = [ CharmHadSharedSecondaryLambdaLL ]
                , nickname = 'H2LamLam_LL' )

        H2LamLamBar_2LL = MassFilter('H2LamLam_LL', inputs=[ H2LamLamBar_2LLComb ])


        H2LamLamBar_LLDDComb = H2LambdaLambdaCombiner( 'Comb'
                , decay = "D0 -> Lambda0 Lambda~0"
                , inputs = [ CharmHadSharedSecondaryLambdaLL, CharmHadSharedSecondaryLambdaDD ]
                , lldd = True, nickname = 'H2LamLam_DD')

        H2LamLamBar_LLDD = MassFilter('H2LamLam_DD', inputs = [ H2LamLam_LLDDComb ])


        H2LamLamBar_2DDComb = H2LambdaLambdaCombiner( 'Comb'
                , decay = "D0 -> Lambda0 Lambda~0"
                , inputs = [ CharmHadSharedSecondaryLambdaDD ]
                , nickname = 'H2LamLam_DD' )

        H2LamLamBar_2DD = MassFilter('H2LamLam_DD', inputs = [H2LamLamBar_2DDComb])


        stages = {
                  'PromptH2LamLam_LamLLTurbo'           : [H2LamLam_2LL],
                  'PromptH2LamLam_LamLL_LamDDTurbo'     : [H2LamLam_LLDD],
                  'PromptH2LamLam_LamDDTurbo'           : [H2LamLam_2DD],
                  'PromptH2LamLamBar_LamLLTurbo'        : [H2LamLamBar_2LL],
                  'PromptH2LamLamBar_LamLL_LamDDTurbo'  : [H2LamLamBar_LLDD],
                  'PromptH2LamLamBar_LamDDTurbo'        : [H2LamLamBar_2DD],
                 }
        return stages
