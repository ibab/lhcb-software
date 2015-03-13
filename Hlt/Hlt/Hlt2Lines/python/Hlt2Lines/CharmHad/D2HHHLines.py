from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CharmHadD2HHHLines() :
    def localcuts(self) : 
        return { # Now the combiner for the CPV lines 
                 'D2HHH' : {
                                 'Trk_2OF3_MIPCHI2DV_MIN'   :  9.0,
                                 'Trk_1OF3_MIPCHI2DV_MIN'   :  16.0,
                                 'BPVVDCHI2_MIN'            :  20.0,
                                 'BPVLTIME_MIN'             :  0.2 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9994,
                                 'ASUMPT_MIN'               :  1500 * MeV,
                                 'AM_MIN'                   :  1779 * MeV,
                                 'AM_MAX'                   :  2059 * MeV,
                                 'Mass_M_MIN'               :  1789.0 * MeV,
                                 'Mass_M_MAX'               :  2049.0 * MeV,
                                },
                 'Lc2HHH'    : {
                                 'Trk_2OF3_MIPCHI2DV_MIN'   :  4.0,
                                 'Trk_1OF3_MIPCHI2DV_MIN'   :  9.0,
                                 'BPVVDCHI2_MIN'            :  10.0,
                                 'BPVLTIME_MIN'             :  0.1 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9994,
                                 'ASUMPT_MIN'               :  1500 * MeV,
                                 'AM_MIN'                   :  2201 * MeV,
                                 'AM_MAX'                   :  2371 * MeV,
                                 'Mass_M_MIN'               :  2211.0 * MeV,
                                 'Mass_M_MAX'               :  2361.0 * MeV,
                                },
                 # The combiner for the lifetime unbiased lines 
                 'Dpm2HHH_LTUNB' : {
                                 'BPVLTIME_MIN'             :  0.3 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9,
                                 'ASUMPT_MIN'               :  3000 * MeV,
                                 'AM_MIN'                   :  1779 * MeV,
                                 'AM_MAX'                   :  1959 * MeV,
                                 'Mass_M_MIN'               :  1789.0 * MeV,
                                 'Mass_M_MAX'               :  1949.0 * MeV,
                                },
                 'Ds2HHH_LTUNB' : { 
                                 'BPVLTIME_MIN'             :  0.1 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9,
                                 'ASUMPT_MIN'               :  3000 * MeV,
                                 'AM_MIN'                   :  1879 * MeV,
                                 'AM_MAX'                   :  2059 * MeV,
                                 'Mass_M_MIN'               :  1889.0 * MeV,
                                 'Mass_M_MAX'               :  2049.0 * MeV,
                                },
                 'Lc2HHH_LTUNB'    : {
                                 'BPVLTIME_MIN'             :  0.1 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9,
                                 'ASUMPT_MIN'               :  3000 * MeV,
                                 'AM_MIN'                   :  2201 * MeV,
                                 'AM_MAX'                   :  2371 * MeV,
                                 'Mass_M_MIN'               :  2211.0 * MeV,
                                 'Mass_M_MAX'               :  2361.0 * MeV,
                                },
                 # Now the combiner for the PID calib lines 
                 # The pointing cut is tight to ensure a purer signal even without
                 # a PID cut on the proton
                 'Lc2KPPi_PIDCALIB' : {
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  6.0,
                                 'Trk_2OF3_MIPCHI2DV_MIN'   :  9.0,
                                 'Trk_1OF3_MIPCHI2DV_MIN'   :  12.0,
                                 'BPVVDCHI2_MIN'            :  10.0,
                                 'BPVLTIME_MIN'             :  0.1 * picosecond,
                                 'BPVDIRA_MIN'              :  0.99985,
                                 'ASUMPT_MIN'               :  2000 * MeV,
                                 'AM_MIN'                   :  2201 * MeV,
                                 'AM_MAX'                   :  2371 * MeV,
                                 'Mass_M_MIN'               :  2211.0 * MeV,
                                 'Mass_M_MAX'               :  2361.0 * MeV,
                                },
                 'Lc2KPPi_PIDCALIB_TAG' : {
                                 'DeltaM_AM_MIN'            :  150.0 * MeV,
                                 'DeltaM_MIN'               :  155.0 * MeV,
                                 'DeltaM_AM_MAX'            :  180.0 * MeV,
                                 'DeltaM_MAX'               :  175.0 * MeV,
                                 'TagVCHI2PDOF_MAX'         :  100.0
                                },
                }
    
    def locallines(self):
        from Stages import MassFilter,TagDecay
        from Stages import D2KPiPi_SS,D2KPiPi_OS,D2KKPi_SS,D2KKPi_OS,D2PiPiPi,D2KKK
        from Stages import Lc2KPPi, Lc2KPK, Lc2PiPPi, Lc2PiPK
        from Stages import D2KPiPi_SS_LTUNB,D2KKPi_OS_LTUNB,Lc2KPPi_LTUNB
        from Stages import Lc2KPPi_PIDCALIB
        from Stages import SharedSoftTagChild_pi
        stages = {# First the CPV D2HHH lines
                  'D2KPiPi_SS'        : [MassFilter('D2HHH',inputs=[D2KPiPi_SS('D2HHH')])],
                  'D2KPiPi_OS'        : [MassFilter('D2HHH',inputs=[D2KPiPi_OS('D2HHH')])],
                  'D2KKPi_SS'         : [MassFilter('D2HHH',inputs=[D2KKPi_SS('D2HHH')])],
                  'D2KKPi_OS'         : [MassFilter('D2HHH',inputs=[D2KKPi_OS('D2HHH')])],
                  'D2PiPiPi'          : [MassFilter('D2HHH',inputs=[D2PiPiPi('D2HHH')])],
                  'D2KKK'             : [MassFilter('D2HHH',inputs=[D2KKK('D2HHH')])],
                  # Now the CPV Lc2HHH lines
                  'Lc2KPPi'           : [MassFilter('Lc2HHH',inputs=[Lc2KPPi('Lc2HHH')])],
                  'Lc2KPK'            : [MassFilter('Lc2HHH',inputs=[Lc2KPK('Lc2HHH')])],
                  'Lc2PiPPi'          : [MassFilter('Lc2HHH',inputs=[Lc2PiPPi('Lc2HHH')])],
                  'Lc2PiPK'           : [MassFilter('Lc2HHH',inputs=[Lc2PiPK('Lc2HHH')])], 
                  # Now the three CF lifetime unbiased lines
                  'D2KPiPi_SS_LTUNB'  : [MassFilter('Dpm2HHH_LTUNB',inputs=[D2KPiPi_SS_LTUNB('Dpm2HHH_LTUNB')])],
                  'D2KKPi_OS_LTUNB'   : [MassFilter('Ds2HHH_LTUNB',inputs=[D2KKPi_OS_LTUNB('Ds2HHH_LTUNB')])],
                  'Lc2KPPi_LTUNB'     : [MassFilter('Lc2HHH_LTUNB',inputs=[Lc2KPPi_LTUNB('Lc2HHH_LTUNB')])],
                  # Now the PID calib Sigma_c->Lambda_c(pKpi)pi line
                  'Lc2KPPi_PIDCALIB'  : [TagDecay('Lc2KPPi_PIDCALIB_TAG', "[Sigma_c0 -> Lambda_c+ pi-]cc",
                                                  inputs = [ MassFilter('Lc2KPPi_PIDCALIB', 
                                                                        inputs = [Lc2KPPi_PIDCALIB('Lc2KPPi_PIDCALIB')]),
                                                             SharedSoftTagChild_pi]
                                                 )]
                 }
        return stages
