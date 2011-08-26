"""
Line config dictionaries for the Gamma from Loops WG


Dictionary names should have the same name as the corresponding
StrippingSelections file containing the line builder instance.

   e.g  StrippingPromptCharm linebuilder should be added as:

      PromptCharm = { ... }
"""

from GaudiKernel.SystemOfUnits import *


##
## B-> hh, pp 
##
Hb2Charged2Body = {
    'BUILDERTYPE' : 'Hb2Charged2BodyLines',
    'CONFIG'      : {
    'PrescaleB2Charged2Body'   : 1,      'PrescaleB2PPbar' : 1,
    'MinPTB2Charged2Body'      : 1100,    'MinPTB2PPbar' : 900,
    'MinIPB2Charged2Body'      : 0.12,
    'MinIPChi2B2Charged2Body'  : 100,     'MinIPChi2B2PPbar' : 9,
    'TrChi2'                   : 5,
    'PIDppi'                   : 0,
    'PIDpk'                    : -2,
    'MaxPTB2Charged2Body'      : 2500,   'MaxPTB2PPbar' : 2100,
    'MaxIPB2Charged2Body'      : 0.25,
    'MaxIPChi2B2Charged2Body'  : 200,    'MaxIPChi2B2PPbar' : 25,
    'CombMassLow'              : 4800,
    'CombMassHigh'             : 5800,   'CombMassWindow' : 200,
    'DOCA'                     : 0.08,   'VertexChi2B2PPbar' : 16,
    'BPT'                      : 1200,   'BPTB2PPbar' : 1000,
    'BIP'                      : 0.1,
    'BIPChi2B2Charged2Body'    : 12,     'BIPChi2B2PPbar' : 36,
    'BDIRA'                    : 0.9995,
    'BTAU'                     : 0.0006,
    'MassLow'                  : 4800,
    'MassHigh'                 : 5800
    },
    'STREAMS'     : [ 'Bhadron' ],
    'WGs'         : ['GammaFromLoops']
    }

##
## B->hh, lifetime  unbiased
##
B2hhLTUnbiased = {
    'BUILDERTYPE' : 'StrippingB2hhLTUnbiasedConf' ,
    'CONFIG' : {'TrackChi2'               :    5.0
                , 'DaughterPtMin'         :    1.5 # GeV
                , 'DaughterPtMax'         :    2.0 # GeV
                , 'DaughterPMin'          :   10.0 # GeV
                , 'DaughterPIDKMax'       :    0.1
                , 'BMassMin'              :    5.0 # GeV
                , 'BMassMax'              :    6.0 # GeV
                , 'DOCA'                  :    0.07
                , 'VertexChi2'            :   25.0
                , 'PrescaleLoose'         :    0.005
                , 'PostscaleLoose'        :    1.0
                , 'PrescaleNB'            :    0.15
                , 'PostscaleNB'           :    1.0
                , 'NetCut'                :    0.9
                , 'BLifetime'             : -999.0
                },
    'STREAMS' : [ 'Bhadron' ],
    'WGs'    : ['GammaFromLoops']
    }

B2hhLTUnbiasedDetached= {
    'BUILDERTYPE' : 'StrippingB2hhLTUnbiasedConf' ,
    'CONFIG' : {'TrackChi2'               :    5.0
                , 'DaughterPtMin'         :    1.5 # GeV
                , 'DaughterPtMax'         :    2.0 # GeV
                , 'DaughterPMin'          :   10.0 # GeV
                , 'DaughterPIDKMax'       :    0.1
                , 'BMassMin'              :    5.0 # GeV
                , 'BMassMax'              :    6.0 # GeV
                , 'DOCA'                  :    0.07
                , 'VertexChi2'            :   25.0
                , 'PrescaleLoose'         :    0.005
                , 'PostscaleLoose'        :    1.0
                , 'PrescaleNB'            :    1.0
                , 'PostscaleNB'           :    1.0
                , 'NetCut'                :    0.9
                , 'BLifetime'             :    0.3
                },
    'STREAMS' : [ 'Bhadron' ],
    'WGs'    : ['GammaFromLoops']
    }

##
## B->hhh
##
B2hhh = {
    'BUILDERTYPE' : 'Bu2hhhBuilder',
    'CONFIG'      : {
    'MaxTrSIZE'            :  200., # long tracks!
    '_h_PT'                : 100. ,
    '_h_P'                 : 1500. ,
    '_h_IPCHI2'            : 1. ,
    '_h_TRCHI2DOF'         : 3.0 ,
    '_3h_DOCA'             : .2 ,
    '_3h_PTmax'            : 1500 ,
    '_3h_DIRA'             : .99998 ,# !
    '_3h_FDCHI2'           : 500. ,
    '_3h_PVDOCAmin'        : 3.0 ,
    '_3h_CHI2'             : 12.0 ,
    '_3h_IPCHI2'           : 10. , # 8.!
    '_3h_PT'               : 1000. ,
    '_3h_PTsum'            : 4500.  ,# 4500?
    '_3h_Psum'             : 20000.  ,
    '_3h_PVIPCHI2sum'      : 500. ,
    '_3h_TRKCHIDOFmin'     : 3.,
    '_3h_CORRMmax'         : 7000. ,
    '_3h_CORRMmin'         : 4000. ,
    '_3hKKK_Mmax'          : 6300. , # 6200?
    '_3hKKK_Mmin'          : 5050. , # 5050!
    '_3hpph_deltaMmax'     :400, # 300!
    '_3hpph_deltaMmin'     :200,
    'KKK_inclLinePrescale' : 1. ,
    'KKK_inclLinePostscale': 1 ,
    'pph_inclLinePrescale' : 1 ,
    'pph_inclLinePostscale': 1
    },
    'STREAMS' : [ 'Bhadron' ],
    'WGs'    : ['GammaFromLoops']
    }

##
## B->hpi pi0
##
B2HHPi0 = {
    'BUILDERTYPE' : 'StrippingB2HHPi0Conf',
    'WGs'         : [ 'GammaFromLoops' ] ,
    'STREAMS'     : [ 'Bhadron' ],
    'CONFIG'      : {
    'PiMinPT'              : 500       # MeV
    ,'PiMinP'               : 5000      # MeV
    ,'PiMinTrackProb'       : 0.000001  # unitless
    ,'PiMinIPChi2'          : 25        # unitless
    ,'Pi0MinPT_M'           : 2500      # MeV
    ,'Pi0MinPT_R'           : 1500      # MeV
    ,'ResPi0MinMM'          : 100       # MeV  # no effect if bellow 105
    ,'ResPi0MaxMM'          : 170       # MeV  # no effect if above  165
    ,'ResPi0MinGamCL'       : 0.2       # unitless
    ,'BMinM'                : 4200      # MeV
    ,'BMaxM'                : 6400      # MeV
    ,'BMinPT_M'             : 3000      # MeV
    ,'BMinPT_R'             : 2500      # MeV
    ,'BMinVtxProb'          : 0.001     # unitless
    ,'BMaxIPChi2'           : 9         # unitless
    ,'BMinDIRA'             : 0.99995   # unitless
    ,'BMinVVDChi2'          : 64        # unitless
    ,'MergedLinePrescale'   : 1.        # unitless
    ,'MergedLinePostscale'  : 1.        # unitless
    ,'ResolvedLinePrescale' : 1.        # unitless
    ,'ResolvedLinePostscale': 1.        # unitless
    }
    }

##
## B->Kshh
##
B2KShh = {
    'BUILDERTYPE' : 'B2KShhConf',
    'CONFIG' : { 'Trk_Chi2'         	: 4.0,
                 'KS_DD_MassWindow' 	: 30.0,
                 'KS_DD_VtxChi2'    	: 12.0,
                 'KS_DD_FDChi2'     	: 50.0,
                 'KS_DD_Pmin'       	: 6000.0,
                 'KS_LL_MassWindow' 	: 20.0,
                 'KS_LL_VtxChi2'    	: 12.0,
                 'KS_LL_FDChi2'     	: 80.0,
                 'B_Mlow'           	: 200.0,
                 'B_Mhigh'          	: 280.0,
                 'BDaug_MedPT_PT'   	: 800.0,
                 'BDaug_MaxPT_IP'   	: 0.05,
                 'BDaug_DD_maxDocaChi2' : 3.0,
                 'BDaug_LL_maxDocaChi2' : 5.0,
                 'BDaug_DD_PTsum'   	: 4800.0,
                 'BDaug_LL_PTsum'   	: 4500.0,
                 'B_VtxChi2'        	: 12.0,
                 'B_Dira'           	: 0.9999,
                 'B_DD_IPCHI2wrtPV'	: 8.0,
                 'B_LL_IPCHI2wrtPV'	: 8.0,
                 'BDaug_LL_IPChi2sum'  	: 15.0,
                 'BDaug_DD_IPChi2sum'  	: 15.0,
                 'B_FDwrtPV'		: 1.0,
                 'B_DD_FDChi2'		: 50.0,
                 'B_LL_FDChi2'		: 50.0,
                 'GEC_MaxTracks'	: 250,
                 'Prescale'		: 1.0,
                 'Postscale'		: 1.0
                 },
    'STREAMS' : [ 'Bhadron' ],
    'WGs' : ['GammaFromLoops']
    }
