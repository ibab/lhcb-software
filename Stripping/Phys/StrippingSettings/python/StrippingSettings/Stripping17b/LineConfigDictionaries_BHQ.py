TwoBodyPrompt = {
    'BUILDERTYPE'  :  'TwoBody_promptConf',
    'CONFIG' :  { 'nbody':              2,
                  'MinBMass':        3690.0,
                  'MaxBMass':        100000.0,
                  'MinBPt':          2000.0,
                  'MaxBVertChi2DOF':   10.0,
                  'MaxBPVVDCHI2Comb' : 10.,
                  'MinBPVDIRA' : 0.999,
                  'MinNvc':             3,
                  'MinFly':1,
                  'doLm':            True,
                  'doDz':            True,
                  'doDp':            True,
                  'doDs':            True,
                  'doLc':            True,
                  'doPh':            True,
                  'doJp':            True,
                  'doDS':            True,
                  'MinSingleTrackPtPi' : 6000.0,
                  'MinSingleTrackPtK' : 4000.0,
                  'MinSingleTrackPtp' : 3500.0,  
                  'MaxSingleTrackIPChi2DV' : 5.0,   # changed 
                  'MinDPIDpSingle' : 10.0,
                  'MinDPIDKSingle' : 5.0,
                  'MinLongLivedDauPt' : 200.0,
                  'MinLongLivedDauIPChi2' : 4.0,      
                  'MinLongLivedPt' : 3000.0,
                  'MinKsPt':2500.,
                  'MaxLongLivedIPChi2' : 10.0,
                  'MaxLongLivedVertChi2DOF' : 10.0,
                  'MinLongLivedPVVDChi2' : 200.0,   # halved
                  'MaxKsDeltaM' : 25.0, 
                  'MaxLmDeltaM' : 15.0, 
                  
                  'MinDPIDK' : 5.0,
                  'MinDPIDp' : 5.0,
                  
                  'MinDPIDpK' : 5.0,
                  'MinDPIDKforD0Dplus' : 0.0,
                  'MinDDauPt' : 250.0,
                  'MinDDauIPChi2' : 7.0,      
                  'MinDPt' : 1200.0,       
                  'MaxDIPChi2' : 10.0,   # changed
                  'MaxDVertChi2DOF' : 10.0,
                  'MinDPVVDChi2' : 50.0,  # halved/2 
                  'MaxDDeltaM' : 30.0,
                  
                  'MinDDauIPChi2Lc': 2. ,   
                  'MinDPVVDChi2Lc': 5., 
                  
                  
                  'MinPhDauPt' : 1000.0,      
                  'MaxPhDauIPChi2' : 5.0,  # changed      
                  'MinPhPt' : 2500.0,      
                  'MaxPhVertChi2DOF' : 10.0,
                  'MaxPhDeltaM' : 20.0, 
                  
                  'MinJpDauPt' : 500.0,      
                  'MaxJpDauIPChi2' : 10.0,   # changed
                  'MinJpPt' : 1000.0,      
                  'MaxJpVertChi2DOF' : 10.0,
                  
                  'MaxDSD0VertChi2DOF' : 10.0,
                  'MinDSD0PVVDChi2' : 60.0,   # halved/2
                  'MaxDSD0DeltaM' : 30.0,
                  'MinDSPt' : 1000.0,      
                  'MaxDSDeltaM':3.0,
                  'MaxDSSlowPionIPChi2':7.,  # added   
                  'MaxDSD0IPChi2':7.,  # added
                  'MinDSD0DauPt' : 250.0, # added
                  'MinDSD0DauIPChi2' : 9.0,
                  'MinDSSlowPionDauPt' : 150.0 # added
                  },
    'STREAMS' : [ 'Bhadron' ],
    'WGs'    : [ 'BHQ' ]
    }

ChiCJPsiGammaConversion = {
    'BUILDERTYPE'  : 'StrippingChiCJPsiGammaConversionConf',
    'CONFIG'       : {    'trackChi2'               :    5.0
                          , 'JPsiMassMin'           :    3.04 # GeV
                          , 'JPsiMassMax'           :    3.14 # GeV
                          , 'JPsiVertexChi2'        :   25.0
                          , 'NBCutJPsi'             :    0.7
                          , 'ExpertiseJPsi'         : 'Muon/mumu_net_noip.nb'
                          , 'NBVersionJPsi'         : "TuneSep2010"
                          , 'eDLLe'                 :  -5.0
                          , 'GammaEEMass'           :   7.0 #MeV
                          , 'GammaEEChi2'           :   2.0
                          , 'GammaEETau'            :   5.0  #ps
                          , 'mMinChiCRaw'           :   2.9 #GeV
                          , 'mMaxChiCRaw'           :   4.1 #GeV
                          , 'mMinChiCFit'           :   3.0 #GeV
                          , 'mMaxChiCFit'           :   4.0 #GeV
                          , 'PrescaleChiC'          :   1.0
                          , 'PostscaleChiC'         :   1.0
                          , 'PrescaleChiCWS'        :   0.5
                          , 'PostscaleChiCWS'       :   1.0
                          },
    'STREAMS' : [ 'Dimuon' ],
    'WGs'    : [ 'BHQ' ]
    }


Ccbar2PpbarDetached = {
    'BUILDERTYPE'       : 'Ccbar2PpbarConf',
    'CONFIG'    : {
    'LinePrescale'     :     1.  ,
    'LinePostscale'    :     1.  ,
    
    'SpdMult'          :   600.  , # dimensionless, Spd Multiplicy cut 
    'ProtonPT'         :  1000.  , # MeV
    'ProtonP'          :    -2.  , # MeV
    'ProtonTRCHI2DOF'  :     5.  ,
    'ProtonPIDppi'     :    15.  , # CombDLL(p-pi)
    'ProtonPIDpK'      :    10.  , # CombDLL(p-K)
    'ProtonIPCHI2Cut'  : " & (BPVIPCHI2()>9)",
    'CombMaxMass'      :  1.0e+6 , # MeV, before Vtx fit
    'CombMinMass'      :  2650.  , # MeV, before Vtx fit
    'MaxMass'          :  1.0e+6 , # MeV, after Vtx fit
    'MinMass'          :  2700.  , # MeV, after Vtx fit
    'VtxCHI2'          :     9.  , # dimensionless
    'CCCut'            :  " & (BPVDLS>5)"     
    },
    'STREAMS' : [ 'Charm' ] ,
    'WGs'    : [ 'BHQ' ]
    }

Ccbar2PhiPhi = {
    'BUILDERTYPE'       : 'Ccbar2PhiPhiConf',
    'CONFIG'    : {
        'TRCHI2DOF'        :     5.  ,
        'KaonPIDK'         :     2.  ,
        'KaonPT'           :   500.  , # MeV
        'PhiVtxChi2'       :    16.  ,
        'PhiMassW'         :    12.  , 
        'CombMaxMass'      :  4100.  , # MeV, before Vtx fit
        'CombMinMass'      :  2750.  , # MeV, before Vtx fit
        'MaxMass'          :  4000.  , # MeV, after Vtx fit
        'MinMass'          :  2800.  , # MeV, after Vtx fit
        'Phi_TisTosSpecs'  : { "Hlt1Global%TIS" : 0, "Hlt2Global%TIS" : 0 }
        },
    'STREAMS' : [ 'Charm' ] ,
    'WGs'    : [ 'BHQ' ]
    }
    



