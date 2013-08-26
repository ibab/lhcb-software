"""
Line config dictionaries for the EW Stream. Mostly QEE WG lines


Dictionary names should have the same name as the corresponding
StrippingSelections file containing the line builder instance.

   e.g  StrippingPromptCharm linebuilder should be added as:

      PromptCharm = { ... }
"""
from GaudiKernel.SystemOfUnits import GeV,MeV, mm

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


#SbarSCorrelations
SbarSCorrelations = {
    'BUILDERTYPE'  : 'SbarSCorrelationsBuilder',   
    'CONFIG'       : {
    'HLT' : "HLT_PASS('Hlt1MBNoBiasDecision')|HLT_PASS('Hlt1MBMicroBiasTStationDecision')|HLT_PASS('Hlt1MBMicroBiasVeloDecision')|HLT_PASS('Hlt1MBMicroBiasTStationRateLimitedDecision')|HLT_PASS('Hlt1MBMicroBiasVeloRateLimitedDecision')"
    ,  'LongTrackGEC'          :    1000 # 150 or 500 might be a better choice
    ,  'Trk_P_MIN'             : 5000 # to limit material interactions for Kaons
    ,  'isLong'                : '(ISLONG)'
    ,  'KAON_PIDK_MIN'         : 8 # DLL_KAON_vs_PION
    ,  'KAON_PIDKp_MIN'        : 0 # DLL_KAON_vs_PROTON
    ,  'KAON_ipChi2_MAX'                : 49 # IPCHI2 with respect to best PV (do not know if UPCHI2 or IP is used, but Iguess IPCHI2 was used to tag the best PV )
#
    ,   'PION_ipChi2_MIN'       : 9
    ,   'PROTON_ipChi2_MIN'     : 9
    ,   'PION_P_MIN'            : 2000
    ,   'PROTON_P_MIN'          : 2000
#
    ,   'Fisher'                : 10
    ,   'Lambda_V_Chi2_Max'      : 9
    ,   'Lambda_Adamass'        : 50
    ,   'Lambda_ipChi2_MAX'     : 49
#                   
    , 'Phiprescale'             :    0.05
    , 'F2prescale'              :    1.0
    , 'LambdaCprescale'         :    1.0                   
    , 'postscale'             :    1.0
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }

#WeJets
WeJets = {
    'BUILDERTYPE'  : 'WeJetsConf',
    'CONFIG'       : {
    'WeJets_Prescale'    : 1.0,
    'WeJets_Postscale'   : 1.0,
    'PrsCalMin'          : 50.,
    'ECalMin'            : 0.10,
    'HCalMax'            : 0.05,
    'min_pT'             : 10.,
    'max_pT'             : 20.,
    'GEC_TrkPt'          : 60.,
    'GEC_TrkSPt'         : 15.,
    'min_pT_IP_Particle'  : 3,
    'min_IPchi2'          : 40.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }

#WMuJets
WMuJets = {
    'BUILDERTYPE'  : 'WMuJetsConf',
    'CONFIG'       : {
    'WMuJets_Prescale'    : 1.0,
    'WMuJets_Postscale'   : 1.0,
    'min_pT'              : 10.,
    'max_pT'              : 20.,
    'pT_isol'             : 3.,
    'GEC_TrkSumPt'        : 70., 
    'GEC_TrkMissPt'       : 25., 
    'min_pT_IP_Particle'  : 3,
    'min_IPchi2'          : 40.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }

#HighPtTopoJets   
HighPtTopoJets = {
    'BUILDERTYPE'  : 'HighPtTopoJetsConf',
    'CONFIG'       : {
    'HighPtTopo40000_Prescale'    : 1.0,
    'HighPtTopo20000_Prescale'    : 0.1,  
    'HighPtTopo10000_Prescale'    : 0.008,
    'HighPtTopo1000_Prescale'    : 0.001, 
    'HighPtTopo_Postscale'   : 1.0,
    'pT'        : 40000,
    'pT20'      : 20000,
    'pT10'      : 10000,
    'pT1'       : 1000, 
    'conesize'  : 0.3, 
    'pTcone'    : 55000
    },
    'WGs' : [ 'QEE' ],     
    'STREAMS' : [ 'EW' ]   
    }

#Bjets
Bjets = {
    'BUILDERTYPE'  : 'BJetsLinesConf',  
    'CONFIG'       : {
    "1B2jetlow" : { "PTmin" : 8*GeV ,  "prescale" : 0.005 },
    "1B2jethigh" : { "PTmin" : 14*GeV ,  "prescale" : 0.1 },
    "1B4jet" : { "PTmin" : 16*GeV ,  "prescale" : 1 },
    "2B3jet" : { "PTmin" : 11*GeV ,  "prescale" : 1 },
    "2B4jet" : { "PTmin" : 8*GeV ,  "prescale" : 1 },
    "my2TOPO" : {"ANGLE_MIN":2/57., "M_MIN":0, "DPHI_MIN":0}
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }

