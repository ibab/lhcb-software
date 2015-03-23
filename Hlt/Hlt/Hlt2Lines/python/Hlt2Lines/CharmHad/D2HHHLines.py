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
                 # Now the combiner for the PID calib lines 
                 # The pointing cut is tight to ensure a purer signal even without
                 # a PID cut on the proton
                 'Lc2KPPi_PIDCALIB' : {
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  6.0,
                                 'Trk_2OF3_MIPCHI2DV_MIN'   :  9.0,
                                 'Trk_1OF3_MIPCHI2DV_MIN'   :  12.0,
                                 'Trk_ALL_PT_MIN'           :  200.0 * MeV,
                                 'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
                                 'Trk_1OF3_PT_MIN'          :  400.0 * MeV,
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
        from Stages import D2KPiPi_SS,D2KPiPi_OS,D2KKPi_SS,D2KKPi_OS,D2PiPiPi,D2KKK
        from Stages import Lc2KPPi, Lc2KPK, Lc2PiPPi, Lc2PiPK
        from Stages import D2KPiPi_SS_LTUNB,D2KKPi_OS_LTUNB,Lc2KPPi_LTUNB
        from Stages import Lc2KPPi_PIDCALIB
        from Stages import D2KPiPi_ForKPiAsym
        from Stages import SharedSoftTagChild_pi
        
        stages = {# First the CPV D2HHH lines
                  'D2KPiPi_SS'        : [MassFilter('D2KPiPi_SS', nickname = 'D2HHH',
                                                    inputs=[D2KPiPi_SS('D2HHH')], shared = True)],
                  'D2KPiPi_OS'        : [MassFilter('D2HHH', inputs=[D2KPiPi_OS('D2HHH')])],
                  'D2KKPi_SS'         : [MassFilter('D2HHH',inputs=[D2KKPi_SS('D2HHH')])],
                  'D2KKPi_OS'         : [MassFilter('D2KKPi_OS', nickname = 'D2HHH',
                                                    inputs=[D2KKPi_OS('D2HHH')], shared = True)],
                  'D2PiPiPi'          : [MassFilter('D2HHH',inputs=[D2PiPiPi('D2HHH')])],
                  'D2KKK'             : [MassFilter('D2HHH',inputs=[D2KKK('D2HHH')])],
                  # Now the CPV Lc2HHH lines
                  'Lc2KPPi'           : [MassFilter('Lc2KPPi', nickname = 'Lc2HHH',
                                                    inputs=[Lc2KPPi('Lc2HHH')], shared = True)],
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
                                                             SharedSoftTagChild_pi ]
                                                 )],
                  # Now the KPi asymmetry line
                  'Dpm2KPiPi_ForKPiAsym' : [MassFilter('Dpm2KPiPi_ForKPiAsym',
                                                       inputs=[D2KPiPi_ForKPiAsym('Dpm2KPiPi_ForKPiAsym')])]
            }
        
        
        return stages
