from GaudiKernel.SystemOfUnits import MeV, picosecond, mm

class Exotica_25ns_Draft2016:

    __all__ = ( 'ActiveHlt2Lines' )

    def ActiveHlt2Lines(self) :
        """Returns a list of active lines."""

        lines = [
                 'Hlt2DisplPhiPhi',
                 'Hlt2QuadMuonNoIP',
                 'Hlt2DisplDiMuon',
                 'Hlt2DisplDiMuonNoPoint',
                 'Hlt2PrmptDiMuonTurbo',
                 'Hlt2PrmptDiMuonSSTurbo',
                 'Hlt2PrmptDiMuonHighMass'
                ]

        return lines
    
    def Thresholds(self) :
        """Return the trigger thresholds."""

        d = {}

        from Hlt2Lines.Exotica.Lines import ExoticaLines
        d.update( 
            { ExoticaLines : {
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
                                 'MuProbNNmu' : 0.5,
                                 'MuIPChi2' : 16,
                                 'PT' : 1000*MeV,                                   
                                 'IPChi2' : 16, 
                                 'FDChi2' : 30},
                'DisplDiMuonNoPoint' : {'TisTosSpec' : None,
                                        'MuProbNNmu' : 0.8,
                                        'MuIPChi2' : 16,
                                        'PT' : 1000*MeV,                                   
                                        'FDChi2' : 30},
                'PrmptDiMuonTurbo' : {'TisTosSpec' : "Hlt1DiMuon(HighMass)?(NoIP)?Decision%TOS",
                                      'MuPT' : 1000*MeV,
                                      'MuP' : 20000*MeV,
                                      'MuProbNNmu' : 0.95,
                                      'MuIPChi2' : 6,
                                      'PT' : 1000*MeV,                                   
                                      'FDChi2' : 45},
                'PrmptDiMuonSSTurbo' : {'TisTosSpec' : "Hlt1DiMuon(HighMass)?(NoIP)?Decision%TOS",
                                        'MuPT' : 1000*MeV,
                                        'MuP' : 20000*MeV,
                                        'MuProbNNmu' : 0.95, 
                                        'MuIPChi2' : 6,
                                        'DOCA' : 0.2*mm,
                                        'VChi2' : 10,
                                        'PT' : 1000*MeV,                                   
                                        'FDChi2' : 45},
                'PrmptDiMuonHighMass' : {'TisTosSpec' : "Hlt1DiMuonHighMassDecision%TOS",
                                         'MuPT' : 500*MeV,
                                         'MuP' : 10000*MeV,                                    
                                         'M'          : 2700*MeV,
                                         'MuProbNNmu' : 0.9, 
                                         'MuIPChi2' : 6,
                                         'PT' : 1000*MeV,                                   
                                         'FDChi2' : 45}
            }
          }
        )

        return d
