# =============================================================================
# @file:   Definition of the lines devoted to the study of DiElectron decays
# @author: Miguel Ramos Pernas miguel.ramos.pernas@cern.ch
# @author: Jessica Prisciandaro jessica.prisciandaro@cern.ch
# @date:   2016-02-12
# =============================================================================

from GaudiKernel.SystemOfUnits import GeV, MeV, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class DiElectronLines(Hlt2LinesConfigurableUser) :
    __slots__ = {'Prescale' : { },

                 'Common'   : { },

                 'ElSoft'   : { 'VDZ'       :        0,
                                'CosAngle'  : 0.999997,
                                'IpDzRatio' :     0.02,
                                'IpProd'    :      0.5 * mm * mm,
                                'SumGP'     :      0.1,
                                'MinIpChi2' :        6,
                                'IpChi2Prod':     2000,
                                'Rho2'      :       64 * mm * mm,
                                'DOCA'      :      0.3 * mm,
                                'MaxIpChi2' :     1000,
                                'SVZ'       :      600 * mm,
                                'Mass'      :     1000 * MeV,
                                'Dira'      :        0
                                }
                 }
    
    def stages(self, nickname = "" ):
        from Stages import DiElectronFromKS0
        self._stages = { 'ElSoft'  : [DiElectronFromKS0] }
        if nickname:
            return self._stages[nickname]
        else:
            return self._stages

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        for nickname, algos in self.algorithms(self.stages()):
            linename = 'DiElectron' + nickname if nickname != 'DiElectron' else nickname
            Hlt2Line(linename, prescale = self.prescale,
                     algos = algos, postscale = self.postscale)
