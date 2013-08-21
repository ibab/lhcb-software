"""
Line config dictionaries for the EW Stream. Mostly QEE WG lines


Dictionary names should have the same name as the corresponding
StrippingSelections file containing the line builder instance.

   e.g  StrippingPromptCharm linebuilder should be added as:

      PromptCharm = { ... }
"""
from GaudiKernel.SystemOfUnits import MeV, mm

#LowMult
LowMult = {
    'BUILDERTYPE'  : 'LowMultConf',
    'CONFIG'       : {
    'LowMultPrescale'           : 1.0
    , 'LowMultZeroPrescale'     : 0.0
    , 'LowMultWSPrescale'       : 0.1
    , 'LowMultHHIncPrescale'    : 0.1
    , 'LowMultLMRPrescale'      : 0.2
    , 'LowMultPrescale_ps'      : 0.005
    , 'LowMultNoFilterPrescale' : 0.1
    , 'LowMultPostscale'        : 1.0
    # Final-state particles
    , 'H_PTmin'         : 100.0 * MeV
    , 'H_Pmin'          : 5000.0 * MeV
    , 'H_TrkChi2max'    : 3.0
    , 'K_PIDKmin'       : 0.0
    , 'P_PIDPmin'       : 0.0
    , 'KKInc_K_Pmin'    : 10000.0 * MeV
    , 'KKInc_K_PIDKmin' : 5.0
    # D0 -> KPi
    , 'D2KPi_APTmin'        : 0.0 * MeV
    , 'D2KPi_ADAMASSmax'    : 80.0 * MeV
    , 'D2KPi_ADOCAmax'      : 0.5 * mm
    , 'D2KPi_APmin'         : 10000.0 * MeV
    , 'D2KPi_VtxChi2DoFmax' : 15.0
    # D+- -> KPiPi
    , 'D2KPiPi_APTmin'        : 0.0 * MeV
    , 'D2KPiPi_ADAMASSmax'    : 80.0 * MeV
    , 'D2KPiPi_ADOCAmax'      : 0.5 * mm
    , 'D2KPiPi_APmin'         : 10000.0 * MeV
    , 'D2KPiPi_VtxChi2DoFmax' : 15.0
    # D0 -> K3Pi
    , 'D2K3Pi_APTmin'        : 0.0 * MeV
    , 'D2K3Pi_ADAMASSmax'    : 80.0 * MeV
    , 'D2K3Pi_ADOCAmax'      : 0.7 * mm
    , 'D2K3Pi_APmin'         : 10000.0 * MeV
    , 'D2K3Pi_VtxChi2DoFmax' : 15.0
    # 'ChiC' -> HH (H = K, Pi)
    , 'ChiC2HH_APTmin'        : 0.0 * MeV
    , 'ChiC2HH_APTmax'        : 5000.0 * MeV
    , 'ChiC2HH_AMmin'         : 2850.0 * MeV
    , 'ChiC2HH_AMmax'         : 4500.0 * MeV
    , 'ChiC2HH_ADOCAmax'      : 0.5 * mm
    , 'ChiC2HH_APmin'         : 10000.0 * MeV
    , 'ChiC2HH_VtxChi2DoFmax' : 15.0
    # 'ChiC' -> PP
    , 'ChiC2PP_APTmin'        : 0.0 * MeV
    , 'ChiC2PP_APTmax'        : 5000.0 * MeV
    , 'ChiC2PP_AMmin'         : 2850.0 * MeV
    , 'ChiC2PP_AMmax'         : 3650.0 * MeV
    , 'ChiC2PP_ADOCAmax'      : 0.5 * mm
    , 'ChiC2PP_APmin'         : 10000.0 * MeV
    , 'ChiC2PP_VtxChi2DoFmax' : 15.0
    # 'ChiC' -> HHHH (H = K, Pi)
    , 'ChiC2HHHH_APTmin'        : 0.0 * MeV
    , 'ChiC2HHHH_APTmax'        : 5000.0 * MeV
    , 'ChiC2HHHH_AMmin'         : 2850.0 * MeV
    , 'ChiC2HHHH_AMmax'         : 4500.0 * MeV
    , 'ChiC2HHHH_ADOCAmax'      : 0.7 * mm
    , 'ChiC2HHHH_APmin'         : 10000.0 * MeV
    , 'ChiC2HHHH_VtxChi2DoFmax' : 15.0
    # Low-mass resonance -> HH (H = K, Pi)
    , 'LMR2HH_APTmin'        : 500.0 * MeV
    , 'LMR2HH_APTmax'        : 1500.0 * MeV
    , 'LMR2HH_AMmin'         : 450.0 * MeV
    , 'LMR2HH_AMmax'         : 1500.0 * MeV
    , 'LMR2HH_ADOCAmax'      : 0.1 * mm
    , 'LMR2HH_APmin'         : 15000.0 * MeV
    , 'LMR2HH_VtxChi2DoFmax' : 3.0
    # Phi resonance -> KK
    , 'PHI2KK_APTmin'        : 0.0 * MeV
    , 'PHI2KK_APTmax'        : 1500.0 * MeV
    , 'PHI2KK_AMmin'         : 990.0 * MeV
    , 'PHI2KK_AMmax'         : 1050.0 * MeV
    , 'PHI2KK_ADOCAmax'      : 0.1 * mm
    , 'PHI2KK_APmin'         : 4000.0 * MeV
    , 'PHI2KK_VtxChi2DoFmax' : 3.0
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }



