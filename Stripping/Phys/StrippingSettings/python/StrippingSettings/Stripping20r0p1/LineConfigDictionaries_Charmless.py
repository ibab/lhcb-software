##
## Line config dictionaries for Charmless WG
##


##
## StrippingB2HHBDT.py
##
B2HHBDT = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'B2HHBDTLines',
    'CONFIG'      : {'PrescaleB2HHBDT' : 1.,
                     'MinPT'           : 1000,
                     'MinIP'           : 0.12,
                     'TrChi2'          : 3,
                     'TrGhostProb'     : 0.5,
                     'CombMassLow'     : 4600,
                     'CombMassHigh'    : 6000,
                     'DOCA'            : 0.1,
                     'BPT'             : 1200,
                     'BIP'             : 0.12,
                     'BTAU'            : 0.0006,
                     'MassLow'         : 4800,
                     'MassHigh'        : 5800,
                     'BDTCut'          : -0.3,
                     'BDTWeightsFile'  : "$TMVAWEIGHTSROOT/data/B2HHBDT.xml"
                    },
    'STREAMS'     : { 'Bhadron' : [],
                      'BhadronCompleteEvent' : ['StrippingB2HHBDTLine']
                    }
    }

##
## StrippingD2HHBDT.py
##
D2HHBDT = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'D2HHBDTLines',
    'CONFIG'      : {'PrescaleD2HHBDT' : 1.,
                     'PrescaleDSt'     : 1.,
                     'MinPT'           : 1000,
                     'MinIP'           : 0.12,
                     'TrChi2'          : 3,
                     'TrGhostProb'     : 0.5,
                     'CombMassLow'     : 1500,
                     'CombMassHigh'    : 2800,
                     'DOCA'            : 0.1,
                     'BPT'             : 1200,
                     'BIP'             : 0.12,
                     'BTAU'            : 0.,
                     'MassLow'         : 1800,
                     'MassHigh'        : 2600,
                     'BDTCut'          : -0.3,
                     'BDTWeightsFile'  : "$TMVAWEIGHTSROOT/data/B2HHBDT.xml",
                     'VertexChi2'      : 64
                    },
    'STREAMS'     : { 'Charm' : ['StrippingD2HHBDTLine',
                                 'StrippingDStD0PiWithD2HHBDTLine']}
    }

##
## StrippingB2TwoBaryons.py
##
B2TwoBaryons = {
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
    'STREAMS'     : { 'Bhadron' : ['StrippingB2TwoBaryonsB2PPbarLine'],
                      'BhadronCompleteEvent' : []
                    }
    }

##
## StrippingBu2KSh.py
##
Bu2KSh = {           
    'WGs'         : ['Charmless'],             
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
    'STREAMS'     : { 'Bhadron' : ['StrippingBu2KSPiDDLine',
                                   'StrippingBu2KSPiLLLine'],
                      'BhadronCompleteEvent' : []
                    }
    }                

##
## StrippingButo5h.py
##
Buto5h = {
    'WGs' : ['Charmless'],
    'BUILDERTYPE' : 'Buto5hBuilder',
    'CONFIG'      : {'MaxTrSIZE'             : 120 ,
                     '_h_PT'                 : 250. ,
                     '_h_IPCHI2'             : 6. ,
                     '_h_TRCHI2DOF'          : 1.7 ,
                     '_h_TRGHP'              : 0.2 ,
                     '_5h_DOCA'              : .14 ,
                     '_5h_DIRA'              : .99999 ,
                     '_5h_FDCHI2'            : 500. ,
                     '_5h_CHI2'              : 14. ,
                     '_5h_PT'                : 1000. ,
                     '_5h_PVIPCHI2sum'       : 400. ,
                     '_5h_Mmax'              : 5679. ,
                     '_5h_Mmin'              : 5079. ,
                     '5pi_exclLinePrescale'  : 1.0,
                     '5pi_exclLinePostscale' : 1.0,
                     'K4pi_exclLinePrescale' : 1.0,
                     'K4pi_exclLinePostscale': 1.0
                    },
    'STREAMS'     : { 'Bhadron' : ['StrippingButo5h_5pi_exclLine',
                                   'StrippingButo5h_K4pi_exclLine'],
                      'BhadronCompleteEvent' : []
                    }
    }

##
## StrippingB2KSKpi.py
##
B2KSKpi = {
    'WGs' : ['Charmless'],
    'BUILDERTYPE' : 'B2KSKpiConf',
    'CONFIG'      : {'Trk_Chi2'                : 3.0,
                     'Trk_GhostProb'           : 0.3,
                     'KS_DD_MassWindow'        : 30.0,
                     'KS_DD_VtxChi2'           : 12.0,
                     'KS_DD_FDChi2'            : 50.0,
                     'KS_DD_Pmin'              : 6000.0,
                     'KS_LL_MassWindow'        : 20.0,
                     'KS_LL_VtxChi2'           : 12.0,
                     'KS_LL_FDChi2'            : 80.0,
                     'KS_LD_MassWindow'        : 25.0,
                     'KS_LD_VtxChi2'           : 12.0,
                     'KS_LD_FDChi2'            : 50.0,
                     'KS_LD_Pmin'              : 6000.0,
                     'B_Mlow'                  : 587.0,
                     'B_Mhigh'                 : 591.0,
                     'B_APTmin'                : 1000.0,
                     'BDaug_MedPT_PT'          : 800.0,
                     'BDaug_MaxPT_IP'          : 0.05,
                     'BDaug_DD_maxDocaChi2'    : 5.0,
                     'BDaug_LL_maxDocaChi2'    : 5.0,
                     'BDaug_LD_maxDocaChi2'    : 5.0,
                     'BDaug_DD_PTsum'          : 4200.0,
                     'BDaug_LL_PTsum'          : 3000.0,
                     'BDaug_LD_PTsum'          : 4200.0,
                     'B_DD_Dira'               : 0.,
                     'B_LL_Dira'               : 0.,
                     'B_LD_Dira'               : 0.,
                     'DD_BDTCutValue'          : -0.05,
                     'LD_BDTCutValue'          : -0.05,
                     'LL_BDTCutValue'          : -0.10,
                     'DD_BDTWeightsFile'       : '$TMVAWEIGHTSROOT/data/B2KShhBDT_DD.xml',
                     'LD_BDTWeightsFile'       : '$TMVAWEIGHTSROOT/data/B2KShhBDT_DD.xml',
                     'LL_BDTWeightsFile'       : '$TMVAWEIGHTSROOT/data/B2KShhBDT_LL.xml',
                     'GEC_MaxTracks'           : 250,
                     'Prescale'                : 1.0,
                     'Prescale_SameSign'       : 1.0,
                     'Postscale'               : 1.0
                    },
    'STREAMS'     : { 'Bhadron' : [],
                      'BhadronCompleteEvent' : ['StrippingB2KSKpiDDLine',
                                                'StrippingB2KSKpiLLLine',
                                                'StrippingB2KSKpiLDLine',
                                                'StrippingB2KSKpiDDSameSignLine',
                                                'StrippingB2KSKpiLLSameSignLine',
                                                'StrippingB2KSKpiLDSameSignLine'] 
                    }
    } 
