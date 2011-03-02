"""
Line config dictionaries for Calibration channels

Stripping13 definitions.
Dictionaries have the name of the line builder instance.
"""

NoPIDDstarWithD02RSKPi = {
    'BUILDERTYPE' : 'NoPIDDstarWithD02RSKPiConf',
    'CONFIG' : {'DaugPt'             : 250       ## MeV
                , 'DaugP'            : 2000      ## MeV
                , 'DaugIPChi2'       : 16        ## unitless
                , 'DaugTrkChi2'      : 5         ## unitless
                , 'D0MassWin'        : 75        ## MeV
                , 'D0Pt'             : 1500      ## MeV
                , 'D0VtxChi2Ndof'    : 13        ## unitless
                , 'D0FDChi2'         : 49        ## unitless
                , 'D0BPVDira'        : 0.9999    ## unitless
                , 'D0IPChi2'         : 30        ## unit
                , 'SlowPiPt'         : 150       ## MeV
                , 'SlowPiTrkChi2'    : 5         ## unitless
                , 'DstarPt'          : 2200      ## MeV
                , 'DstarVtxChi2Ndof' : 13        ## unitless
                , 'DeltaM_Min'       : 130       ## MeV
                , 'DeltaM_Max'       : 155       ## MeV
                , 'DCS_WrongMass'    : 25        ## MeV (3 sigma veto)
                , 'KK_WrongMass'     : 25        ## MeV (3 sigma veto)
                , 'PiPi_WrongMass'   : 25        ## MeV (3 sigma veto)
                , 'Prescale'         : 0.45      ## unitless
                , 'Postscale'        : 1.0       ## unitless
                , 'Monitor'          : None      ## Monitoring?
                },
    'STREAM' : 'Calibration',
    'WGs'    : ['ALL']
    }

# NoPID Inclusive Phi Lines
# A. Powell & S. Schleich
InclPhi={
    'BUILDERTYPE' : 'InclPhiConf',
    'CONFIG' : {'KaonPT'                : 500      # MeV
                , 'KaonDLL'             : 15       # adimensional
                , 'PhiMassWindow'       : 30       # MeV
                , 'HighPtPrescale'      : 0.0015   # adimensional
                , 'LowPtPrescale'       : 0.0020   # adimensional
                , 'LDPrescale'          : 0.00002  # adimensional
                },
    'STREAM' : 'Calibration',
    'WGs'    : ['ALL']
    }

# NoPID V0 Lines
# A. Powell
V0ForPID = {
    'BUILDERTYPE' : 'StrippingV0ForPIDConf',
    'CONFIG' : {'KS0LL_Prescale'   : 0.006,
                'KS0DD_Prescale'   : 0.006,
                'LamLL_Prescale'   : 0.025,
                'LamDD_Prescale'   : 0.022,
                'LamLLIsMUON_Prescale' : 0.72,
                'LamDDIsMUON_Prescale' : 0.47
                },
    'STREAM' : 'Calibration',
    'WGs'    : ['ALL']
    }


Jpsi2eeForElectronID = {
    'BUILDERTYPE'	: 'ElectronIDConf',
    'CONFIG'	: {'Both_MIPCHI2': 9.0,
                   'Both_P': 3000.0,
                   'Both_PT': 500.0,
                   'Both_TRCHI2DOF': 5.0,
                   'LinePostscale': 1.0,
                   'LinePrescale': 0.40000000000000002,
                   'Probe_MIPCHI2': 9.0,
                   'Probe_P': 3000.0,
                   'Probe_PT': 500.0,
                   'Tag_MIPCHI2': 9.0,
                   'Tag_P': 6000.0,
                   'Tag_PIDe': 5.0,
                   'Tag_PT': 1500.0,
                   'eeCombMaxMass': 4300.0,
                   'eeCombMinMass': 2100.0,
                   'eeFDCHI2': 225.0,
                   'eeMaxMass': 4200.0,
                   'eeMinMass': 2200.0,
                   'eeVCHI2PDOF': 9.0},
    'STREAM' : 'Calibration',
    'WGs'    : ['ALL']
}

