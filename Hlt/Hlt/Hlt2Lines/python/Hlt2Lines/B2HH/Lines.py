from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class B2HHLines(Hlt2LinesConfigurableUser):
    __slots__ = {'Prescale' : {},
                 'Common'   : { 'BMassWinLow'   : 4800 * MeV,  
                                'BMassWinHigh'  : 6200 * MeV,  
                                'TrChi2'        : 3,    
                                'PionPT'        : 1000 * MeV, 
                                'PidCutPions'   : -5,
                                'PidCutKaons'   : 2,
                                'PidCutProtons' : 2, 
                                'SumPT'         : 4500 * MeV,  
                                'PionIPCHI2'    : 9,  
                                'DOCACHI2'      : 25,   
                                'BIPCHI2'       : 9,  
                                'BDIRA'         : 0.9,  
                                'BPT'           : 1200 * MeV,  
                                'BFDCHI2'       : 100,
                                'BLT'           : 0.2 * picosecond }
                }
    
    def __apply_configuration__(self):
        from Stages import SelB2HH, SelB2PiPi, SelB2KPi, SelB2KK, SelLb2PK, SelLb2PPi
        stages = { 'B2HH'   : [ SelB2HH   ],
                   'B2PiPi' : [ SelB2PiPi ],
                   'B2KPi'  : [ SelB2KPi  ],
                   'B2KK'   : [ SelB2KK   ],
                   'Lb2PK'  : [ SelLb2PK  ],
                   'Lb2PPi' : [ SelLb2PPi ] }

        from HltLine.HltLine import Hlt2Line
        for nickname, algos in self.algorithms(stages).iteritems():
            Hlt2Line(nickname, prescale = self.prescale,
                     algos = algos, postscale = self.postscale)
