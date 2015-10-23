from GaudiKernel.SystemOfUnits import GeV, MeV, mm

class LowMult_25ns_October2015 :
    """
    Threshold settings for Hlt2 Low Multiplicity (LowMult) lines

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS

    @author D. Johnson daniel.johnson@cern.ch
    @date 2015-10-20
    """

    __all__ = ( 'ActiveHlt2Lines' )


    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [
             'Hlt2LowMultDiMuon_PS'
            ,'Hlt2LowMultDiMuon'
            # ,'Hlt2LowMultDiMuonTurboCalib'
            # ,'Hlt2LowMultLMR2HHTurboCalib'
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
            ,'Hlt2NoBiasNonBeamBeam',
        ]


        return lines

    def Thresholds(self) :

        d = {}
        from Hlt2Lines.LowMult.Lines import LowMultLines
        d.update ({LowMultLines : { 
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
                                 , 'Hlt2LowMultLMR2HH'      : 0.5
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
                                 , 'Hlt2LowMultHadron_noTrFilt' : 0.05
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
                                 , 'Hlt2NonBeamBeamNoBias'              : 0.33 # Aim for 100 Hz from 300 Hz non-bb crossings
                                 , 'Hlt2LowMultTechnical_MinBias'       : 1.0
                                 }
                    , 'HLT'               :   {"Hadron"     : "( HLT_PASS_RE('Hlt1LowMultPassThroughDecision') | HLT_PASS_RE('Hlt1LowMultDecision') | HLT_PASS_RE('Hlt1LowMultVeloCut_HadronsDecision') )",
                                               "Muon"       : "( HLT_PASS_RE('Hlt1LowMultPassThroughDecision') | HLT_PASS_RE('Hlt1LowMultDecision') | HLT_PASS_RE('Hlt1LowMultVeloCut_LeptonsDecision') )",
                                               "Photon"     : "( HLT_PASS_RE('Hlt1LowMultPassThroughDecision') | HLT_PASS_RE('Hlt1LowMultMaxVeloCutDecision') )",
                                               "Electron"   : "( HLT_PASS_RE('Hlt1LowMultPassThroughDecision') | HLT_PASS_RE('Hlt1LowMultDecision') | HLT_PASS_RE('Hlt1LowMultVeloCut_LeptonsDecision') | HLT_PASS_RE('Hlt1LowMultMaxVeloCutDecision') )",
                                               "TechnicalNoBias"  : "HLT_PASS_RE('Hlt1NoBiasNonBeamBeamDecision')"
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
                    # ElectronLines
                    , 'DiElectron'      :   {'nVeloTracksmax'  :     8,
                                             'nBackTracksmax'  :     1,
                                             'e_PTmin'         :     250 * MeV}
                    , 'DiElectron_noTrFilt':{'nVeloTracksmax'  :     10000,
                                             'nBackTracksmax'  :     10000,
                                             'e_PTmin'         :     250 * MeV} 
                    # MuonLines
                    , 'DiMuon'    :   {'nVeloTracksmax'   :     100,
                                       'nBackTracksmax'   :     5,
                                       'AMmin'            :     0.0 * GeV,
                                       'mu_PTmin'         :     400 * MeV}
                    , 'DiMuon_PS' :   {'nVeloTracksmax'   :     100,
                                       'nBackTracksmax'   :     5,
                                       'AMmin'            :     0.0 * GeV,
                                       'mu_PTmin'         :     400 * MeV}
                    , 'Muon'      :   {'nVeloTracksmax'   :     8,
                                       'nBackTracksmax'   :     1,
                                       'mu_PTmin'         :     400 * MeV}
                    # PhotonLines
                    , 'DiPhoton'           :   {'nVeloTracksmax'   :     10
                                               ,'nBackTracksmax'   :     1
                                               ,'gamma_PTmin'      :     500 * MeV
                                               ,'digamma_AMmin'    :     0.0 * GeV
                                               ,'digamma_AMmax'    :     10000.0 * GeV}
                    , 'DiPhoton_HighMass'  :   {'nVeloTracksmax'   :     10
                                               ,'nBackTracksmax'   :     1
                                               ,'gamma_PTmin'      :     500 * MeV
                                               ,'digamma_AMmin'    :     2.0 * GeV
                                               ,'digamma_AMmax'    :     10000.0 * GeV}
                    , 'pi0'                :   {'nVeloTracksmax'   :     10
                                               ,'nBackTracksmax'   :     1
                                               ,'pi0_PTmin'        :     200 * MeV}
                    # HadronLines
                    , 'L2pPi'    :   {'APTmin'         :     0.0 * MeV,
                                      'APTmax'         :     100000.0 * GeV,
                                      'APmin'          :     10000.0 * MeV,
                                      'ADOCAmax'       :     0.5 * mm,
                                      'VtxChi2DoFmax'  :     15.0,
                                      'nVeloTracksmax' :     12,
                                      'nBackTracksmax' :     1,
                                      'AMmin'          :     1036.0 * MeV,
                                      'AMmax'          :     1196.0 * MeV,
                                      'P_PIDpmin'      :     0.0,
                                      'Pi_PIDKmax'     :     10000}
                    , 'D2KPi'    :   {'APTmin'         :     0.0 * MeV,
                                      'APTmax'         :     100000.0 * GeV,
                                      'APmin'          :     10000.0 * MeV,
                                      'ADOCAmax'       :     0.5 * mm,
                                      'VtxChi2DoFmax'  :     15.0,
                                      'nVeloTracksmax' :     12,
                                      'nBackTracksmax' :     1,
                                      'AMmin'          :     1785.0 * MeV,
                                      'AMmax'          :     1945.0 * MeV,
                                      'K_PIDKmin'      :     0.0,
                                      'Pi_PIDKmax'     :     10000}
                    , 'D2KPiPi'  :   {'APTmin'         :     0.0 * MeV,
                                      'APTmax'         :     100000.0 * GeV,
                                      'APmin'          :     10000.0 * MeV,
                                      'ADOCAmax'       :     0.5 * mm,
                                      'VtxChi2DoFmax'  :     15.0,
                                      'nVeloTracksmax' :     12,
                                      'nBackTracksmax' :     1,
                                      'AMmin'          :     1785.0 * MeV,
                                      'AMmax'          :     1945.0 * MeV,
                                      'K_PIDKmin'      :     0.0,
                                      'Pi_PIDKmax'     :     10000}
                    , 'D2K3Pi'   :   {'APTmin'         :     0.0 * MeV,
                                      'APTmax'         :     100000.0 * GeV,
                                      'APmin'          :     10000.0 * MeV,
                                      'ADOCAmax'       :     0.7 * mm,
                                      'VtxChi2DoFmax'  :     15.0,
                                      'nVeloTracksmax' :     12,
                                      'nBackTracksmax' :     1,
                                      'AMmin'          :     1785.0 * MeV,
                                      'AMmax'          :     1945.0 * MeV,
                                      'K_PIDKmin'      :     0.0,
                                      'Pi_PIDKmax'     :     10000}
                    , 'ChiC2HH'   :   {'APTmin'         :     0.0 * MeV,
                                       'APTmax'         :     5000.0 * MeV,
                                       'APmin'          :     10000.0 * MeV,
                                       'ADOCAmax'       :     0.5 * mm,
                                       'VtxChi2DoFmax'  :     15.0,
                                       'nVeloTracksmax' :     8,
                                       'nBackTracksmax' :     1,
                                       'AMmin'          :     2850.0 * MeV,
                                       'AMmax'          :     3600.0 * MeV,
                                       'K_PIDKmin'      :     0.0,
                                       'Pi_PIDKmax'     :     10000}
                    , 'ChiC2HHHH' :   {'APTmin'         :     0.0 * MeV,
                                       'APTmax'         :     5000.0 * MeV,
                                       'APmin'          :     10000.0 * MeV,
                                       'ADOCAmax'       :     0.7 * mm,
                                       'VtxChi2DoFmax'  :     15.0,
                                       'nVeloTracksmax' :     8,
                                       'nBackTracksmax' :     1,
                                       'AMmin'          :     2850.0 * MeV,
                                       'AMmax'          :     4500.0 * MeV,
                                       'K_PIDKmin'      :     0.0,
                                       'Pi_PIDKmax'     :     10000}
                    , 'ChiC2PP'   :   {'APTmin'         :     0.0 * MeV,
                                       'APTmax'         :     5000.0 * MeV,
                                       'APmin'          :     10000.0 * MeV,
                                       'ADOCAmax'       :     0.5 * mm,
                                       'VtxChi2DoFmax'  :     15.0,
                                       'nVeloTracksmax' :     8,
                                       'nBackTracksmax' :     1,
                                       'AMmin'          :     2850.0 * MeV,
                                       'AMmax'          :     3600.0 * MeV,
                                       'P_PIDpmin'      :     0.0}
                    , 'LMR2HH'    :   {'APTmin'         :     0.0 * MeV,
                                       'APTmax'         :     10000.0 * GeV,
                                       'APmin'          :     0.0 * MeV,
                                       'ADOCAmax'       :     0.2 * mm,
                                       'VtxChi2DoFmax'  :     4.0,
                                       'nVeloTracksmax' :     6,
                                       'nBackTracksmax' :     1,
                                       'AMmin'          :     0.240 * GeV,
                                       'AMmax'          :     15000.0 * GeV,
                                       'K_PIDKmin'      :     0.0,
                                       'Pi_PIDKmax'     :     10000,
                                       'P_PIDpmin'      :     0.0}
                    , 'LMR2HHHH'  :   {'APTmin'         :     0.0 * MeV,
                                       'APTmax'         :     5000.0 * MeV,
                                       'APmin'          :     0.0 * MeV,
                                       'ADOCAmax'       :     0.3 * mm,
                                       'VtxChi2DoFmax'  :     4.0,
                                       'nVeloTracksmax' :     8,
                                       'nBackTracksmax' :     1,
                                       'AMmin'          :     400.0 * MeV,
                                       'AMmax'          :     10000.0 * GeV,
                                       'K_PIDKmin'      :     0.0,
                                       'Pi_PIDKmax'     :     10000}
                    , 'LMR2HH_mediumPS':   {'APTmin'         :     0.0 * MeV,
                                            'APTmax'         :     5000.0 * MeV,
                                            'APmin'          :     15000.0 * MeV,
                                            'ADOCAmax'       :     0.2 * mm,
                                            'VtxChi2DoFmax'  :     4.0,
                                            'nVeloTracksmax' :     8,
                                            'nBackTracksmax' :     1,
                                            'AMmin'          :     0.0 * MeV,
                                            'AMmax'          :     15000.0 * GeV,
                                            'K_PIDKmin'      :     0.0,
                                            'Pi_PIDKmax'     :     10000,
                                            'P_PIDpmin'      :     0.0}
                    , 'LMR2HHHH_mediumPS'  :   {'APTmin'         :     0.0 * MeV,
                                            'APTmax'         :     5000.0 * MeV,
                                            'APmin'          :     15000.0 * MeV,
                                            'ADOCAmax'       :     0.2 * mm,
                                            'VtxChi2DoFmax'  :     4.0,
                                            'nVeloTracksmax' :     8,
                                            'nBackTracksmax' :     1,
                                            'AMmin'          :     400.0 * MeV,
                                            'AMmax'          :     5000.0 * MeV,
                                            'K_PIDKmin'      :     0.0,
                                            'Pi_PIDKmax'     :     10000}
                    , 'LMR2HH_heavyPS'    :   {'APTmin'         :     0.0 * MeV,
                                            'APTmax'         :     5000.0 * MeV,
                                            'APmin'          :     15000.0 * MeV,
                                            'ADOCAmax'       :     0.2 * mm,
                                            'VtxChi2DoFmax'  :     4.0,
                                            'nVeloTracksmax' :     8,
                                            'nBackTracksmax' :     1,
                                            'AMmin'          :     0.0 * MeV,
                                            'AMmax'          :     15000.0 * GeV,
                                            'K_PIDKmin'      :     0.0,
                                            'Pi_PIDKmax'     :     10000,
                                            'P_PIDpmin'      :     0.0}
                    , 'LMR2HHHH_heavyPS'  :   {'APTmin'         :     0.0 * MeV,
                                            'APTmax'         :     5000.0 * MeV,
                                            'APmin'          :     15000.0 * MeV,
                                            'ADOCAmax'       :     0.2 * mm,
                                            'VtxChi2DoFmax'  :     4.0,
                                            'nVeloTracksmax' :     8,
                                            'nBackTracksmax' :     1,
                                            'AMmin'          :     400.0 * MeV,
                                            'AMmax'          :     5000.0 * MeV,
                                            'K_PIDKmin'      :     0.0,
                                            'Pi_PIDKmax'     :     10000}
                    , 'Hadron_noTrFilt':{'PTmin'        :     0.5 * GeV}
                }
        })

        return d
