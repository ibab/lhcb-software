#
# Rare Decay Lines


####################################################################
#
# Lines for radiative decay sub-WG
#
####################################################################

Beauty2XGamma = {
    'BUILDERTYPE' : 'Beauty2XGamma',
    'CONFIG'      : {
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
              'BPVDIRA_MIN'   : -1.1,   ## was 0.998 (not sure)
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
            'BPVVDCHI2_MIN'    : -1,   ## WAS 81
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
    'WGs'    : ['RD'],
    'STREAMS' : { 'Leptonic' : [ 'StrippingB2PhiGamma_B2VG_Line', 'StrippingB2KstarGamma_B2VG_Line' ] } 
    }

####################################################################
#
# Lines for very rare decay sub-WG
#
####################################################################

RareStrange = {
    'BUILDERTYPE'  : 'RareStrangeLinesConf',
    'CONFIG'       : {
    'Postscale'           :1,
    'SigmaPMuMuPrescale'      :1,
    'SigmaPEEPrescale'      :1,
    'SigmaPMuMuLFVPrescale'      :1,
    'K3PiPrescale' : 0,
    'SigmaMinTauPs' : 6,
    'SigmaMinPt' : 500.,
    'SigmaMaxDOCA' : 2.,
    'SigmaMassWin' : 500.,
    'SigmaMinDIRA' : 0.9,
    'SigmaMaxIpChi2' : 36.,
    'SigmaVtxChi2' : 36,
    'muonMinIpChi2' : 4.,
    'protonPIDp': 5.,
    'electronPIDe':2.,
    'electronMinIpChi2': 9
    },
    'WGs' : [ 'RD' ],
    'STREAMS' : [ 'Dimuon' ]
    }


# 2012 control lines only

B2XTau =  {
    'BUILDERTYPE' : 'B2XTauConf',
    'WGs' : [ 'RD' ],
    'STREAMS' : {
    'BhadronCompleteEvent' : ['StrippingB2XTau_DPi_TOSLine' ,
                              'StrippingB2XTau_DPi_TISLine' ,
                              'StrippingB2XTau_DPi_SameSign_TOSLine',
                              'StrippingB2XTau_DPi_SameSign_TISLine' ]
    },
    'CONFIG'  : { 
    'PT_HAD_ALL_FINAL_STATE'        : '200',  # MeV
    'PTMAX_HAD_ALL_FINAL_STATE'     : '500',  # MeV
    'P_HAD_ALL_FINAL_STATE'         : '2000', # MeV
    'IPCHI2_HAD_ALL_FINAL_STATE'    : '9',    # dimensionless
    'TRACKCHI2_HAD_ALL_FINAL_STATE' : '4',    # dimensionless
    'PID_HAD_ALL_FINAL_STATE'       : '5',    # dimensionless
    'TRGHOPROB_HAD_ALL_FINAL_STATE' : '0.35', # dimensionless
    'PT_MU'                         : '1200', # MeV
    'P_MU'                          : '6000', # MeV 
    'IPCHI2_MU'                     : '16',   # MeV
    'TRACKCHI2_MU'                  : '4',    # dimensionless
    'TRGHOPROB_MU'                  : '0.35', # dimensionless
    'PT_B_TT'                       : '5000', # MeV
    'PT_B_TT_HIGH'                  : '10000',# MeV 
    'PT_B_TM'                       : '2000', # MeV
    'PT_B_TM_HIGH'                  : '7500', # MeV  
    'VCHI2_B'                       : '100',  # dimensionless
    'FDCHI2_B'                      : '144',  # dimensionless
    'DIRA_B'                        : '0.99', # dimensionless
    'MASS_LOW_B'                    : '2000', # MeV  
    'MASS_HIGH_B'                   : '7000', # MeV (old value: 5750)
    'MCOR_LOW_B'                    : '4000', # MeV
    'MCOR_HIGH_B'                   :'10000', # MeV (old value: 7000)
    'MCOR_LOW_SB'                   : '2000', # MeV
    'MCOR_HIGH_SB'                  : '4000', # MeV 
    'MIPCHI2_B'                     : '150',  # dimensionless 
    'MIPCHI2_B_HIGH'                : '36',   # dimensionless  
    'PT_TAU'                        : '1250', # MeV
    'VCHI2_TAU'                     : '12',   # dimensionless
    'IPCHI2_TAU'                    : '9',    # dimensionless
    'FDCHI2_TAU'                    : '16',   # dimensionless
    'FDRHO_TAU'                     : '0.1',  # mm
    'FDZ_TAU'                       : '2.0',  # mm
    'DOCA_TAU'                      : '0.4',  # mm
    'DIRA_TAU'                      : '0.98', # dimensionless
    'MASS_LOW_TAU'                  : '500',  # MeV
    'MASS_HIGH_TAU'                 : '1800', # MeV
    'PT_DIPI'                       : '200',  # MeV
    'VCHI2_DIPI'                    : '20',   # dimensionless
    'IPCHI2_DIPI'                   : '1',    # dimensionless
    'FDCHI2_DIPI'                   : '9',    # dimensionless
    'FDRHO_DIPI'                    : '0.05', # mm
    'FDZ_DIPI'                      : '1.0',  # mm
    'DOCA_DIPI'                     : '0.4',  # mm
    'DIRA_DIPI'                     : '0.9',  # dimensionless
    'PT_B_CHILD_BEST'               : '1800', # MeV
    'P_B_CHILD_BEST'                : '10000',# MeV
    'IPCHI2_B_CHILD_BEST'           : '16',   # dimensionless
    'PT_B_TAU_CHILD_BEST'           : '3000', # MeV
    'IPCHI2_B_TAU_CHILD_BEST'       : '16',   # dimensionless
    'MASS_LOW_D'                    : '1800', # MeV
    'MASS_HIGH_D'                   : '2030', # MeV  
    'B2TauTau_TOSLinePrescale'          : 1,
    'B2TauTau_TOSLinePostscale'         : 1,
    'B2TauTau_SB_TOSLinePrescale'       : 0.05,
    'B2TauTau_SB_TOSLinePostscale'      : 1,
    'B2DD_TOSLinePrescale'              : 1,
    'B2DD_TOSLinePostscale'             : 1,
    'B2TauMu_TOSLinePrescale'           : 1,
    'B2TauMu_TOSLinePostscale'          : 1,
    'B2TauMu_SB_TOSLinePrescale'        : 0.05,
    'B2TauMu_SB_TOSLinePostscale'       : 1,
    'B2DMu_TOSLinePrescale'             : 0.2,
    'B2DMu_TOSLinePostscale'            : 1,
    'B2DPi_TOSLinePrescale'             : 0.1,
    'B2DPi_TOSLinePostscale'            : 1,
    'B2TauTau_TISLinePrescale'          : 1,
    'B2TauTau_TISLinePostscale'         : 1,
    'B2DD_TISLinePrescale'              : 1,
    'B2DD_TISLinePostscale'             : 1,
    'B2TauMu_TISLinePrescale'           : 1,
    'B2TauMu_TISLinePostscale'          : 1,
    'B2DMu_TISLinePrescale'             : 0.2,
    'B2DMu_TISLinePostscale'            : 1.,
    'B2DPi_TISLinePrescale'             : 0.1,
    'B2DPi_TISLinePostscale'            : 1.
    }
    }

Inflaton2MuMu = {
    'BUILDERTYPE' : 'StrippingInflaton2MuMuConf' ,
    'STREAMS' : [ 'Dimuon' ],
    'WGs'     : [ 'RD' ] ,
    'CONFIG'  : {
    'Inflaton2MuMuLongPrescale'    : 0,
    'Inflaton2MuMuDownstreamPrescale'    : 0,
    'Inflaton2MuMuLongPostscale'    : 0,
    'Inflaton2MuMuDownstreamPostscale'    : 0,
    'Bu2InflatonKPrescale'    : 0, 
    'Bu2InflatonKPostscale'   : 0, 
    'Bs2InflatonPhiPrescale'  : 0, 
    'Bs2InflatonPhiPostscale' : 0, 
    'Bd2InflatonKstPrescale'  : 1, 
    'Bd2InflatonKstPostscale' : 1,
    'Bd2InflatonRhoPrescale'  : 0, 
    'Bd2InflatonRhoPostscale' : 0
    }
    }


####################################################################
#
# Lines for rare EW penguin sub-group 
#
####################################################################


Bu2LLK = {
    'BUILDERTYPE'         : 'Bu2LLKConf',
    'CONFIG'              : {
    'BFlightCHI2'         : 100        
    ,  'BDIRA'            : 0.9995     
    ,  'BIPCHI2'             : 25        
    ,  'BVertexCHI2'         : 9        
    ,  'DiLeptonPT'          : 0         # MeV (not used)
    ,  'DiLeptonFDCHI2'      : 16       
    ,  'DiLeptonIPCHI2'      : 0         
    ,  'LeptonIPCHI2'        : 16        
    ,  'LeptonPT'            : 500       # MeV
    ,  'KaonIPCHI2'          : 9        
    ,  'KaonPT'              : 800       
    ,  'UpperMass'           : 5500      # MeV
    ,  'Bu2eeKLinePrescale'  : 1
    ,  'Bu2mmKLinePrescale'  : 1
    ,  'Bu2meKLinePrescale'  : 1
     },
    'WGs'    : [ 'RD' ] ,
    'STREAMS' : ['Leptonic']
    }



B2XMuMu =  {
    'BUILDERTYPE' : 'B2XMuMuConf',
    'CONFIG' : {
      'KpiVXCHI2NDOF'	   : 9.0           # dimensionless
    , 'MuonPID'            : -3.0           # dimensionless
    , 'DimuonUPPERMASS'    : 7100.0        # MeV
    , 'Pi0MINPT'           : 800.0         # MeV
    , 'DplusLOWERMASS'     : 1600.0        # MeV
    , 'DplusUPPERMASS'     : 2300.0        # MeV
    , 'KstarplusWINDOW'    : 300.0         # MeV
    , 'KsWINDOW'           : 30.0          # MeV
    , 'LambdaWINDOW'	   : 30.0          # MeV
    , 'LongLivedPT'        : 0.0          # MeV , used to be 500.0 MeV
    , 'LongLivedTau'        : 2          # ps

    # A1 cuts
    , 'A1_Comb_MassLow'  :    0.0
    , 'A1_Comb_MassHigh' : 6050.0
    , 'A1_MassLow'       :    0.0
    , 'A1_MassHigh'      : 6000.0
    , 'A1_MinIPCHI2'     :    4.0
    , 'A1_FlightChi2'    :   25.0
    , 'A1_Dau_MaxIPCHI2' :    9.0

    ,'UseNoPIDsHadrons'          : True,

    # B cuts
    'B_Comb_MassLow'	  : 4500.0,
    'B_Comb_MassHigh'     : 7100.0,
    'B_MassLow'           : 4600.0,
    'B_MassHigh'          : 7000.0,
    'B_VertexCHI2'        :    8.0,
    'B_IPCHI2'            :   16.0,
    'B_DIRA'              :    0.9999,
    'B_FlightCHI2'        :  121.0,
    'B_Dau_MaxIPCHI2'     : 9.0,

    # Daughter cuts
    'Dau_VertexCHI2'	  :   12.0,
    'Dau_DIRA'            :   -0.9,

    # Kstar cuts
    'Kstar_Comb_MassLow'  :  0.0,
    'Kstar_Comb_MassHigh' : 6200.0,
    'Kstar_MassLow'	  :  0.0,
    'Kstar_MassHigh'	  : 6200.0,
    'Kstar_MinIPCHI2'     :    0.0,
    'Kstar_FlightChi2'    :    9.0,
    'Kstar_Dau_MaxIPCHI2' : 9.0,

    # JPsi (dimu) cuts
    'Dimu_FlightChi2'     :   9.0,
    'Dimu_Dau_MaxIPCHI2'  :   9.0,

    # Track cuts
    'Track_CHI2nDOF'	  :    5.0,
    'Track_GhostProb'     :    0.4,

    # Hadron cuts
    'Hadron_MinIPCHI2'    :    9.0,

    # Muon cuts
    'Muon_MinIPCHI2'	  :    9.0,
    'Muon_IsMuon'         :    True,
    'MuonNoPIDs_PIDmu'    :    0.0,

    # Wrong sign combinations
    'DimuonWS'            :   True,
    'HadronWS'            :   False,

    # GEC
    'SpdMult'             :  600 ,
      
    'Hadronic_Selection'  :   "(M > 5000*MeV)&(M < 5700*MeV)&(INTREE((ABSID == 'K*(892)0')&(M>800*MeV)&(M < 1000*MeV)&(INTREE((ABSID=='K+')&(HASRICH)&(PIDK>-5)))))&((INTREE((ABSID=='rho(770)0')&((M<2800)|(M>3800))&(2 == NINTREE((ABSID=='pi+')&(HASRICH)&(PIDK < 5)))))|(INTREE((ABSID=='phi(1020)')&(M<1200*MeV))))" , 
      
    'DECAYS'              : [ "[B+ -> J/psi(1S) rho(770)+]cc" ],
    'HADRONICDECAYS'      : [ "[B0 -> K*(892)0 rho(770)0]cc" , "[B0 -> K*(892)0 phi(1020)]cc" ]
    },
    'WGs' : [ 'RD' ],
    'STREAMS' : [ 'Dimuon' ]
    }
