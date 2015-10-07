"""
Hlt2 lines for Bc->J/psi X, including the following lines
Hlt2TFBc2JpsiMuX
Hlt2Bc2JpsiH
"""
__author__  = "Jibo He, Jibo.He@cern.ch"

from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class Bc2JpsiXLines(Hlt2LinesConfigurableUser):
    __slots__ = {'_stages' : {},
                 'TFBc2JpsiMuX' : {'Prescale': 1.0,
                                   'JpsiCuts': """
                                               (MINTREE('mu+'==ABSID,PT) >  1.2*GeV)
                                               & (ADMASS('J/psi(1S)') < 220 *MeV)
                                               & (VFASPF(VCHI2PDOF) < 20)
                                               """,
                                   'DauCuts' : """
                                               (PT > 2.0*GeV)
                                               & (TRCHI2DOF < 3)
                                               """,
                                   'ComCuts' : """
                                               (in_range( 3.0*GeV, AM, 7.1*GeV))
                                               """,
                                   'MomCuts' : """
                                               (VFASPF(VCHI2PDOF) < 25)
                                               & (in_range( 3.1*GeV, MM, 7.0*GeV))
                                               & (PT> 5.0 *GeV)
                                               """
                                   },
                 'Bc2JpsiH'     : {'Prescale': 1.0,
                                   'JpsiCuts': """
                                               (MINTREE('mu+'==ABSID,PT) > 0.8*GeV)
                                               & (ADMASS('J/psi(1S)') < 100*MeV)
                                               & (VFASPF(VCHI2PDOF) < 25)
                                               """,
                                   'DauCuts' : """
                                               (PT > 1.5*GeV)
                                               & (TRCHI2DOF < 3)
                                               """,
                                   'ComCuts' : """
                                               (in_range( 6.0*GeV, AM, 6.7*GeV))
                                               """,
                                   'MomCuts' : """
                                               (VFASPF(VCHI2PDOF) < 25)
                                               & (in_range( 6.1*GeV, MM, 6.6*GeV))
                                               & (PT > 5.*GeV)
                                               & (BPVLTIME()>0.2*ps)
                                               """
                                   }
                 }

    def stages(self, nickname = ""):
        if hasattr(self, '_stages') and self._stages:
            if nickname:
                return self._stages[nickname]
            else:
                return self._stages

        from Stages import Bc2JpsiMuXCombiner, Bc2JpsiHCombiner
        self._stages = {'TFBc2JpsiMuX'          : [Bc2JpsiMuXCombiner('TFBc2JpsiMuX')],
                        'Bc2JpsiH'              : [Bc2JpsiHCombiner('Bc2JpsiH')]
                        }
        if nickname:
            return self._stages[nickname]
        else:
            return self._stages


    def __apply_configuration__(self):
        from HltLine.HltLine import Hlt2Line

        stages = self.stages()
        cuts   = self.getProps()
        for nickname, algos in self.algorithms(stages):
            Hlt2Line(nickname,
                     prescale = cuts[nickname].get('Prescale', 1.0),
                     L0DU     = cuts[nickname].get('L0Req'   , None),
                     HLT1     = cuts[nickname].get('Hlt1Req' , None),
                     algos = algos, postscale = self.postscale)
