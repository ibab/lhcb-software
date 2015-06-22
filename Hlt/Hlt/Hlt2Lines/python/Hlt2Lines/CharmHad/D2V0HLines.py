## Exclusive reconstruction of D+ -> KS0 h+ and Lambda_c+ -> Lambda h+ modes.
##
## @author unclaimed
from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm

class CharmHadD2V0HLines():
    def localcuts(self):
        return {
                 # Now the combiner for the CPV lines, separate for LL and DD
                 'D2V0H' : {
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  36.0,
                                 'BPVVDCHI2_MIN'            :  30.0,
                                 'BPVLTIME_MIN'             :  0.25 * picosecond,
                                 'BPVDIRA_MIN'              :  0.99985,
                                 'ASUMPT_MIN'               :  2000 * MeV,
                                 'AM_MIN'                   :  1779 * MeV,
                                 'AM_MAX'                   :  2059 * MeV,
                                 'Mass_M_MIN'               :  1789.0 * MeV,
                                 'Mass_M_MAX'               :  2049.0 * MeV,
                                 'TisTosSpec'               : "Hlt1.*Track.*Decision%TOS"
                                },
                 'Lc2V0H'    : {
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  16.0,
                                 'BPVVDCHI2_MIN'            :  20.0,
                                 'BPVLTIME_MIN'             :  0.15 * picosecond,
                                 'BPVDIRA_MIN'              :  0.99985,
                                 'ASUMPT_MIN'               :  2000 * MeV,
                                 'AM_MIN'                   :  2201 * MeV,
                                 'AM_MAX'                   :  2371 * MeV,
                                 'Mass_M_MIN'               :  2211.0 * MeV,
                                 'Mass_M_MAX'               :  2361.0 * MeV,
                                 'TisTosSpec'               : "Hlt1.*Track.*Decision%TOS"
                                },
                }
    
    def locallines(self):
        from Stages import MassFilter, DetachedV0HCombiner
        from Stages import SharedDetachedDpmChild_pi, SharedDetachedDpmChild_K
        from Stages import CharmHadSharedKsLL, CharmHadSharedKsDD
        from Stages import SharedDetachedLcChild_pi, SharedDetachedLcChild_K
        from Inputs import Lambda_LL, Lambda_DD


        D2V0H_D2KS0Pi_LL = DetachedV0HCombiner( 'Comb'
                , decay = "[D+ -> pi+ KS0]cc"
                , inputs = [ SharedDetachedDpmChild_pi, CharmHadSharedKsLL ]
                , nickname = 'D2V0H' )

        D2KS0Pi_LL = MassFilter('D2V0H', inputs = [ D2V0H_D2KS0Pi_LL ])


        D2V0H_D2KS0K_LL = DetachedV0HCombiner( 'Comb'
                , decay = "[D+ -> K+ KS0]cc"
                , inputs = [ SharedDetachedDpmChild_K, CharmHadSharedKsLL ]
                , nickname = 'D2V0H' )

        D2KS0K_LL = MassFilter('D2V0H', inputs = [ D2V0H_D2KS0K_LL ])


        D2V0H_D2KS0Pi_DD = DetachedV0HCombiner( 'Comb'
                , decay = "[D+ -> pi+ KS0]cc"
                , inputs = [ SharedDetachedDpmChild_pi, CharmHadSharedKsDD ]
                , nickname = 'D2V0H' )

        D2KS0Pi_DD = MassFilter('D2V0H', inputs = [ D2V0H_D2KS0Pi_DD ])


        D2V0H_D2KS0K_DD = DetachedV0HCombiner( 'Comb'
                , decay = "[D+ -> K+ KS0]cc"
                , inputs = [ SharedDetachedDpmChild_K, CharmHadSharedKsDD ]
                , nickname = 'D2V0H' )

        D2KS0K_DD = MassFilter('D2V0H', inputs = [ D2V0H_D2KS0K_DD ] )


        Lc2V0H_Lc2LambdaPi_LL = DetachedV0HCombiner( 'Comb'
                , decay = "[Lambda_c+ -> pi+ Lambda0]cc"
                , inputs = [ SharedDetachedLcChild_pi, Lambda_LL ]
                , nickname = 'Lc2V0H' )

        Lc2LambdaPi_LL = MassFilter('Lc2V0H', inputs = [Lc2V0H_Lc2LambdaPi_LL])


        Lc2V0H_Lc2LambdaK_LL = DetachedV0HCombiner( 'Comb'
                , decay = "[Lambda_c+ -> K+ Lambda0]cc"
                , inputs = [ SharedDetachedLcChild_K, Lambda_LL ]
                , nickname = 'Lc2V0H' )

        Lc2LambdaK_LL = MassFilter('Lc2V0H', inputs = [Lc2V0H_Lc2LambdaK_LL])


        Lc2V0H_Lc2LambdaPi_DD = DetachedV0HCombiner( 'Comb'
                , decay = "[Lambda_c+ -> pi+ Lambda0]cc"
                , inputs = [ SharedDetachedLcChild_pi, Lambda_DD ]
                , nickname = 'Lc2V0H' )

        Lc2LambdaPi_DD = MassFilter('Lc2V0H', inputs = [Lc2V0H_Lc2LambdaPi_DD])


        Lc2V0H_Lc2LambdaK_DD = DetachedV0HCombiner( 'Comb'
                , decay = "[Lambda_c+ -> K+ Lambda0]cc"
                , inputs = [ SharedDetachedLcChild_K, Lambda_DD ]
                , nickname = 'Lc2V0H' )

        Lc2LambdaK_DD = MassFilter('Lc2V0H', inputs = [Lc2V0H_Lc2LambdaK_DD])


        stages = {
                  'Dp2KS0Pip_KS0LLTurbo'        : [D2KS0Pi_LL],
                  'Dp2KS0Kp_KS0LLTurbo'         : [D2KS0K_LL],
                  'Dp2KS0Pip_KS0DDTurbo'        : [D2KS0Pi_DD],
                  'Dp2KS0Kp_KS0DDTurbo'         : [D2KS0K_DD],
                  #
                  'Lcp2LamPip_LamLLTurbo'       : [Lc2LambdaPi_LL],
                  'Lcp2LamKp_LamLLTurbo'        : [Lc2LambdaK_LL],
                  'Lcp2LamPip_LamDDTurbo'       : [Lc2LambdaPi_DD],
                  'Lcp2LamKp_LamDDTurbo'        : [Lc2LambdaK_DD],
                 }
        return stages
