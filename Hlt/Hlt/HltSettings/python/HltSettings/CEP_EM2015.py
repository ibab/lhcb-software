from GaudiKernel.SystemOfUnits import GeV, MeV, mm

class CEP_EM2015 :
    """
    Threshold settings for Hlt2 CEP lines: EM 2015

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS

    @author C. Linn
    @date 2015-05-19
    """

    __all__ = ( 'ActiveHlt2Lines' )


    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [
             'Hlt2LowMultDiMuon_PS'
            ,'Hlt2LowMultDiMuon'
            ,'Hlt2LowMultMuon'
            ,'Hlt2LowMultDiElectron_noTrFilt'
            ,'Hlt2LowMultDiElectron'
            ,'Hlt2LowMultDiPhoton'
            ,'Hlt2LowMultPi0'
            ,'Hlt2LowMultDiPhoton_HighMass'
            ,'Hlt2LowMultChiC2HHHHWS'
            ,'Hlt2LowMultD2KPi'
            ,'Hlt2LowMultChiC2HHWS'
            ,'Hlt2LowMultLMR2HH_mediumPS'
            ,'Hlt2LowMultChiC2HHHH'
            ,'Hlt2LowMultLMR2HHWS'
            ,'Hlt2LowMultLMR2HHWS_mediumPS'
            ,'Hlt2LowMultLMR2HHWS_heavyPS'
            ,'Hlt2LowMultD2K3PiWS'
            ,'Hlt2LowMultLMR2HH'
            ,'Hlt2LowMultHadron_noTrFilt'
            ,'Hlt2LowMultD2KKPiWS'
            ,'Hlt2LowMultChiC2PP'
            ,'Hlt2LowMultLMR2HHHHWS_heavyPS'
            ,'Hlt2LowMultD2KPiPi'
            ,'Hlt2LowMultChiC2HH'
            ,'Hlt2LowMultD2KKPi'
            ,'Hlt2LowMultLMR2HHHH_heavyPS'
            ,'Hlt2LowMultL2pPiWS'
            ,'Hlt2LowMultLMR2HHHHWS_mediumPS'
            ,'Hlt2LowMultL2pPi'
            ,'Hlt2LowMultChiC2PPWS'
            ,'Hlt2LowMultD2K3Pi'
            ,'Hlt2LowMultLMR2HHHH'
            ,'Hlt2LowMultD2KPiPiWS'
            ,'Hlt2LowMultLMR2HHHHWS'
            ,'Hlt2LowMultLMR2HH_heavyPS'
            ,'Hlt2LowMultD2KPiWS'
            ,'Hlt2LowMultLMR2HHHH_mediumPS'
            ,'Hlt2LowMultTechnical_MinBias'
            ,'Hlt2NonBeamBeamNoBias',
        ]


        return lines

    def Thresholds(self) :

        d = {}
        from Hlt2Lines.CEP.Lines import CEPLines
        d.update ({CEPLines : { 
                    'Prescale' : { 'Hlt2LowMultL2pPi'       : 1.0
                                 , 'Hlt2LowMultL2pPiWS'     : 0.2
                                 , 'Hlt2LowMultD2KPi'       : 1.0
                                 , 'Hlt2LowMultD2KPiWS'     : 0.2
                                 , 'Hlt2LowMultD2KPiPi'     : 1.0
                                 , 'Hlt2LowMultD2KPiPiWS'   : 0.2
                                 , 'Hlt2LowMultD2KKPi'      : 1.0
                                 , 'Hlt2LowMultD2KKPiWS'    : 0.2
                                 , 'Hlt2LowMultD2K3Pi'      : 1.0
                                 , 'Hlt2LowMultD2K3PiWS'    : 0.2
                                 , 'Hlt2LowMultChiC2HH'     : 1.0
                                 , 'Hlt2LowMultChiC2HHWS'   : 0.2
                                 , 'Hlt2LowMultChiC2HHHH'   : 1.0
                                 , 'Hlt2LowMultChiC2HHHHWS' : 0.2
                                 , 'Hlt2LowMultChiC2PP'     : 1.0
                                 , 'Hlt2LowMultChiC2PPWS'   : 0.2
                                 , 'Hlt2LowMultLMR2HH'      : 1.0
                                 , 'Hlt2LowMultLMR2HHWS'    : 0.2
                                 , 'Hlt2LowMultLMR2HHHH'    : 1.0
                                 , 'Hlt2LowMultLMR2HHHHWS'  : 0.2
                                 , 'Hlt2LowMultLMR2HH_mediumPS'      : 0.0
                                 , 'Hlt2LowMultLMR2HHWS_mediumPS'    : 0.0
                                 , 'Hlt2LowMultLMR2HHHH_mediumPS'    : 0.0
                                 , 'Hlt2LowMultLMR2HHHHWS_mediumPS'  : 0.0
                                 , 'Hlt2LowMultLMR2HH_heavyPS'      : 0.0
                                 , 'Hlt2LowMultLMR2HHWS_heavyPS'    : 0.0
                                 , 'Hlt2LowMultLMR2HHHH_heavyPS'    : 0.0
                                 , 'Hlt2LowMultLMR2HHHHWS_heavyPS'  : 0.0
                                 , 'Hlt2LowMultHadron_noTrFilt' : 1.0
                                 # Muon lines
                                 , 'Hlt2LowMultDiMuon'      : 1.0
                                 , 'Hlt2LowMultDiMuon_PS'   : 1.0
                                 , 'Hlt2LowMultMuon'        : 1.0
                                 # Photon lines
                                 , 'Hlt2LowMultDiPhoton'    : 1.0
                                 , 'Hlt2LowMultDiPhoton_HighMass'    : 1.0
                                 , 'Hlt2LowMultPi0'         : 1.0
                                 # Electron lines
                                 , 'Hlt2LowMultDiElectron'    : 1.0
                                 , 'Hlt2LowMultDiElectron_noTrFilt': 1.0
                                 # Technical lines
                                 , 'Hlt2LowMultTechnical_NoBias'        : 0.33 # Aim for 100 Hz from 300 Hz non-bb crossings
                                 , 'Hlt2LowMultTechnical_MinBias'       : 1.0
                                 }
                    , 'HLT'               :   {"Hadron"     : "HLT_PASS_RE('Hlt1CEPVeloCutDecision')",
                                               "Muon"       : "HLT_PASS_RE('Hlt1NoPVPassThroughDecision')",
                                               "Photon"     : "HLT_PASS_RE('Hlt1NoPVPassThroughDecision')",
                                               "Electron"   : "HLT_PASS_RE('Hlt1NoPVPassThroughDecision')",
                                               "TechnicalNoBias"  : "HLT_PASS_RE('Hlt1NonBeamBeamNoBiasDecision')"
                                               }
                    , 'Common'    : {'H_PTmin'        :     100.0 * MeV,
                                     'H_Pmin'         :     5000.0 * MeV,
                                     'H_TrkChi2max'   :     3.0}
                    , 'L0Channels'         : {'Hadron'  : ['DiHadron,lowMult'],
                                              'Muon'    : ['Muon,lowMult','DiMuon,lowMult'],
                                              'Photon'  : ['Photon,lowMult','DiEM,lowMult'],
                                              'Electron': ['Electron,lowMult','DiEM,lowMult']
                                             }
                    , 'Technical_L0'  : {"NoBias"          : "" ,
                                         "MinBias"         : "(L0_DATA('Spd(Mult)') < 100) & ( (L0_DATA('Spd(Mult)') > 2) | (L0_DATA('Electron(Et)') > 5) | (L0_DATA('Photon(Et)') > 5) | (L0_DATA('Hadron(Et)') > 10) )"}
                    , 'Technical_ODIN': {"ALL"             : "" }
 
                }
        })

        return d
