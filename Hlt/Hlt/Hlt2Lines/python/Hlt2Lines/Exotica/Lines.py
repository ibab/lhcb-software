from GaudiKernel.SystemOfUnits import MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import \
    Hlt2LinesConfigurableUser

class ExoticaLines(Hlt2LinesConfigurableUser):
    __slots__ = {'_stages' : {},
                 'Prescale' : {},
                 'Postscale' : {},
                 'Common' : {'GhostProb' : 0.3},
                 'DisplPhiPhi' : {'TisTosSpec' : "Hlt1IncPhi.*Decision%TOS" ,
                                  'KPT'  : 500*MeV,
                                  'KIPChi2' : 16,
                                  'KProbNNk' : 0.1,
                                  'PhiPT'  : 1000*MeV,
                                  'PhiMassWindow' : 20*MeV,
                                  'VChi2' : 10,
                                  'FDChi2' : 45},
                 'SharedDiMuonNoIP' : {'MuPT' : 500*MeV,
                                       'MuP' : 10000*MeV,
                                       'MuProbNNmu' : 0.2,
                                       'DOCA' : 0.2*mm,
                                       'VChi2' : 10},
                 'QuadMuonNoIP' : {'TisTosSpec' : None,
                                   'PT' : 0,
                                   'VChi2' : 10},       
                 'DisplDiMuon' : {'TisTosSpec' : None,
                                  'MuProbNNmu' : 0.2,
                                  'MuIPChi2' : 16,
                                  'PT' : 1000*MeV,                                   
                                  'IPChi2' : 16, 
                                  'FDChi2' : 30},
                 'DisplDiMuonNoPoint' : {'TisTosSpec' : None,
                                         'MuProbNNmu' : 0.5,
                                         'MuIPChi2' : 16,
                                         'PT' : 1000*MeV,                                   
                                         'FDChi2' : 30},
                 'PrmptDiMuonTurbo' : {'TisTosSpec' : None,
                                       'MuProbNNmu' : 0.99, 
                                       'MuIPChi2' : 6,
                                       'PT' : 1000*MeV,                                   
                                       'FDChi2' : 45}, 
                 'PrmptDiMuonSSTurbo' : {'TisTosSpec' : None,
                                         'MuPT' : 500*MeV,
                                         'MuP' : 10000*MeV,
                                         'MuProbNNmu' : 0.99, 
                                         'MuIPChi2' : 6,
                                         'DOCA' : 0.2*mm,
                                         'VChi2' : 10,
                                         'PT' : 1000*MeV,                                   
                                         'FDChi2' : 45},
                 'PrmptDiMuonHighMass' : {'TisTosSpec' : None,
                                          'M'          : 5000*MeV,
                                          'MuProbNNmu' : 0.99, 
                                          'MuIPChi2' : 6,
                                          'PT' : 1000*MeV,                                   
                                          'FDChi2' : 45}, 
             }



    def stages(self, nickname = ''):
        if hasattr(self, '_stages') and self._stages:
            if nickname: return self._stages[nickname]
            else: return self._stages
        else: self._stages = {}
      
        from Stages import DisplPhiPhi, SharedDiMuonNoIP, QuadMuonNoIP, DisplDiMuon, DisplDiMuonNoPoint, PrmptDiMuon, PrmptDiMuonSS
        sharedDiMuon = [SharedDiMuonNoIP('SharedDiMuonNoIP')]

        self._stages = {'DisplPhiPhi' : [DisplPhiPhi('DisplPhiPhi')],
                        'SharedDiMuonNoIP' : sharedDiMuon,
                        'QuadMuonNoIP' : [QuadMuonNoIP('QuadMuonNoIP',sharedDiMuon)],
                        'DisplDiMuon' : [DisplDiMuon('DisplDiMuon',sharedDiMuon)],
                        'DisplDiMuonNoPoint' : [DisplDiMuonNoPoint('DisplDiMuonNoPoint',sharedDiMuon)],
                        'PrmptDiMuonTurbo' : [PrmptDiMuon('PrmptDiMuonTurbo',sharedDiMuon)],
                        'PrmptDiMuonSSTurbo' : [PrmptDiMuonSS('PrmptDiMuonSSTurbo')],
                        'PrmptDiMuonHighMass' : [PrmptDiMuon('PrmptDiMuonHighMass',sharedDiMuon,True)]}

        # Return the stages.
        if nickname: return self._stages[nickname]
        else: return self._stages

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from Configurables import HltANNSvc
        stages = self.stages()
        for (nickname, algos) in self.algorithms(stages):
            linename = nickname
            print '--> Adding', linename 
            turbo = True if (nickname.find('Turbo') > -1) else False
            Hlt2Line(linename, prescale = self.prescale, algos = algos, Turbo = turbo, postscale = self.postscale)


