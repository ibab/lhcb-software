from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class SMOGLines(Hlt2LinesConfigurableUser) :
  __slots__ =  { '_stages' : {},
      'Prescale' : {},
      'Postscale': {},
      'TrackGEC' : { 'NTRACK_MAX'           : 10000},
      'Common'   : { 'Trk_ALL_TRCHI2DOF_MAX'    :  3.0,    
                     'Trk_ALL_P_MIN'            :  1000 * MeV,
                     'VCHI2PDOF_MAX'            :  10.0,
                },  
      'SMOGSharedDpmChild_K' : {
               'PID_LIM'                  :  5,  
               'Trk_ALL_PT_MIN'           :  200 * MeV
        },
      'SMOGSharedDpmChild_pi' : {
               'PID_LIM'                  :  5,  
               'Trk_ALL_PT_MIN'           :  200 * MeV
        },
      'SMOGSharedDpmChild_TightK' : {
               'PID_LIM'                  :  7,  
               'Trk_ALL_PT_MIN'           :  250 * MeV
        },
      'SMOGSharedDpmChild_Tightpi' : {
               'PID_LIM'                  :  7,  
               'Trk_ALL_PT_MIN'           :  250 * MeV
        },
      'SMOGSharedLcChild_pi' : {
               'PID_LIM'                  :  5,  
               'Trk_ALL_PT_MIN'           :  200 * MeV
        },
      'SMOGSharedLcChild_K'  : {
               'PID_LIM'                  :  5,  
               'Trk_ALL_PT_MIN'           :  200 * MeV
        },
      'SMOGSharedLcChild_p'  : {
               'PID_LIM'                  :  5,  
               'Trk_ALL_PT_MIN'           :  200 * MeV
        },
      'TighterProtons' :             {
                'PIDp_MIN'              :  5.0,
                'DeltaPID_MIN'         :  5.0,
                'P_MIN'                 :  10000 * MeV
        },
      'D02HH'   : {
        'TisTosSpec'               : "Hlt1SMOGSingleTrackDecision%TOS",
        'Pair_AMINDOCA_MAX'        : 0.10 * mm,
        'Trk_Max_APT_MIN'          : 500.0 * MeV, 
        'D0_VCHI2PDOF_MAX'         : 10.0,       # neuter
        'Comb_AM_MIN'              : 1775.0 * MeV, 
        'Comb_AM_MAX'              : 1955.0 * MeV, 
        'Trk_ALL_PT_MIN'           : 250.0 * MeV, 
        'Trk_ALL_P_MIN'            : 2.0  * GeV, 
        'Mass_M_MIN'               :  1784.0 * MeV,
        'Mass_M_MAX'               :  1944.0 * MeV,
        },
      'Dpm2HHH' : {
        'TisTosSpec'               : "Hlt1SMOGSingleTrackDecision%TOS",
        'Trk_ALL_PT_MIN'           :  200.0 * MeV,
        'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
        'Trk_1OF3_PT_MIN'          :  1000.0 * MeV,
        'VCHI2PDOF_MAX'            :  25.0,
        'ASUMPT_MIN'               :  0 * MeV,
        'AM_MIN'                   :  1779 * MeV,
        'AM_MAX'                   :  1959 * MeV,
        'Mass_M_MIN'               :  1789.0 * MeV,
        'Mass_M_MAX'               :  1949.0 * MeV,
        },
      'Ds2HHH' : { 
        'TisTosSpec'               : "Hlt1SMOGSingleTrackDecision%TOS",
        'Trk_ALL_PT_MIN'           :  200.0 * MeV,
        'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
        'Trk_1OF3_PT_MIN'          :  1000.0 * MeV,
        'VCHI2PDOF_MAX'            :  25.0,
        'ASUMPT_MIN'               :  0 * MeV,
        'AM_MIN'                   :  1879 * MeV,
        'AM_MAX'                   :  2059 * MeV,
        'Mass_M_MIN'               :  1889.0 * MeV,
        'Mass_M_MAX'               :  2049.0 * MeV,
        },
      'Lc2HHH' : {
        'TisTosSpec'               : "Hlt1SMOGSingleTrackDecision%TOS",
        'Trk_ALL_PT_MIN'           :  200.0 * MeV,
        'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
        'Trk_1OF3_PT_MIN'          :  1000.0 * MeV,
        'VCHI2PDOF_MAX'            :  25.0,
        'ASUMPT_MIN'               :  0 * MeV,
        'AM_MIN'                   :  2201 * MeV,
        'AM_MAX'                   :  2553 * MeV,
        'Mass_M_MIN'               :  2211.0 * MeV,
        'Mass_M_MAX'               :  2543.0 * MeV,
        },
      'B2PiMu' : {
          'TisTosSpec'             : "Hlt1SMOGSingleTrackDecision%TOS",
          'AM_MIN'                 : 0 * MeV,
          'AM_MAX'                 : 1000.0 * GeV,
          'ASUMPT_MIN'             : 4.0 * GeV,
          'VCHI2PDOF_MAX'          : 30.0,
          'Mass_M_MIN'             : 0 * MeV,
          'Mass_M_MAX'             : 1000.0 * GeV
        },
      }

  def stages(self, nickname = ""):

    if hasattr(self, '_stages') and self._stages:
      if nickname:
        return self._stages[nickname]
      else:
        return self._stages

    from Stages import MassFilter
    from Stages import D02KPi
    from Stages import DpToKmPipPip
    from Stages import DspToKmKpPim
    from Stages import LcpToKmPpPip
    from Stages import B2PiMu

    self._stages = {
        'SMOGDpm2KPiPi'       : [MassFilter('Dpm2KPiPi', nickname='Dpm2HHH', inputs=[DpToKmPipPip], shared=True)],
        'SMOGDs2KKPi'         : [MassFilter('Ds2KKPi',   nickname='Ds2HHH',  inputs=[DspToKmKpPim], shared=True)],
        'SMOGLc2KPPi'         : [MassFilter('Lc2KPPi',   nickname='Lc2HHH',  inputs=[LcpToKmPpPip], shared=True)],
        'SMOGD02KPi'          : [MassFilter('D02KPi',    nickname='D02HH',   inputs=[D02KPi],       shared=True)],
        'SMOGB2PiMu'          : [MassFilter('B2PiMu',    nickname='B2PiMu',  inputs=[B2PiMu],       shared=True)],
        }

    if nickname:
      return self._stages[nickname]
    else:
        return self._stages

  def __apply_configuration__(self):

    from HltLine.HltLine import Hlt2Line
    from Configurables import HltANNSvc
    stages = self.stages()
    for (nickname, algos) in self.algorithms(stages).iteritems():
      linename = nickname
      Hlt2Line( linename, prescale = self.prescale, algos = algos, postscale = self.postscale )
