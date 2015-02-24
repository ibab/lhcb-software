from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class LambdaCLines(Hlt2LinesConfigurableUser):
    __slots__ = {'Prescale' : {},
                 'GEC' : {'NTRACK_MAX'           : 180},
                 'Common' : {'TisTosSpec'               : "Hlt1Track.*Decision%TOS",
                             'Trk_TRCHI2DOF_MAX'        :    3.0,
                             'Trk_PT_MIN'               :  500 * MeV,
                             'Trk_MIPCHI2DV_MIN'        :    9.0,
                             'Trk_1of3_MIPCHI2DV_MIN'   :   15.0,
                             'Lc_AM_MIN'                : 2136 * MeV,
                             'Lc_AM_MAX'                : 2436 * MeV,
                             'Lc_ASUMPT_MIN'            : 2500 * MeV,
                             'Lc_BPVDIRA_MIN'           :    0.99985,
                             'Lc_BPVVDCHI2_MIN'         :   49.0,
                             'Lc_BPVVDR_MAX'            :    4 * mm,
                             'Lc_VCHI2PDOF_MAX'         :   15.0, 
                             'Lc_BPVLTIME_MAX'          :   20 * picosecond,
                             'P_PIDp_MIN'               :    0.0,
                             'P_PT_MIN'                 : 1500 * MeV,
                             'P_P_MIN'                  :   10 * GeV,
                             'SigMass_M_MIN'            : 2211 * MeV,
                             'SigMass_M_MAX'            : 2361 * MeV}
                }
    
    def __apply_configuration__(self):
        from Stages import LC2KPPi, LC2KPK, LC2PiPPi, LC2PiPK
        stages = {'KPK'   : [LC2KPK],
                  'KPPi'  : [LC2KPPi],
                  'PiPPi' : [LC2PiPPi],
                  'PiPK'  : [LC2PiPK]}

        from HltLine.HltLine import Hlt2Line
        for nickname, algos in self.algorithms(stages).iteritems():
            Hlt2Line('NewLambdaC2' + nickname, prescale = self.prescale,
                     algos = algos, postscale = self.postscale)
