from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CharmHadD2V0HLines():
    def localcuts(self):
        return {
                 # Now the combiner for the CPV lines, separate for LL and DD
                 'D2V0H' : {
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  16.0,
                                 'BPVVDCHI2_MIN'            :  20.0,
                                 'BPVLTIME_MIN'             :  0.2 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9994,
                                 'ASUMPT_MIN'               :  1500 * MeV,
                                 'AM_MIN'                   :  1779 * MeV,
                                 'AM_MAX'                   :  2059 * MeV,
                                 'Mass_M_MIN'               :  1789.0 * MeV,
                                 'Mass_M_MAX'               :  2049.0 * MeV,
                                },
                 'Lc2V0H'    : {
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  16.0,
                                 'BPVVDCHI2_MIN'            :  10.0,
                                 'BPVLTIME_MIN'             :  0.1 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9994,
                                 'ASUMPT_MIN'               :  1500 * MeV,
                                 'AM_MIN'                   :  2201 * MeV,
                                 'AM_MAX'                   :  2371 * MeV,
                                 'Mass_M_MIN'               :  2211.0 * MeV,
                                 'Mass_M_MAX'               :  2361.0 * MeV,
                                },
                }
    
    def locallines(self):
        from Stages import MassFilter
        from Stages import D2KS0Pi_LL,D2KS0K_LL,D2KS0Pi_DD,D2KS0K_DD
        from Stages import Lc2LambdaPi_LL,Lc2LambdaK_LL,Lc2LambdaPi_DD,Lc2LambdaK_DD
        stages = {# First the CPV lines
                  'D2KS0Pi_LL'        : [MassFilter('D2V0H',inputs=[D2KS0Pi_LL('D2V0H')])],
                  'D2KS0K_LL'         : [MassFilter('D2V0H',inputs=[D2KS0K_LL('D2V0H')])],
                  'D2KS0Pi_DD'        : [MassFilter('D2V0H',inputs=[D2KS0Pi_DD('D2V0H')])],
                  'D2KS0K_DD'         : [MassFilter('D2V0H',inputs=[D2KS0K_DD('D2V0H')])],
                  #
                  'Lc2LambdaPi_LL'    : [MassFilter('Lc2V0H',inputs=[Lc2LambdaPi_LL('Lc2V0H')])],
                  'Lc2LambdaK_LL'     : [MassFilter('Lc2V0H',inputs=[Lc2LambdaK_LL('Lc2V0H')])],
                  'Lc2LambdaPi_DD'    : [MassFilter('Lc2V0H',inputs=[Lc2LambdaPi_DD('Lc2V0H')])],
                  'Lc2LambdaK_DD'     : [MassFilter('Lc2V0H',inputs=[Lc2LambdaK_DD('Lc2V0H')])],
                 }
        return stages
