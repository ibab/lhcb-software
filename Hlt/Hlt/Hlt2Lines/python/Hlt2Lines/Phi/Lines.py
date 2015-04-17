from GaudiKernel.SystemOfUnits import GeV, MeV, mm 

from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser
class PhiLines(Hlt2LinesConfigurableUser) :
  __slots__ = {'_stages' : {},
               'Prescale': {},
               'IncPhi'  : { 'TisTosSpec'  : "Hlt1IncPhi.*Decision%TOS" ,
                             'KaonPT'      : 800 * MeV ,
                             'KaonIPS'     : 6 ,
                             'KaonPID'     : 0 ,
                             'TrChi2DOF'   : 5,
                             'PhiPT'       : 1800 * MeV,
                             'PhiVCHI2'    : 20 ,
                             'PhiMassWin'  : 20 * MeV,
                           },
	       'GoodKs'  : { 'KS_MASS_WINDOW'    : 25 * MeV,
                             'KS_PT_MIN'         : 200 * MeV,
                             'KS_VCHI2NDOF_MAX'  : 9},
               'Phi2KsKs' :{ 'Phi_MASS_WINDOW'   : 100 * MeV,
                             'Phi_PT_MIN'        : 600 * MeV,
                             'Phi_VCHI2NDOF_MAX' : 9},
               'D02KsKs' : { 'D0_MASS_WINDOW'    : 100 * MeV,
                             'D0_PT_MIN'         : 600 * MeV,
                             'D0_VCHI2NDOF_MAX'  : 9},
              }

  def stages(self, nickname = ""):
    if hasattr(self, '_stages') and self._stages:
      if nickname:
        return self._stages[nickname]
      else:
        return self._stages

    from Stages import IncPhiFilter, Phi2KsKsCombiner, D02KsKsCombiner
    self._stages = {'IncPhi'   : [IncPhiFilter('IncPhi')],
	            'Phi2KsKs' : [Phi2KsKsCombiner('Phi2KsKs')],
                    'D02KsKs'  : [D02KsKsCombiner('D02KsKs')]}

    if nickname:
      return self._stages[nickname]
    else:
      return self._stages

  def __apply_configuration__(self) :
    from HltLine.HltLine import Hlt2Line
    from Configurables import HltANNSvc
    stages = self.stages()
    for (nickname, algos) in self.algorithms(stages).iteritems():
      linename = nickname
      Hlt2Line(linename, prescale = self.prescale, algos = algos, postscale = self.postscale)
