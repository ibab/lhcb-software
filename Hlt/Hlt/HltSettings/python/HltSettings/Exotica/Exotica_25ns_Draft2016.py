from GaudiKernel.SystemOfUnits import MeV, picosecond, mm

class Exotica_25ns_Draft2016:
    """
    Threshold settings for Hlt2 exotica lines for 2016 25 ns data-taking.

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS

    @author P. Ilten and M. Williams
    @date 2016-02-12
    """


    __all__ = ( 'ActiveHlt2Lines' )

    def ActiveHlt2Lines(self) :
        """Returns a list of active lines."""

        lines = [
                 'Hlt2ExoticaDisplPhiPhi',
                 'Hlt2ExoticaQuadMuonNoIP',
                 'Hlt2ExoticaDisplDiMuon',
                 'Hlt2ExoticaDisplDiMuonNoPoint',
                 'Hlt2ExoticaPrmptDiMuonTurbo',
                 'Hlt2ExoticaPrmptDiMuonSSTurbo',
                 'Hlt2ExoticaPrmptDiMuonHighMass'
                ]

        return lines
    
    def Thresholds(self) :
        """Return the trigger thresholds."""

        d = {}

        from Hlt2Lines.Exotica.Lines import ExoticaLines
        d.update( 
            { ExoticaLines : {
                'Common' : {'GhostProb' : 0.3},
                'ExoticaDisplPhiPhi' : {'TisTosSpec' : "Hlt1IncPhi.*Decision%TOS" ,
                                        'KPT'  : 500*MeV,
                                        'KIPChi2' : 16,
                                        'KProbNNk' : 0.1,
                                        'PhiPT'  : 1000*MeV,
                                        'PhiMassWindow' : 20*MeV,
                                        'VChi2' : 10,
                                        'FDChi2' : 45},
                'ExoticaSharedDiMuonNoIP' : {'MuPT' : 500*MeV,
                                             'MuP' : 10000*MeV,
                                             'MuProbNNmu' : 0.2,
                                             'DOCA' : 0.2*mm,
                                             'VChi2' : 10},
                'ExoticaQuadMuonNoIP' : {'TisTosSpec' : None,
                                         'PT' : 0,
                                         'VChi2' : 10},       
                'ExoticaDisplDiMuon' : {'TisTosSpec' : None,
                                        'MuProbNNmu' : 0.5,
                                        'MuIPChi2' : 16,
                                        'PT' : 1000*MeV,                                   
                                        'IPChi2' : 16, 
                                        'FDChi2' : 30},
                'ExoticaDisplDiMuonNoPoint' : {'TisTosSpec' : None,
                                               'MuProbNNmu' : 0.8,
                                               'MuIPChi2' : 16,
                                               'PT' : 1000*MeV,                                   
                                               'FDChi2' : 30},
                'ExoticaPrmptDiMuonTurbo' : {'TisTosSpec' : "Hlt1DiMuon(HighMass)?(NoIP)?Decision%TOS",
                                             'MuPT' : 1000*MeV,
                                             'MuP' : 20000*MeV,
                                             'MuProbNNmu' : 0.95,
                                             'MuIPChi2' : 6,
                                             'PT' : 1000*MeV,                                   
                                             'FDChi2' : 45},
                'ExoticaPrmptDiMuonSSTurbo' : {'TisTosSpec' : "Hlt1DiMuon(HighMass)?(NoIP)?Decision%TOS",
                                               'MuPT' : 1000*MeV,
                                               'MuP' : 20000*MeV,
                                               'MuProbNNmu' : 0.95, 
                                               'MuIPChi2' : 6,
                                               'DOCA' : 0.2*mm,
                                               'VChi2' : 10,
                                               'PT' : 1000*MeV,                                   
                                               'FDChi2' : 45},
                'ExoticaPrmptDiMuonHighMass' : {'TisTosSpec' : "Hlt1DiMuonHighMassDecision%TOS",
                                                'MuPT' : 500*MeV,
                                                'MuP' : 10000*MeV,                                    
                                                'M'          : 3200*MeV,
                                                'MuProbNNmu' : 0.9, 
                                                'MuIPChi2' : 6,
                                                'PT' : 1000*MeV,                                   
                                                'FDChi2' : 45}
            }
          }
        )

        return d
