from GaudiKernel.SystemOfUnits import GeV, MeV 

from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser
class DiMuonLines(Hlt2LinesConfigurableUser) :
    __slots__ = {'Prescale' : {},

                 'Common' :        {'TrChi2'     :   10,
                                    'TrChi2Tight':    5},

                 'DiMuon' :        {'MinMass'    :  2700 * MeV,
                                    'Pt'         :  1000 * MeV,
                                    'MuPt'       :   500 * MeV,
                                    'VertexChi2' :    25},
                  
                 'JPsi' :          {'MassWindow' :    70 * MeV,
                                    'Pt'         :  1000 * MeV,
                                    'MuPt'       :   500 * MeV,
                                    'VertexChi2' :    25},

                 'JPsiHighPT' :    {'Pt'         :  4000 * MeV,
                                    'MassWindow' :   150 * MeV,
                                    'MuPt'       :   500 * MeV,
                                    'VertexChi2' :    25},

                 'Psi2S' :         {'MassWindow' :    70 * MeV,
                                    'Pt'         :  1000 * MeV,
                                    'MuPt'       :  1500 * MeV,
                                    'VertexChi2' :    25},

                 'Psi2SHighPT' :   {'MassWindow' :    70 * MeV,
                                    'Pt'         :  3500 * MeV,
                                    'MuPt'       :  1500 * MeV,
                                    'VertexChi2' :    25},
                 
                 'B' :             {'MinMass'    :  5200 * MeV,
                                    'VertexChi2' :    25},

                 'Z' :             {'MinMass'    : 40000 * MeV,
                                    'Pt'         :     0 * MeV},
                                                 
                 'Detached' :      {'IPChi2'     :     9,
                                    'DLS'        :     5},

                 'DetachedHeavy' : {'MinMass'    :  2950 * MeV,
                                    'Pt'         :     0 * MeV,
                                    'MuPt'       :   300 * MeV,
                                    'VertexChi2' :    25,
                                    'IPChi2'     :     9,
                                    'DLS'        :     3},
                 
                 'DetachedJPsi' :  {'DLS'        :     3},

                 'DetachedPsi2S' : {'DLS'        :     3},
                 }

    def stages(self):
        if hasattr(self, '__stages') and self.__stages:
            return self.__stages

        from Stages import (DiMuonFilter, JpsiFilter, Psi2SFilter,
                            BFilter, ZFilter, DetachedDiMuonFilter,
                            DetachedDiMuonHeavyFilter, DetachedJpsiFilter,
                            DetachedPsi2SFilter)
        self.__stages = {'DiMuon'        : [DiMuonFilter('DiMuon')],
                        'JPsi'          : [JpsiFilter('JPsi')],
                        'JPsiHighPT'    : [JpsiFilter('JPsiHighPT')],
                        'Psi2S'         : [Psi2SFilter('Psi2S')],
                        'Psi2SHighPT'   : [Psi2SFilter('Psi2SHighPT')],
                        'B'             : [BFilter('B')],
                        'Z'             : [ZFilter('Z')],
                        'Detached'      : [DetachedDiMuonFilter('Detached')],
                        'DetachedHeavy' : [DetachedDiMuonHeavyFilter('DetachedHeavy')],
                        'DetachedJPsi'  : [DetachedJpsiFilter('DetachedJPsi')],
                        'DetachedPsi2S' : [DetachedPsi2SFilter('DetachedPsi2S')]}
        return self.__stages

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        stages = self.stages()
        for (nickname, algos) in self.algorithms(stages).iteritems():
            linename = 'DiMuon' + nickname if nickname != 'DiMuon' else nickname
            Hlt2Line(linename, prescale = self.prescale,
                     algos = algos, postscale = self.postscale)
