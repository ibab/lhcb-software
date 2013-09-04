##
## Line config dictionaries for Charmless WG
##

##
## StrippingBs2Phif0.py
##
Bs2Phif0 = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'Bs2Phif0Conf',
    'CONFIG'      : {'Q2BPrescale'       : 1.,
                     'Q2BResMinPT'       : 900.,
                     'Q2BResMinP'        : 1.,
                     'Q2BResPhiMinMass'  : 969.445,
                     'Q2BResPhiMaxMass'  : 1069.445,
                     'Q2BResf0MinMass'   : 0.,
                     'Q2BResf0MaxMass'   : 1800.,
                     'Q2BResVtxChiDOF'   : 12.,
                     'Q2BBMinM'          : 4800.,
                     'Q2BBMaxM'          : 5750.,
                     'Q2BBVtxChi2DOF'    : 12.,
                     'Q2BIPCHI2' : 20
                    },
    'STREAMS'     : { 'Bhadron' : [],
                      'BhadronCompleteEvent' : ['StrippingBs2Phif0Line']
                    }
    }

##
## StrippingLb2V0hh.py
##
Lb2V0h = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'Lb2V0hhConf',
    'CONFIG'      : {'Trk_Chi2'                 : 3.0,
                     'Lambda_DD_MassWindow'     : 20.0,
                     'Lambda_DD_VtxChi2'        : 12.0,
                     'Lambda_DD_FDChi2'         : 50.0,
                     'Lambda_DD_FD'             : 300.0,
                     'Lambda_DD_Pmin'           : 8000.0,
                     'Lambda_LL_MassWindow'     : 15.0,
                     'Lambda_LL_VtxChi2'        : 12.0,
                     'Lambda_LL_FDChi2'         : 50.0,
                     'Lb_Mlow'                  : 1319.0,
                     'Lb_Mhigh'                 : 500.0,
                     'Lb_2bodyMlow'             : 800.0,
                     'Lb_2bodyMhigh'            : 800.0,
                     'Lb_APTmin'                : 1000.0,
                     'Lb_PTmin'                 : 1500.0,
                     'LbDaug_MedPT_PT'          : 800.0,
                     'LbDaug_MaxPT_IP'          : 0.05,
                     'LbDaug_DD_maxDocaChi2'    : 5.0,
                     'LbDaug_LL_maxDocaChi2'    : 5.0,
                     'LbDaug_DD_PTsum'          : 4200.0,
                     'LbDaug_LL_PTsum'          : 3000.0,
                     'Lbh_DD_PTMin'             : 500.0,
                     'Lbh_LL_PTMin'             : 500.0,
                     'Lb_VtxChi2'               : 12.0,
                     'Lb_DD_Dira'               : 0.995,
                     'Lb_LL_Dira'               : 0.995,
                     'Lb_DD_IPCHI2wrtPV'        : 8.0,
                     'Lb_LL_IPCHI2wrtPV'        : 8.0,
                     'Lb_FDwrtPV'               : 1.0,
                     'Lb_DD_FDChi2'             : 50.0,
                     'Lb_LL_FDChi2'             : 50.0,
                     'GEC_MaxTracks'            : 250,
                     'Prescale'                 : 1.0,
                     'Postscale'                : 1.0
                    },
    'STREAMS'     : { 'Bhadron' : ['StrippingLb2V0hDDLine',
                                   'StrippingLb2V0hLLLine',
                                   'StrippingLb2V0hhDDLine',
                                   'StrippingLb2V0hhLLLine'],
                      'BhadronCompleteEvent' : []
                    }
    }

##
## StrippingB2XEta.py
##
B2XEta = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'B2XEtaConf',
    'CONFIG'      : {'Trk_Chi2'                : 3.0,
                     'Trk_PT'                  : 300.0,
                     'Trk_GP'                  : 0.5,
                     'KS_DD_MassWindow'        : 23.0,
                     'KS_DD_VtxChi2'           : 15.0,
                     'KS_DD_FDChi2'            : 20.0,
                     'KS_DD_PTmin'             : 1200.0,
                     'KS_LL_MassWindow'        : 14.0,
                     'KS_LL_VtxChi2'           : 15.0,
                     'KS_LL_FDChi2'            : 50.0,
                     'KS_LL_PTmin'             : 1200.0,
                     'L_DD_MassWindow'         : 20.0,
                     'L_DD_VtxChi2'            : 15.0,
                     'L_DD_PTmin'              : 1000.0,
                     'L_LL_MassWindow'         : 15.0,
                     'L_LL_VtxChi2'            : 15.0,
                     'L_LL_PTmin'              : 1000.0,
                     'eta_PT'                  : 2000,
                     'eta_MassWindow'          : 150,
                     'eta_vtxChi2'             : 15,
                     'eta_DOCA'                : 20,
                     'gamma_PT'                : 500, #photons from eta
                     'eta_prime_MassWindow'    : 150.0,
                     'eta_prime_PT'            : 2000.0,
                     'eta_prime_vtxChi2'       : 15.0,
                     'eta_prime_DOCA'          : 20.0,
                     'B_MassWindow'            : 750.0,
                     'B_PTmin'                 : 1500.0,
                     'BDaug_DD_maxDocaChi2'    : 20.0,
                     'BDaug_LL_maxDocaChi2'    : 20.0,
                     'B_VtxChi2'               : 15.0,
                     'B_Dira'                  : 0.9995,
                     'B_IPCHI2'                : 20.0,
                     'B_eta_IPCHI2'            : 6.0,
                     'Lb_MassWindow'           : 750.0,
                     'Lb_PTmin'                : 1000.0,
                     'LbDaug_DD_maxDocaChi2'   : 20.0,
                     'LbDaug_LL_maxDocaChi2'   : 20.0,
                     'Lb_VtxChi2'              : 20.0,
                     'Lb_Dira'                 : 0.9995,
                     'Lb_IPCHI2'               : 20.0,
                     'Lb_eta_IPCHI2'           : 6.0,
                     'GEC_MaxTracks'           : 250,
                     'Prescale'                : 1.0,
                     'Postscale'               : 1.0,
                     'etaGG_Prescale'          : 0.0
                    },
    'STREAMS'     : { 'Bhadron' : ['StrippingB2XEtaB2etapKSLLLine',
                                   'StrippingB2XEtaB2etapKSDDLine',
                                   #'StrippingB2XEtaB2etaGGKSLLLine',
                                   #'StrippingB2XEtaB2etaGGKSDDLine',
                                   'StrippingB2XEtaB2eta3piKSLLLine',
                                   'StrippingB2XEtaB2eta3piKSDDLine',
                                   'StrippingB2XEtaLb2etapLLLLine',
                                   'StrippingB2XEtaLb2etapLDDLine',
                                   #'StrippingB2XEtaLb2etaGGLLLLine',
                                   #'StrippingB2XEtaLb2etaGGLDDLine',
                                   'StrippingB2XEtaLb2eta3piLLLLine',
                                   'StrippingB2XEtaLb2eta3piLDDLine'],
                      'BhadronCompleteEvent' : []
                    }
    }

##
## StrippingB2CharmlessQuasi2Body.py
##
BetaSQ2B = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'B2Quasi2BodyConf',
    'CONFIG'      : {'Q2BPrescale'     : 1.,
                     'Q2BTrkGhostProb' : 0.5,
                     'Q2BTrkMinIPChi2' : 16.,
                     'Q2BTrkMinHiPT'   : 1000.,
                     'Q2BResMinPT'     : 600.,
                     'Q2BResMinHiPT'   : 1000.,
                     'Q2BResMaxMass'   : 1100.,
                     'Q2BResVtxChi2DOF': 9.,
                     'Q2BBMinPT'       : 2500.,
                     'Q2BBMinM3pi'     : 4200.,
                     'Q2BBMinM4pi'     : 3500.,
                     'Q2BBMaxM3pi'     : 6700.,
                     'Q2BBMaxM4pi'     : 5700.,
                     'Q2BBMaxCorrM3pi' : 7000.,
                     'Q2BBMaxCorrM4pi' : 6000.,
                     'Q2BBVtxChi2DOF'  : 9.
                    },
    'STREAMS'     : { 'Bhadron' : [],
                      'BhadronCompleteEvent' : ['StrippingBetaSQ2B4piSelectionLine',
                                                'StrippingBetaSQ2B3piSelectionLine']
                    }
    }

##
## StrippingBu2Kpi0.py
##
Bu2Kpi0 = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'StrippingBu2Kpi0Conf',
    'CONFIG'      : {"KuMinPTasy"     : -0.6,
                     "RKuMIPCHI2DV"   : 25,
                     "KuMinPROBNNK"   : 0.8,
                     "KuMaxTRCHI2DOF" : 1.5,
                     "Rpi0MinP"       : 10000,
                     "Rpi0MinPT"      : 1600,
                     "Rpi0MinCL"      : 0.2,
                     "RBuMinM"        : 4800, # 4550 4800
                     "RBuMaxM"        : 5800, # 6050 5800
                     "RBuMinPT"       : 0,
                     "PrescaleBu2Kpi0Resolved" : 1.0,
                     "MKuMIPCHI2DV"   : 49,
                     "Mpi0MinP"       : 10000,
                     "Mpi0MinPT"      : 2600,
                     "Mpi0MinCL"      : 0.9,
                     "MBuMinM"        : 4000,
                     "MBuMaxM"        : 6050,
                     "MBuMinPT"       : 1500,
                     "PrescaleBu2Kpi0Merged"   : 1.0
                    },
    'STREAMS'     : ['Bhadron']
    #'STREAMS'     : { 'Bhadron' : [],
    #                  'BhadronCompleteEvent' : []
    #                }
    }


