from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser


class ChargedHyperonLines():
    def localcuts(self):
        return {
                 # 
                 'Ximinus2LambdaPi_LLL':{
                                    'VCHI2PDOF_MAX'            :  10.0,
                                    'Trk_ALL_MIPCHI2DV_MIN'    :  16.0,
                                    'BPVVDCHI2_MIN'            :  10.0,
                                    'BPVLTIME_MIN'             :  2.0 * picosecond,
                                    'BPVDIRA_MIN'              :  0.9994,
                                    'ASUMPT_MIN'               :  500 * MeV,
                                    'AM_MIN'                   :  1280 * MeV,
                                    'AM_MAX'                   :  1365 * MeV,
                                    'Mass_M_MIN'               :  1290.0 * MeV,
                                    'Mass_M_MAX'               :  1355.0 * MeV,
                                   },
                 'Ximinus2LambdaPi_DDL':{
                                    'VCHI2PDOF_MAX'            :  10.0,
                                    'Trk_ALL_MIPCHI2DV_MIN'    :  16.0,
                                    'BPVVDCHI2_MIN'            :  10.0,
                                    'BPVLTIME_MIN'             :  2.0 * picosecond,
                                    'BPVDIRA_MIN'              :  0.9994,
                                    'ASUMPT_MIN'               :  500 * MeV,
                                    'AM_MIN'                   :  1280 * MeV,
                                    'AM_MAX'                   :  1365 * MeV,
                                    'Mass_M_MIN'               :  1290.0 * MeV,
                                    'Mass_M_MAX'               :  1355.0 * MeV,
                                   },
                 # 
                 'Omegaminus2LambdaK_LLL':{
                                    'VCHI2PDOF_MAX'            :  10.0,
                                    'Trk_ALL_MIPCHI2DV_MIN'    :  16.0,
                                    'BPVVDCHI2_MIN'            :  10.0,
                                    'BPVLTIME_MIN'             :  2.0 * picosecond,
                                    'BPVDIRA_MIN'              :  0.9994,
                                    'ASUMPT_MIN'               :  500 * MeV,
                                    'AM_MIN'                   :  1630 * MeV,
                                    'AM_MAX'                   :  1715 * MeV,
                                    'Mass_M_MIN'               :  1640.0 * MeV,
                                    'Mass_M_MAX'               :  1705.0 * MeV,
                                   },
                 'Omegaminus2LambdaK_DDL':{
                                    'VCHI2PDOF_MAX'            :  10.0,
                                    'Trk_ALL_MIPCHI2DV_MIN'    :  16.0,
                                    'BPVVDCHI2_MIN'            :  10.0,
                                    'BPVLTIME_MIN'             :  2.0 * picosecond,
                                    'BPVDIRA_MIN'              :  0.9994,
                                    'ASUMPT_MIN'               :  500 * MeV,
                                    'AM_MIN'                   :  1630 * MeV,
                                    'AM_MAX'                   :  1715 * MeV,
                                    'Mass_M_MIN'               :  1640.0 * MeV,
                                    'Mass_M_MAX'               :  1705.0 * MeV,
                                   },
                }
    
    def locallines(self):
        from Stages import MassFilter
        from Stages import Xi2LambdaPi_LLL, Xi2LambdaPi_DDL
        from Stages import Omega2LambdaK_LLL, Omega2LambdaK_DDL
        stages = {# 
                  'Xim2LamPim_LamLL'       : [MassFilter('Ximinus2LambdaPi_LLL', inputs=[Xi2LambdaPi_LLL])],
                  'Xim2LamPim_LamDD'       : [MassFilter('Ximinus2LambdaPi_DDL',inputs=[Xi2LambdaPi_DDL])],
                  'Omm2LamKm_LamLL'        : [MassFilter('Omegaminus2LambdaK_LLL',inputs=[Omega2LambdaK_LLL])],
                  'Omm2LamKm_LamDD'        : [MassFilter('Omegaminus2LambdaK_DDL',inputs=[Omega2LambdaK_DDL])],
                 }
        return stages
