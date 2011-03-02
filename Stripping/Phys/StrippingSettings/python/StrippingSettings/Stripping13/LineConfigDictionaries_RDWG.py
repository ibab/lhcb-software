"""
Line config dictionaries for the Rare Decay WG.

Stripping13 definitions.
Dictionaries have the name of the line builder instance.
"""

B2XGamma = {
    'BUILDERTYPE' : 'StrippingB2XGammaConf',
    'CONFIG' : {'TrIPchi2Phi'          : 10.       # Dimensionless
                ,'TrIPchi2Kst'         : 10.       # Dimensionless
                ,'PhiMassWin'          : 15.       # MeV
                ,'KstMassWin'          : 100.      # MeV
                ,'KstMassWinSB'        : 200.      # MeV
                ,'BsMassWin'           : 1000.     # MeV
                ,'B0MassWin'           : 1000.     # MeV
                ,'BMassWinSB'          : 2000.     # MeV
                ,'BsDirAngle'          : 0.02      # radians
                ,'B0DirAngle'          : 0.02      # radians
                ,'BDirAngleMoni'       : 0.06      # radians
                ,'BsPVIPchi2'          : 15.       # Dimensionless
                ,'B0PVIPchi2'          : 15.       # Dimensionless
                ,'photonPT'            : 2600.     # MeV
                ,'PhiVCHI2'            : 15.       # dimensionless
                ,'KstVCHI2'            : 15.       # dimensionless
                ,'TrChi2'              : 5.        # dimensionless
                # Pre- and postscales
                ,'Bs2PhiGammaWideBMassPreScale'      : 0.1
                ,'Bs2PhiGammaWideBMassPostScale'     : 1.0
                ,'Bs2PhiGammaLooseDiraPreScale'      : 0.1
                ,'Bs2PhiGammaLooseDiraPostScale'     : 1.0
                ,'Bs2PhiGammaPreScale'               : 1.0
                ,'Bs2PhiGammaPostScale'              : 1.0
                ,'Bd2KstGammaWideBMassPreScale'      : 0.05
                ,'Bd2KstGammaWideBMassPostScale'     : 1.0
                ,'Bd2KstGammaLooseDiraPreScale'      : 0.05
                ,'Bd2KstGammaLooseDiraPostScale'     : 1.0
                ,'Bd2KstGammaWideKstMassPreScale'    : 0.05
                ,'Bd2KstGammaWideKstMassPostScale'   : 1.0
                ,'Bd2KstGammaWidePreScale'           : 0.05
                ,'Bd2KstGammaWidePostScale'          : 1.0
                ,'Bd2KstGammaPreScale'               : 1.0
                ,'Bd2KstGammaPostScale'              : 1.0
                },
    'WGs'    : ['RD'],
    'STREAM' : 'Radiative' 
    }



# Bs2MuMuPhi
# P. Schaak 

Bs2MuMuPhi = {
    'BUILDERTYPE'          : 'Bs2MuMuPhiConf' ,
    'CONFIG'               : {
    'BsIPCHI2'             : 9.0           # dimensionless
    , 'BsLT'               : 0.0002        # ns
    , 'BsVertexCHI2'       : 40.0          # dimensionless
    , 'KaonPIDK'           : 0             # dimensionless
    , 'KaonMINIPCHI2'      : 9.0           # dimensionless
    , 'MuonMINIPCHI2'      : 9.0           # dimensionless
    },
    'WGs' : [ 'RD' ] ,
    'STREAM' : 'Dimuon'
    }

# Bd2KstarMuMu for MVA
# J. Dickens, M. Patel 

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
    'B_Comb_MassLow'            : 4600.0,
    'B_Comb_MassHigh'           : 6000.0,
    'B_MassLow'                 : 4850.0,
    'B_MassHigh'                : 5780.0,
    'B_VertexCHI2'              : 6.0,
    'B_IPCHI2'                  : 16.0,
    'B_DIRA'                    : 0.014,
    'B_FlightCHI2'              : 121.0,
    'Dau_VertexCHI2'            : 12.0,
    'Dau_DIRA'                  : -0.9,
    'Kstar_Comb_MassLow'        : 550.0,
    'Kstar_Comb_MassHigh'       : 2200.0,
    'Kstar_MassLow'             : 600.0,
    'Kstar_MassHigh'            : 2000.0,
    'Kstar_MinIPCHI2'           : 4.0,
    'Kstar_FlightChi2'          : 25.0,
    'Dimu_FlightChi2'           : 81.0,
    'Track_CHI2nDOF'            : 5.0,
    'Hadron_MinIPCHI2'          : 9.0,
    'Muon_MinIPCHI2'            : 9.0,
    'Muon_IsMuon'               : False,
    'B_Dau_MaxIPCHI2'           : 9.0,
    'Kstar_Dau_MaxIPCHI2'       : 9.0,
    'Dimu_Dau_MaxIPCHI2'        : 9.0
    },
    'WGs' : [ 'RD' ] ,
    'STREAM' : 'Dimuon'
    }


# B2XMuMu for RD
# P. Schaak, M. Patel & T.Blake

B2XMuMu =  {
    'BUILDERTYPE' : 'B2XMuMuConf',
    'CONFIG' : {
    'BVXCHI2NDOF'        : 4.0           # dimensionless
    , 'BIPCHI2'            : 9.0           # dimensionless 
    , 'BDIRA'              : 0.999968      # dimensionless
    , 'BFDCHI2'            : 100.0         # dimensionless
    , 'KpiMINIPCHI2'       : 9.0           # dimensionless
    , 'KpiTRACKCHI2'       : 4.0           # dimensionless         
    , 'KpiVXCHI2NDOF'      : 9.0           # dimensionless       
    , 'MuonMINIPCHI2'      : 16.0           # dimensionless      
    , 'MuonTRACKCHI2'      : 4.0           # dimensionless    
    , 'MuonPID'            : 0.0           # dimensionless            
    , 'DimuonVXCHI2NDOF'   : 9.0           # dimensionless 
    , 'DimuonUPPERMASS'    : 5050.0        # MeV      
    , 'Pi0MINPT'           : 800.0         # MeV  
    , 'DplusLOWERMASS'     : 1600.0        # MeV
    , 'DplusUPPERMASS'     : 2300.0        # MeV 
    , 'KstarplusWINDOW'    : 300.0         # MeV
    },
    'WGs' : [ 'RD' ],
    'STREAM' : 'Dimuon'
    }

# B2XMuMu lines for Majorana searches
# S. Redford

B2XMuMuSS = {
    'BUILDERTYPE'   : 'B2XMuMuSSConf',
    'CONFIG'        : {
    'MuonP'         : 3000. ,    #MeV
    'MuonPT'        : 500.  ,    #MeV
    'MuonMINIPCHI2' : 4     ,    #adminensional
    'PionP'         : 2000. ,    #MeV
    'PionPT'        : 500.  ,    #MeV
    'PionMINIPCHI2' : 4     ,    #adminensional
    'KaonP'         : 2000. ,    #MeV
    'KaonPT'        : 300.  ,    #MeV
    'KaonPIDK'      : -1.   ,    #adimensional
    'KaonMINIPCHI2' : 4     ,    #adminensional
    'DimuonMass'    : 250.  ,    #MeV
    'BVCHI2DOF'     : 7    ,    #adminensional                              
    'BDIRA'         : 0.9995 ,    #adimensional
    'BIPCHI2'       : 30    ,    #adimensional
    'BMassWin'      : 400.  ,    #MeV, mass window
    'B2PiMuMuOSLinePrescale'  : 1 ,
    'B2PiMuMuOSLinePostscale' : 1 ,
    'B2PiMuMuSSLinePrescale'  : 1 ,
    'B2PiMuMuSSLinePostscale' : 1 ,
    'B2KMuMuOSLinePrescale'   : 1 ,
    'B2KMuMuOSLinePostscale'  : 1 ,
    'B2KMuMuSSLinePrescale'   : 1 ,
    'B2KMuMuSSLinePostscale'  : 1
    },
    'WGs' : [ 'RD' ] ,
    'STREAM' : 'Dimuon'
    }




### Bd2eeKstar

Bd2eeKstar = {
    'BUILDERTYPE'             : 'Bd2eeKstarConf',
    'CONFIG'                  : {
    'LinePrescale'            :    1.   ,
    'LinePostscale'           :    1.   ,
    'ElectronPT'              :  350.   ,  # MeV
    'ElectronTrackCHI2pNDOF'  :  100.   ,
    'ElectronIPCHI2'          :    2.25 ,
    'ElectronPIDepi'          :   -2.   ,          
    'eeCombMinMass'           :    0.   ,  # MeV         
    'eeCombMaxMass'           : 1550.   ,  # MeV   
    'eeVertexCHI2'            :   15.   ,  
    'eeMinMass'               :   20.   ,  # MeV 
    'eeMaxMass'               : 1500.   ,  # MeV
    'eeFD'                    :    1.   ,  # mm
    'KaonPT'                  :  400.   ,  # MeV 
    'KaonP'                   : 3000.   ,  # MeV  
    'KaonTrackCHI2pNDOF'      :    5.   , 
    'KaonIPCHI2'              :    4.   , 
    'KaonPIDKpi'              :   -5.   , 
    'PionPT'                  :  300.   ,  # MeV
    'PionP'                   : 3000.   ,  # MeV 
    'PionTrackCHI2pNDOF'      :    5.   , 
    'PionIPCHI2'              :    4.   , 
    'PionPIDpiK'              :   10.   ,  # PIDpi-PIDK > -5, i.e., PIDK<5 
    'KstarComMassW'           :  200.   ,  # MeV  
    'KstarVertexCHI2'         :   25.   , 
    'KstarMassW'              :  130.   ,  # MeV
    'KstarIPCHI2'             :    1.   , 
    'KstarFDCHI2'             :    1.   , 
    'BComMassW'               : 1200.   ,  # MeV
    'BVertexCHI2'             :    9.   ,  # /ndf
    'BMassW'                  : 1000.   ,  # MeV  
    'BIPCHI2'                 :   64.   ,  # pointing
    'BFDCHI2'                 :    9.   , 
    'BDIRA'                   :    0.999, 
    'BIP'                     :    0.05    # mm
    },
    'WGs'    : [ 'RD' ],
    'STREAM' : 'Dielectron'
    }

Bd2JpsieeKstar = {
    'BUILDERTYPE' : 'Bd2JpsieeKstarConf',
    'CONFIG'      : {
    'LinePrescale'            :    1.   ,
    'LinePostscale'           :    1.   ,
    'ElectronPT'              :  350.   ,  # MeV
    'ElectronTrackCHI2pNDOF'  :  100.   ,
    'ElectronIPCHI2'          :    2.25 ,
    'ElectronPIDepi'          :   -2.   ,          
    'eeCombMinMass'           : 2100.   ,  # MeV         
    'eeCombMaxMass'           : 4300.   ,  # MeV   
    'eeVertexCHI2'            :   15.   ,  
    'eeMinMass'               : 2200.   ,  # MeV 
    'eeMaxMass'               : 4200.   ,  # MeV
    'eeFD'                    :    1.   ,  # mm
    'KaonPT'                  :  400.   ,  # MeV 
    'KaonP'                   : 3000.   ,  # MeV  
    'KaonTrackCHI2pNDOF'      :    5.   , 
    'KaonIPCHI2'              :    4.   , 
    'KaonPIDKpi'              :   -5.   , 
    'PionPT'                  :  300.   ,  # MeV
    'PionP'                   : 3000.   ,  # MeV 
    'PionTrackCHI2pNDOF'      :    5.   , 
    'PionIPCHI2'              :    4.   , 
    'PionPIDpiK'              :   10.   ,  # PIDpi-PIDK > -5, i.e., PIDK<5 
    'KstarComMassW'           :  200.   ,  # MeV  
    'KstarVertexCHI2'         :   25.   , 
    'KstarMassW'              :  130.   ,  # MeV
    'KstarIPCHI2'             :    1.   , 
    'KstarFDCHI2'             :    1.   , 
    'BComMassW'               : 1200.   ,  # MeV
    'BVertexCHI2'             :    9.   ,  # /ndf
    'BMassW'                  : 1000.   ,  # MeV  
    'BIPCHI2'                 :   64.   ,  # pointing
    'BFDCHI2'                 :    9.   , 
    'BDIRA'                   :    0.999, 
    'BIP'                     :    0.05    # mm
    },
    'WGs' : [ 'RD' ],
    'STREAM' : 'Dielectron' 
    }


### Bu2LLK

Bu2LLK = {
    'BUILDERTYPE'         : 'Bu2LLKConf',
    'CONFIG'              : {
    'BFlightCHI2'         : 100       # adimentional 
    ,  'BDIRA'               : 0.9995     # adimentional    TIGHTENED
    ,  'BIPCHI2'             : 25        # adimentional  
    ,  'BVertexCHI2'         : 16        # adimentional
    ,  'DiLeptonPT'          : 0         # MeV (not used)
    ,  'DiLeptonFDCHI2'      : 16        # adimentional
    ,  'DiLeptonIPCHI2'      : 9         # adimentional
    ,  'LeptonIPCHI2'        : 16        # adimentional      TIGHTENED
    ,  'LeptonPT'            : 800       # MeV               
    ,  'KaonIPCHI2'          : 16        # adimentional      TIGHTENED
    ,  'KaonPT'              : 800       # MeV               LOOSENED
    ,  'UpperMass'           : 5500      # MeV (Higher bound of signal box)
    ,  'Bu2eeKLinePrescale'  : 1
    ,  'Bu2eeKLinePostscale' : 1
    ,  'Bu2mmKLinePrescale'  : 1
    ,  'Bu2mmKLinePostscale' : 1
    },
    'WGs'    : [ 'RD' ] ,
    'STREAM' : 'Dimuon' 
    }


### Bs2MuMuLines

Bs2MuMuLines = {
    'BUILDERTYPE' : 'Bs2MuMuLinesConf',
    'CONFIG'      : {
    'DefaultLinePrescale'    : 1,
    'DefaultLinePostscale'   : 1,
    'Bs2mmWideLinePrescale'  : 1,
    'Bs2mmWideLinePostscale' : 1,
    'LooseLinePrescale'      : 0.05,
    'LooseLinePostscale'     : 1,
    'JPsiLinePrescale'       : 1,
    'JPsiLinePostscale'      : 1,
    'JPsiLooseLinePrescale'  : 0.1,
    'JPsiLooseLinePostscale' : 1,
    'JPsiPromptLinePrescale' : 0.01,
    'JPsiPromptLinePostscale': 1,
    'MuIPChi2_loose'        :  9,
    'MuTrChi2_loose'        : 10,
    'BIPChi2_loose'         : 64,
    'BFDChi2_loose'         : 100
    },
    'WGs'    : [ 'RD' ],
    'STREAM' : 'Dimuon'
    }

### B2MuMuMuMuLines

B2MuMuMuMuLines = {
    'BUILDERTYPE' : 'B2MuMuMuMuLinesConf',
    'CONFIG'      : {
    'B2MuMuMuMuLinePrescale'    : 1,
    'B2MuMuMuMuLinePostscale'   : 1,
    },
    'WGs' : [ 'RD' ],
    'STREAM' : 'Dimuon'
    }

### Stripping TriMuons

TriMuon = {
    'BUILDERTYPE'  : 'StrippingTriMuonsConf',
    'CONFIG'       : {
    'GoodMuons'    : " ( PT > 300 * MeV ) & ( TRCHI2DOF < 5  ) & ( BPVIPCHI2 () >  6 ) " ,
    'GoodMuonsForBc' : " ( BPVIPCHI2 () >  9 ) " ,
    'TightMuons'   : " ( PT > 1.5 * GeV ) & ( BPVIPCHI2 () > 16 ) " ,
    'HLT'          : " HLT_PASS_RE('Hlt.*(MicroBias|Muon|MuMu|DiMu).*Decision') " , 
    '3mu-Prescale' : 1.00 , 
    'Bc-Prescale'  : 1.00 , 
    'Tau-Prescale' : 1.00 , 
    'Preambulo'    : [
    'chi2vx = VFASPF(VCHI2) '                               ,
    "from GaudiKernel.PhysicalConstants import c_light"     ,
    "ctau    =                    BPVLTIME ( ) * c_light "  ,
    "ctauBc  = PDGM('B_c+') / M * BPVLTIME ( ) * c_light "  
    ]
    },
    'WGs'    : [ 'RD' ] ,
    'STREAM' : 'Dimuon' 
    }
