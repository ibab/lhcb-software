##################################################################################
##                          S T R I P P I N G  21r1p1                           ##
##                                                                              ##
##  Configuration for Calibration and PID WGs                                   ##
##  Contact person: Michael Kolpin (michael.kolpin@cern.ch)                     ##
##################################################################################

from GaudiKernel.SystemOfUnits import *


LowPTMuID = {
    "BUILDERTYPE": "LowPTMuConf", 
    "CONFIG": {
        "DIRA": 0.9995, 
        "FDCHI2": 225, 
        "MassWindow": 200, 
        "Prescale": 1, 
        "ProbeIPCHI2": 25, 
        "TagIPCHI2": 45, 
        "TagPT": 1000, 
        "VertexCHI2": 5
    }, 
    "STREAMS": [ "PID" ], 
    "WGs": [ "ALL" ]
}

MuIDCalib = {
    "BUILDERTYPE": "MuIDCalibConf", 
    "CONFIG": {
        "DetachedNoMIPHiPPrescale": 0.0, 
        "DetachedNoMIPKPrescale": 1.0, 
        "DetachedNoMIPPrescale": 1, 
        "DetachedPrescale": 0.0, 
        "FromLambdacPrescale": 1.0, 
        "KFromLambdacPrescale": 0.0, 
        "KISMUONFromLambdacPrescale": 0.0, 
        "PFromLambdacPrescale": 0.0, 
        "PISMUONFromLambdacPrescale": 0.0, 
        "PiFromLambdacPrescale": 0.0, 
        "PiISMUONFromLambdacPrescale": 0.0, 
        "PromptPrescale": 0.0
    }, 
    "STREAMS": {
        "PID": [
            "StrippingMuIDCalib_JpsiFromBNoPIDNoMip", 
            "StrippingMuIDCalib_JpsiKFromBNoPIDNoMip", 
            "StrippingMuIDCalib_FromLambdacDecay"
        ]
    }, 
    "WGs": [ "ALL" ]
}

VznoPID = {
    "BUILDERTYPE": "StrippingV0ForPIDConf", 
    "CONFIG": {
        "CTauK0S": 1.0, 
        "CTauK0S_DD": 10.0, 
        "CTauLambda0": 5.0, 
        "CTauLambda0_DD": 20.0, 
        "DaughtersIPChi2": 25, 
        "DeltaMassK0S": 50.0, 
        "DeltaMassLambda": 25.0, 
        "HLT": "HLT_PASS_RE('Hlt1MB.*Decision')", 
        "KS0DD_Prescale": 0.024, 
        "KS0LL_Prescale": 0.02, 
        "LTimeFitChi2": 49, 
        "LamDDIsMUON_Prescale": 1.0, 
        "LamDD_Prescale": 0.1, 
        "LamLLIsMUON_Prescale_HiP": 1.0, 
        "LamLLIsMUON_Prescale_LoP": 1.0, 
        "LamLL_Prescale_HiP": 1.0, 
        "LamLL_Prescale_LoP": 0.14, 
        "MaxZ": 2200.0, 
        "Monitor": False, 
        "Preambulo": [
            "from GaudiKernel.PhysicalConstants import c_light", 
            "DD =    CHILDCUT ( ISDOWN , 1 ) & CHILDCUT ( ISDOWN , 2 ) ", 
            "LL =    CHILDCUT ( ISLONG , 1 ) & CHILDCUT ( ISLONG , 2 ) "
        ], 
        "Proton_IsMUONCut": "(INTREE( (ABSID=='p+') & ISMUON ) )", 
        "TrackChi2": 5, 
        "VertexChi2": 16, 
        "WrongMassK0S": 9.0, 
        "WrongMassK0S_DD": 18.0, 
        "WrongMassLambda": 20.0, 
        "WrongMassLambda_DD": 40.0
    }, 
    "STREAMS": [ "PID" ], 
    "WGs": [ "ALL" ]
}

noPIDDstar = {
    "BUILDERTYPE": "NoPIDDstarWithD02RSKPiConf", 
    "CONFIG": {
        "D0BPVDira": 0.9999, 
        "D0FDChi2": 49, 
        "D0IPChi2": 30, 
        "D0MassWin": 75.0, 
        "D0Pt": 1500.0, 
        "D0VtxChi2Ndof": 13, 
        "DCS_WrongMass": 25.0, 
        "DaugIPChi2": 16, 
        "DaugP": 2000.0, 
        "DaugPt": 250.0, 
        "DaugTrkChi2": 5, 
        "DeltaM_Max": 155.0, 
        "DeltaM_Min": 130.0, 
        "DstarPt": 2200.0, 
        "DstarVtxChi2Ndof": 13, 
        "KK_WrongMass": 25.0, 
        "Monitor": False, 
        "PiPi_WrongMass": 25.0, 
        "Postscale": 1.0, 
        "Prescale": 0.8, 
        "SlowPiPt": 150.0, 
        "SlowPiTrkChi2": 5
    }, 
    "STREAMS": [ "PID" ], 
    "WGs": [ "ALL" ]
}

