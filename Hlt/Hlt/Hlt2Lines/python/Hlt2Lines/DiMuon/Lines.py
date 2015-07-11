from GaudiKernel.SystemOfUnits import GeV, MeV, mm

from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser
class DiMuonLines(Hlt2LinesConfigurableUser) :
    __slots__ = {'_stages' : {},
                 'Prescale' : {},

                 'Common' :        {'TrChi2'     :   10,
                                    'TrChi2Tight':    4},

                 'DiMuon' :        {'MinMass'    :  2900 * MeV,
                                    'Pt'         :     0 * MeV,
                                    'MuPt'       :     0 * MeV,
                                    'VertexChi2' :    25},
                 
                 'JPsi' :          {'MassWindow' :   120 * MeV,
                                    'Pt'         :     0 * MeV,
                                    'MuPt'       :     0 * MeV,
                                    'VertexChi2' :    25},

                 'JPsiHighPT' :    {'Pt'         :  2000 * MeV,
                                    'MassWindow' :   120 * MeV,
                                    'MuPt'       :     0 * MeV,
                                    'VertexChi2' :    25},

                 'Psi2S' :         {'MassWindow' :   120 * MeV,
                                    'Pt'         :  2000 * MeV,
                                    'MuPt'       :     0 * MeV,
                                    'VertexChi2' :    25},

                 'Psi2SHighPT' :   {'MassWindow' :   120 * MeV,
                                    'Pt'         :  3500 * MeV,
                                    'MuPt'       :     0 * MeV,
                                    'VertexChi2' :    25},
                 
                 'B' :             {'MinMass'    :   4700 * MeV,
                                    'VertexChi2' :    25},

                 'Z' :             {'MinMass'    : 40000 * MeV,
                                    'Pt'         :     0 * MeV},
                                                 
                 'Detached' :      {'IPChi2'     :    25,
                                    'DLS'        :     7},
                 'Soft' :          {'IP'         :   0.3 * mm ,
                                    'IPChi2Min'  :   1.5,
                                    'IPChi2Max'  :   1500,
                                    'TTHits'     :      -1,
                                    'TRACK_TRGHOSTPROB_MAX': 0.4,
                                    'MaxMass'    :   1000 * MeV,
                                    'VertexChi2' :    25,
                                    'Rho'        :     3,
                                    'SVZ'        :   650,
                                    'doca'       :   0.3,
                                    'MinVDZ'     :     0,
                                    'MinBPVDira' :     0,
                                    'MaxIpDistRatio':  1./60,
                                    'cosAngle'   :9.99998
                                    },                    

                 'DetachedHeavy' : {'MinMass'    :  2950 * MeV,
                                    'Pt'         :     0 * MeV,
                                    'MuPt'       :   300 * MeV,
                                    'VertexChi2' :    25,
                                    'IPChi2'     :     0,
                                    'DLS'        :     5},
                 
                 'DetachedJPsi' :  {'DLS'        :     3},

                 'DetachedPsi2S' : {'DLS'        :     3},
                 }

    def stages(self, nickname = ""):
        if hasattr(self, '_stages') and self._stages:
            if nickname:
                return self._stages[nickname]
            else:
                return self._stages

        from Stages import (DiMuonFilter, JpsiFilter, Psi2SFilter,
                            BFilter, ZFilter, DetachedDiMuonFilter,
                            DetachedDiMuonHeavyFilter, DetachedJpsiFilter,
                            DetachedPsi2SFilter, SoftDiMuonFilter)
        self._stages = {'DiMuon'        : [DiMuonFilter('DiMuon')],
                        'JPsi'          : [JpsiFilter('JPsi')],
                        'JPsiHighPT'    : [JpsiFilter('JPsiHighPT')],
                        'Psi2S'         : [Psi2SFilter('Psi2S')],
                        'Psi2SHighPT'   : [Psi2SFilter('Psi2SHighPT')],
                        'B'             : [BFilter('B')],
                        'Z'             : [ZFilter('Z')],
                        'Detached'      : [DetachedDiMuonFilter('Detached')],
                        'Soft'          : [SoftDiMuonFilter('Soft')],
                        'DetachedHeavy' : [DetachedDiMuonHeavyFilter('DetachedHeavy')],
                        'DetachedJPsi'  : [DetachedJpsiFilter('DetachedJPsi')],
                        'DetachedPsi2S' : [DetachedPsi2SFilter('DetachedPsi2S')]}
        if nickname:
            return self._stages[nickname]
        else:
            return self._stages

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from Configurables import HltANNSvc
        stages = self.stages()
        for (nickname, algos) in self.algorithms(stages).iteritems():
            linename = 'DiMuon' + nickname if nickname != 'DiMuon' else nickname
            Hlt2Line(linename, prescale = self.prescale,
                     algos = algos, postscale = self.postscale)
            if nickname is 'JPsi':
                HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonJPsiDecision":  50201 } )
