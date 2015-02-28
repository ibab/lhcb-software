from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CharmHadD2HHHLines(Hlt2LinesConfigurableUser):
    __slots__ = {'Prescale' : {},
                 'GEC' : {'NTRACK_MAX'           : 10000},
                 'Common' : {'TisTosSpec'               : "Hlt1Track.*Decision%TOS",
                             'Trk_TRCHI2DOF_MAX'        :  3.0,     
                             'Trk_PT_MIN'               :  200 * MeV, 
                             'Trk_P_MIN'                :  1000 * MeV,
                             'Trk_ALL_MIPCHI2DV_MIN'    :  1.0,
                             'Trk_2OF3_MIPCHI2DV_MIN'   :  4.0,
                             'Trk_1OF3_MIPCHI2DV_MIN'   :  4.0,
                             'Kaon_PIDK_MIN'            :  0,
                             'Pion_PIDK_MAX'            :  10,
                             'AM_MIN'                   :  1779 * MeV,
                             'AM_MAX'                   :  2059 * MeV,
                             'D_ASUMPT_MIN'             :  0 * MeV,
                             'D_BPVDIRA_MIN'            :  0.9994,
                             'D_BPVVDCHI2_MIN'          :  20.0,
                             'D_VCHI2PDOF_MAX'          :  20.0,
                             'D_BPVLTIME_MIN'           :  0.2 * picosecond,
                             'Dp_Mass_M_MIN'            :  1789.0 * MeV,
                             'Dp_Mass_M_MAX'            :  1949.0 * MeV,
                             'Ds_Mass_M_MIN'            :  1889.0 * MeV,
                             'Ds_Mass_M_MAX'            :  2049.0 * MeV,
                            }

                }
    
    def __apply_configuration__(self):
        from Stages import MassFilterDp,MassFilterDs
        from Stages import D2KPiPi_SS,D2KPiPi_OS,D2KKPi_SS,D2KKPi_OS,D2PiPiPi,D2KKK
        stages = {'Dp2KPiPi_SS'   : [MassFilterDp('Dp2KPiPi_SS',inputs=[D2KPiPi_SS])],
                  'Dp2KPiPi_OS'   : [MassFilterDp('Dp2KPiPi_OS',inputs=[D2KPiPi_OS])],
                  'Dp2KKPi_SS'    : [MassFilterDp('Dp2KKPi_SS',inputs=[D2KKPi_SS])],
                  'Dp2KKPi_OS'    : [MassFilterDp('Dp2KKPi_OS',inputs=[D2KKPi_OS])],
                  'Dp2PiPiPi'     : [MassFilterDp('Dp2PiPiPi',inputs=[D2PiPiPi])],
                  'Dp2KKK'        : [MassFilterDp('Dp2KKK',inputs=[D2KKK])],
                  #
                  'Ds2KPiPi_SS'   : [MassFilterDs('Ds2KPiPi_SS',inputs=[D2KPiPi_SS])],
                  'Ds2KPiPi_OS'   : [MassFilterDs('Ds2KPiPi_OS',inputs=[D2KPiPi_OS])],
                  'Ds2KKPi_SS'    : [MassFilterDs('Ds2KKPi_SS',inputs=[D2KKPi_SS])],
                  'Ds2KKPi_OS'    : [MassFilterDs('Ds2KKPi_OS',inputs=[D2KKPi_OS])],
                  'Ds2PiPiPi'     : [MassFilterDs('Ds2PiPiPi',inputs=[D2PiPiPi])],
                  'Ds2KKK'        : [MassFilterDs('Ds2KKK',inputs=[D2KKK])]  }
                
        from HltLine.HltLine import Hlt2Line
        for nickname, algos in self.algorithms(stages).iteritems():
            Hlt2Line(nickname, prescale = self.prescale,
                     algos = algos, postscale = self.postscale)
