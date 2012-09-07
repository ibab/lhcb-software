##
## StrippingBu2KSh.py
##
Bu2KSh = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'Bu2KShConf',
    'CONFIG'      : {'KSDaug_LTrk_GhostProb' : 0.5,
                     'Trk_GhostProb'         : 0.5,
                     'Trk_Chi2'              : 3.0,
                     'KS_DD_MassWindow'      : 30.0,
                     'KS_LL_MassWindow'      : 15.0,
                     'KS_DD_VtxChi2'         : 10.0,
                     'KS_LL_VtxChi2'         : 10.0,
                     'KS_DD_FDChi2'          : 50.0,
                     'KS_LL_FDChi2'          : 100.0,
                     'KS_DD_Pmin'            : 8000.0,
                     'KS_LL_Pmin'            : 8000.0,
                     'KS_DD_Ptmin'           : 1000.0,
                     'KS_LL_Ptmin'           : 800.0,
                     'Bach_Ptmin'            : 1000.0,
                     'BDaug_MaxPT_IP'        : 0.05,
                     'BDaug_DD_PTsum'        : 4000.0,
                     'BDaug_LL_PTsum'        : 4000.0,
                     'B_Mlow'                : 500.0,
                     'B_Mhigh'               : 500.0,
                     'B_Pmin'                : 25000.0,
                     'B_LL_VtxChi2'          : 6.0,
                     'B_DD_VtxChi2'          : 6.0,
                     'B_Dira'                : 0.9995,
                     'B_DD_IPCHI2wrtPV'      : 10.0,
                     'B_LL_IPCHI2wrtPV'      : 10.0,
                     'B_FDwrtPV'             : 1.0,
                     'B_DD_FDChi2'           : 50,
                     'B_LL_FDChi2'           : 50,
                     'GEC_MaxTracks'         : 250,
                     'Prescale'              : 1.0,
                     'Postscale'             : 1.0
                    },
    'STREAMS'     : { 'Bhadron' : ['StrippingBu2KSPiDDLine',
                                   'StrippingBu2KSPiLLLine'],
                      'BhadronCompleteEvent' : []
                    }
    }

##
## StrippingHb2Charged2Body.py
##
Hb2Charged2Body = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'Hb2Charged2BodyLines',
    'CONFIG'      : {'PrescaleB2Charged2Body'  : 1,
                     'MinPTB2Charged2Body'     : 1100,
                     'MinIPB2Charged2Body'     : 0.15,
                     'MinIPChi2B2Charged2Body' : 100,
                     'TrChi2'                  : 3,
                     'TrGhostProb'             : 0.8,
                     'MaxPTB2Charged2Body'     : 2700,
                     'MaxIPB2Charged2Body'     : 0.27,
                     'MaxIPChi2B2Charged2Body' : 200,
                     'CombMassLow'             : 4600,
                     'CombMassHigh'            : 6000,
                     'DOCA'                    : 0.08,
                     'BPT'                     : 1200,
                     'BIP'                     : 0.08,
                     'BIPChi2B2Charged2Body'   : 12,
                     'BTAU'                    : 0.0006,
                     'MassLow'                 : 4800,
                     'MassHigh'                : 5800
                    },
    'STREAMS'     : { 'Bhadron' : [],
                      'BhadronCompleteEvent' : [ 'StrippingHb2Charged2BodyB2Charged2BodyLine',
                                                 'StrippingHb2Charged2BodyB2KPlusPiMinusLine',
                                                 'StrippingHb2Charged2BodyB2PiPlusKMinusLine',
                                                 'StrippingHb2Charged2BodyBs2KPlusKMinusLine',
                                                 'StrippingHb2Charged2BodyLb2PPlusPiMinusLine',
                                                 'StrippingHb2Charged2BodyLb2PiPlusPMinusLine',
                                                 'StrippingHb2Charged2BodyLb2PPlusKMinusLine',
                                                 'StrippingHb2Charged2BodyLb2KPlusPMinusLine']
                    }
    }

##
## StrippingB2HHBkg.py
##
B2HHBkg = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'B2HHBkgLines',
    'CONFIG'      : {'PrescaleB2HHBkg' : 0.5,
                     'MinPT'           : 1000,
                     'MinIP'           : 0.12,
                     'TrChi2'          : 3,
                     'CombMassLow'     : 4600,
                     'CombMassHigh'    : 6000,
                     'DOCA'            : 0.1,
                     'BPT'             : 1200,
                     'BIP'             : 0.12,
                     'BTAU'            : 0.0003,
                     'MassLow'         : 4800,
                     'MassHigh'        : 5800 
                    },
    'STREAMS'     : { 'Bhadron' : ['StrippingB2HHBkgLine'],
                      'BhadronCompleteEvent' : []
                    }
    }
##
## StrippingB2hhLTUnbiased.py
##
B2hhLTUnbiased = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'StrippingB2hhLTUnbiasedConf' ,
    'CONFIG'      : {'TrackChi2'       :    5.0,
                     'DaughterPtMin'   :    1.5,
                     'DaughterPtMax'   :    2.0,
                     'DaughterPMin'    :   10.0,
                     'DaughterPIDKMax' :    0.1,
                     'BMassMin'        :    5.0,
                     'BMassMax'        :    6.0,
                     'DOCA'            :    0.07,
                     'VertexChi2'      :   25.0,
                     'PrescaleLoose'   :    0.005,
                     'PostscaleLoose'  :    1.0,
                     'PrescaleNB'      :    0.15,
                     'PostscaleNB'     :    1.0,
                     'NetCut'          :    0.9,
                     'BLifetime'       : -999.0
                    },
    'STREAMS'     : { 'Bhadron' : ['StrippingB2hhLTUnbiasedLooseLine',
                                   'StrippingB2hhLTUnbiasedNeuroBayesLine'],
                      'BhadronCompleteEvent' : []
                    }

    }
##
## StrippingB2hhLTUnbiased.py
##
B2hhLTUnbiasedDetached= {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'StrippingB2hhLTUnbiasedConf' ,
    'CONFIG'      : {'TrackChi2'       :    5.0,
                     'DaughterPtMin'   :    1.5, # GeV
                     'DaughterPtMax'   :    2.0, # GeV
                     'DaughterPMin'    :   10.0, # GeV
                     'DaughterPIDKMax' :    0.1,
                     'BMassMin'        :    5.0, # GeV
                     'BMassMax'        :    6.0, # GeV
                     'DOCA'            :    0.07,
                     'VertexChi2'      :   25.0,
                     'PrescaleLoose'   :    0.005,
                     'PostscaleLoose'  :    1.0,
                     'PrescaleNB'      :    1.0,
                     'PostscaleNB'     :    1.0,
                     'NetCut'          :    0.9,
                     'BLifetime'       :    0.3
                   },
    'STREAMS'    : { 'Bhadron' : ['StrippingB2hhLTUnbiasedDetachedLooseLine',
                                  'StrippingB2hhLTUnbiasedDetachedNeuroBayesLine'],
                     'BhadronCompleteEvent' : []
                   }
    }
##
## StrippingBu2hhh.py
##
B2hhh = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'Bu2hhhBuilder',
    'CONFIG'      : {'MaxTrSIZE'             : 200,
                     '_h_PT'                 : 100.,
                     '_h_P'                  : 1500.,
                     '_h_IPCHI2'             : 1.,
                     '_h_TRCHI2DOF'          : 3.0,
                     '_h_TRGHP'              : .5,
                     '_3h_DOCA'              : .2,
                     '_3h_PTmax'             : 1500,
                     '_3h_DIRA'              : .99998,
                     '_3h_FDCHI2'            : 500.,
                     '_3h_PVDOCAmin'         : 3.0,
                     '_3h_CHI2'              : 12.0,
                     '_3h_IPCHI2'            : 10.,
                     '_3h_PT'                : 1000.,
                     '_3h_PTsum'             : 4500.,
                     '_3h_Psum'              : 20000.,
                     '_3h_PVIPCHI2sum'       : 500.,
                     '_3h_TRKCHIDOFmin'      : 3.0,
                     '_3h_CORRMmax'          : 7000.,
                     '_3h_CORRMmin'          : 4000.,
                     '_3hKKK_Mmax'           : 6300.,
                     '_3hKKK_Mmin'           : 5050.,
                     '_3hpph_deltaMmax'      : 400,
                     '_3hpph_deltaMmin'      : 200,
                     'KKK_inclLinePrescale'  : 1.0,
                     'KKK_inclLinePostscale' : 1.0,
                     'pph_inclLinePrescale'  : 1.0,
                     'pph_inclLinePostscale' : 1.0
                    },
    'STREAMS'     : { 'Bhadron' : ['StrippingB2hhh_KKK_inclLine',
                                   'StrippingB2hhh_pph_inclLine'],
                      'BhadronCompleteEvent' : []
                    }
    }
##
## StrippingB2HHPi0.py
##
B2HHPi0 = {
      'WGs'         : [ 'Charmless' ] ,
      'BUILDERTYPE' : 'StrippingB2HHPi0Conf',
      'CONFIG'      : {'PiMinPT'              : 500,
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
      'STREAMS'     : { 'Bhadron' : [],
                        'BhadronCompleteEvent' : ['StrippingB2HHPi0_R',
                                                  'StrippingB2HHPi0_M']
                      }
    }
##
## StrippingB2TwoBaryons.py
##
B2PPbar = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'B2TwoBaryonLines',
    'CONFIG'      : {'PrescaleB2PPbar'   : 1,
                     'MinPTB2PPbar'      : 900,
                     'MinIPChi2B2PPbar'  : 10,
                     'PIDppi'            : -1,
                     'PIDpk'             : -2,
                     'MaxPTB2PPbar'      : 2100,
                     'MaxIPChi2B2PPbar'  : 25,
                     'CombMassWindow'    : 200,
                     'VertexChi2B2PPbar' : 9,
                     'BPTB2PPbar'        : 1100,
                     'BIPChi2B2PPbar'    : 16,
                     'BDIRA'             : 0.9997,
                     'MaxGhostProb'      : 0.4
                    },
    'STREAMS'     : { 'Bhadron' : ['StrippingB2PPbarB2PPbarLine'],
                      'BhadronCompleteEvent' : []
                    }
    }
##
## StrippingB2KShh.py
##
B2KShh = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'B2KShhConf',
    'CONFIG'      : {'Trk_Chi2'             : 4.0,
                     'Trk_GhostProb'        : 0.5,
                     'KS_DD_MassWindow'     : 30.0,
                     'KS_DD_VtxChi2'        : 12.0,
                     'KS_DD_FDChi2'         : 50.0,
                     'KS_DD_Pmin'           : 6000.0,
                     'KS_LL_MassWindow'     : 20.0,
                     'KS_LL_VtxChi2'        : 12.0,
                     'KS_LL_FDChi2'         : 80.0,
                     'B_Mlow'               : 1279.0,
                     'B_Mhigh'              : 921.0,
                     'B_APTmin'             : 1000.0,
                     'B_PTmin'              : 1500.0,
                     'BDaug_MedPT_PT'       : 800.0,
                     'BDaug_MaxPT_IP'       : 0.05,
                     'BDaug_DD_maxDocaChi2' : 5.0,
                     'BDaug_LL_maxDocaChi2' : 5.0,
                     'BDaug_DD_PTsum'       : 4200.0,
                     'BDaug_LL_PTsum'       : 3000.0,
                     'B_DD_IPCHI2sum'       : 50.0,
                     'B_VtxChi2'            : 12.0,
                     'B_DD_Dira'            : 0.999,
                     'B_LL_Dira'            : 0.9999,
                     'B_DD_IPCHI2wrtPV'     : 6.0,
                     'B_LL_IPCHI2wrtPV'     : 8.0,
                     'B_DD_FDwrtPV'         : 1.7,
                     'B_LL_FDwrtPV'         : 1.0,
                     'B_DD_FDChi2'          : 50.0,
                     'B_LL_FDChi2'          : 50.0,
                     'GEC_MaxTracks'        : 250,
                     'Prescale'             : 1.0,
                     'Prescale_SameSign'    : 1.0,
                     'Postscale'            : 1.0
                    },
    'STREAMS'     : { 'Bhadron' : ['StrippingB2KShhDDSameSignLine',
                                   'StrippingB2KShhLLSameSignLine'], 
                      'BhadronCompleteEvent' : ['StrippingB2KShhDDLine',     
                                                'StrippingB2KShhLLLine']
                    }
    }
##
## StrippingLb2V0hh.py
##
Lb2V0hh = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'Lb2V0hhConf',
    'CONFIG'      : {'Trk_Chi2'              : 4.0,
                     'Lambda_DD_MassWindow'  : 20.0,
                     'Lambda_DD_VtxChi2'     : 12.0,
                     'Lambda_DD_FDChi2'      : 50.0,
                     'Lambda_DD_FD'          : 300.0,
                     'Lambda_DD_Pmin'        : 8000.0,
                     'Lambda_LL_MassWindow'  : 15.0,
                     'Lambda_LL_VtxChi2'     : 12.0,
                     'Lambda_LL_FDChi2'      : 50.0,
                     'Lb_Mlow'               : 1319.0,
                     'Lb_Mhigh'              : 500.0,
                     'Lb_APTmin'             : 1000.0,
                     'Lb_PTmin'              : 1500.0,
                     'LbDaug_MedPT_PT'       : 800.0,
                     'LbDaug_MaxPT_IP'       : 0.05,
                     'LbDaug_DD_maxDocaChi2' : 5.0,
                     'LbDaug_LL_maxDocaChi2' : 5.0,
                     'LbDaug_DD_PTsum'       : 4200.0,
                     'LbDaug_LL_PTsum'       : 3000.0,
                     'Lb_VtxChi2'            : 12.0,
                     'Lb_DD_Dira'            : 0.9999,
                     'Lb_LL_Dira'            : 0.9999,
                     'Lb_DD_IPCHI2wrtPV'     : 8.0,
                     'Lb_LL_IPCHI2wrtPV'     : 8.0,
                     'Lb_FDwrtPV'            : 1.5,
                     'Lb_DD_FDChi2'          : 50.0,
                     'Lb_LL_FDChi2'          : 50.0,
                     'GEC_MaxTracks'         : 250,
                     'Prescale'              : 1.0,
                     'Postscale'             : 1.0
                    },
    'STREAMS'     : { 'Bhadron' : ['StrippingLb2V0hhDDLine',
                                   'StrippingLb2V0hhLLLine'],
                      'BhadronCompleteEvent' : []
                    }
    }
##
## StrippingB2Kphiphi.py
##
B2KPhiPhi = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'B2KPhiPhiConf',
    'CONFIG'      : {'Prescale'       : 1.0 ,
                     'Postscale'      : 1.0 ,
                     'K_PT'           : 400.,
                     'K_IPCHI2'       : 6.25,
                     'K_TRCHI2'       : 3.3,
                     'phi_VDZ'        : 0.,
                     'phi_PT'         : 300.,
                     'phi_IPCHI2'     : 6.,
                     'phi_VCHI2_VDOF' : 16.,
                     'phi_MM_max'     : 1060.,
                     'phi_MM_min'     : 980.,
                     'B_ADAMASS'      : 500.,
                     'B_DIRA'         : 0.999,
                     'B_IPCHI2'       : 12.25,
                     'B_VCHI2_VDOF'   : 16.,
                     'B_LTCHI2'       : 64.,
                     'B_VDZ'          : 0.
                    },
    'STREAMS'     : { 'Bhadron' : ['StrippingB2KPhiPhiNominalLine'],
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
                     'Q2BTrkMinIPChi2' : 25.,
                     'Q2BTrkMinHiPT'   : 1000.,
                     'Q2BResMinPT'     : 600.,
                     'Q2BResMinHiPT'   : 1000.,
                     'Q2BResMaxMass'   : 1000.,
                     'Q2BResVtxChi2DOF': 9.,
                     'Q2BBMinPT'       : 2500.,
                     'Q2BBMinM4pi'     : 3800.,
                     'Q2BBMinM3pi'     : 4400.,
                     'Q2BBMaxM'        : 5700.,
                     'Q2BBMaxCorrM'    : 6000.,
                     'Q2BBVtxChi2DOF'  : 6.
                    },
    'STREAMS'     : { 'Bhadron' : [],
                      'BhadronCompleteEvent' : ['StrippingBetaSQ2B3piSelectionLine',
                                                'StrippingBetaSQ2B4piSelectionLine']
                    }
    }
##
## StrippingBs2PhiKst0.py
##
BetaSBs2PhiKst = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'StrippingBs2PhiKstConf',
    'CONFIG'      : {"KaonPT"       : 500.0,
                     "KaonIPCHI2"   : 9.,    
                     "KaonPIDK"     : 0.,    
                     "PionPT"       : 500.0, 
                     "PionIPCHI2"   : 9.,    
                     "PionPIDK"     : 10.,   
                     "PhiVCHI2"     : 9.,    
                     "PhiPT"        : 900.0, 
                     "PhiMassWin"   : 25.0,  
                     "KstarVCHI2"   : 9.0,   
                     "KstarPT"      : 900.0, 
                     "KstarMassWin" : 150.0, 
                     "BMassWin"     : 500.0, 
                     "BVCHI2"       : 15.0,  
                     "BDOCA"        : 0.3,  
                     "BDIRA"        : 0.99      # adimensional 
                    },
    'STREAMS'     : { 'Bhadron' : [],
                      'BhadronCompleteEvent' : ['StrippingBetaSBs2PhiKstNominalLine']
                    }

    }
##
## StrippingBs2Kst0Kst0.py
##
BetaSBs2KstKst = {
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
    'STREAMS'     : { 'Bhadron' : ['StrippingBetaSBs2KstKstSameChargeLine'],
                      'BhadronCompleteEvent' : ['StrippingBetaSBs2KstKstNominalLine',
                                                'StrippingBd2JPsiKstForBetaSBs2KstKstLine']
                    }
    }
##
## StrippingBs2Kst_0Kst_0.py
##
BetaSBs2K0stK0st = {
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
    'STREAMS'     : { 'Bhadron' : [],
                      'BhadronCompleteEvent' : ['StrippingBetaSBs2K0stK0stNominalLine']
                    }
    }
##
## StrippingBs2Q2B.py
##
BetaSBs2Q2Body4pi = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'Bs2Q2Body4piConf',
    'CONFIG'      : {'Q2BPrescale'     : 1.,
                     'Q2BResMinPT'     : 900.,
                     'Q2BResMinP'      : 1.,
                     'Q2BResMinMass'   : 0.,
                     'Q2BResMaxMass'   : 1100.,
                     'Q2BResVtxChiDOF' : 12.,
                     'Q2BBMinM'        : 4500.,
                     'Q2BBMaxM'        : 5700.,
                     'Q2BBVtxChi2DOF'  : 12.,
                     'Q2BIPCHI2'       : 20
                    },
    'STREAMS'     : { 'Bhadron' : [],
                      'BhadronCompleteEvent' : ['StrippingBetaSBs2Q2Body4piLine']
                    }

    }
##
## StrippingBs2KKhh.py
##
BsPhiRho = {
    'WGs'         :       ['Charmless'],
    'BUILDERTYPE' : 'BsPhiRhoConf',
    'CONFIG'      : {'PRPrescale'     : 1.,
                     'PRResMinPT'     : 900.,
                     'PRResMinP'      : 1.,
                     'PRResMinMass'   : 0.,
                     'PRResMaxMass'   : 4000.,
                     'PRResVtxChiDOF' : 9.,
                     'PRBMinM'        : 4800.,
                     'PRBMaxM'        : 5600.,
                     'PRPhiWindow'    : 25.,
                     'PRBVtxChi2DOF'  : 9.,
                     'PRIPCHI2' : 20
                    },
    'STREAMS'     : { 'Bhadron' : ['StrippingBsPhiRhoLine'],
                      'BhadronCompleteEvent' : []
                    }
    }
##
## StrippingB2FourKHigh.py
##
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
                     'Q2BIPCHI2'       : 20
                    },
    'STREAMS'     : { 'Bhadron' : ['StrippingBs2Q2Body4KLine'],
                      'BhadronCompleteEvent' : []
                    }
    }
##
## StrippingBs2PhiPhi.py
##
BetaSBs2PhiPhi  = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'StrippingBs2PhiPhiConf',
    'CONFIG'      : {'KaonPT'            : 400, 
                     'KaonIPCHI2'        : 2.5, 
                     'PhiPT'             : 0,   
                     'PhiPTsq'           : 2.0, 
                     'PhiVertexCHI2pDOF' : 15,  
                     'PhiMassWindow'     : 25,  
                     'PhiMassMax'        : 1090,
                     'BsVertexCHI2pDOF'  : 15,  
                     'BsMassWindow'      : 300, 
                     'WidePrescale'      : 0.15
                    },
    'STREAMS'     : { 'Bhadron' : [],
                      'BhadronCompleteEvent' : ['StrippingBetaSBs2PhiPhiLine',
                                                'StrippingBetaSBs2PhiPhiWideLine']
                    }
    }
##
## StrippingBs2KSKS.py
##
Bs2KSKS = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'Bs2KSKSConf',
    'CONFIG'      : {'MVA_LL_PROBABILITY_CUT_BCC' : 0.5,
                     'MVA_LL_PROBABILITY_CUT_BCu' : 0.5,
                     'MVA_LD_PROBABILITY_CUT_BCC' : 0.5,
                     'MVA_LD_PROBABILITY_CUT_BCu' : 0.5,
                     'MVA_DD_PROBABILITY_CUT_BCu' : 0.5,
                     'KS_LL_MASS'                 :50.0,
                     'KS_DD_MASS'                 :100.0,
                     'KS_LL_DOCA'                 :1.0,
                     'KS_DD_DOCA'                 :4.0,
                     'KS_LL_TRGHOSTPROB'          :1.0,
                     'KS_DD_TRGHOSTPROB'          :1.0,
                     'B_MassWindow'               : 600.0,
                     'B_DOCA_LL'                  : 1.0,
                     'B_DOCA_DD'                  : 4.0,
                     'Prescale'                   : 1.0,
                     'Postscale'                  : 1.0
                    },
    'STREAMS'     : { 'Bhadron' : ['StrippingBs2KSKSDDLine',
                                   'StrippingBs2KSKSLDLine',
                                   'StrippingBs2KSKSLLLine'],
                      'BhadronCompleteEvent' : []
                    }
    }
##
## StrippingB2XEta.py
##
B2XEta = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'B2XEtaConf',
    'CONFIG'      : {'Trk_Chi2'              : 3.0,
                     'Trk_PT'                : 400.0,
                     'Trk_IPChi2'            : 20.0,
                     'KS_DD_MassWindow'      : 28.0,
                     'KS_DD_VtxChi2'         : 12.0,
                     'KS_DD_FDChi2'          : 500.0,
                     'KS_DD_PTmin'           : 1000.0,
                     'KS_LL_MassWindow'      : 18.0,
                     'KS_LL_VtxChi2'         : 12.0,
                     'KS_LL_FDChi2'          : 15.0,
                     'KS_LL_PTmin'           : 1000.0,
                     'L_DD_MassWindow'       : 20.0,
                     'L_DD_VtxChi2'          : 12.0,
                     'L_DD_PTmin'            : 1500.0,
                     'L_LL_MassWindow'       : 15.0,
                     'L_LL_VtxChi2'          : 12.0,
                     'L_LL_PTmin'            : 1500.0,
                     'eta_P'                 : 12000, 
                     'gamma_PT'              : 800,  
                     'eta_prime_MassWindow'  : 50.0,
                     'eta_prime_PT'          : 800.0,
                     'eta_prime_vtxChi2'     : 10.0,
                     'eta_prime_DOCA'        : 12.0,
                     'B_MassWindow'          : 750.0,
                     'B_PTmin'               : 800.0,
                     'BDaug_DD_maxDocaChi2'  : 20.0,
                     'BDaug_LL_maxDocaChi2'  : 20.0,
                     'B_VtxChi2'             : 20.0,
                     'B_Dira'                : 0.999,
                     'B_DD_IPCHI2wrtPV'      : 20.0,
                     'B_LL_IPCHI2wrtPV'      : 20.0,
                     'B_DD_FDChi2'           : 20.0,
                     'B_LL_FDChi2'           : 20.0,
                     'Lb_MassWindow'         : 750.0,
                     'Lb_Pmin'               : 6000.0,
                     'LbDaug_DD_maxDocaChi2' : 20.0,
                     'LbDaug_LL_maxDocaChi2' : 20.0,
                     'Lb_VtxChi2'            : 20.0,
                     'Lb_Dira'               : 0.999,
                     'Lb_DD_IPCHI2wrtPV'     : 20.0,
                     'Lb_LL_IPCHI2wrtPV'     : 20.0,
                     'GEC_MaxTracks'         : 250,
                     'Prescale'              : 1.0,
                     'Postscale'             : 1.0
                    },
    'STREAMS'     : { 'Bhadron' : ['StrippingB2XEtaB2etaKSLLLine',
                                   'StrippingB2XEtaB2etaKSDDLine',
                                   'StrippingB2XEtaLb2etaLLLLine',
                                   'StrippingB2XEtaLb2etaLDDLine',
                                   'StrippingB2XEtaB2etapKSLLLine',
                                   'StrippingB2XEtaB2etapKSDDLine',
                                   'StrippingB2XEtaLb2etapLLLLine',
                                   'StrippingB2XEtaLb2etapLDDLine'],
                      'BhadronCompleteEvent' : []
                    }
    }
