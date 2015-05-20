from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CharmHadD2V0V0Lines():
    def localcuts(self):
        return {
                 # The combiner for the CPV lines, separate for 2LL, LLDD, and 2DD
                 'D2KS0KS0_LL' : {
                                 'KS0_ALL_PT_MIN'           :  500.0 * MeV,
                                 'KS0_ALL_MIPCHI2DV_MIN'    :  9.0,
                                 'BPVVDCHI2_MIN'            :  20.0,
                                 'BPVLTIME_MIN'             :  0.2 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9994,
                                 'ASUMPT_MIN'               :  1500 * MeV,
                                 'AM_MIN'                   :  1779 * MeV,
                                 'AM_MAX'                   :  1959 * MeV,
                                 'Mass_M_MIN'               :  1789.0 * MeV,
                                 'Mass_M_MAX'               :  1949.0 * MeV,
                                },
                  'D2KS0KS0_DD' : {
                                 'KS0_ALL_PT_MIN'           :  750.0 * MeV,
                                 'KS0_ALL_MIPCHI2DV_MIN'    :  4.0,
                                 'BPVVDCHI2_MIN'            :  10.0,
                                 'BPVLTIME_MIN'             :  0.2 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9994,
                                 'ASUMPT_MIN'               :  2000 * MeV,
                                 'AM_MIN'                   :  1779 * MeV,
                                 'AM_MAX'                   :  1959 * MeV,
                                 'Mass_M_MIN'               :  1789.0 * MeV,
                                 'Mass_M_MAX'               :  1949.0 * MeV,
                                }, 
                }
    
    def locallines(self):
        from Stages import MassFilter
        from Stages import D2KS0KS0_2LL,D2KS0KS0_LLDD,D2KS0KS0_2DD
        stages = {# First the CPV lines
                  # If there is no DD KS0, use the LL dictionary, else use the DD dictionary
                  'D2KS0KS0_2LLTurbo'        : [MassFilter('D2KS0KS0_LL',inputs=[D2KS0KS0_2LL('D2KS0KS0_LL')])],
                  'D2KS0KS0_LLDDTurbo'       : [MassFilter('D2KS0KS0_DD',inputs=[D2KS0KS0_LLDD('D2KS0KS0_DD')])],
                  'D2KS0KS0_2DDTurbo'        : [MassFilter('D2KS0KS0_DD',inputs=[D2KS0KS0_2DD('D2KS0KS0_DD')])],
                 }
        return stages
