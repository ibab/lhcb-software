# Quasi two-body (Fred Blanc)
BetaSQ2B = {
    'WGs'               : ['betas'],
    'BUILDERTYPE'       : 'B2Quasi2Body4piConf',
    'CONFIG'    : {'Q2BPrescale'     : 1.,
                   'Q2BResMinPT'     : 1500.,
                   'Q2BResMinP'      : 10.,
                   'Q2BResMaxMass'   : 1000.,
                   'Q2BResVtxChiDOF' : 9.,
                   'Q2BBMinM'        : 2500.,
                   'Q2BBMaxM'        : 5700.,
                   'Q2BBMinCorrM'    : 3500.,
                   'Q2BBMaxCorrM'    : 7000.,
                   'Q2BBVtxChi2DOF'  : 9.,
                   'Q2BBMinTrkPT'    : 1500.
                   },
    'STREAMS' : [ 'Bhadron' ]
    }

# Bs2KstKst (Paula)
BetaSBs2KstKst = {
    'WGs'               : ['betas'],
    'BUILDERTYPE'       : 'StrippingBs2KstKstConf',
    'CONFIG': {     
    "KaonPT"              : 500.0 # MeV
    ,   "KaonIPCHI2"            : 9.    # adimensional
    ,   "PionPT"                : 500.0 # MeV    
    ,   "PionIPCHI2"            : 9.       # adimensional
    ,   "KstarVCHI2"            : 9.0   # adimensional
    ,   "KstarPT"               : 900.0 # MeV 
    ,   "KaonPIDK"              : -5.0  # adimensional
    ,   "KstarMassWin"          : 150.0 # MeV 
    ,   "BMassWin"              : 500.0 # MeV
    ,   "BVCHI2"                : 15.0  # adimensional
    ,   "BDOCA"                 : 0.3   # mm
    ,   "BIPCHI2"               : 25    # adimensional
    },
    'STREAMS' : [ 'Bhadron' ]
    }


# Cibran
BetaSBs2PhiKst = {
        'WGs'           : ['betas'],
        'BUILDERTYPE'   : 'StrippingBs2PhiKstConf',
        'CONFIG'        : {     
                "KaonPT"                : 500.0 # MeV
          ,     "KaonIPCHI2"            : 9.    # adimensional
          ,     "KaonPIDK"              : 0.    # adimensional
          ,     "PionPT"                : 500.0 # MeV
          ,     "PionIPCHI2"            : 9.    # adimensional
          ,     "PhiVCHI2"              : 9.    # adimensional
          ,     "PhiPT"                 : 900.0 # MeV
          ,     "PhiMassWin"            : 25.0  # MeV
          ,     "KstarVCHI2"            : 9.0   # adimensional
          ,     "KstarPT"               : 900.0 # MeV
          ,     "KstarMassWin"          : 150.0 # MeV
          ,     "BMassWin"              : 500.0 # MeV
          ,     "BVCHI2"                : 15.0  # adimensional
          ,     "BDOCA"                 : 0.3   # mm
          },
        'STREAMS' : [ 'Bhadron' ]
        }
