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
                             'Proton_PIDp_MIN'          :  0,
                             'AM_MIN'                   :  1779 * MeV,
                             'AM_MAX'                   :  2436 * MeV,
                             'D_ASUMPT_MIN'             :  0 * MeV,
                             'D_BPVDIRA_MIN'            :  0.9994,
                             'D_BPVVDCHI2_MIN'          :  20.0,
                             'D_VCHI2PDOF_MAX'          :  20.0,
                             'D_BPVLTIME_MIN'           :  0.2 * picosecond,
                             'D_Mass_M_MIN'             :  1789.0 * MeV,
                             'D_Mass_M_MAX'             :  2049.0 * MeV,
                             'Lc_Mass_M_MIN'            :  2211.0 * MeV,
                             'Lc_Mass_M_MAX'            :  2361.0 * MeV,
                            }

                }
    
    def __apply_configuration__(self):
        from Stages import MassFilterD,MassFilterLc
        from Stages import D2KPiPi_SS,D2KPiPi_OS,D2KKPi_SS,D2KKPi_OS,D2PiPiPi,D2KKK
        from Stages import Lc2KPPi, Lc2KPK, Lc2PiPPi, Lc2PiPK
        stages = {'D2KPiPi_SS'   : [MassFilterD('D2KPiPi_SS',inputs=[D2KPiPi_SS])],
                  'D2KPiPi_OS'   : [MassFilterD('D2KPiPi_OS',inputs=[D2KPiPi_OS])],
                  'D2KKPi_SS'    : [MassFilterD('D2KKPi_SS',inputs=[D2KKPi_SS])],
                  'D2KKPi_OS'    : [MassFilterD('D2KKPi_OS',inputs=[D2KKPi_OS])],
                  'D2PiPiPi'     : [MassFilterD('D2PiPiPi',inputs=[D2PiPiPi])],
                  'D2KKK'        : [MassFilterD('D2KKK',inputs=[D2KKK])],
                  #
                  'Lc2KPPi'      : [MassFilterLc('Lc2KPPi',inputs=[Lc2KPPi])],
                  'Lc2KPK'       : [MassFilterLc('Lc2KPK',inputs=[Lc2KPK])],
                  'Lc2PiPPi'     : [MassFilterLc('Lc2PiPPi',inputs=[Lc2PiPPi])],
                  'Lc2PiPK'      : [MassFilterLc('Lc2PiPK',inputs=[Lc2PiPK])] }
                
        from HltLine.HltLine import Hlt2Line
        for nickname, algos in self.algorithms(stages).iteritems():
            Hlt2Line(nickname, prescale = self.prescale,
                     algos = algos, postscale = self.postscale)
