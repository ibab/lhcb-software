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
        'BDTWeightsFile'          : '$TMVASELECTIONSROOT/weights/Bd2eeKstar_BDTG.weights.xml'    
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
        'BDTWeightsFile'          : '$TMVASELECTIONSROOT/weights/Bd2eeKstar_BDTG.weights.xml'    
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
        'BDTWeightsFile'          : '$TMVASELECTIONSROOT/weights/Bd2eeKstar_BDTG.weights.xml'
    },
    'WGs'    : [ 'RD' ],
    'STREAMS' : [ 'Radiative' ]
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
        'BDTWeightsFile'          : '$TMVASELECTIONSROOT/weights/Bd2eeKstar_BDTG.weights.xml'
    },
    'WGs'    : [ 'RD' ],
    'STREAMS' : [ 'Radiative' ]
    }
