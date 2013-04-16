#
# Rare Decay Lines
from GaudiKernel.SystemOfUnits import *


####################################################################
#
# Lines for radiative decay sub-WG
#
####################################################################

Beauty2XGamma = {
    'BUILDERTYPE' : 'Beauty2XGamma',
    'CONFIG' : {
    "ALL" : {  'TRCHI2DOF_MAX'  : 3,
               'PT_MIN'         : '300*MeV',
               'P_MIN'          : '1000*MeV',
               'MIPCHI2DV_MIN'  : 16,
               'TRGHP_MAX'      : 0.4},
    "GAMMA" : { 'PT_MIN'       : '2500*MeV',
                'CL_MIN'       : 0.25
                },
    "GAMMACONV" : { 'VCHI2DOF_MAX' : 9,
                    'MM_MAX'       : '100*MeV'
                    },
    "KS0" : { 'PT_MIN'        : '1000*MeV',
              'BPVVDCHI2_MIN' : 81 ,
              'VCHI2DOF_MAX'  : 9,
              'MM_MIN'        : '480.*MeV',
              'MM_MAX'        : '515.*MeV' },
    "Pi0" : { 'PT_MIN'        : '1200*MeV',
              'P_MIN'         : '4000*MeV',
              'CHILDCL1_MIN'  : 0.25,
              'CHILDCL2_MIN'  : 0.25 },
    "B2X" : { 'SUMPT_MIN'     : '5000*MeV',
              'VCHI2DOF_MAX'  : 9,
              'BPVIPCHI2_MAX' : 9,
              'BPVDIRA_MIN'   : 0.9998,
              'AM_MIN'        : '4000*MeV',
              'AM_MAX'        : '7000*MeV',
              },
    "HH": { 'MASS_WINDOW'      : {'KST':'150*MeV','RHO':
                                  ['520*MeV','2.0*GeV'],'PHI':'15*MeV','OMEGA':'30*MeV'},
            'DAUGHTERS'        : {'PT_MIN':'500*MeV','P_MIN':'3000*MeV'},
            'piLAMBDADAUGHTERS':
            {'PT_MIN':'300*MeV','P_MIN':'3000*MeV','PIDK_MAX':'2'},   
            'pLAMBDADAUGHTERS' :
            {'PT_MIN':'1200*MeV','P_MIN':'10000*MeV','PIDp_MIN':'10','PIDpK_MIN':'0'},
            'kLAMBDADAUGHTERS' :
            {'PT_MIN':'500*MeV','P_MIN':'3000*MeV','PIDK_MIN':'5','PIDKp_MIN':'0'},
            'VCHI2DOF_MAX'     : 9,
            'BPVVDCHI2_MIN'    : 81,
            'ASUMPT_MIN'       : '1500*MeV',
            },
    "HHH": { 'MASS_WINDOW'   : {'OMEGA': '2.5*GeV', 'K1':
                                ['0.8*GeV', '3.5*GeV']},
             'DAUGHTERS'     : {'PT_MIN':'350*MeV','P_MIN':'2000*MeV'},
             'AMAXDOCA_MAX'  : '0.50*mm',
             'VCHI2DOF_MAX'  : 9,
             'BPVVDCHI2_MIN' : 64,
             'ASUMPT_MIN'    : '1600*MeV',
             'PT_MIN'        : '500*MeV',
             'MIPCHI2DV_MIN' : 0.0,
             },
    "PID" : { 'P'  : {'PIDp_MIN' : 0},
              'PI' : {'PIDK_MAX' : 20},
              'K'  : {'PIDK_MIN' : -10} },
    "Prescales"  : {},
    "GECNTrkMax" : 500
    },
    'WGs'       : [ 'RD' ],
    'STREAMS'   : {
    'Radiative' : [
    'StrippingB2PhiGammaConv_B2VG_Line',
    'StrippingB2RhoGamma_B2VG_Line',
    'StrippingB2KstarGammaConv_B2VG_Line',
    'StrippingLb2LGammaLb2LGammaLine',
    'StrippingB2JpsiKGamma_B2VPG_Line'
    ]
    }
    }

Bd2eeKstarBDT = {
    'BUILDERTYPE'             : 'Bd2eeKstarBDTConf',
    'CONFIG'                  : {
        'LinePrescale'            :    1.   ,
        'LinePostscale'           :    1.   ,
        #
        'ElectronPT'              :  200.   ,  # MeV
        'ElectronTrackCHI2pNDOF'  :    5.   ,
        'ElectronGhostProb'       :    0.5  ,
        'ElectronIPCHI2'          :    1.   ,
        'ElectronPIDepi'          :   -2.   ,          
        #
        'eeCuts'                  : "(VFASPF(VCHI2)<16) & ((MM<1.2*GeV) | ((MM>2.2*GeV) & (MM<4.2*GeV)))",
        #
        'KaonPT'                  :  400.   ,  # MeV 
        'KaonP'                   : 3000.   ,  # MeV  
        'KaonTrackCHI2pNDOF'      :    5.   ,
        'KaonGhostProb'           :    0.35 ,
        'KaonIPCHI2'              :    4.   , 
        'KaonPIDKpi'              :   -5.   , 
        #
        'PionPT'                  :  250.   ,  # MeV
        'PionP'                   : 2000.   ,  # MeV 
        'PionTrackCHI2pNDOF'      :    5.   ,
        'PionGhostProb'           :    0.35 ,
        'PionIPCHI2'              :    4.   , 
        'PionPIDpiK'              :   10.   ,  # PIDpi-PIDK > -5, i.e., PIDK<5 
        #
        'KstarVertexCHI2'         :   16.   , 
        'KstarMassW'              :  150.   ,  # MeV
        #
        'BComMassW'               : 1200.   ,  # MeV
        'BVertexCHI2'             :   16.   ,  # /ndf
        'BMassW'                  : 1000.   ,  # MeV  
        'BDIRA'                   :    0.999,
        'BDTCutValue'             :   -0.95 ,
        'BDTWeightsFile'          : '$TMVAWEIGHTSROOT/data/Bd2eeKstar_BDTG_v1r0.xml'
        },
    'WGs'    : [ 'RD' ],
    'STREAMS' : {
    'Radiative' :  [ 'StrippingBd2eeKstarBDTLine2' ]
    }
    }




####################################################################
#
# Lines for very rare decay sub-WG
#
####################################################################

BLVLines = {
    'BUILDERTYPE' : 'BLVLinesConf',
    'WGs'         : [ 'RD' ],
    'STREAMS'     : [ 'Dimuon' ],
    'CONFIG'      : {
    'Postscale'           :1,
    'La2KmuPrescale'      :1,
    'Lb2KmuPrescale'      :1,
    'Lb2DmuPrescale'      :1,
    'Lb2DsmuPrescale'     :1,
    'B2LcmuPrescale'      :1,
    'Lb2LcpiPrescale'     :0.1,
    'B2DpiPrescale'       :0.1,
    'Bs2DspiPrescale'     :0.1,
    }
    }

Lc23MuLines = {
    'BUILDERTYPE' : 'Lc23MuLinesConf',
    'WGs'         : [ 'RD' ],
    'STREAMS'     : [ 'Dimuon' ],
    'CONFIG'      : {
    'Postscale'           :1,
    'Lc23muPrescale'      :1,
    'Lc2mueePrescale'     :1,
    'Lc2pKpiPrescale'     :0.01,
    }
    }

Inflaton2MuMu = {
    'BUILDERTYPE' : 'StrippingInflaton2MuMuConf' ,
    'STREAMS' : {
    'Dimuon'  : [
    'StrippingInflaton2MuMu_Bu2InflatonKDnLine',
    'StrippingInflaton2MuMu_Bs2InflatonPhiDnLine',
    'StrippingInflaton2MuMu_Bd2InflatonKstDnLine',
    'StrippingInflaton2MuMu_Bd2InflatonRhoDnLine'
    ] 
    },
    'WGs'     : [ 'RD' ] ,
    'CONFIG'  : {
    'Inflaton2MuMuLongPrescale'    : 0,
    'Inflaton2MuMuDownstreamPrescale'    : 0,
    'Inflaton2MuMuLongPostscale'    : 0,
    'Inflaton2MuMuDownstreamPostscale'    : 0,
    'Bu2InflatonKPrescale' : 1, 
    'Bu2InflatonKPostscale' : 1, 
    'Bs2InflatonPhiPrescale' : 1, 
    'Bs2InflatonPhiPostscale' : 1, 
    'Bd2InflatonKstPrescale' : 1, 
    'Bd2InflatonKstPostscale' : 1,
    'Bd2InflatonRhoPrescale' : 1, 
    'Bd2InflatonRhoPostscale' : 1
    }
    }

# J. Albrecht, K. Petridis + M. de Cian 
# Lines for micro-DST
Inflaton2MuMuForMicro = {
    'BUILDERTYPE' : 'StrippingInflaton2MuMuConf' ,
    'STREAMS' : {
    'Leptonic' : [ 'StrippingInflaton2MuMuForMicroLongLine',
                   'StrippingInflaton2MuMuForMicroDownstreamLine' ]
    },
    'WGs'     : [ 'RD' ] ,
    'CONFIG'  : {
    'Inflaton2MuMuLongPrescale'    : 1,
    'Inflaton2MuMuDownstreamPrescale'    : 1,
    'Inflaton2MuMuLongPostscale'    : 1,
    'Inflaton2MuMuDownstreamPostscale'    : 1,
    'Bu2InflatonKPrescale' : 0, 
    'Bu2InflatonKPostscale' : 0, 
    'Bs2InflatonPhiPrescale' : 0, 
    'Bs2InflatonPhiPostscale' : 0, 
    'Bd2InflatonKstPrescale' : 0, 
    'Bd2InflatonKstPostscale' : 0,
    'Bd2InflatonRhoPrescale' : 0, 
    'Bd2InflatonRhoPostscale' : 0
    }
    }


Bu2MuNu = {
    'BUILDERTYPE' : 'Bu2MuNuConf',
    'WGs'         : [ 'RD' ],
    'STREAMS'     : [ 'Semileptonic' ],
    'CONFIG'      : {
    'Prescale'  : 0.02,
    'Postscale' : 1.0,
    'pTmin'     : 5.,
    'pTmax'     : 40.,
    'pmax'      : 500.,
    'IPchi2'    : 400.,
    'TrChi2'    : 3
    }
    }



####################################################################
#
# Lines for rare EW penguin sub-group 
#
####################################################################

