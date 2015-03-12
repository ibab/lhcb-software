from GaudiKernel.SystemOfUnits import GeV, MeV, mm 
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser
class EWLines(Hlt2LinesConfigurableUser) :

    __slots__ = {'Prescale' : {'Hlt2EWDiMuonDY1'              : 0.005, #or 0.1
                               'Hlt2EWDiMuonDY2'              :  0.03, #or none
                               'Hlt2EWDiMuonDY3'              : 1.0,
                               'Hlt2EWDiMuonDY4'              : 1.0,
                               'Hlt2EWSingleMuonLowPt'        : 0.002, #or 0.1
                               'Hlt2EWSingleMuonHighPt'       : 1.0,
                               'Hlt2EWSingleMuonVHighPt'      : 1.0,
                               'Hlt2EWDiElectronDY'           : 1.0,
                               'Hlt2EWDiElectronHighMass'     : 1.0,
                               'Hlt2EWSingleTFElectronLowPt'  : 0.001, #or 0.1
                               'Hlt2EWSingleTFElectronHighPt' : 0.01,  #or none
                               'Hlt2EWSingleTFElectronVHighPt': 1.0},
                 'Common'    :{},
                 #=====DiMuon=====
                 'DiMuonZ' :                 {'MinMass'    : 40000 * MeV,
                                              'Pt'         :     0 * MeV},
                 
                 'DiMuonDY1' :               {'MinMass'    :  2500 * MeV,
                                              'MinExcMass' :  3000 * MeV,
                                              'MaxExcMass' :  3200 * MeV,
                                              'MuPt'       :   800 * MeV,
                                              'Pt'         :     0 * MeV,
                                              'TkChi2'     :    10},
                 
                 'DiMuonDY2' :               {'MinMass'    :  5000 * MeV,
                                              'MuPt'       :  1000 * MeV,
                                              'Pt'         :     0 * MeV,
                                              'TkChi2'     :    10},
                 
                 'DiMuonDY3' :               {'MinMass'    : 10000 * MeV,
                                              'Pt'         :     0 * MeV,
                                              'TkChi2'     :    10},
                 
                 'DiMuonDY4' :               {'MinMass'    : 20000 * MeV,
                                              'Pt'         :     0 * MeV,
                                              'TkChi2'     :    10},
                 
                 #=====SingleMuon=====
                 'SingleMuonLowPt' :         {'Pt'         :  4800 * MeV,
                                              'TkChi2'     :    10},

                 'SingleMuonHighPt' :        {'Pt'         : 10000 * MeV},
                 
                 'SingleMuonVHighPt' :       {'Pt'         : 15000 * MeV},

                 #=====DiElectron=====
                 'DiElectronDY' :            {'L0Req'      : "L0_CHANNEL('Electron')",
                                              'Hlt1Req'    : "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')",
                                              'MinMass'    :  4800 * MeV,
                                              'MaxMass'    : 1e+10 * MeV,
                                              'VtxChi2'    :    25,
                                              'Pt'         :  -999 * MeV,
                                              'ElecPt'     :  1000 * MeV,
                                              'ElecPIDe'   :   1.5,
                                              'ElecTkChi2' :    10,
                                              'PrsMin'     :    50,
                                              'EcalMin'    :   0.1,
                                              'HcalMax'    :  0.05},

                 'DiElectronHighMass' :      {'L0Req'      : "L0_CHANNEL('Electron')",
                                              'Hlt1Req'    : "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')",
                                              'MinMass'    : 20000 * MeV,
                                              'VtxChi2'    :    25,
                                              'TkChi2'     :    10,
                                              'Pt'         :  -999 * MeV,
                                              'ElecPt'     : 10000 * MeV,
                                              'PrsMin'     :    50,
                                              'EcalMin'    :   0.1,
                                              'HcalMax'    :  0.05},

                 #=====SingleElectron=====
                 'SingleTFElectron' :        {'L0Req'      : "L0_CHANNEL('Electron')",
                                              'Hlt1Req'    : "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')",
                                              'Pt'         : 10000 * MeV,
                                              'IP'         :  0.05 * mm,
                                              'PIDe'       :     4,  #unused?
                                              'IPChi2'     :    -1,
                                              'TkChi2'     :     5,
                                              'PrsMin'     :    50,
                                              'EcalMin'    :   0.1,
                                              'HcalMax'    :  0.05},
                 
                 'SingleTFElectronLowPt' :   {'L0Req'      : "L0_CHANNEL('Electron')",
                                              'Hlt1Req'    : "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')",
                                              'Pt'         :  4800 * MeV,
                                              'PIDe'       :     4,  #unused?
                                              'TkChi2'     :     5,
                                              'PrsMin'     :    50,
                                              'EcalMin'    :   0.1,
                                              'HcalMax'    :  0.05},

                 'SingleTFElectronHighPt' :  {'L0Req'      : "L0_CHANNEL('Electron')",
                                              'Hlt1Req'    : "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')",
                                              'Pt'         : 10000 * MeV,
                                              'PrsMin'     :    50,
                                              'EcalMin'    :   0.1,
                                              'HcalMax'    :  0.05,
                                              'TkChi2'     :    20},
                 
                 'SingleTFElectronVHighPt' : {'L0Req'      : "L0_CHANNEL('Electron')",
                                              'Hlt1Req'    : "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')",
                                              'Pt'         : 15000 * MeV,
                                              'PrsMin'     :    50,
                                              'EcalMin'    :   0.1,
                                              'HcalMax'    :  0.05,
                                              'TkChi2'     :    20}
                 }                               

    def __apply_configuration__(self) :
        from Stages import (DiMuonZFilter,DiMuonDY1Filter,DiMuonDY2Filter,DiMuonDY3Filter,DiMuonDY4Filter,
                            SingleMuonLowPtFilter,SingleMuonHighPtFilter,SingleMuonVHighPtFilter,
                            DiElectronDYFilter,DiElectronHighMassFilter,
                            SingleTFElectronFilter,SingleTFElectronLowPtFilter,SingleTFElectronHighPtFilter,SingleTFElectronVHighPtFilter)

        stages = {'DiMuonZ'   : [DiMuonZFilter('DiMuonZ')],
                  'DiMuonDY1' : [DiMuonZFilter('DiMuonDY1')],
                  'DiMuonDY2' : [DiMuonZFilter('DiMuonDY2')],
                  'DiMuonDY3' : [DiMuonZFilter('DiMuonDY3')],
                  'DiMuonDY4' : [DiMuonZFilter('DiMuonDY4')],

                  'SingleMuonLowPt'   : [SingleMuonLowPtFilter('SingleMuonLowPt')],
                  'SingleMuonHighPt'  : [SingleMuonHighPtFilter('SingleMuonHighPt')],
                  'SingleMuonVHighPt' : [SingleMuonVHighPtFilter('SingleMuonVHighPt')],

                  'DiElectronDY'       : [DiElectronDYFilter('DiElectronDY')],
                  'DiElectronHighMass' : [DiElectronHighMassFilter('DiElectronHighMass')],

                  'SingleTFElectron'        : [SingleTFElectronFilter('SingleTFElectron')],
                  'SingleTFElectronLowPt'   : [SingleTFElectronLowPtFilter('SingleTFElectronLowPt')],
                  'SingleTFElectronHighPt'  : [SingleTFElectronHighPtFilter('SingleTFElectronHighPt')],
                  'SingleTFElectronVHighPt' : [SingleTFElectronVHighPtFilter('SingleTFElectronVHighPt')]}
        
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltDecodeRaw import DecodeL0CALO
        for (nickname, algos) in self.algorithms(stages).iteritems():
            cuts = self.getProp(nickname)
            linename = 'EW' + nickname if nickname != 'EW' else nickname
            Hlt2Line(linename, prescale = self.prescale,L0DU = cuts.get('L0Req', None), HLT1 = cuts.get('Hlt1Req', None),
                     algos = algos, postscale = self.postscale)
