Bd2KstarMuMu = {
    'BUILDERTYPE' : 'StrippingBdToKstarMuMuConf',
    'CONFIG' : { 
    'UseNoPIDsHadrons'          : True,
    'Prescale_BdToKstarMuMu'    : 1.0,
    'Postscale_BdToKstarMuMu'   : 1.0,
    'Prescale_BdToKstarMuMuSS'  : 1.0,
    'Postscale_BdToKstarMuMuSS' : 1.0,
    'Prescale_BuToKMuMu'        : 1.0,
    'Postscale_BuToKMuMu'       : 1.0,
    'Prescale_BuToKMuMuSS'      : 1.0,
    'Postscale_BuToKMuMuSS'     : 1.0,
    'B_Comb_MassLow'      : 4600.0,
    'B_Comb_MassHigh'     : 6000.0,
    'B_MassLow'           : 4850.0,
    'B_MassHigh'          : 5780.0,
    'B_VertexCHI2'        :    6.0,
    'B_IPCHI2'            :   16.0,
    'B_DIRA'              :   0.9999,
    'B_FlightCHI2'        :  121.0,
    'B_Dau_MaxIPCHI2'     : 9.0, 
    'Dau_VertexCHI2'      :   12.0,
    'Dau_DIRA'            :   -0.9,
    'Kstar_Comb_MassLow'  :  550.0,
    'Kstar_Comb_MassHigh' : 2200.0,
    'Kstar_MassLow'       :  600.0,
    'Kstar_MassHigh'      : 2000.0,
    'Kstar_MinIPCHI2'     :    0.0,
    'Kstar_FlightChi2'    :   9.0, 
    'Kstar_Dau_MaxIPCHI2' : 9.0, 
    'Dimu_FlightChi2'     :   9.0, 
    'Dimu_Dau_MaxIPCHI2'  : 9.0, 
    'Track_CHI2nDOF'      :    5.0,
    'Hadron_MinIPCHI2'    :    9.0, 
    'Muon_MinIPCHI2'      :    9.0,
    'Muon_IsMuon'         :   False,
    'MuonNoPIDs_PIDmu'    :    0.0
    },
    'WGs' : [ 'RD' ] ,
    'STREAMS' : [ 'Dimuon' ]
    }


B2XMuMu =  {
    'BUILDERTYPE' : 'B2XMuMuConf',
    'CONFIG' : {
    'BVXCHI2NDOF'          : 8             # dimensionless
    , 'BIPCHI2'            : 9.0           # dimensionless
    , 'BDIRA'              : 0.999968      # dimensionless
    , 'BFDCHI2'            : 100.0         # dimensionless
    , 'KpiMINIPCHI2'       : 9.0           # dimensionless
    , 'KpiTRACKCHI2'       : 4.0           # dimensionless    
    , 'KpiVXCHI2NDOF'      : 9.0           # dimensionless
    , 'MuonMINIPCHI2'      : 16.0          # dimensionless
    , 'MuonTRACKCHI2'      : 4.0           # dimensionless
    , 'MuonPID'            : 0.0           # dimensionless
    , 'DimuonVXCHI2NDOF'   : 9.0           # dimensionless
    , 'DimuonUPPERMASS'    : 5050.0        # MeV
    , 'Pi0MINPT'           : 800.0         # MeV
    , 'DplusLOWERMASS'     : 1600.0        # MeV
    , 'DplusUPPERMASS'     : 2300.0        # MeV      
    , 'KstarplusWINDOW'    : 300.0         # MeV      
    , 'KsWINDOW'           : 30.0          # MeV     
    , 'LambdaWINDOW'       : 30.0          # MeV    
    , 'LongLivedPT'        : 250.0         # MeV  
    , 'LongLivedTau'        : 2            # ps 
    , 'K1_Comb_MassLow'  :  720.0
    , 'K1_Comb_MassHigh' : 2450.0
    , 'K1_MassLow'       :  750.0
    , 'K1_MassHigh'      : 2400.0
    , 'K1_MinIPCHI2'     :    4.0
    , 'K1_FlightChi2'    :   25.0
    , 'K1_Dau_MaxIPCHI2' : 9.0,
    },
    'WGs' : [ 'RD' ],
    'STREAMS' : [ 'Dimuon' ]
    }


Bs2MuMuLines = {
    'BUILDERTYPE' : 'Bs2MuMuLinesConf',
    'CONFIG'      : {
    'DefaultLinePrescale'    : 1,
    'DefaultLinePostscale'   : 1,
    'Bs2mmWideLinePrescale'  : 1,
    'Bs2mmWideLinePostscale'  : 1,
    'LooseLinePrescale'      : 0.02,
    'LooseLinePostscale'     : 1,
    'BuPrescale'    : 1,
    'BuPostscale'   : 1,
    'BsPrescale'    : 1,
    'BsPostscale'   : 1,
    'BdPrescale'    : 1,
    'BdPostscale'   : 1,
    'JPsiLinePrescale'       : 1,
    'JPsiLinePostscale'      : 1,
    'JPsiLooseLinePrescale'  : 0.1,
    'JPsiLooseLinePostscale' : 1,
    'JPsiPromptLinePrescale' : 0.005,
    'JPsiPromptLinePostscale': 1,
    'MuIPChi2_loose'        :  9,
    'MuTrChi2_loose'        : 10,
    'BIPChi2_loose'         : 64,
    'BFDChi2_loose'         : 100
    },
    'WGs'    : [ 'RD' ],
    'STREAMS' : [ 'Dimuon' ]
    }


LFVLines = {
    'BUILDERTYPE' : 'LFVLinesConf' ,
    'STREAMS' : [ 'Dimuon' ],
    'WGs'     : [ 'RD' ],
    'CONFIG'  : {
    'Postscale'           : 1,
    'TauPrescale'         : 1,
    'Tau2MuMuePrescale'   : 1,
    'B2eMuPrescale'       : 1,
    'B2eePrescale'        : 1,
    'B2heMuPrescale'      : 1 
    }
    }


Tau23Mu = {
    'BUILDERTYPE' : 'Tau23MuLinesConf',
    'STREAMS'     : ['Dimuon'],
    'WGs'         : ['RD'],
    'CONFIG'      : {
    'TauPrescale'         :1,
    'TauPostscale'        :1,
    'Ds23PiTISPrescale'   :0.01,
    'Ds23PiTISPostscale'  :1,
    'Ds23PiPrescale'      :0.005,
    'Ds23PiPostscale'     :1,
    'Ds2PhiPiPrescale'    :1,
    'Ds2PhiPiPostscale'   :1,        
    'Tau25Prescale'       :1,
    'Tau25Postscale'      :1      
    }
    }


Inflaton2MuMu = {
    'BUILDERTYPE' : 'StrippingInflaton2MuMuConf' ,
    'STREAMS' : [ 'Dimuon' ],
    'WGs'     : [ 'RD' ] ,
    'CONFIG'  : {
        'Inflaton2MuMuLongPrescale'    : 1,
        'Inflaton2MuMuDownstreamPrescale'    : 1,
        'Inflaton2MuMuLongPostscale'    : 1,
        'Inflaton2MuMuDownstreamPostscale'    : 1,
        'Bu2InflatonKPrescale' : 1,
        'Bu2InflatonKPostscale' : 1,
        'Bs2InflatonPhiPrescale' : 1,
        'Bs2InflatonPhiPostscale' : 1,
        'Bd2InflatonKstPrescale' : 1,
        'Bd2InflatonKstPostscale' : 1
        }
    }


BuToK1MuMu = {
    'BUILDERTYPE' : 'StrippingBuToK1MuMuConf' ,
    'STREAMS'     : [ 'Dimuon' ],
    'WGs'         : [ 'RD' ],
    'CONFIG'      : {
    'UseNoPIDsHadrons'       : True,
    'Prescale_BuToK1MuMu'    : 1.0,
    'Postscale_BuToK1MuMu'   : 1.0,
    'Prescale_BuToK1MuMuSS'  : 1.0,
    'Postscale_BuToK1MuMuSS' : 1.0,
    'B_Comb_MassLow'      : 4600.0,
    'B_Comb_MassHigh'     : 6000.0,
    'B_MassLow'           : 4850.0,
    'B_MassHigh'          : 5780.0,
    'B_VertexCHI2'        :    6.0,
    'B_IPCHI2'            :   16.0,
    'B_DIRA'              :    0.014,
    'B_FlightCHI2'        :  121.0,
    'B_Dau_MaxIPCHI2'     : 9.0,
    'Dau_VertexCHI2'      :   12.0,
    'Dau_DIRA'            :   -0.9,
    'K1_Comb_MassLow'     :  720.0,
    'K1_Comb_MassHigh'    : 2450.0,
    'K1_MassLow'          :  750.0,
    'K1_MassHigh'         : 2400.0,
    'K1_MinIPCHI2'        :    4.0,
    'K1_FlightChi2'       :   25.0,
    'K1_Dau_MaxIPCHI2'    : 9.0,
    'Dimu_FlightChi2'     :   81.0,
    'Dimu_Dau_MaxIPCHI2'  : 9.0,
    'Track_CHI2nDOF'      :    5.0,
    'Hadron_MinIPCHI2'    :    9.0,
    'Muon_MinIPCHI2'      :    9.0,
    'Muon_IsMuon'         :   False,
    'MuonNoPIDs_PIDmu'    :    0.0
    }
    }


Z02TauTauProng = {
    'BUILDERTYPE'  : 'Z02TauTauProngConf',
    'CONFIG'       : {
                 'Z2TauTau_Prong_LinePrescale'  : 1.0,
                 'Z2TauTau_Prong_LinePostscale' : 1.0,
                 'TAU_MASS_LOW'                 : '600.0',    # MeV/c2
                 'TAU_MASS_HIGH'                : '1600.0',   # MeV/c2
                 'Z_MASS_LOW'                   : '45000.0',  # MeV/c2
                 'PT_HAD_MIN'                   : '1000',     # MeV/c
                 'TRACKCHI2_HAD_MAX'            : '10',       # dl
                 'PT_TAU_MIN'                   : '8000',     # MeV/c
                 'VCHI2_TAU_MAX'                : '20'        # dl
                 },
    'WGs' : [ 'RD' ],
    'STREAMS' : [ 'EW' ]
    }

B2MuMuX = {
    'BUILDERTYPE' : 'B2MuMuXConf' ,
    'STREAMS' : [ 'Dimuon' ],
    'WGs'     : [ 'RD' ] ,
    'CONFIG'  : {
    'MuonsLoose': {
        'ISMUON' : True,
        'TRCHI2DOF_MAX' : 5,
        'MIPCHI2DV_MIN' : 4,
        'InAccMuon'  : True
        },
    'MuonsNoPid': {
        'ISMUON' : False,
        'TRCHI2DOF_MAX' : 5,
        'MIPCHI2DV_MIN' : 4,
        'InAccMuon'  : True
        },
    'DiMuons': {
        'AMAXDOCA_MAX'  : '0.5*mm',
        'ASUMPT_MIN'    : '1000*MeV',
        'VCHI2DOF_MAX'  : 16,
        'BPVVDCHI2_MIN' : 16,
        'BPVDIRA_MIN'   : 0.0
        },
    'DiMuonsLowM': {
        'AMAXDOCA_MAX'  : '0.5*mm',
        'ASUMPT_MIN'    : '1000*MeV',
        'VCHI2DOF_MAX'  : 16,
        'BPVVDCHI2_MIN' : 16,
        'BPVDIRA_MIN'   : 0.0
        },
    'DiMuonsHighM': {
        'AMAXDOCA_MAX'  : '0.5*mm',
        'ASUMPT_MIN'    : '1000*MeV',
        'VCHI2DOF_MAX'  : 16,
        'BPVVDCHI2_MIN' : 0,
        'BPVDIRA_MIN'   : 0
        },
    'DiMuonsCorrM': {
        'AMAXDOCA_MAX'  : '0.3*mm',
        'ASUMPT_MIN'    : '2000*MeV',
        'VCHI2DOF_MAX'  : 8,
        'BPVVDCHI2_MIN' : 225,
        'BPVIPCHI2_MAX' : 25,
        'BPVDIRA_MIN'   : 0.4,
        'CORRM_MIN'     : '4600*MeV',
        'CORRM_MAX'     : '6000*MeV',
        'PTMU'          : '700*MeV'
        },
    'Photons': {
        'PT_MIN' : '1300*MeV'
        },    
    "V0s": { # Cuts for rho, K*, phi
        'MASS_MIN'      : {'KST':'700*MeV','RHO':'600*MeV','PHI':'900*MeV'},
        'MASS_MAX'      : {'KST':'1100*MeV','RHO':'1000*MeV','PHI':'1100*MeV'},
        'DAUGHTERS'     : {'PT_MIN':'100*MeV','P_MIN':'2000*MeV',
                           'MIPCHI2DV_MIN' : 4, 'TRCHI2DOF_MAX' : 4},
        'AMAXDOCA_MAX'  : '0.5*mm',
        'VCHI2DOF_MAX'  : 16,
        'BPVVDCHI2_MIN' : 16, 
        'BPVDIRA_MIN'   : 0,
        'ASUMPT_MIN'    : '1000*MeV'
        },    
    "B2X3BODY" : { 
        'SUMPT_MIN'     : '4800*MeV',
        'VCHI2DOF_MAX'  : 10,
        'BPVIPCHI2_MAX' : 25,
        'BPVVDCHI2_MIN' : 25,
        'BPVDIRA_MIN'   : 0.4, #0.0
        'MASS_MIN'      : {'B':'4300*MeV'},
        'MASS_MAX'      : {'B':'6400*MeV'},
        },
    "B2X3BODYLOWM" : { 
        'SUMPT_MIN'     : '3900*MeV',
        'VCHI2DOF_MAX'  : 10,
        'BPVIPCHI2_MAX' : 25,
        'BPVVDCHI2_MIN' : 25,
        'BPVDIRA_MIN'   : 0.4, #0.0
        'MASS_MIN'      : {'J':'2600*MeV'},
        'MASS_MAX'      : {'J':'3600*MeV'},
        },
    "B2X3BODYHIGHM" : { 
        'SUMPT_MIN'     : '4800*MeV',
        'VCHI2DOF_MAX'  : 10,
        'BPVDIRA_MIN'   : 0.0,
        'MASS_MIN'      : {'Y':'9000*MeV'},
        'MASS_MAX'      : {'Y':'10000*MeV'},
        },
    "B2X4BODY" : { 
        'SUMPT_MIN'     : '5000*MeV',
        'VCHI2DOF_MAX'  : 10,
        'BPVIPCHI2_MAX' : 25,
        'BPVVDCHI2_MIN' : 25,
        'BPVDIRA_MIN'   : 0.0,
        'MASS_MIN'      : {'B':'4300*MeV'},
        'MASS_MAX'      : {'B':'6300*MeV'}
        },
    "Prescales" : {
        'OS'            : 1.0,
        'SS'            : 0.5
        },
    'GECNTrkMax'   : 500 }
    }

    
Bd2eeKstar = {
    'BUILDERTYPE'             : 'Bd2eeKstarConf',
    'CONFIG'                  : {
        'LinePrescale'            :    1.   ,
        'LinePostscale'           :    1.   ,
        #
        'ElectronPT'              :  300.   ,  # MeV
        'ElectronTrackCHI2pNDOF'  :    5.   ,
        'ElectronIPCHI2'          :    2.25 ,
        'ElectronPIDepi'          :   -2.   ,          
        #
        'eeVertexCHI2'            :   16.   ,  
        'eeMinMass'               :   20.   ,  # MeV 
        'eeMaxMass'               : 1500.   ,  # MeV
        'eeFD'                    :    1.   ,  # mm
        #
        'KaonPT'                  :  400.   ,  # MeV 
        'KaonP'                   : 3000.   ,  # MeV  
        'KaonTrackCHI2pNDOF'      :    5.   , 
        'KaonIPCHI2'              :    4.   , 
        'KaonPIDKpi'              :   -5.   , 
        #
        'PionPT'                  :  300.   ,  # MeV
        'PionP'                   : 3000.   ,  # MeV 
        'PionTrackCHI2pNDOF'      :    5.   , 
        'PionIPCHI2'              :    4.   , 
        'PionPIDpiK'              :   10.   ,  # PIDpi-PIDK > -5, i.e., PIDK<5 
        #
        'KstarVertexCHI2'         :   16.   , 
        'KstarMassW'              :  130.   ,  # MeV
        'KstarIPCHI2'             :    1.   , 
        'KstarFDCHI2'             :    1.   , 
        #
        'BComMassW'               : 1200.   ,  # MeV
        'BVertexCHI2'             :    9.   ,  # /ndf
        'BMassW'                  : 1000.   ,  # MeV  
        'BIPCHI2'                 :   64.   ,  # pointing
        'BFDCHI2'                 :    9.   , 
        'BDIRA'                   :    0.999, 
        'SumIPSCut'               : " & (SUMTREE(((ABSID=='K+') | (ABSID=='pi-') | (ID=='e+') | (ID=='e-')),sqrt(BPVIPCHI2()))>15)" 
           },
    'WGs'    : [ 'RD' ],
    'STREAMS' : [ 'Radiative' ]
    }

Bd2JpsieeKstar = {
    'BUILDERTYPE'             : 'Bd2eeKstarConf',
    'CONFIG'                  : {
        'LinePrescale'            :    1.   ,
        'LinePostscale'           :    1.   ,
        #
        'ElectronPT'              :  300.   ,  # MeV
        'ElectronTrackCHI2pNDOF'  :    5.   ,
        'ElectronIPCHI2'          :    2.25 ,
        'ElectronPIDepi'          :   -2.   ,          
        #
        'eeVertexCHI2'            :   16.   ,  
        'eeMinMass'               : 2200.   ,  # MeV 
        'eeMaxMass'               : 4200.   ,  # MeV
        'eeFD'                    :    1.   ,  # mm
        #
        'KaonPT'                  :  400.   ,  # MeV 
        'KaonP'                   : 3000.   ,  # MeV  
        'KaonTrackCHI2pNDOF'      :    5.   , 
        'KaonIPCHI2'              :    4.   , 
        'KaonPIDKpi'              :   -5.   , 
        #
        'PionPT'                  :  300.   ,  # MeV
        'PionP'                   : 3000.   ,  # MeV 
        'PionTrackCHI2pNDOF'      :    5.   , 
        'PionIPCHI2'              :    4.   , 
        'PionPIDpiK'              :   10.   ,  # PIDpi-PIDK > -5, i.e., PIDK<5 
        #
        'KstarVertexCHI2'         :   16.   , 
        'KstarMassW'              :  130.   ,  # MeV
        'KstarIPCHI2'             :    1.   , 
        'KstarFDCHI2'             :    1.   , 
        #
        'BComMassW'               : 1200.   ,  # MeV
        'BVertexCHI2'             :    9.   ,  # /ndf
        'BMassW'                  : 1000.   ,  # MeV  
        'BIPCHI2'                 :   64.   ,  # pointing
        'BFDCHI2'                 :    9.   , 
        'BDIRA'                   :    0.999, 
        'SumIPSCut'               : " & (SUMTREE(((ABSID=='K+') | (ABSID=='pi-') | (ID=='e+') | (ID=='e-')),sqrt(BPVIPCHI2()))>15)" 
           },
    'WGs'    : [ 'RD' ],
    'STREAMS' : [ 'Radiative' ]
    }


Bd2eeKstarBDT = {
    'BUILDERTYPE'             : 'Bd2eeKstarBDTConf',
    'CONFIG'                  : {
        'LinePrescale'            :    1.   ,
        'LinePostscale'           :    1.   ,
        #
        'ElectronPT'              :  200.   ,  # MeV
        'ElectronTrackCHI2pNDOF'  :    5.   ,
        'ElectronIPCHI2'          :    1.   ,
        'ElectronPIDepi'          :   -2.   ,          
        #
        'eeVertexCHI2'            :   16.   ,  
        'eeMinMass'               :   20.   ,  # MeV 
        'eeMaxMass'               : 1500.   ,  # MeV
        #
        'KaonPT'                  :  400.   ,  # MeV 
        'KaonP'                   : 3000.   ,  # MeV  
        'KaonTrackCHI2pNDOF'      :    5.   , 
        'KaonIPCHI2'              :    4.   , 
        'KaonPIDKpi'              :   -5.   , 
        #
        'PionPT'                  :  250.   ,  # MeV
        'PionP'                   : 2000.   ,  # MeV 
        'PionTrackCHI2pNDOF'      :    5.   , 
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
        'BDTCutValue'             :   -0.98 ,
        'BDTWeightsFile'          : '$TMVAWEIGHTSROOT/data/Bd2eeKstar_BDTG_v1r0.xml'    
    },
    'WGs'    : [ 'RD' ],
    'STREAMS' : [ 'Radiative' ]
    }

Bd2JpsieeKstarBDT = {
    'BUILDERTYPE'             : 'Bd2eeKstarBDTConf',
    'CONFIG'                  : {
        'LinePrescale'            :    1.   ,
        'LinePostscale'           :    1.   ,
        #
        'ElectronPT'              :  200.   ,  # MeV
        'ElectronTrackCHI2pNDOF'  :    5.   ,
        'ElectronIPCHI2'          :    1.   ,
        'ElectronPIDepi'          :   -2.   ,          
        #
        'eeVertexCHI2'            :   16.   ,  
        'eeMinMass'               : 2200.   ,  # MeV 
        'eeMaxMass'               : 4200.   ,  # MeV
        #
        'KaonPT'                  :  400.   ,  # MeV 
        'KaonP'                   : 3000.   ,  # MeV  
        'KaonTrackCHI2pNDOF'      :    5.   , 
        'KaonIPCHI2'              :    4.   , 
        'KaonPIDKpi'              :   -5.   , 
        #
        'PionPT'                  :  250.   ,  # MeV
        'PionP'                   : 2000.   ,  # MeV 
        'PionTrackCHI2pNDOF'      :    5.   , 
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
        'BDTCutValue'             :   -0.98 ,
        'BDTWeightsFile'          : '$TMVAWEIGHTSROOT/data/Bd2eeKstar_BDTG_v1r0.xml'    
            },
    'WGs'    : [ 'RD' ],
    'STREAMS' : [ 'Radiative' ]
    }


Bd2MuMuKstarBDT = {
    'BUILDERTYPE'             : 'Bd2MuMuKstarBDTConf',
    'CONFIG'                  : {
        'LinePrescale'            :    1.   ,
        'LinePostscale'           :    1.   ,
        #
        'ElectronPT'              :  200.   ,  # MeV
        'ElectronTrackCHI2pNDOF'  :    5.   ,
        'ElectronIPCHI2'          :    1.   ,
        'ElectronPIDepi'          :   -5.   ,          
        #
        'eeVertexCHI2'            :   16.   ,  
        'eeMinMass'               :   20.   ,  # MeV 
        'eeMaxMass'               : 5200.   ,  # MeV
        #
        'KaonPT'                  :  400.   ,  # MeV 
        'KaonP'                   : 3000.   ,  # MeV  
        'KaonTrackCHI2pNDOF'      :    5.   , 
        'KaonIPCHI2'              :    4.   , 
        'KaonPIDKpi'              :   -5.   , 
        #
        'PionPT'                  :  250.   ,  # MeV
        'PionP'                   : 2000.   ,  # MeV 
        'PionTrackCHI2pNDOF'      :    5.   , 
        'PionIPCHI2'              :    4.   , 
        'PionPIDpiK'              :   10.   ,  # PIDpi-PIDK>-10, i.e., PIDK<10 
        #
        'KstarVertexCHI2'         :   16.   , 
        'KstarMassW'              :  150.   ,  # MeV
        #
        'BComMassW'               : 1200.   ,  # MeV
        'BVertexCHI2'             :   16.   ,  # /ndf
        'BMassW'                  : 1000.   ,  # MeV  
        'BDIRA'                   :    0.999,
        'BDTCutValue'             :   -0.98 ,
        'BDTWeightsFile'          : '$TMVAWEIGHTSROOT/data/Bd2eeKstar_BDTG_v1r0.xml'
    },
    'WGs'    : [ 'RD' ],
    'STREAMS' : [ 'Dimuon' ]
    }

Bd2MuMuKPiBDT = {
    'BUILDERTYPE'             : 'Bd2MuMuKstarBDTConf',
    'CONFIG'                  : {
        'LinePrescale'            :    1.   ,
        'LinePostscale'           :    1.   ,
        #
        'ElectronPT'              :  200.   ,  # MeV
        'ElectronTrackCHI2pNDOF'  :    5.   ,
        'ElectronIPCHI2'          :    1.   ,
        'ElectronPIDepi'          :   -5.   ,          
        #
        'eeVertexCHI2'            :   16.   ,  
        'eeMinMass'               :   20.   ,  # MeV 
        'eeMaxMass'               : 5200.   ,  # MeV
        #
        'KaonPT'                  :  400.   ,  # MeV 
        'KaonP'                   : 3000.   ,  # MeV  
        'KaonTrackCHI2pNDOF'      :    5.   , 
        'KaonIPCHI2'              :    4.   , 
        'KaonPIDKpi'              :   -5.   , 
        #
        'PionPT'                  :  250.   ,  # MeV
        'PionP'                   : 2000.   ,  # MeV 
        'PionTrackCHI2pNDOF'      :    5.   , 
        'PionIPCHI2'              :    4.   , 
        'PionPIDpiK'              :   10.   ,  # PIDpi-PIDK>-10, i.e., PIDK<10 
        #
        'KstarVertexCHI2'         :   16.   , 
        'KstarMassW'              :  1400.  ,  # MeV
        #
        'BComMassW'               :  550.   ,  # MeV
        'BVertexCHI2'             :   16.   ,  # /ndf
        'BMassW'                  :  500.   ,  # MeV  
        'BDIRA'                   :    0.999,
        'BDTCutValue'             :   -0.98 ,
        'BDTWeightsFile'          : '$TMVAWEIGHTSROOT/data/Bd2eeKstar_BDTG_v1r0.xml'
    },
    'WGs'    : [ 'RD' ],
    'STREAMS' : [ 'Dimuon' ]
    }

Beauty2XGamma = {
    'BUILDERTYPE' : 'Beauty2XGamma',
    'CONFIG' : {
    # Cuts made on all charged input particles in all lines
           "ALL" : {  'TRCHI2DOF_MAX' : 4,
                      'PT_MIN'        : '250*MeV',
                      'P_MIN'         : '1000*MeV',
                      'MIPCHI2DV_MIN' : 4 },
           # Cuts made on the photon
           "GAMMA" : { 'PT_MIN'       : '2500*MeV',
                       'CL_MIN'       : 0.25
                     },
           # Cuts made on all K shorts
           "KS0" : { 'PT_MIN'        : '250*MeV',
                     'MIPCHI2DV_MIN' : 4,
                     'MM_MIN'        : '467.*MeV',
                     'MM_MAX'        : '527.*MeV' },
           # Cuts made on all pi0's
           "Pi0" : { 'PT_MIN'        : '1200*MeV',
                     'P_MIN'         : '10000*MeV',
                     'CHILDCL1_MIN'  : 0.25,
                     'CHILDCL2_MIN'  : 0.25 },
           # Cuts made on all B's and Lb's used in all lines
           "B2X" : { 'SUMPT_MIN'     : '4000*MeV',
                     'VCHI2DOF_MAX'  : 15,
                     'BPVIPCHI2_MAX' : 15,
                     'BPVLTIME_MIN'  : '0.2*ps',
                     'BPVDIRA_MIN'   : 0.9998,
                     'AM_MIN'        : '4000*MeV',
                     'AM_MAX'        : '7000*MeV',
                     'B2CBBDT_MIN'   : 0.00
                     },
           # Cuts for rho, K*, phi, omega
           "HH": { 'MASS_WINDOW'   : {'KST':'150*MeV',
                                      'RHO':'250*MeV',
                                      'PHI':'15*MeV',
                                      'OMEGA':'30*MeV'},
                   'DAUGHTERS'     : {'PT_MIN':'100*MeV','P_MIN':'2000*MeV'},
                   'LAMBDADAUGHTERS' : {'PT_MIN':'300*MeV','P_MIN':'2500*MeV'},
                   'AMAXDOCA_MAX'  : '0.5*mm',
                   'VCHI2DOF_MAX'  : 16,
                   'BPVVDCHI2_MIN' : 16,
                   'BPVDIRA_MIN'   : 0.0,
                   'ASUMPT_MIN'    : '1000*MeV',
                   'pP_MIN'        : '10000*MeV' # for pH only (obviously)
                 },
           # Cuts for PiPiPi0 for omega decay
           # "HHH": { 'MASS_WINDOW'   : {'OMEGA': '30*MeV'},
           #          'KDAUGHTERS'    : {'PT_MIN':'100*MeV',
           #                             'P_MIN':'2000*MeV',
           #                             'PIDK_MIN':'-5'},
           #          'PiDAUGHTERS'   : {'PT_MIN':'100*MeV',
           #                             'P_MIN':'2000*MeV',
           #                             'PIDK_MAX':'10'},
           #          'pDAUGHTERS'    : {'PT_MIN':'100*MeV',
           #                             'P_MIN':'2000*MeV',
           #                             'PIDp_MIN':'-5'},
           #          'AMAXDOCA_MAX'  : '0.40*mm',
           #          'VCHI2DOF_MAX'  : 8,
           #          'BPVVDCHI2_MIN' : 16, 
           #          'BPVDIRA_MIN'   : 0.98,
           #          'ASUMPT_MIN'    : '1250*MeV',
           #          'MIPCHI2DV_MIN' : 0.0,
           #          'BPVVDRHO_MIN'  : '0.1*mm',
           #          'BPVVDZ_MIN'    : '2.0*mm',
           #          'PTMIN1'       : '300*MeV'},
           # PID cuts
           "PID" : { 'P'  : {'PIDp_MIN' : 0},
                     'PI' : {'PIDK_MAX' : 20},
                     'K'  : {'PIDK_MIN' : -10} },
    "Prescales"  : {},
    "GECNTrkMax" : 500}, 
    'WGs'    : ['RD'],
    'STREAMS' : [ 'Radiative' ] 
    }

