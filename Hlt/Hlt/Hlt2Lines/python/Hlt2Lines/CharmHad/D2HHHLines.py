from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CharmHadD2HHHLines() :
    def localcuts(self) : 
        return { # Now the combiner for the CPV lines 
                 'D2HHH' : {
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                 'Trk_2OF3_MIPCHI2DV_MIN'   :  10.0,
                                 'Trk_1OF3_MIPCHI2DV_MIN'   :  50.0,
                                 'Trk_ALL_PT_MIN'           :  200.0 * MeV,
                                 'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
                                 'Trk_1OF3_PT_MIN'          :  1000.0 * MeV,
                                 'BPVVDCHI2_MIN'            :  20.0,
                                 'BPVLTIME_MIN'             :  0.3 * picosecond,
                                 'BPVDIRA_MIN'              :  0.99995,
                                 'ASUMPT_MIN'               :  3000 * MeV,
                                 'AM_MIN'                   :  1779 * MeV,
                                 'AM_MAX'                   :  2059 * MeV,
                                 'Mass_M_MIN'               :  1789.0 * MeV,
                                 'Mass_M_MAX'               :  2049.0 * MeV,
                                },
                 # For the Lc lines, despite the name, the mass window is
                 # wide enough to catch the Xi_c as well.
                 'Lc2HHH'    : {
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                 'Trk_2OF3_MIPCHI2DV_MIN'   :  8.0,
                                 'Trk_1OF3_MIPCHI2DV_MIN'   :  12.0,
                                 'Trk_ALL_PT_MIN'           :  200.0 * MeV,
                                 'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
                                 'Trk_1OF3_PT_MIN'          :  1000.0 * MeV,
                                 'BPVVDCHI2_MIN'            :  10.0,
                                 'BPVLTIME_MIN'             :  0.1 * picosecond,
                                 'BPVDIRA_MIN'              :  0.99995,
                                 'ASUMPT_MIN'               :  3000 * MeV,
                                 'AM_MIN'                   :  2201 * MeV,
                                 'AM_MAX'                   :  2553. * MeV,
                                 'Mass_M_MIN'               :  2211.0 * MeV,
                                 'Mass_M_MAX'               :  2543.0 * MeV,
                                },
                 # The combiner for the lifetime unbiased lines 
                 'Dpm2HHH_LTUNB' : {
                                 'TisTosSpec'               : "Hlt1Track.*Decision%TIS",
                                 'BPVLTIME_MIN'             :  0.5 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9,
                                 'ASUMPT_MIN'               :  3000 * MeV,
                                 'AM_MIN'                   :  1779 * MeV,
                                 'AM_MAX'                   :  1959 * MeV,
                                 'Mass_M_MIN'               :  1789.0 * MeV,
                                 'Mass_M_MAX'               :  1949.0 * MeV,
                                },
                 'Ds2HHH_LTUNB' : { 
                                 'TisTosSpec'               : "Hlt1Track.*Decision%TIS",
                                 'BPVLTIME_MIN'             :  0.25 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9,
                                 'ASUMPT_MIN'               :  3000 * MeV,
                                 'AM_MIN'                   :  1879 * MeV,
                                 'AM_MAX'                   :  2059 * MeV,
                                 'Mass_M_MIN'               :  1889.0 * MeV,
                                 'Mass_M_MAX'               :  2049.0 * MeV,
                                },
                 # Wide mass window accomodates the Xi_c+ 
                 'Lc2HHH_LTUNB'    : {
                                 'TisTosSpec'               : "Hlt1Track.*Decision%TIS",
                                 'BPVLTIME_MIN'             :  0.1 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9,
                                 'ASUMPT_MIN'               :  3000. * MeV,
                                 'AM_MIN'                   :  2201. * MeV,
                                 'AM_MAX'                   :  2553. * MeV,
                                 'Mass_M_MIN'               :  2211.0 * MeV,
                                 'Mass_M_MAX'               :  2543.0 * MeV,
                                },
                 # The combiner for a KPi asymmetry line
                 'Dpm2KPiPi_ForKPiAsym' : { 
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  25.0,
                                 'Trk_2OF3_MIPCHI2DV_MIN'   :  49.0,
                                 'Trk_1OF3_MIPCHI2DV_MIN'   :  100.0,
                                 'Trk_ALL_PT_MIN'           :  200.0 * MeV,
                                 'Trk_2OF3_PT_MIN'          :  200.0 * MeV,
                                 'Trk_1OF3_PT_MIN'          :  200.0 * MeV,
                                 'BPVVDCHI2_MIN'            :  0.0,
                                 'BPVLTIME_MIN'             :  0.5 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9999,
                                 'ASUMPT_MIN'               :  1000 * MeV,
                                 'AM_MIN'                   :  1809 * MeV,
                                 'AM_MAX'                   :  1929 * MeV,
                                 'Mass_M_MIN'               :  1819.0 * MeV,
                                 'Mass_M_MAX'               :  1919.0 * MeV,
                                }, 
                }
    
    def locallines(self):
        from Stages import MassFilter,TagDecay
        from Stages import D2KPiPi_SS_LTUNB,D2KKPi_OS_LTUNB,Lc2KPPi_LTUNB
        from Stages import DetAsym_DpToKmPipPip
        from Stages import SharedSoftTagChild_pi
        from Stages import D2HHH_DpToKmPipPip, D2HHH_DpToKpPimPip
        from Stages import D2HHH_DpToKpKpPim, D2HHH_DpToKmKpPim
        from Stages import D2HHH_DpToPimPipPip, D2HHH_DpToKmKpKm
        from Stages import Lc2HHH_LcpToKmPpPip, Lc2HHH_LcpToKmPpKp
        from Stages import Lc2HHH_LcpToPimPpPip, Lc2HHH_LcpToPimPpKp
        
        stages = {# First the CPV D2HHH lines
                  'D2KPiPi_SS'        : [MassFilter('D2KPiPi_SS', nickname = 'D2HHH',
                                                    inputs=[D2HHH_DpToKmPipPip], shared = True)],
                  'D2KPiPi_OS'        : [MassFilter('D2HHH', inputs=[D2HHH_DpToKpPimPip])],
                  'D2KKPi_SS'         : [MassFilter('D2HHH',inputs=[D2HHH_DpToKpKpPim])],
                  'D2KKPi_OS'         : [MassFilter('D2KKPi_OS', nickname = 'D2HHH',
                                                    inputs=[D2HHH_DpToKmKpPim], shared = True)],
                  'D2PiPiPi'          : [MassFilter('D2HHH',inputs=[D2HHH_DpToPimPipPip])],
                  'D2KKK'             : [MassFilter('D2HHH',inputs=[D2HHH_DpToKmKpKm])],
                  # Now the CPV Lc2HHH lines
                  'Lc2KPPi'           : [MassFilter('Lc2KPPi', nickname = 'Lc2HHH',
                                                    inputs=[Lc2HHH_LcpToKmPpPip], shared = True)],
                  'Lc2KPK'            : [MassFilter('Lc2HHH',inputs=[Lc2HHH_LcpToKmPpKp])],
                  'Lc2PiPPi'          : [MassFilter('Lc2HHH',inputs=[Lc2HHH_LcpToPimPpPip])],
                  'Lc2PiPK'           : [MassFilter('Lc2HHH',inputs=[Lc2HHH_LcpToPimPpKp])], 
                  # Now the three CF lifetime unbiased lines
                  'D2KPiPi_SS_LTUNB'  : [MassFilter('Dpm2HHH_LTUNB',inputs=[D2KPiPi_SS_LTUNB('Dpm2HHH_LTUNB')])],
                  'D2KKPi_OS_LTUNB'   : [MassFilter('Ds2HHH_LTUNB',inputs=[D2KKPi_OS_LTUNB('Ds2HHH_LTUNB')])],
                  'Lc2KPPi_LTUNB'     : [MassFilter('Lc2HHH_LTUNB',inputs=[Lc2KPPi_LTUNB('Lc2HHH_LTUNB')])],
                  # Now the KPi asymmetry line
                  'Dpm2KPiPi_ForKPiAsym' : [MassFilter('Dpm2KPiPi_ForKPiAsym',
                                                       inputs=[DetAsym_DpToKmPipPip])]
            }
        
        
        return stages
