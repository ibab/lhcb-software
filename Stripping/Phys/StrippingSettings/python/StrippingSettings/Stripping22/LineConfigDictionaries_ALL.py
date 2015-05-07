"""
Line config dictionaries for Early Measurements data stripping.
"""

from GaudiKernel.SystemOfUnits import GeV





#-----#
# QEE #
#-----#

InclbJets = {
    'BUILDERTYPE' : 'InclbJetsConf',
    'WGs'         : [ 'QEE' ],
    'STREAMS'     : [ 'EW'  ],
    'CONFIG'      : {
        'NrPVsCut'      : 1,
        'NrSeedsCut'    : 2,
        'TrkChi2Cut'    : 3.0,
        'PrtMomCut'     : 2.0,
        'PrtPtCut'      : 0.6,
        'PrtIPSCut'     : 2.5,
        'DZSVPVCut'     : 1.0,
        'SumMomSVCut'   : 1.0,
        'VtxChi2Cut'    : 20.0,
        'scale'         : 0.05
    },
}

LowMultINC = {
    'BUILDERTYPE' : 'LowMultINCLines',
    'WGs'         : [ 'QEE' ],
    'STREAMS'     : [ 'BhadronCompleteEvent' ],
    'CONFIG'      : {
        'LowMultRequiredRawEvents' : None, #["Velo","Heschel"], Please we need to add this!
        #
        'PrescaleL0DiHadron'  : 0,
        'DecisionL0DiHadron'  : None,#"L0_CHANNEL('DiHadron,lowMult')",
        'PrescaleL0DiEM'      : 0,
        'DecisionL0DiEM'      : None,#"L0_CHANNEL('DiEM,lowMult')",
        'PrescaleL0Photon'    : 0,
        'DecisionL0Photon'    : None,#"L0_CHANNEL('Photon,lowMult')",
        'PrescaleL0Muon'      : 0,
        'DecisionL0Muon'      : None,#"L0_CHANNEL('Muon,lowMult')",
        'PrescaleL0DiMuon'    : 0,
        'DecisionL0DiMuon'    : None,#"L0_CHANNEL('DiMuon,lowMult')",
        'PrescaleL0Electron'  : 0,
        'DecisionL0Electron'  : None,#"L0_CHANNEL('Electron,lowMult')",
        #
        #
        #
        #
        'PrescaleLowMultL2pPi'     : 1,
        'DecisionLowMultL2pPi'     : "HLT_PASS_RE('Hlt2LowMultL2pPiDecision') | HLT_PASS_RE('Hlt2LowMultL2pPiWSDecision')",
        'PrescaleLowMultD2KPi'     : 1,
        'DecisionLowMultD2KPi'     : "HLT_PASS_RE('Hlt2LowMultD2KPiDecision') | HLT_PASS_RE('Hlt2LowMultD2KPiWSDecision')",
        'PrescaleLowMultD2KPiPi'   : 1,
        'DecisionLowMultD2KPiPi'   : "HLT_PASS_RE('Hlt2LowMultD2KPiPiDecision') | HLT_PASS_RE('Hlt2LowMultD2KPiPiWSDecision')",
        'PrescaleLowMultD2KKPi'    : 1,
        'DecisionLowMultD2KKPi'    : "HLT_PASS_RE('Hlt2LowMultD2KKPiDecision') | HLT_PASS_RE('Hlt2LowMultD2KKPiWSDecision')",
        'PrescaleLowMultD2K3Pi'    : 1,
        'DecisionLowMultD2K3Pi'    :"HLT_PASS_RE('Hlt2LowMultD2K3PiDecision') | HLT_PASS_RE('Hlt2LowMultD2K3PiWSDecision')",
        'PrescaleLowMultChiC2HH'   : 1,
        'DecisionLowMultChiC2HH'   :"HLT_PASS_RE('Hlt2LowMultChiC2HHDecision') | HLT_PASS_RE('Hlt2LowMultChiC2HHWSDecision')",
        'PrescaleLowMultChiC2HHHH' : 1,
        'DecisionLowMultChiC2HHHH' :"HLT_PASS_RE('Hlt2LowMultChiC2HHHHDecision') | HLT_PASS_RE('Hlt2LowMultChiC2HHHHWSDecision')",
        'PrescaleLowMultChiC2PP'   : 1,
        'DecisionLowMultChiC2PP'   :"HLT_PASS_RE('Hlt2LowMultChiC2PPDecision') | HLT_PASS_RE('Hlt2LowMultChiC2PPWSDecision')",
        #
        'PrescaleLowMultLMR2HH'           : 1,
        'DecisionLowMultLMR2HH'           :"HLT_PASS_RE('Hlt2LowMultLMR2HHDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHWSDecision')",
        'PrescaleLowMultLMR2HHHH'         : 1,
        'DecisionLowMultLMR2HHHH'         :"HLT_PASS_RE('Hlt2LowMultLMR2HHHHDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHHHWSDecision')",
        'PrescaleLowMultLMR2HH_mediumPS'  : 1,
        'DecisionLowMultLMR2HH_mediumPS'  :"HLT_PASS_RE('Hlt2LowMultLMR2HH_mediumPSDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHWS_mediumPSDecision')",
        'PrescaleLowMultLMR2HHHH_mediumPS': 1,
        'DecisionLowMultLMR2HHHH_mediumPS': "HLT_PASS_RE('Hlt2LowMultLMR2HHHH_mediumPSDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHHHWS_mediumPSDecision')",
        'PrescaleLowMultLMR2HH_heavyPS'   : 1,
        'DecisionLowMultLMR2HH_heavyPS'   : "HLT_PASS_RE('Hlt2LowMultLMR2HH_heavyPSDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHWS_heavyPSDecision')",
        'PrescaleLowMultLMR2HHHH_heavyPS' : 1,
        'DecisionLowMultLMR2HHHH_heavyPS' : "HLT_PASS_RE('Hlt2LowMultLMR2HHHH_heavyPSDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHHHWS_heavyPSDecision')",
        # Unbiased hadron line
        'PrescaleLowMultHadron_noTrFilt' : 1,
        'DecisionLowMultHadron_noTrFilt' : "HLT_PASS_RE('Hlt2LowMultHadron_noTrFiltDecision')",
        #
        #
        #
        'PrescaleLowMultDiMuon'             : 1,
        'DecisionLowMultDiMuon'             : "HLT_PASS('Hlt2LowMultDiMuonDecision')",
        'PrescaleLowMultDiMuon_PS'          : 1,
        'DecisionLowMultDiMuon_PS'          : "HLT_PASS('Hlt2LowMultDiMuon_PSDecision')",
        'PrescaleLowMultMuon'               : 1,
        'DecisionLowMultMuon'               : "HLT_PASS('Hlt2LowMultMuonDecision')",
        #
        'PrescaleLowMultDiElectron'         : 1,
        'DecisionLowMultDiElectron'         : "HLT_PASS_RE('Hlt2LowMultDiElectronDecision')",
        'PrescaleLowMultDiElectron_noTrFilt': 1,
        'DecisionLowMultDiElectron_noTrFilt': "HLT_PASS_RE('Hlt2LowMultDiElectron_noTrFiltDecision')",
        #
        'PrescaleLowMultDiPhoton'         : 1,
        'DecisionLowMultDiPhoton'         : "HLT_PASS_RE('Hlt2LowMultDiPhotonDecision')",
        'PrescaleLowMultDiPhoton_HighMass': 1,
        'DecisionLowMultDiPhoton_HighMass': "HLT_PASS_RE('Hlt2LowMultDiPhoton_HighMassDecision')",
        'PrescaleLowMultPi0'              : 1,
        'DecisionLowMultPi0'              : "HLT_PASS_RE('Hlt2LowMultPi0Decision')",
        #
        'PrescaleLowMultTMP'         : 0,
        'DecisionLowMultTMP'         : None,
    }
}

MuMuSS = {
    'BUILDERTYPE' : 'MuMuSSConf',
    'WGs'         : [ 'QEE' ],
    'STREAMS'     : [ 'EW'  ],
    'CONFIG'      : { 
        'MuMuSSLine1Prescale' : 0.1,
        'MuMuSSLine2Prescale' : 1.0,
        'MuMuSSLine3Prescale' : 1.0,
        'MuMuSSLine4Prescale' : 1.0,
        'MuMuSSLinePostscale' : 1.0,  
        'MuMuSSLine1MinMass'  : 3.2  * GeV,
        'MuMuSSLine1MaxMass'  :  5.  * GeV,
        'MuMuSSLine2MinMass'  :  5.  * GeV,
        'MuMuSSLine2MaxMass'  : 10.  * GeV,
        'MuMuSSLine3MinMass'  : 10.  * GeV,
        'MuMuSSLine3MaxMass'  : 20.  * GeV,
        'MuMuSSLine4MinMass'  : 20.  * GeV,
        'p'     : 10. * GeV,
        'pT1'   : 1.5 * GeV,
        'pT2'   :  3. * GeV
    },
}

SingleTrackTIS = {
    'BUILDERTYPE' : 'SingleTrackTISConf',
    'WGs'         : [ 'QEE' ],
    'STREAMS'     : [ 'EW'  ],
    'CONFIG'      : { 
        'SingleTrackTIS_Prescale'    : 0.1,
        'SingleTrackTISLow_Prescale' : 0.01,
        'SingleTrackTIS_Postscale'   : 1.00,
        'pT'    : 20. * GeV,
        'pTlow' : 15. * GeV,
    },
}


WMu = {
    'BUILDERTYPE' : 'WMuConf',
    'WGs'         : [ 'QEE' ],
    'STREAMS'     : [ 'EW'  ],
    'CONFIG'      : { 
        'WMu_Prescale'        : 1.0,
        'WMuLow_Prescale'     : 0.1,
        'STNB_Prescale'       : 0.2,
        'WMu_Postscale'       : 1.0,
        'SingMuon10_Prescale' : 0.01,
        'SingMuon48_Prescale' : 0.4,
        'pT'                  : 20. * GeV,
        'pTlow'               : 15. * GeV,
        'pTvlow'              :  5. * GeV,
        'SingMuon10_pT'       : 10. * GeV,
        'SingMuon48_pT'       : 4.8 * GeV,
    },
}


Z02MuMu = {
    'BUILDERTYPE' : 'Z02MuMuConf',
    'WGs'         : [ 'QEE' ],
    'STREAMS'     : [ 'EW'  ],
    'CONFIG'      : { 
        'Z02MuMu_Prescale'  : 1.0,
        'Z02MuMu_Postscale' : 1.0,
        'pT'    : 3.  * GeV,
        'MMmin' : 40. * GeV,
    },
}
