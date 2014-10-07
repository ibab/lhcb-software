################################################################################
##                          S T R I P P I N G  2 1                            ##
##                                                                            ##
##  Configuration for B2noC WG                                                ##
##  Contact person:                                                           ##
################################################################################

from GaudiKernel.SystemOfUnits import *

## StrippingBu2KSh.py
Bu2KSh = {
    'WGs' : ['Charmless'],
    'BUILDERTYPE' : 'Bu2KShConf',
    'CONFIG'      : {'KSDaug_LTrk_GhostProb'   : 0.5,
                     'Trk_GhostProb'           : 0.5,
                     'Trk_Chi2'                : 3.0,
                     'KS_DD_MassWindow'        : 30.0,
                     'KS_LL_MassWindow'        : 15.0,
                     'KS_DD_VtxChi2'           : 10.0,
                     'KS_LL_VtxChi2'           : 10.0,
                     'KS_DD_FDChi2'            : 50.0,
                     'KS_LL_FDChi2'            : 100.0,
                     'KS_DD_Pmin'              : 8000.0,
                     'KS_LL_Pmin'              : 8000.0,
                     'KS_DD_Ptmin'             : 1000.0,
                     'KS_LL_Ptmin'             : 800.0,
                     'Bach_Ptmin'              : 1000.0,
                     'BDaug_MaxPT_IP'          : 0.05,
                     'BDaug_DD_PTsum'          : 4000.0,
                     'BDaug_LL_PTsum'          : 4000.0,
                     'B_Mlow'                  : 500.0,
                     'B_Mhigh'                 : 1500.0,
                     'B_Pmin'                  : 25000.0,
                     'B_LL_VtxChi2'            : 6.0,
                     'B_DD_VtxChi2'            : 6.0,
                     'B_Dira'                  : 0.9995,
                     'B_DD_IPCHI2wrtPV'        : 10.0,
                     'B_LL_IPCHI2wrtPV'        : 10.0,
                     'B_FDwrtPV'               : 1.0,
                     'B_DD_FDChi2'             : 50,
                     'B_LL_FDChi2'             : 50,
                     'GEC_MaxTracks'           : 250,
                     'Prescale'                : 1.0,
                     'Postscale'               : 1.0
                     },
    'STREAMS' : ['Bhadron']
    }

## StrippingB2HHPi0.py
B2HHPi0 = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'StrippingB2HHPi0Conf',
    'CONFIG'      : { 'PiMinPT'              : 500,
                      'PiMinP'               : 5000,
                      'PiMinTrackProb'       : 0.000001,
                      'PiMaxGhostProb'       : 0.5,
                      'PiMinIPChi2'          : 25,
                      'Pi0MinPT_M'           : 2500,
                      'Pi0MinPT_R'           : 1500,
                      'ResPi0MinGamCL'       : 0.2,
                      'BMinM'                : 4200,
                      'BMaxM'                : 6400,
                      'BMinPT_M'             : 3000,
                      'BMinPT_R'             : 2500,
                      'BMinVtxProb'          : 0.001,
                      'BMaxIPChi2'           : 9,
                      'BMinDIRA'             : 0.99995,
                      'BMinVVDChi2'          : 64,
                      'MergedLinePrescale'   : 1.,
                      'MergedLinePostscale'  : 1.,
                      'ResolvedLinePrescale' : 1.,
                      'ResolvedLinePostscale': 1.
                      },
    'STREAMS'     : ['Bhadron']
    }

## StrippingBs2PhiKst0.py
Bs2PhiKst = {
    "WGs"           : ['Charmless'],
    "BUILDERTYPE"   : 'StrippingBs2PhiKstConf',
    "CONFIG"        : { "KaonPT"                : 500.0,
                        "KaonIPCHI2"            : 9.,
                        "KaonPIDK"              : 0.,
                        "PionPT"                : 500.0,
                        "PionIPCHI2"            : 9.,
                        "PionPIDK"              : 10.,
                        "PhiVCHI2"              : 9.,
                        "PhiPT"                 : 900.0,
                        "PhiMassWin"            : 25.0,
                        "KstarVCHI2"            : 9.0,
                        "KstarPT"               : 900.0,
                        "KstarMassWin"          : 150.0,
                        "BMassWin"              : 500.0,
                        "BVCHI2"                : 15.0,
                        "BDOCA"                 : 0.3,
                        "BDIRA"                 : 0.99
                        },
    "STREAMS"       : ['Bhadron']
    }

## StrippingBs2KSKS.py
Bs2KSKS = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'Bs2KSKSConf',
    'CONFIG'      : {'MVA_LL_PROBABILITY_CUT_BCC'  : 0.5,
                     'MVA_LL_PROBABILITY_CUT_BCu'  : 0.5,
                     'MVA_LD_PROBABILITY_CUT_BCC'  : 0.5,
                     'MVA_LD_PROBABILITY_CUT_BCu'  : 0.5,
                     'MVA_DD_PROBABILITY_CUT_BCu'  : 0.5,
                     'KS_LL_MASS'                  : 50.0,
                     'KS_DD_MASS'                  : 100.0,
                     'KS_LL_DOCA'                  : 1.0,
                     'KS_DD_DOCA'                  : 4.0,
                     'KS_LL_TRGHOSTPROB'           : 1.0,
                     'KS_DD_TRGHOSTPROB'           : 1.0,
                     'B_MassWindow'                : 600.0,
                     'B_DOCA_LL'                   : 1.0,
                     'B_DOCA_DD'                   : 4.0,
                     'Prescale'                    : 1.0,
                     'Postscale'                   : 1.0
                     },
    'STREAMS'     : ['Bhadron']
    }

## StrippingB2HHBDT.py
B2HHBDT = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'B2HHBDTLines',
    'CONFIG'      : { 'PrescaleB2HHBDT' : 1.,
                      'MinPT'           : 1000,
                      'MinIP'           : 0.12,
                      'TrChi2'          : 3,
                      'TrGhostProb'     : 0.5,
                      'CombMassLow'     : 4600,
                      'CombMassHigh'    : 6400,
                      'DOCA'            : 0.1,
                      'BPT'             : 1200,
                      'BIP'             : 0.12,
                      'BTAU'            : 0.0006,
                      'MassLow'         : 4800,
                      'MassHigh'        : 6200,
                      'BDTCut'          : -0.3,
                      'BDTWeightsFile'  : "$TMVAWEIGHTSROOT/data/B2HH_BDT_v1r4.xml"
                      },
    'STREAMS'     : ['Bhadron']
    }

## StrippingD2HHBDT.py
D2HHBDT = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'D2HHBDTLines',
    'CONFIG'      : {'PrescaleD2HHBDT' : 1.,
                     'PrescaleDSt'     : 1.,
                     'PostscaleD02HH'  : 0.1,
                     'PostscaleDSt'    : 0.3,
                     'MinPT'           : 1000,
                     'MinIP'           : 0.12,
                     'TrChi2'          : 3,
                     'TrGhostProb'     : 0.5,
                     'CombMassLow'     : 1000,
                     'CombMassHigh'    : 2800,
                     'DOCA'            : 0.1,
                     'BPT'             : 1200,
                     'BIP'             : 0.12,
                     'BTAU'            : 0.,
                     'MassLow'         : 1800,
                     'MassHigh'        : 2600,
                     'BDTCut'          : -0.3,
                     'BDTWeightsFile'  : "$TMVAWEIGHTSROOT/data/B2HH_BDT_v1r4.xml",
                     'VertexChi2'      : 64
                    },
    'STREAMS'     : ['Charm']
    }

## StrippingB2KShh.py
B2KShh = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'B2KShhConf',
    'CONFIG'      : {'Trk_Chi2'                : 3.0,
                     'Trk_GhostProb'           : 0.3,
                     'KS_DD_MassWindow'        : 30.0,
                     'KS_DD_VtxChi2'           : 12.0,
                     'KS_DD_FDChi2'            : 50.0,
                     'KS_DD_Pmin'              : 6000.0,
                     'KS_LL_MassWindow'        : 20.0,
                     'KS_LL_VtxChi2'           : 12.0,
                     'KS_LL_FDChi2'            : 80.0,
                     'KS_LL_Pmin'              : 0.0,
                     'KS_LD_MassWindow'        : 25.0,
                     'KS_LD_VtxChi2'           : 12.0,
                     'KS_LD_FDChi2'            : 50.0,
                     'KS_LD_Pmin'              : 6000.0,
                     'B_Mlow'                  : 1279.0,
                     'B_Mhigh'                 : 921.0,
                     'B_APTmin'                : 1000.0,
                     'BDaug_MedPT_PT'          : 800.0,
                     'BDaug_MaxPT_IP'          : 0.05,
                     'BDaug_DD_maxDocaChi2'    : 5.0,
                     'BDaug_LL_maxDocaChi2'    : 5.0,
                     'BDaug_LD_maxDocaChi2'    : 5.0,
                     'BDaug_DD_PTsum'          : 4200.0,
                     'BDaug_LL_PTsum'          : 3000.0,
                     'BDaug_LD_PTsum'          : 4200.0,
                     'B_DD_Dira'               : 0.999,
                     'B_LL_Dira'               : 0.999,
                     'B_LD_Dira'               : 0.999,
                     'KS_FD_Z'                 : 15.,
                     'B_DD_MVA_2011'           : -0.20,
                     'B_LL_MVA_2011'           : -0.20,
                     'B_LD_MVA_2011'           : -0.20,
                     'B_DD_MVA_2012a'          : -0.20,
                     'B_LL_MVA_2012a'          : -0.20,
                     'B_LD_MVA_2012a'          : -0.20,
                     'B_DD_MVA_2012b'          : -0.20,
                     'B_LL_MVA_2012b'          : -0.20,
                     'B_LD_MVA_2012b'          : -0.20,
                     'GEC_MaxTracks'           : 250,
                     'ConeAngle'               : 1.5,
                     'Prescale'                : 1.0,
                     'Prescale_SameSign'       : 1.0,
                     'Postscale'               : 1.0,
                     'TCK_2011'                : ('0x00470032','0x00790038','0x40470032','0x40790038'),
                     'TCK_2012a'               : ('0x007E003A','0x0097003D','0x407E003A','0x4097003D'),
                     'TCK_2012b'               : ('0x00990042','0x00AC0046','0x40990042','0x40AC0046')
                     },
    'STREAMS'     : { 'Bhadron' : ['StrippingB2KShh_DD_2011_OS_Line',
                                   'StrippingB2KShh_LL_2011_OS_Line',
                                   'StrippingB2KShh_LD_2011_OS_Line',
                                   'StrippingB2KShh_DD_2011_SS_Line',
                                   'StrippingB2KShh_LL_2011_SS_Line',
                                   'StrippingB2KShh_LD_2011_SS_Line'
                                   ]
                      }
    }

## StrippingBs2Q2B.py
BetaSBs2Q2Body4pi = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'Bs2Q2Body4piConf',
    'CONFIG'      : {'Q2BPrescale'     : 1.,
                     'Q2BResMinPT'     : 1200.,
                     'Q2BResMinP'      : 3.,
                     'Q2BResMinMass'   : 0.,
                     'Q2BResMaxMass'   : 1100.,
                     'Q2BResVtxChiDOF' : 9.,
                     'Q2BBMinM'        : 4500.,
                     'Q2BBMaxM'        : 5700.,
                     'Q2BBVtxChi2DOF'  : 9.,
                     'Q2BIPCHI2' : 20
                     },
    'STREAMS'     : ['BhadronCompleteEvent']
    }

## StrippingBs2PhiPhi.py
BetaSBs2PhiPhi = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'StrippingBs2PhiPhiConf',
    'CONFIG'      : {'KaonPT'              : 400      # MeV/c
                     , 'KaonIPCHI2'          : 0.0      # adimensional
                     , 'PhiPT'               : 0        # MeV/c
                     , 'PhiPTsq'             : 1.2      # GeV*GeV/cc
                     , 'PhiVertexCHI2pDOF'   : 15       # adimensional
                     , 'PhiMassWindow'       : 25       # MeV/cc
                     , 'PhiMassMax'          : 1090     # MeV/cc
                     , 'BsVertexCHI2pDOF'    : 15       # adimensional
                     , 'BsMassWindow'        : 300      # MeV/cc
                     , 'WidePrescale'        : 0.15     # adimensional
                     },
    'STREAMS'     : { 'BhadronCompleteEvent' : ['StrippingBetaSBs2PhiPhiLine',
                                                'StrippingBetaSBs2PhiPhiUpLine'],
                      'Bhadron'              : ['StrippingBetaSBs2PhiPhiWideLine']
                    }
    }

## StrippingBc2XK.py
Bc2XK = {
  'WGs'  : ['Charmless'],
  'BUILDERTYPE' : 'Bc2XKConf',
  'CONFIG' : { 'CheckPV'   : True ,
               'FILTER'    : None ,
               'ODIN'      : None ,
               'L0DU'      : None ,
               'HLT'       : None ,
               'CTAU'      : 140 * micrometer ,
               'CTAU_BC'   :  80 * micrometer ,
               'KstarCut'   : """
               ( PT          > 1000 * MeV               ) &
               in_range ( 2          , ETA , 4.9       ) &
               in_range ( 5 * GeV  , P   , 500 * GeV ) &
               ( MIPCHI2DV()  > 9.    ) &
               ( CHILDCUT ( ( PT> 300*MeV) , 1 ) ) &
               ( CHILDCUT ( ( PT> 300*MeV) , 2 ) )
               """ ,
               'PhiCut'   : """
               ( PT          > 750 * MeV               ) &
               in_range ( 2          , ETA , 4.9       ) &
               in_range ( 1 * GeV  , P   , 500 * GeV ) &
               ( MIPCHI2DV()  > 9.    ) &
               ( CHILDCUT ( ( PT> 300*MeV) , 1 ) ) &
               ( CHILDCUT ( ( PT> 300*MeV) , 2 ) )
               """ ,
               'KaonCut'   : """
               ( CLONEDIST   > 5000   ) &
               ( TRCHI2DOF   < 4      ) &
               ( TRGHOSTPROB < 0.4    ) &
               ( PT          > 750 * MeV               ) &
               in_range ( 2          , ETA , 4.9       ) &
               in_range ( 3.2 * GeV  , P   , 150 * GeV ) &
               ( MIPCHI2DV()  > 9.    )
               """ ,
               'Preambulo' : ['chi2vx = VFASPF(VCHI2)   '                                ,
                              "from GaudiKernel.PhysicalConstants import c_light"        ,
                              "ctau      = BPVLTIME ( 25 ) * c_light "                   ,
                              "light_9    = BPVLTIME (  9 ) * c_light "                   ,
                              "ctau_16   = BPVLTIME ( 16 ) * c_light "                   ,
                              "mbp_acut  = in_range ( 5.050 * GeV , AM , 5.550 * GeV ) " ,
                              "mbc_acut  = in_range ( 6.000 * GeV , AM , 6.600 * GeV ) " ,
                              "mbp_cut   = in_range ( 5.100 * GeV ,  M , 5.500 * GeV ) " ,
                              "mbc_cut   = in_range ( 6.050 * GeV ,  M , 6.550 * GeV ) " ,
                              ] ,
               'Bc2KstKPrescale'  : 1.0 ,
               'Bc2phiKPrescale'  : 1.0 ,
               },
  'STREAMS' : ['Bhadron']
  }

## StrippingHb2Charged2Body.py
Hb2Charged2Body = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'Hb2Charged2BodyLines',
    'CONFIG'      : {'PrescaleB2Charged2Body'  : 1,
                     'MinPTB2Charged2Body'     : 1000,
                     'MinIPChi2B2Charged2Body' : 12,
                     'TrChi2'                  : 3,
                     'TrGhostProb'             : 0.5,
                     'MaxPTB2Charged2Body'     : 1400,
                     'MaxIPChi2B2Charged2Body' : 40,
                     'CombMassLow'             : 4600,
                     'CombMassHigh'            : 6000,
                     'DOCA'                    : 0.08,
                     'BPT'                     : 1200,
                     'BIPChi2B2Charged2Body'   : 12,
                     'BTAU'                    : 0.0006,
                     'MassLow'                 : 4800,
                     'MassHigh'                : 5800
                    },
    'STREAMS'     : ['BhadronCompleteEvent']
    }

## StrippingXb2p2hOrXb2p3h.py
Xb2p2hOrXb2p3h = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'Xb2p2hOrXb2p3hConf',
    'CONFIG'      : {'Trk_MaxChi2Ndof'   : 3.0,
                     'Trk_MaxGhostProb'  : 0.4,
                     'Trk_MinIPChi2'     : 16.0,
                     'Trk_MinP'          : 1500.0,
                     'Trk_MinProbNNp'    : 0.05,
                     'Xb_MinSumPTppi'    : 1500.0,
                     'Xb_MinSumPTppipi'  : 2500.0,
                     'Xb_MinM_3body'     : 5195.0,
                     'Xb_MaxM_3body'     : 6405.0,
                     'Xb_MinSumPT_3body' : 3500.0,
                     'Xb_MinPT_3body'    : 1500.0,
                     'Xb_MinM_4body'     : 5195.0,
                     'Xb_MaxM_4body'     : 6405.0,
                     'Xb_MinSumPT_4body' : 3500.0,
                     'Xb_MinPT_4body'    : 1500.0,
                     'Xb_MaxDOCAChi2'    : 20.0,
                     'Xb_MaxVtxChi2'     : 20.0,
                     'Xb_MinFDChi2'      : 50.0,
                     'Xb_MaxIPChi2'      : 16.0,
                     'Xb_MinDira'        : 0.9999,
                     'Prescale'          : 1.0,
                     'Postscale'         : 1.0
                     },
    'STREAMS'     : ['Bhadron']
    }

## StrippingB2XEta.py
B2XEta = {
    'WGs'          : ['Charmless'],
    'BUILDERTYPE'  : 'B2XEtaConf',
    'CONFIG'       : {'Trk_Chi2'                : 3.0,
                      'Trk_PT'                  : 300.0,
                      'Trk_GP'                  : 0.5,
                      'pK_PT'                   : 500., #1000.
                      'pK_IPCHI2'               : 20.,
                      'ProbNNCut'               : 0.1,
                      'kstar_daug_PT'           : 500.,
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
                      'Kstar_PTmin'             : 1200.0,
                      'Kstar_massWdw'           : 100.0,
                      'Kstar_vtxChi2'           : 9.0,
                      'Kstar_ipChi2'            : 5.0,
                      'eta_PT'                  : 2000,
                      'eta_MassWindow'          : 150,
                      'eta_vtxChi2'             : 10.,
                      'eta_DOCA'                : 10.0, #20
                      'gamma_PT'                : 500, #photons from eta
                      'eta_prime_MassWindow'    : 100.0,
                      'eta_prime_PT'            : 2000.0,
                      'eta_prime_vtxChi2'       : 10.0,
                      'eta_prime_DOCA'          : 10.0, #15
                      'B_MassWindow'            : 750.0,
                      'B_PTmin'                 : 1500.0,
                      'BDaug_DD_maxDocaChi2'    : 15.0, #20
                      'BDaug_LL_maxDocaChi2'    : 15.0,  #20
                      'B_VtxChi2'               : 15.0,
                      'B_Dira'                  : 0.9995,
                      'B_IPCHI2'                : 20.0,
                      'B_eta_IPCHI2'            : 6.0,
                      'Lb_MassWindow'           : 750.0,
                      'Lb_PTmin'                : 1000.0,
                      'LbDaug_DD_maxDocaChi2'   : 15.0, #20
                      'LbDaug_LL_maxDocaChi2'   : 15.0, #20
                      'Lb_VtxChi2'              : 15.0,
                      'Lb_Dira'                 : 0.9995,
                      'Lb_IPCHI2'               : 20.0,
                      'Lb_eta_IPCHI2'           : 6.0,
                      'GEC_MaxTracks'           : 250,
                      'Prescale'                : 1.0,
                      'Postscale'               : 1.0,
                      'etaGG_Prescale'          : 0.0
                      #'TCKs'                : ('0x00470032','0x00790038')
                      #'TCKs'                : ('0x00470032','0x00790038','0x007E003A','0x0097003D','0x407E003A','0x4097003D','0x00990042','0x00AC0046','0x40990042','0x40AC0046')
                      },
    'STREAMS'     : { 'Bhadron' : ['StrippingB2XEtaB2etapKSLLLine',
                                   'StrippingB2XEtaB2etapKSDDLine',
                                   'StrippingB2XEtaB2eta3piKSLLLine',
                                   'StrippingB2XEtaB2eta3piKSDDLine',
                                   'StrippingB2XEtaLb2etapLLLLine',
                                   'StrippingB2XEtaLb2etapLDDLine',
                                   'StrippingB2XEtaLb2eta3piLLLLine',
                                   'StrippingB2XEtaLb2eta3piLDDLine',
                                   'StrippingB2XEtaB2etapKstarLine',
                                   'StrippingB2XEtaB2eta3piKstarLine',
                                   'StrippingB2XEtaLb2pKetapLine',
                                   'StrippingB2XEtaLb2pKeta3piLine']
                      }
    }


## StrippingHb2V0V0h.py
Hb2V0V0h = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'Hb2V0V0hConf',
    'CONFIG'      : {
                  'Trk_Chi2'                : 3.0,
                  'Trk_GhostProb'           : 0.3,
                  'V0_DD_MassWindow'        : 30.0,
                  'V0_DD_VtxChi2'           : 12.0,
                  'V0_DD_FDChi2'            : 50.0,
                  'V0_LL_MassWindow'        : 20.0,
                  'V0_LL_VtxChi2'           : 12.0,
                  'V0_LL_FDChi2'            : 80.0,
                  'B_Mlow'                  : 1279.0,
                  'B_Mhigh'                 : 921.0,
                  'Hb_Mlow'                 : 400.0,
                  'Hb_Mhigh'                : 400.0,
                  'B_APTmin'                : 1000.0,
                  'B_Dira'                  : 0.999,
                  'B_VtxChi2'               : 12.0,
                  'B_FDChi2'                : 50.0,
                  'B_IPCHI2wrtPV'           : 12.0,
                  'GEC_MaxTracks'           : 250,
                  'Prescale'                : 1.0,
                  'Prescale_SameSign'       : 1.0,
                  'Postscale'               : 1.0,
                  'RelatedInfoTools'        : [ { "Type" : "RelInfoConeVariables"
                                                , "ConeAngle" : 1.0
                                                , "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM']
                                                , "Location"  : 'P2ConeVar1'},
                                                { "Type" : "RelInfoConeVariables"
                                                , "ConeAngle" : 1.5
                                                , "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM']
                                                , "Location"  : 'P2ConeVar2'},
                                                { "Type" : "RelInfoConeVariables"
                                                , "ConeAngle" : 1.7
                                                , "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM']
                                                , "Location"  : 'P2ConeVar3'},
                                                {'Type' : 'RelInfoVertexIsolation'
                                                , 'Location': "VtxIsolationInfo"  }
                                                ]
                  },
    'STREAMS'     : [ 'Bhadron' ]
    }

## StrippingB2pphh.py
B2pphh = {
    'WGs'  : ['Charmless'],
    'BUILDERTYPE' : 'B2pphhConf',
    'CONFIG' : { 'MaxTrSIZE'             : 200 ,      ## GEC maximim recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG)
                 'CombMass12Max_kk'         : 4700.0,
                 'CombMass12Max_kpi'         : 5000.0,
                 'CombMass12Max_pipi'         : 5350.0,
                 'd_achi2doca12'             : 20.0,
                 'd_achi2doca13'             : 20.0,
                 'd_achi2doca14'             : 20.0,
                 'd_achi2doca23'             : 20.0,
                 'd_achi2doca24'             : 20.0,
                 'd_achi2doca34'             : 20.0,
                 'p_PTSUM'               : 750.0,
                 'p_PSUM'               : 7000.0,
                 'p_PTMIN'              : 400.0,
                 'p_PMIN'              : 4.0,
                 'p_PROBNNpPROD'        : 0.05,
                 'CombMass123Max'         : 5600.0,
                 'CombMassMax'         : 5.60,
                 'CombMassMin'         : 5.00,
                 'MassMax'             : 5.55,
                 'MassMin'             : 5.05,
                 '4h_PTSUM'            : 3000.0,
                 'B_DIRA'              : .9999 ,
                 '4h_AMAXDOCA'                : 0.3 ,
                 'B_CHI2'              : 30.0 ,
                 'B_PT'                 : 1000.,
                 'B_MIPDV'              : 0.2,
                 'PionCuts' : "(TRCHI2DOF < 3.0) & HASRICH & (P > 1500*MeV) & (PT > 300*MeV) & (MIPCHI2DV(PRIMARY) > 6.0) & (TRGHOSTPROB < 0.35) & (PROBNNpi > 0.05)",
                 'KaonCuts' : "(TRCHI2DOF < 3.0) & HASRICH & (P > 1500*MeV) & (PT > 300*MeV) & (MIPCHI2DV(PRIMARY) > 4.0) & (TRGHOSTPROB < 0.35) & (PROBNNk > 0.05)",
                 'ProtonCuts' : "(TRCHI2DOF < 3.0) & HASRICH & (P > 1500*MeV) & (PT > 300*MeV) & (MIPCHI2DV(PRIMARY) > 2.0) & (TRGHOSTPROB < 0.35) & (PROBNNp > 0.05)",
                 'Prescaleppkk'  : 1.0,
                 'Prescalepppipi'  : 1.0,
                 'Prescaleppkpi'  : 1.0
                 },
    'STREAMS' : ['Bhadron']
    }

## StrippingBs2Phif0.py
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
    'STREAMS'     : ['Bhadron'],
    }

## StrippingLb2PhipK.py
Lb2PhipK = {
    'WGs' : ['Charmless'],
    'BUILDERTYPE' : 'StrippingLb2PhipKConf',
    'CONFIG'      : {  'KaonPT'              : 400      # MeV/c
                       , 'KaonIPCHI2'          : 4.0      # adimensional
                       , 'PhiPT'               : 0        # MeV/c
                       , 'PhiPTsq'             : 2.0      # GeV*GeV/cc
                       , 'PhiVertexCHI2pDOF'   : 15       # adimensional
                       , 'PhiMassWindow'       : 25       # MeV/cc
                       , 'PhiMassMax'          : 1090     # MeV/cc
                       , 'BsVertexCHI2pDOF'    : 15       # adimensional
                       , 'BsMassWindow'        : 300      # MeV/cc
                       },
    'STREAMS'     : ['Bhadron']
    }

## StrippingButo5h.py
Buto5h = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'Buto5hBuilder',
    'CONFIG'      : {'MaxTrSIZE'             : 200 ,
                     '_h_PT'                 : 250. ,
                     '_h_IPCHI2'             : 6. ,
                     '_h_TRCHI2DOF'          : 1.7 ,
                     '_h_TRGHP'              : 0.2 ,
                     '_5h_DOCA'              : .14 ,
                     '_5h_DIRA'              : .99999 ,
                     '_5h_FDCHI2'            : 500. ,
                     '_5h_CHI2'              : 12. ,
                     '_5h_PT'                : 1000. ,
                     '_5h_PVIPCHI2sum'       : 400. ,
                     '_5h_Mmax'              : 5679. ,
                     '_5h_Mmin'              : 5079. ,
                     '_probnnpi'             : .15,
                     '_probnnk'              : .20,
                     '_probnnp'              : .05,
                     '5pi_exclLinePrescale'  : 1.0,
                     '5pi_exclLinePostscale' : 1.0,
                     'K4pi_exclLinePrescale' : 1.0,
                     'K4pi_exclLinePostscale': 1.0,
                     'pp3pi_exclLinePrescale' : 1.0,
                     'pp3pi_exclLinePostscale': 1.0,
                     'ppKpipi_exclLinePrescale' : 1.0,
                     'ppKpipi_exclLinePostscale': 1.0
                     },
    'STREAMS'     : ['Bhadron']
    }

## StrippingBs2Kst0Kst0.py
Bs2KstKst = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'StrippingBs2KstKstConf',
    'CONFIG'      : {"KaonPT"       : 500.0,
                     "KaonIPCHI2"   : 9.,
                     "PionPT"       : 500.0,
                     "PionIPCHI2"   : 9.,
                     "PionPIDK"     : 10.,
                     "KstarVCHI2"   : 9.0,
                     "KstarPT"      : 900.0,
                     "KaonPIDK"     : -5.0,
                     "KstarMassWin" : 150.0,
                     "BMassWin"     : 500.0,
                     "BVCHI2"       : 15.0,
                     "BDOCA"        : 0.3,
                     "BIPCHI2"      : 25,
                     "MaxGHOSTPROB" : 0.8,
                     "BDIRA"        : 0.99
                     },
    'STREAMS'     : { 'Bhadron' : ['StrippingBs2KstKstSameChargeLine'],
                      'BhadronCompleteEvent' :  ['StrippingBs2KstKstNominalLine']
                      }
    }

## StrippingBs2Kst_0Kst_0.py
Bs2K0stK0st = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'StrippingBs2Kst_0Kst_0Conf',
    'CONFIG'      : {"KaonPT"         : 500.0,
                     "KaonIPCHI2"     : 9.,
                     "PionPT"         : 500.0,
                     "PionIPCHI2"     : 9.,
                     "PionPIDK"       : 0.,
                     "KstarVCHI2"     : 9.0,
                     "KstarPT"        : 900.0,
                     "KaonPIDK"       : 2.0,
                     "KstarMassWin"   : 680.0,
                     "KstarAPT"       : 800.0,
                     "BMassWin"       : 500.0,
                     "BVCHI2"         : 15.0,
                     "BDOCA"          : 0.3,
                     "BIPCHI2"        : 25.,
                     "BFDistanceCHI2" : 81.,
                     "SumPT"          : 5000,
                     "MaxGHOSTPROB"   : 0.8,
                     "BDIRA"          : 0.99
                     },
    'STREAMS'     : ['BhadronCompleteEvent']
    }

## StrippingLb2V0hh.py
Lb2V0h = {
    'WGs'  : ['Charmless'],
    'BUILDERTYPE' : 'Lb2V0hhConf',
    'CONFIG' : {'Trk_Chi2'                 : 3.0,
                'Trk_GhostProb'            : 0.5,
                'Lambda_DD_MassWindow'     : 25.0,
                'Lambda_DD_VtxChi2'        : 15.0,
                'Lambda_DD_FDChi2'         : 50.0,
                'Lambda_DD_FD'             : 300.0,
                'Lambda_DD_Pmin'           : 5000.0,
                'Lambda_LL_MassWindow'     : 20.0,
                'Lambda_LL_VtxChi2'        : 15.0,
                'Lambda_LL_FDChi2'         : 80.0,
                'Lambda_LD_MassWindow'     : 25.0,
                'Lambda_LD_VtxChi2'        : 15.0,
                'Lambda_LD_FDChi2'         : 50.0,
                'Lambda_LD_FD'             : 300.0,
                'Lambda_LD_Pmin'           : 5000.0,
                'Lb_Mlow'                  : 1319.0,
                'Lb_Mhigh'                 : 600.0,
                'Lb_2bodyMlow'             : 800.0,
                'Lb_2bodyMhigh'            : 800.0,
                'Lb_APTmin'                : 1000.0,
                'Lb_PTmin'                 : 800.0,
                'LbDaug_MedPT_PT'          : 800.0,
                'LbDaug_MaxPT_IP'          : 0.05,
                'LbDaug_DD_maxDocaChi2'    : 5.0,
                'LbDaug_LL_maxDocaChi2'    : 5.0,
                'LbDaug_LD_maxDocaChi2'    : 5.0,
                'LbDaug_DD_PTsum'          : 4200.0,
                'LbDaug_LL_PTsum'          : 3000.0,
                'LbDaug_LD_PTsum'          : 4200.0,
                'Lbh_DD_PTMin'             : 500.0,
                'Lbh_LL_PTMin'             : 500.0,
                'Lbh_LD_PTMin'             : 500.0,
                'Lb_VtxChi2'               : 12.0,
                'Lb_DD_Dira'               : 0.995,
                'Lb_LL_Dira'               : 0.995,
                'Lb_LD_Dira'               : 0.995,
                'Lb_DD_IPCHI2wrtPV'        : 15.0,
                'Lb_LL_IPCHI2wrtPV'        : 15.0,
                'Lb_LD_IPCHI2wrtPV'        : 15.0,
                'Lb_FDwrtPV'               : 1.0,
                'Lb_DD_FDChi2'             : 30.0,
                'Lb_LL_FDChi2'             : 30.0,
                'Lb_LD_FDChi2'             : 30.0,
                'GEC_MaxTracks'            : 250,
                'Prescale'                 : 1.0,
                'Postscale'                : 1.0,
                'RelatedInfoTools' : [    { "Type" : "RelInfoConeVariables",
                                            "ConeAngle" : 1.7,
                                            "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            "Location"  : 'ConeVar17'
                                            },
                                          { "Type" : "RelInfoConeVariables",
                                            "ConeAngle" : 1.5,
                                            "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            "Location"  : 'ConeVar15'
                                            },
                                          { "Type" : "RelInfoConeVariables",
                                            "ConeAngle" : 1.0,
                                            "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            "Location"  : 'ConeVar10'
                                            },
                                          { "Type" : "RelInfoConeVariables",
                                            "ConeAngle" : 0.8,
                                            "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            "Location"  : 'ConeVar08'
                                            },
                                          { "Type" : "RelInfoVertexIsolation",
                                            "Location" : "VtxIsolationVar"
                                            }
                                          ]
                },
    'STREAMS' : ['Bhadron']
    }

## StrippingB2CharmlessQuasi2Body.py
B2CharmlessQ2B = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'B2Quasi2Body',
    'CONFIG'      : { 'Q2BPrescale'     : 1.,
                      'Q2BTrkGhostProb' : 0.5,
                      'Q2BTrkMinIPChi2' : 16.,
                      'Q2BTrkMinPT'     : 400.,
                      'Q2BTrkMinHiPT'   : 1000.,
                      'Q2BResMinPT'     : 600.,
                      'Q2BResMinHiPT'   : 1000.,
                      'Q2BResMaxMass'   : 1100.,
                      'Q2BResVtxChi2DOF': 6.,
                      'Q2BBMinPT'       : 2500.,
                      'Q2BBMinM3pi'     : 4200.,
                      'Q2BBMinM4pi'     : 3500.,
                      'Q2BBMaxM3pi'     : 6700.,
                      'Q2BBMaxM4pi'     : 5700.,
                      'Q2BBMaxCorrM3pi' : 7000.,
                      'Q2BBMaxCorrM4pi' : 6000.,
                      'Q2BBVtxChi2DOF'  : 6.
                    },
    'STREAMS'     : ['BhadronCompleteEvent']
    }

## StrippingBu2rho0rhoPlus.py
Bu2rho0rhoPlus = {
    "WGs"         : [ "Charmless" ],
    "BUILDERTYPE" : "StrippingBu2rho0rhoPlusConf",
    "CONFIG"      : { "isMC"           : False,  # True = no Hlt filter
                      "refitPVs"       : True,

                      "vetoTrISMUON"   : True,   # ~ISMUON causes errors?
                      "trMinIPChi2"    : 4,
                      "trUpMinIPChi2"  : 8,
                      "trMinProbNNpi"  : 0.0,
                      "trMaxChi2Dof"   : 3.0,
                      "trMaxGhostProb" : 0.5,

                      "longLines"     : { "rhoCombMassMax"        : 1300,   # MeV
                                          "rhoCombMassMin"        : 100,    # MeV
                                          "rhoMothMassMax"        : 1200,   # MeV
                                          "rhoMothMassMin"        : 200,    # MeV
                                          "rho0MinVChi2Dof"       : 14,

                                          "pi0ResMinCL"           : 0.0009,
                                          "pi0ResMinP"            : 3500,   # MeV
                                          "pi0ResMinPT"           : 400,    # MeV
                                          "piPRhoPResMinIPChi2"   : 20,
                                          "rho0ResMinIPChi2"      : 20,
                                          "rho0ResMinFDChi2"      : 27,
                                          "rhoPResMinP"           : 7000,   # MeV
                                          "rhoPResMinPT"          : 1000,   # MeV

                                          "BuResCombMassWindow"   : 650,    # MeV
                                          "BuResMaxTrIPChi2Min"   : 40,
                                          "BuResMaxTrPTMin"       : 1900,   # MeV
                                          "BuResSumTrPTMin"       : 3800,   # MeV
                                          "BuResMinVChi2Dof"      : 8,
                                          "BuResMothMassWindow"   : 600,    # MeV
                                          "BuResMinFDChi2"        : 120,
                                          "BuResMaxIPChi2"        : 30,
                                          "BuResMinDira"          : 0.9998,
                                          "BuResMinPT"            : 1000,   # MeV

                                          "pi0MgdMinPT"           : 1900,   # MeV
                                          "piPRhoPMgdMinPT"       : 960,    # MeV
                                          "rho0MgdDauMinPT"       : 100,    # MeV
                                          "rho0MgdMinIPChi2"      : 33,
                                          "rho0MgdMinFDChi2"      : 25,
                                          "rho0MgdMaxTrIPChi2Min" : 20,

                                          "BuMgdCombMassMin"      : 3900,   # MeV
                                          "BuMgdCombMassMax"      : 7150,   # MeV
                                          "BuMgdMaxTrIPChi2Min"   : 20,
                                          "BuMgdMinFDChi2"        : 120,
                                          "BuMgdMaxIPChi2"        : 450,
                                          "BuMgdMinVChi2Dof"      : 8,
                                          "BuMgdMothMassMin"      : 4000,   # MeV
                                          "BuMgdMothMassMax"      : 7000,   # MeV
                                          "BuMgdMinDira"          : 0.9997,
                                          "BuMgdMinPT"            : 4000,   # MeV
                                          },
                      "upstreamLines" : { "rhoCombMassMax"        : 1300,   # MeV
                                          "rhoCombMassMin"        : 100,    # MeV
                                          "rhoMothMassMax"        : 1200,   # MeV
                                          "rhoMothMassMin"        : 200,    # MeV
                                          "rho0MinVChi2Dof"       : 14,

                                          "pi0ResMinCL"           : 0.0009,
                                          "pi0ResMinP"            : 1000,   # MeV
                                          "pi0ResMinPT"           : 400,    # MeV
                                          "piPRhoPResMinIPChi2"   : 10,
                                          "rho0ResMinIPChi2"      : 55,
                                          "rho0ResMinFDChi2"      : 55,
                                          "rhoPResMinP"           : 9000,   # MeV
                                          "rhoPResMinPT"          : 1400,   # MeV

                                          "BuResCombMassWindow"   : 650,    # MeV
                                          "BuResMaxTrIPChi2Min"   : 0,
                                          "BuResMaxTrPTMin"       : 1600,   # MeV
                                          "BuResSumTrPTMin"       : 3500,   # MeV
                                          "BuResMinVChi2Dof"      : 8,
                                          "BuResMothMassWindow"   : 600,    # MeV
                                          "BuResMinFDChi2"        : 55,
                                          "BuResMaxIPChi2"        : 20,
                                          "BuResMinDira"          : 0.9998,
                                          "BuResMinPT"            : 1500,   # MeV

                                          "pi0MgdMinPT"           : 0,      # MeV
                                          "piPRhoPMgdMinPT"       : 0,      # MeV
                                          "rho0MgdDauMinPT"       : 0,      # MeV
                                          "rho0MgdMinIPChi2"      : 55,
                                          "rho0MgdMinFDChi2"      : 55,
                                          "rho0MgdMaxTrIPChi2Min" : 55,

                                          "BuMgdCombMassMin"      : 3900,   # MeV
                                          "BuMgdCombMassMax"      : 7150,   # MeV
                                          "BuMgdMaxTrIPChi2Min"   : 55,
                                          "BuMgdMinFDChi2"        : 55,
                                          "BuMgdMaxIPChi2"        : 55,
                                          "BuMgdMinVChi2Dof"      : 8,
                                          "BuMgdMothMassMin"      : 4000,   # MeV
                                          "BuMgdMothMassMax"      : 7000,   # MeV
                                          "BuMgdMinDira"          : 0.999,
                                          "BuMgdMinPT"            : 5000,   # MeV
                                          },

                      "PrescaleBu2rho0rhoPlusResolved"   : 1.0,
                      "PrescaleBu2rho0rhoPlusMerged"     : 1.0,
                      "PrescaleBu2rho0rhoPlusUpResolved" : 1.0,
                      "PrescaleBu2rho0rhoPlusUpMerged"   : 1.0
                      },
    "STREAMS"     : ["Bhadron"]
    }

## StrippingBu2Kpi0.py
Bu2Kpi0 = {
    "WGs"         : [ "Charmless" ],
    "BUILDERTYPE" : "StrippingBu2Kpi0Conf",
    "CONFIG"      : { "TISTOSSpecs" : { 'Hlt1Track(AllL0|Photon)Decision%TOS' : 0 },
                      "isMC"             : False, ### True = no Hlt filter
                      "refitPVs"         : True,

                      "vetoTrISMUON"     : True,
                      "trMaxChi2Dof"     : 3.0,
                      "trMaxGhostProb"   : 0.5,
                      "trMinProbNNk"     : 0.08,
                      "trMinPTasy"       : -0.7,  # unitless

                      "MKuMIPCHI2DV"     : 30,
                      "MKuMinP"          : 8000,  # MeV
                      "MKuMinPT"         : 1100,  # MeV
                      "Mpi0MinP"         : 0,     # MeV
                      "Mpi0MinPT"        : 2400,  # MeV
                      "Mpi0MinCL"        :-1000,
                      "MBuMinM"          : 4200,  # MeV
                      "MBuMaxM"          : 6300,  # MeV
                      "MBuMinPT"         : 1800,  # MeV
                      "MBuMaxMTDOCAChi2" : 10,
                      "PrescaleBu2Kpi0Merged"   : 1.0,
    
                      "RKuMIPCHI2DV"     : 25,
                      "RKuMinPT"         : 1300,  # MeV
                      "Rpi0MinP"         : 8000,  # MeV
                      "Rpi0MinPT"        : 1400,  # MeV
                      "RgammaMinCL"      :-1000,
                      "RBuMinM"          : 4800,  # MeV
                      "RBuMaxM"          : 5800,  # MeV
                      "RBuMinPT"         : 0,     # MeV
                      "RBuMaxMTDOCAChi2" : 7,
                      "PrescaleBu2Kpi0Resolved" : 1.0
                      },
    "STREAMS"     : ["Bhadron"]
    }

## StrippingBc2hhh_BnoC.py
Bc2hhh = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'Bc2hhhBuilder',
    'CONFIG' : {'MaxTrSIZE'             : 200,      ## GEC maximim recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG)
                '_h_PT'                 : 250.,     ## tracks min PT
                '_h_P'                  : 2500.,    ## tracks min P
                '_h_IPCHI2'             : 1.,       ## min tracks IP wrt OWNPV
                '_h_TRCHI2DOF'          : 3.0,      ## max tracks CHI2DOF
                '_h_TRGHP'              : .5,        ## Track PROBNNghost
                '_3h_DOCA'              : .2,       ## max DOCA between h and 2h
                '_3h_PTmax'             : 1500,     ## min PT of the 3h highest PT track
                '_3h_DIRA'              : .9999,   ## min cos angle between 3h momentum and PV decay direction
                '_3h_FDCHI2'            : 150.,     ## min 3h FDCHI2 wrt best 3h PV
                '_3h_SVPV'              : 1.5,      ## Distance between sv and pv
                '_3h_CHI2'              : 20.0,     ## max 3h vertex CHI2
                '_3h_IPCHI2'            : 10.,      ## max 3h IP CHI2 wrt best 3h PV
                '_3h_PT'                : 1000.,    ## min 3h PT
                '_3h_PTsum'             : 4500.,    ## min of 3h tracks PT sum
                '_3h_Psum'              : 22000.,   ## min of 3h tracks P sum
                '_3h_PVIPCHI2sum'       : 200.,     ## min of the 3h tracks IP wrt best 3h PV
                '_3h_TRKCHIDOFmin'      : 3.0,       ## max track CHI2DOF for the track with smalest CHI2DOF
                '_3h_Mmax'              : 6500.,     ## max 3h mass for exclusive lines
                '_3h_Mmin'              : 6000.,     ## min 3h mass for exclusive KKK lines
                '_bu3h_Mmax'            : 5500.,     ## min buto3h mass for exclusive lines
                '_bu3h_Mmin'            : 5100.,     ## min buto3h mass for exclusive lines
                '_probnnpi'             : .15,       ## PROBNNpi for pion tracks. WARNING: DO NOT APPLY THIS CUT FOR PIPIPI OR KPIPI LINES
                '_probnnk'              : .20,       ## PROBNNk for kaon tracks.
                '_probnnp'              : .05,       ## PROBNNp for proton tracks.
                'pipipi_exclLinePrescale'  : 1.0,
                'pipipi_exclLinePostscale'  : 1.0,
                'Kpipi_exclLinePrescale'  : 1.0,
                'Kpipi_exclLinePostscale'  : 1.0,
                'KKpi_exclLinePrescale'  : 1.0,
                'KKpi_exclLinePostscale'  : 1.0,
                'KKK_exclLinePrescale'  : 1.0,
                'KKK_exclLinePostscale'  : 1.0,
                'pppi_exclLinePrescale'  : 1.0,
                'pppi_exclLinePostscale'  : 1.0,
                'ppK_exclLinePrescale'  : 1.0,
                'ppK_exclLinePostscale'  : 1.0,
                },
    'STREAMS'   : ['Bhadron']
    }

## StrippingB2TwoBaryons.py
B2TwoBaryons = {
    'WGs'               : ['Charmless'],
    'BUILDERTYPE'       : 'B2TwoBaryonLines',
    'CONFIG'            : {'PrescaleB2PPbar'   : 1,
                           'MinPTB2PPbar'      : 900,
                           'MinIPChi2B2PPbar'  : 10,
                           #'TrChi2'            : 4,
                           'PIDppi'            : -1,
                           'PIDpk'             : -2,
                           'MaxPTB2PPbar'      : 2100,
                           'MaxDaughtPB2PPbar' : 300000,
                           'MaxIPChi2B2PPbar'  : 25,
                           'CombMassWindow'    : 200,
                           'VertexChi2B2PPbar' : 9,
                           'BPTB2PPbar'        : 1100,
                           'BIPChi2B2PPbar'    : 16,
                           'BDIRA'             : 0.9997,
                           'MaxGhostProb'      : 0.4,
                           'RelatedInfoTools'  : [ { "Type" : "RelInfoConeVariables",
                                                     "ConeAngle" : 1.4,
                                                     "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                                     "RecursionLevel" : 1,
                                                     "Location"  : 'ConeVar14'
                                                     },
                                                   { "Type" : "RelInfoConeVariables",
                                                     "ConeAngle" : 1.2,
                                                     "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                                     "RecursionLevel" : 1,
                                                     "Location"  : 'ConeVar12'
                                                     },
                                                   { "Type" : "RelInfoConeVariables",
                                                     "ConeAngle" : 1.0,
                                                     "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                                     "RecursionLevel" : 1,
                                                     "Location"  : 'ConeVar10'
                                                     },
                                                   { "Type" : "RelInfoConeVariables",
                                                     "ConeAngle" : 0.8,
                                                     "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                                     "RecursionLevel" : 1,
                                                     "Location"  : 'ConeVar08'
                                                     },
                                                   { "Type" : "RelInfoConeVariables",
                                                     "ConeAngle" : 0.6,
                                                     "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                                     "RecursionLevel" : 1,
                                                     "Location"  : 'ConeVar06'
                                                     },
                                                   { "Type" : "RelInfoVertexIsolation",
                                                     "Location" : "VtxIsolationVar"
                                                     }
                                                   ],
                           'ExtraInfoTools'    : [ { "Type" : "ConeVariables"
                                                     , "ConeAngle" : 0.6
                                                     , "ConeNumber" : 1
                                                     , "Variables" : ['angle', 'mult', 'ptasy']},
                                                   { "Type" : "ConeVariables"
                                                     , "ConeAngle" : 0.8
                                                     , "ConeNumber" : 2
                                                     , "Variables" : ['angle', 'mult', 'ptasy']},
                                                   { "Type" : "ConeVariables"
                                                     , "ConeAngle" : 1.0
                                                     , "ConeNumber" : 3
                                                     , "Variables" : ['angle', 'mult', 'ptasy']},
                                                   {'Type' : 'VertexIsolation'}
                                                   ] ,
                           'Bs0_APT_Min'              : 2.0*GeV,
                           'Bs0_AM_Max'               : 700*MeV,
                           'Bs0_ADOCAMAX_Long_Max'    : 5*mm,
                           'Bs0_BPVDIRA_Long_Min'     : 0.9,
                           'Bs0_BPVIPCHI2_Long_Max'   : 25,
                           'Bs0_VtxChi2_NDF_Long_Max' : 16,
                           'Bs0_BPVVDCHI2_Long_Min'   : 4,
                           'Trk_Chi2'                 : 3.0,
                           'Lambda_DD_MassWindow'     : 20.0,
                           'Lambda_DD_VtxChi2'        : 12.0,
                           'Lambda_DD_FDChi2'         : 50.0,
                           'Lambda_DD_FD'             : 300.0,
                           'Lambda_DD_Pmin'           : 8000.0,
                           'Lambda_LL_MassWindow'     : 15.0,
                           'Lambda_LL_VtxChi2'        : 12.0,
                           'Lambda_LL_FDChi2'         : 50.0,
                           'B_Mlow'                   : 500.0,
                           'B_Mhigh'                 : 500.0,
                           'B_2bodyMlow'             : 800.0,
                           'B_2bodyMhigh'            : 800.0,
                           'B_APTmin'                : 1000.0,
                           'B_PTmin'                 : 1500.0,
                           'BDaug_MedPT_PT'          : 800.0,
                           'BDaug_MaxPT_IP'          : 0.05,
                           'BDaug_DD_maxDocaChi2'    : 5.0,
                           'BDaug_LL_maxDocaChi2'    : 5.0,
                           'BDaug_DD_PTsum'          : 4200.0,
                           'BDaug_LL_PTsum'          : 3000.0,
                           'B_DD_PTMin'             : 500.0,
                           'B_LL_PTMin'             : 500.0,
                           'B_VtxChi2'               : 12.0,
                           'B_DD_Dira'               : 0.995,
                           'B_LL_Dira'               : 0.995,
                           'B_DD_IPCHI2wrtPV'        : 8.0,
                           'B_LL_IPCHI2wrtPV'        : 8.0,
                           'B_FDwrtPV'               : 1.0,
                           'B_DD_FDChi2'             : 50.0,
                           'B_LL_FDChi2'             : 50.0,
                           'GEC_MaxTracks'           : 250,
                           'Prescale'                : 1.0,
                           'Postscale'               : 1.0,
                           'MVAResponseLL'           : 0.95,
                           'MVAResponseDD'           : 0.97,
                           },
    "STREAMS"    : ['Bhadron']
    }

## StrippingB2FourKHigh.py
Bs2Q2Body4K = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'Bs2Q2Body4KConf',
    'CONFIG'      : {'Q2BPrescale'     : 1.,
                     'Q2BResMinPT'     : 900.,
                     'Q2BResMinP'      : 1.,
                     'Q2BResMinMass'   : 990.,
                     'Q2BResMaxMass'   : 2500.,
                     'Q2BResVtxChiDOF' : 9.,
                     'Q2BBMinM'        : 5200.,
                     'Q2BBMaxM'        : 5500.,
                     'Q2BBVtxChi2DOF'  : 9.,
                     'Q2BIPCHI2' : 20
                     },
    'STREAMS'     : ['Bhadron']
    }

## StrippingBu2hhh.py
B2hhh = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'Bu2hhhBuilder',
    'CONFIG'      : {'MaxTrSIZE'             : 200 ,      ## GEC maximim recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG)
                     '_h_PT'                 : 100. ,     ## tracks min PT
                     '_h_P'                  : 1500. ,    ## tracks min P
                     '_h_IPCHI2'             : 1. ,       ## min tracks IP wrt OWNPV
                     '_h_TRCHI2DOF'          : 3.0 ,      ## max tracks CHI2DOF
                     '_h_TRGHP'              : .5,        ## Track GhostProbability
                     '_3h_DOCA'              : .2 ,       ## max DOCA between h and 2h
                     '_3h_PTmax'             : 1500 ,     ## min PT of the 3h highest PT track
                     '_3h_DIRA'              : .99998 ,   ## min cos angle between 3h momentum and PV decay direction
                     '_3h_FDCHI2'            : 500. ,     ## min 3h FDCHI2 wrt best 3h PV
                     '_3h_PVDOCAmin'         : 3.0 ,      ## min value of the 3h doca wrt any PV
                     '_3h_CHI2'              : 12.0 ,     ## max 3h vertex CHI2
                     '_3h_IPCHI2'            : 10. ,      ## max 3h IP CHI2 wrt best 3h PV
                     '_3h_PT'                : 1000. ,    ## min 3h PT
                     '_3h_PTsum'             : 4500. ,    ## min of 3h tracks PT sum
                     '_3h_Psum'              : 20000. ,   ## min of 3h tracks P sum
                     '_3h_PVIPCHI2sum'       : 500. ,     ## min of the 3h tracks IP wrt best 3h PV
                     '_3h_TRKCHIDOFmin'      : 3.0,       ## max track CHI2DOF for the track with smalest CHI2DOF
                     '_3h_CORRMmax'          : 7000. ,    ## max corrected mass for 3h candidate
                     '_3h_CORRMmin'          : 4000. ,    ## min corrected mass for 3h candidate
                     '_3hKKK_Mmax'           : 6300. ,    ## max 3h mass for inclusive KKK line
                     '_3hKKK_Mmin'           : 5050. ,    ## min 3h mass for inclusive KKK line
                     '_3hpph_deltaMmax'      : 400,       ## max 3h mass difference for inclusive ppK line
                     '_3hpph_deltaMmin'      : 200,       ## min 3h mass difference for inclusive ppK line
                     'KKK_inclLinePrescale'  : 1.0,
                     'KKK_inclLinePostscale' : 1.0,
                     'KpKpKp_inclLinePrescale'  : 1.0,
                     'KpKpKp_inclLinePostscale' : 1.0,
                     'pph_inclLinePrescale'  : 1.0,
                     'pph_inclLinePostscale' : 1.0
                     },
    'STREAMS' : ['Bhadron']
    }

## StrippingBs2KKhh.py
BsPhiRho = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'BsPhiRhoConf',
    'CONFIG'      : {'PRPrescale'     : 1.,
                     'PRResMinPT'     : 1200.,
                     'PRResMinP'      : 3.,
                     'PRResMinMass'   : 0.,
                     'PRResMaxMass'   : 4000.,
                     'PRResVtxChiDOF' : 9.,
                     'PRBMinM'        : 4800.,
                     'PRBMaxM'        : 5600.,
                     'PRPhiWindow'    : 20.,
                     'PRBVtxChi2DOF'  : 9.,
                     'PRIPCHI2' : 20
                     },
    'STREAMS'     : ['Bhadron']
    }


