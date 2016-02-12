##
#  @author F. Redi Federico.Redi@cern.ch
#  @date 2016-02-10
#
#  Please contact the abovementioned responsibles before editing this file
#
##


from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class MajoranaLines(Hlt2LinesConfigurableUser):
    __slots__ = {'_stages' : {},
                 'Prescale' : {},
                 'Common' : {'TrChi2' : 3,
                             'TrGP' : 0.3},
                 'LambdaMuPi': {"MuonGHOSTPROB"         : 0.5   ,#adimensional
                                "MuonTRCHI2"            : 4.    ,#adimensional
                                "MuonP"                 : 3000.* MeV,
                                "MuonPT"                : 250. * MeV,
                                "MuonPIDK"              : 0.    ,#adimensional
                                "MuonPIDmu"             : 0.    ,#adimensional
                                "MuonPIDp"              : 0.    ,#adimensional
                                "MuonMINIPCHI2"         : 12    ,#adminensional
                                #Lambda Daughter Cuts
                                "Lambda0DaugP"          : 2000.* MeV,
                                "Lambda0DaugPT"         : 250. * MeV,
                                "Lambda0DaugTrackChi2"  : 4.    ,#adimensional
                                "Lambda0DaugMIPChi2"    : 10.   ,#adimensional
                                #Lambda cuts
                                "MajoranaCutFDChi2"     : 100.,  #adimensional
                                "MajoranaCutM"          : 300. * MeV,
                                "Lambda0VertexChi2"     : 10.   ,#adimensional
                                "Lambda0PT"             : 700.  ,#adimensional
                                },
                 'BLambdaMu':  {#B Mother Cuts
                                "BVCHI2DOF"             : 4.    ,#adminensional
                                "BDIRA"                 : 0.99  ,#adminensional
                                "LambdaMuMassLowTight"  : 300.  * MeV,
                                "XMuMassUpperHigh"      : 6500. * MeV,
                                'LambdaZ'               : 0. * mm
                                }
                }

    def stages(self, nickname=""):
        if hasattr(self, '_stages') and self._stages:
            if nickname:
                return self._stages[nickname]
            else:
                return self._stages

        from Stages import BLambdaMuLL, BLambdaMuDD
        self._stages = {'BLambdaMuLL' : [BLambdaMuLL],
                        'BLambdaMuDD' : [BLambdaMuDD]}

        if nickname:
            return self._stages[nickname]
        else:
            return self._stages

    def __apply_configuration__(self):
        from HltLine.HltLine import Hlt2Line
        stages = self.stages()

        for (nickname, algos) in self.algorithms(stages):
            Hlt2Line('Majorana' + nickname,
                     prescale = self.prescale,
                     algos = algos,
                     postscale = self.postscale)
