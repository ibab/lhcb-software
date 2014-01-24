#
# Rare Decay Lines

#=================================================================================================================================================================================
# Radiative lines

Beauty2XGamma = {
    'BUILDERTYPE' : 'Beauty2XGammaConf',
    'CONFIG'      : { 
    'B2XG2piPrescale'        : 1.,
    'B2XG2piCNVPrescale'     : 1.,
    'B2XGpiKsPrescale'       : 1.,
    
    'B2XG3piPrescale'        : 1.,
    'B2XG3piCNVPrescale'     : 1.,
    'B2XG2pipi0MPrescale'    : 1.,
    'B2XG2pipi0RPrescale'    : 1.,
    'B2XG2piKsPrescale'      : 1.,
    
    'B2XG4piPrescale'        : 1.,
    'B2XG3piKsPrescale'      : 1.,
    'B2XG3pipi0MPrescale'    : 1.,
    'B2XG3pipi0RPrescale'    : 1.,
    'B2XG2pi2KsPrescale'     : 1.,
    
    'B2XGLambdapiPrescale'   : 1.,
    'B2XGLambda2piPrescale'  : 1.,
    'B2XGLambda3piPrescale'  : 1.,
    'B2XGLbLambdaPrescale'   : 1., # UNUSED
    
    # TRACK CUTS
    'B2XGTrkChi2DOF'   : 3., # was nothing
    'B2XGTrkGhostProb' : 0.4, # was 0.5
    'B2XGTrkMinIPChi2' : 16., # was 5
    'B2XGTrkMinPT'     : 300., # was 100
    'B2XGTrkMinP'      : 1000, # P was 2000, old stripping has 1 GeV...
    
    # PHOTON CUTS
    'B2XGGammaPTMin'       : 2000.,
    'B2XGGammaCL'          : 0.25,
    'B2XGGammaCNVPTMin'    : 1000.,
    
    # TRACK COMBINATION CUTS
    'B2XGResMinPT'     : 0.,
    'B2XGResMinMass'   : 0., #was 520
    'B2XGResMaxMass'   : 7900., # was 4940 (3500+4*(mK-mpi). Now is maxMLambda+(mp-mpi)+(mK-mpi) to allow for Kp decays is StdLooseNoPIDPions also covers proton tracks
    'B2XGResVtxChi2DOF': 10., # was 25
    'B2XGResSumPtMin'  : 1500.,
    'B2XGResDocaMax'   : 100.5, # NOT USED
    'B2XGResBPVVDCHI2Min'  : -1., # FD avoid if possible. was 64
    'B2XGResIPCHI2Min' : 0.0,
    
    # B HADRON CUTS
    'B2XGBMinPT'       : 0., #250., # was 300
    'B2XGBMinM2pi'     : 3280., # was 3280
    'B2XGBMinM3pi'     : 2900., # was 2900
    'B2XGBMinM4pi'     : 2560., # was 2560
    'B2XGBMinMLambda'  : 2560., # was 2900
    'B2XGBMaxM'        : 9000., # was 8540
    'B2XGBSumPtMin' : 5000,  #SUMPT
    'B2XGBMinBPVDIRA'  : -1.1,  #BPVDIRA avoid if possible
    'B2XGBMaxCorrM'    : 73000., # NOT USED
    'B2XGBVtxChi2DOF'  : 9.,
    'B2XGBVtxMaxIPChi2': 9. # was 25
    },
    'WGs'     : [ 'RD' ] ,
    'STREAMS' : [ 'Leptonic' ] 
    }



#=================================================================================================================================================================================
# EW penguin lines

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
    'B_Comb_MassLow'	  : 4700.0,
    'B_Comb_MassHigh'     : 6100.0,
    'B_MassLow'           : 4800.0,
    'B_MassHigh'          : 6000.0,
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
    'DimuonWS'            :   False,
    'HadronWS'            :   False,

      # Omega cuts         
      'Omega_MassWin'       :   100, #MeV
      'Omega_CombMassWin'   :   200, #MeV
    'OmegaChi2Prob'       : 0.00001,  # dimensionless               
     'Pi0ForOmegaMINPT'   : 500.0,         # MeV
      
    #K1->OmegaK cuts
    'K12OmegaK_MassLow'   :   300, #MeV
    'K12OmegaK_MassHigh'  :  2500, #MeV
      'K12OmegaK_CombMassLow'   :   400, #MeV
      'K12OmegaK_CombMassHigh'  :  2400, #MeV
      
    # GEC
    'SpdMult'             :  600 ,

    # Inclusive sel requirements

    'InclDimu_FlightChi2'     :   100.0,
    'InclDimu_Dau_MaxIPCHI2'  :     9.0,
    'InclDiMu_VertexCHI2'     :     6.0,
    'InclDiMu_DIRA'           :   -0.95,
    'InclDiMu_CCbarPrescale'  :    0.07,
    'InclDimuLOWERMASS'       :  3870.0, # MeV                                                                                           
    'InclDimuUPPERMASS'       :  5800.0, # MeV                                                                                           
    'InclDiMuCORRM_MIN'       :     0.0, # MeV                                                                                           
    'InclDiMuCORRM_MAX'       : 15000.0, # MeV    
      
    'Hadronic_Selection'  :   "(M > 5000*MeV)&(M < 5700*MeV)&(INTREE((ABSID == 'K*(892)0')&(M>800*MeV)&(M < 1000*MeV)&(INTREE((ABSID=='K+')&(HASRICH)&(PIDK>-5)))))&((INTREE((ABSID=='rho(770)0')&((M<2800)|(M>3800))&(2 == NINTREE((ABSID=='pi+')&(HASRICH)&(PIDK < 5)&(P>3000*MeV)))))|(INTREE((ABSID=='phi(1020)')&(M<1200*MeV))))" , 
      
    'DECAYS'              : [ "B0 -> J/psi(1S) K_1(1270)0",   "[B+ -> J/psi(1S) K_1(1400)+]cc"  ],
    'HADRONICDECAYS'      : [ ] 

    },
    'WGs' : [ 'RD' ],
    'STREAMS' : [ 'Dimuon' ]
    }



Bu2LLK = {
    'BUILDERTYPE'         : 'Bu2LLKConf',
    'CONFIG'              : {
    'BFlightCHI2'         : 100      # adimentional 
    ,  'BDIRA'               : 0.9995    # adimentional    TIGHTENED
    ,  'BIPCHI2'             : 25      #  adimentional  
    ,  'BVertexCHI2'         : 9     #   adimentional
    ,  'DiLeptonPT'          : 0      #   MeV (not used)
    ,  'DiLeptonFDCHI2'      : 16     #   adimentional
    ,  'DiLeptonIPCHI2'      : 0      #   adimentional
    ,  'LeptonIPCHI2'        : 16     #   adimentional      TIGHTENED
    ,  'LeptonPT'            : 500   #    MeV               
    ,  'KaonIPCHI2'          : 9      #  adimentional                       CHANGED
    ,  'KaonPT'              : 800   #    MeV               LOOSENED
    ,  'UpperMass'           : 5500  #    MeV (Higher bound of signal box)
    ,  'Bu2eeKLinePrescale'  : 1
    ,  'Bu2mmKLinePrescale'  : 1
    ,  'Bu2meKLinePrescale'  : 1
    ,  'ExtraInfoTools'      : [ { "Type" : "ConeVariables"
                                   , "ConeAngle" : 1.5
                                   , "ConeNumber" : 1
                                   , "Variables" : ['angle', 'mult', 'ptasy'] } 
                                 , {'Type' : 'VertexIsolation'} 
                                 ]
    },
    'WGs'     : [ 'RD' ] ,
    'STREAMS' : ['Leptonic']
    }



#=================================================================================================================================================================================
# VRD lines

B2XTau = {
    'BUILDERTYPE' : 'B2XTauConf',
    'CONFIG' :  {
    'PT_HAD_ALL_FINAL_STATE'        : '250',  # MeV
    'P_HAD_ALL_FINAL_STATE'         : '2000', # MeV
    'IPCHI2_HAD_ALL_FINAL_STATE'    : '16',    # dimensionless
    'TRACKCHI2_HAD_ALL_FINAL_STATE' : '3',    # dimensionless
    'TRGHOPROB_HAD_ALL_FINAL_STATE' : '0.3',    # dimensionless
    #
    'PT_MU'                         : '1000', # MeV
    'P_MU'                          : '6000', # MeV 
    'IPCHI2_MU'                     : '16',   # MeV
    'TRACKCHI2_MU'                  : '3',    # dimensionless
    'TRGHOPROB_MU'                  : '0.3', # dimensionless
    
    #
    'PT_B_TT'                       : '1900', # MeV
    'PT_B_TT_HIGH'                  : '2000',# MeV 
    'PT_B_TM'                       : '1900', # MeV
    'PT_B_TM_HIGH'                  : '5000', # MeV  
    'VCHI2_B'                       :   '90', # dimensionless
    'FDCHI2_B'                      : '225',  # dimensionless
    'FD_B'                          : '90',   # mm
    'FD_B_MU'                       : '35',   # mm
    'DIRA_B'                        : '0.99', # dimensionless
    'MASS_LOW_B'                    : '2000', # MeV  
    'MASS_HIGH_B'                   : '7000', # MeV 
    'MCOR_LOW_B'                    :    '0', # MeV
    'MCOR_HIGH_B'                   :'10000', # MeV 
    
    #
    'PT_B_CHILD_BEST'               : '2000', # MeV
    'IPCHI2_B_CHILD_BEST'           : '16',   # dimensionless
    'PT_B_TAU_CHILD_BEST'           : '4000', # MeV
    'IPCHI2_B_TAU_CHILD_BEST'       : '150',   # dimensionless
    'IPCHI2_B_TAU_CHILD_WORSE'       : '16',   # dimensionless
    'PT_B_PIONS_TOTAL'              :  '7000',# MeV
    'PT_B_MU_PIONS_TOTAL'           :  '2500',# MeV
    'IPCHI2_B_TAU_MU'               :  '50',
    'IPCHI2_B_MU'                   :  '200',
    'B_TAUPI_2NDMINIPS'             :  '20',  # dimensionless
    'FDCHI2_TAU'                    :  '4000',
    'VCHI2_B_TAU_MU'                : '12',
    #
    'MASS_LOW_D'                    : '1750', # MeV
    'MASS_HIGH_D'                   : '2080', # MeV
    'APT_D'                         : '800',  #MeV
    'AMAXDOCA_D'                    : '0.2',  # mm
    'MaxPT_D'                       : '800',  #MeV
    'PT_D'                          : '1000',  #MeV
    'DIRA_D'                        : '0.99',
    'VCHI2_D'                       : '16',
    'FDCHI2_D'                      : '16',
    'VDRHOmin_D'                    : '0.1',  #mm
    'VDRHOmax_D'                    : '7.0',  #mm
    'VDZ_D'                         : '5.0',  #mm
    #
    'MASS_LOW_TAU'                    : '400', # MeV
    'MASS_HIGH_TAU'                   : '2100', # MeV
    #
    'B2TauTau_TOSLinePrescale'      : 1,
    'B2TauTau_TOSLinePostscale'     : 1,
    'B2DD_LinePrescale'             : 1,
    'B2DD_LinePostscale'            : 1,
    'B2TauMu_TOSLinePrescale'       : 1,
    'B2TauMu_TOSLinePostscale'      : 1,
    'B2DMu_LinePrescale'            : 0.0,
    'B2DMu_LinePostscale'           : 1,
    'B2DPi_LinePrescale'            : 1,
    'B2DPi_LinePostscale'           : 1,
    'B2TauTau_SameSign_TOSLinePrescale'      : 0.5,
    'B2TauTau_SameSign_TOSLinePostscale'     : 1,
    'B2DD_SameSign_LinePrescale'             : 0.5,
    'B2DD_SameSign_LinePostscale'            : 1,
    'B2TauMu_SameSign_TOSLinePrescale'       : 0.5,
    'B2TauMu_SameSign_TOSLinePostscale'      : 1,
    'B2DMu_SameSign_LinePrescale'            : 0,
    'B2DMu_SameSign_LinePostscale'           : 0,
    'B2DPi_SameSign_LinePrescale'            : 0.5,
    'B2DPi_SameSign_LinePostscale'           : 1,
    },
    'WGs' : [ 'RD' ],
    'STREAMS' : [ 'BhadronCompleteEvent' ]
    }

B2XTauMicro = {
    'BUILDERTYPE' : 'B2XTauConf',
    'CONFIG' :  {
    'PT_HAD_ALL_FINAL_STATE'        : '250',  # MeV
    'P_HAD_ALL_FINAL_STATE'         : '2000', # MeV
    'IPCHI2_HAD_ALL_FINAL_STATE'    : '16',    # dimensionless
    'TRACKCHI2_HAD_ALL_FINAL_STATE' : '3',    # dimensionless
    'TRGHOPROB_HAD_ALL_FINAL_STATE' : '0.3',    # dimensionless
    #
    'PT_MU'                         : '1000', # MeV
    'P_MU'                          : '6000', # MeV 
    'IPCHI2_MU'                     : '16',   # MeV
    'TRACKCHI2_MU'                  : '3',    # dimensionless
    'TRGHOPROB_MU'                  : '0.3', # dimensionless
    
    #
    'PT_B_TT'                       : '1900', # MeV
    'PT_B_TT_HIGH'                  : '2000',# MeV 
    'PT_B_TM'                       : '1900', # MeV
    'PT_B_TM_HIGH'                  : '5000', # MeV  
    'VCHI2_B'                       :   '90', # dimensionless
    'FDCHI2_B'                      : '225',  # dimensionless
    'FD_B'                          : '90',   # mm
    'FD_B_MU'                       : '35',   # mm
    'DIRA_B'                        : '0.99', # dimensionless
    'MASS_LOW_B'                    : '2000', # MeV  
    'MASS_HIGH_B'                   : '7000', # MeV 
    'MCOR_LOW_B'                    :    '0', # MeV
    'MCOR_HIGH_B'                   :'10000', # MeV 
    
    #
    'PT_B_CHILD_BEST'               : '2000', # MeV
    'IPCHI2_B_CHILD_BEST'           : '16',   # dimensionless
    'PT_B_TAU_CHILD_BEST'           : '4000', # MeV
    'IPCHI2_B_TAU_CHILD_BEST'       : '150',   # dimensionless
    'IPCHI2_B_TAU_CHILD_WORSE'       : '16',   # dimensionless
    'PT_B_PIONS_TOTAL'              :  '7000',# MeV
    'PT_B_MU_PIONS_TOTAL'           :  '2500',# MeV
    'IPCHI2_B_TAU_MU'               :  '50',
    'IPCHI2_B_MU'                   :  '200',
    'B_TAUPI_2NDMINIPS'             :  '20',  # dimensionless
    'FDCHI2_TAU'                    :  '4000',
    'VCHI2_B_TAU_MU'                : '12',
    #
    'MASS_LOW_D'                    : '1750', # MeV
    'MASS_HIGH_D'                   : '2080', # MeV
    'APT_D'                         : '800',  #MeV
    'AMAXDOCA_D'                    : '0.2',  # mm
    'MaxPT_D'                       : '800',  #MeV
    'PT_D'                          : '1000',  #MeV
    'DIRA_D'                        : '0.99',
    'VCHI2_D'                       : '16',
    'FDCHI2_D'                      : '16',
    'VDRHOmin_D'                    : '0.1',  #mm
    'VDRHOmax_D'                    : '7.0',  #mm
    'VDZ_D'                         : '5.0',  #mm
    #
    'MASS_LOW_TAU'                    : '400', # MeV
    'MASS_HIGH_TAU'                   : '2100', # MeV
    #
    'B2TauTau_SameSign_TOSLinePrescale'      : 0,
    'B2TauTau_SameSign_TOSLinePostscale'     : 0,
    'B2DD_SameSign_LinePrescale'             : 0,
    'B2DD_SameSign_LinePostscale'            : 0,
    'B2TauMu_SameSign_TOSLinePrescale'       : 0,
    'B2TauMu_SameSign_TOSLinePostscale'      : 0,
    'B2DMu_SameSign_LinePrescale'            : 1,
    'B2DMu_SameSign_LinePostscale'           : 1,
    'B2DPi_SameSign_LinePrescale'            : 0,
    'B2DPi_SameSign_LinePostscale'           : 0,
    
    'B2TauTau_TOSLinePrescale'      : 0,
    'B2TauTau_TOSLinePostscale'     : 0,
    'B2DD_LinePrescale'             : 0,
    'B2DD_LinePostscale'            : 0,
    'B2TauMu_TOSLinePrescale'       : 0,
    'B2TauMu_TOSLinePostscale'      : 0,
    'B2DMu_LinePrescale'            : 1,
    'B2DMu_LinePostscale'           : 1,
    'B2DPi_LinePrescale'            : 0,
    'B2DPi_LinePostscale'           : 0,
    },
    'WGs' : [ 'RD' ],
    'STREAMS' : [ 'Leptonic' ]
    }


B2XTauZVTOP = { 
    'BUILDERTYPE' : 'ZVTOP_Conf',
    'CONFIG'      : {
    'B2TauTau_LinePrescale'      : 1,
    'B2TauTau_LinePostscale'     : 1,
    'High_LinePrescale'          : 1,
    'High_LinePostscale'         : 1,
    'PT_HAD_ALL_FINAL_STATE'        : '1200',  # MeV
    'PTMAX_HAD_ALL_FINAL_STATE'     : '500',#MeV
    'P_HAD_ALL_FINAL_STATE'         : '6000', # MeV
    'IPCHI2_HAD_ALL_FINAL_STATE'    : '40',    # dimensionless
    'TRACKCHI2_HAD_ALL_FINAL_STATE' : '3',    # dimensionless
    'PID_HAD_ALL_FINAL_STATE'       : '5',    # dimensionless
    'TRGHOPROB_HAD_ALL_FINAL_STATE' : '0.3',    # dimensionless
    #
    'PT_B_TT'                       : '1900', # MeV
    'PT_B_TT_HIGH'                  : '2000',# MeV 
    'PT_B_TM'                       : '1900', # MeV
    'PT_B_TM_HIGH'                  : '2000', # MeV  
    'VCHI2_B'                       :   '90', # dimensionless
    'FDCHI2_B'                      : '225',  # dimensionless
    'FD_B'                          : '90',   # mm
    'DIRA_B'                        : '0.99', # dimensionless
    'MASS_LOW_B'                    : '2000', # MeV  
    'MASS_HIGH_B'                   : '7000', # MeV (old value: 5750)
    'MCOR_LOW_B'                    :    '0', # MeV
    'MCOR_HIGH_B'                   :'10000', # MeV (old value: 7000)
    'MIPCHI2_B'                     : '16',  # dimensionless 
    'MIPCHI2_B_HIGH'                : '16',   # dimensionless  
    #
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
    #
    'PT_B_CHILD_BEST'               : '2000', # MeV
    'P_B_CHILD_BEST'                :'10000', # MeV
    'IPCHI2_B_CHILD_BEST'           : '16',   # dimensionless
    'PT_B_TAU_CHILD_BEST'           : '4000', # MeV
    'IPCHI2_B_TAU_CHILD_BEST'       : '150',   # dimensionless
    'IPCHI2_B_TAU_CHILD_WORSE'       : '16',   # dimensionless
    'PT_B_PIONS_TOTAL'              :  '7000',# MeV
    'B_TAUPI_2NDMINIPS'             :  '20',  # dimensionless
    'HltFilter'               : "HLT_PASS_RE('Hlt2(Topo2BodyBBDT|Topo3BodyBBDT|Topo4BodyBBDT).*Decision')"
    },
    'WGs'     : [ 'RD' ],
    'STREAMS' : [ 'BhadronCompleteEvent' ]
    }



BLVLines = { 
    'BUILDERTYPE' : 'BLVLinesConf',
    'WGs'         : [ 'RD' ],
    'STREAMS'     : [ 'Dimuon' ],
    'CONFIG'      : {
    'Postscale'           :1,
    ### Signal Ch.
    'La2KmuPrescale'      : 0.,
    'Lb2KmuPrescale'      : 0.,
    'Lb2DmuPrescale'      : 0., 
    'Lb2DsmuPrescale'     : 0.,     
    'B2LcmuPrescale'      : 0.,
    ### Control Ch.
    'Lb2LcpiPrescale'     :0.,
    'B2DpiPrescale'       :0.,
    'Bs2DspiPrescale'     :0.,
    'B2LcpPrescale'      : 0.2,
    }
    }






DarkBosons = {
    'BUILDERTYPE' : 'DarkBosonConf',
    'WGs'         : [ 'RD' ],
    'STREAMS'     : { 'Dimuon'   : [  'StrippingB2KpiX2MuMuDarkBosonLine',
                                      'StrippingB2KpiX2MuMuDDDarkBosonLine',
                                      'StrippingB2KpiX2MuMuSSDarkBosonLine',
                                      'StrippingB2KpiX2MuMuDDSSDarkBosonLine',
                                      'StrippingB2KKX2MuMuDarkBosonLine',
                                      'StrippingB2KKX2MuMuDDDarkBosonLine',
                                      'StrippingB2KKX2MuMuSSDarkBosonLine',
                                      'StrippingB2KKX2MuMuDDSSDarkBosonLine',
                                      'StrippingB2KX2MuMuDarkBosonLine',
                                      'StrippingB2KX2MuMuDDDarkBosonLine',
                                      'StrippingB2KX2PiPiDarkBosonLine',
                                      'StrippingB2KX2PiPiDDDarkBosonLine',
                                      'StrippingB2KX2MuMuSSDarkBosonLine',
                                      'StrippingB2KX2MuMuDDSSDarkBosonLine',
                                      'StrippingB2KX2PiPiSSDarkBosonLine',
                                      'StrippingB2KX2PiPiDDSSDarkBosonLine',
                                      'StrippingB2JKDarkBosonLine',
                                      'StrippingB2JKstDarkBosonLine',
                                      'StrippingB2RhoX2MuMuDarkBosonLine',
                                      'StrippingB2RhoX2MuMuDDDarkBosonLine' ],
                      'Leptonic' : [ 'StrippingB2KpiX2EEDarkBosonLine',
                                     'StrippingB2KpiX2EESSDarkBosonLine',
                                     'StrippingB2KKX2EEDarkBosonLine',
                                     'StrippingB2KKX2EESSDarkBosonLine',
                                     'StrippingB2KX2EEDarkBosonLine',
                                     'StrippingB2KX2KKDarkBosonLine',
                                     'StrippingB2KX2KKDDDarkBosonLine',
                                     'StrippingB2KX2EESSDarkBosonLine',
                                     'StrippingB2KX2KKSSDarkBosonLine',
                                     'StrippingB2KX2KKDDSSDarkBosonLine' ]
                      },
    'CONFIG'      : { 
    "KB" : { # K directly from B
    'TRCHI2DOF_MAX' : 3,
    'PT_MIN'        : '250*MeV',
    'P_MIN'         : '2000*MeV',
    'MIPCHI2DV_MIN' : 9,
    'TRGHP_MAX'     : 0.3,
    'PROBNNK_MIN'   : 0.1,
    },
    "KBhard" : { # K directly from B only for B -> KS0(pipi) K
    'TRCHI2DOF_MAX' : 3,
    'PT_MIN'        : '250*MeV',
    'P_MIN'         : '3000*MeV',
    'MIPCHI2DV_MIN' : 36,
    'TRGHP_MAX'     : 0.3,
    'PROBNNK_MIN'   : 0.2,
    },
    "PiB" : { # pi directly from B
    'TRCHI2DOF_MAX' : 3,
    'PT_MIN'        : '250*MeV',
    'P_MIN'         : '2000*MeV',
    'MIPCHI2DV_MIN' : 9,
    'TRGHP_MAX'     : 0.3,
    'PROBNNpi_MIN'  : 0.2
    },
    "KX" : { # K from X
    'TRCHI2DOF_MAX' : 3,
    'PT_MIN'        : '250*MeV',
    'P_MIN'         : '3000*MeV',
    'MIPCHI2DV_MIN' : 25,
    'TRGHP_MAX'     : 0.3,
    'PROBNNK_MIN'   : 0.1
    },
     "PiX" : { # pi from X
    'TRCHI2DOF_MAX' : 3,
    'PT_MIN'        : '250*MeV',
    'P_MIN'         : '3000*MeV',
    'MIPCHI2DV_MIN' : 36,
    'TRGHP_MAX'     : 0.3,
    'PROBNNpi_MIN'  : 0.6
    },
    "KDX" : { # K DOWN from X
    'TRCHI2DOF_MAX' : 4,
    'PT_MIN'        : '125*MeV',
    'P_MIN'         : '0*MeV',
    'MIPCHI2DV_MIN' : 25,
    'TRGHP_MAX'     : 0.3,
    'PROBNNK_MIN'   : 0.1
    },
    "PiDX" : { # pi DOWN from X
    'TRCHI2DOF_MAX' : 4,
    'PT_MIN'        : '125*MeV',
    'P_MIN'         : '0*MeV',
    'MIPCHI2DV_MIN' : 25,
    'TRGHP_MAX'     : 0.3,
    'PROBNNpi_MIN'  : 0.1
    },
    "MuX" : { # muon from X
    'TRCHI2DOF_MAX' : 3,
    'PT_MIN'        : '100*MeV',
    'P_MIN'         : '0*MeV',
    'MIPCHI2DV_MIN' : 9,
    'TRGHP_MAX'     : 0.3,
    'PIDmu_MIN'     : -5
    },
      "MuJ" : { # muon from J/psi
    'TRCHI2DOF_MAX' : 4,
    'PT_MIN'        : '125*MeV',
    'P_MIN'         : '0*MeV',
    'MIPCHI2DV_MIN' : 25,
    'TRGHP_MAX'     : 0.3,
    'PIDmu_MIN'     : -4
    },
    "E" : { # electron cuts 
    'TRCHI2DOF_MAX' : 5,
    'MIPCHI2DV_MIN' : 9,
    'PT_MIN'        : '100*MeV',
    'TRGHP_MAX'     : 0.4,
    'PIDe_MIN'      : 0
    },
    "XLL" : { # making the X for LL
    'VCHI2DOF_MAX'  : 10,
    'BPVVDCHI2_MIN' : 25,
    'PT_MIN'        : '250*MeV'
    },
    "XLLhard" : { # making the X for LL hard vertex for the pions
    'VCHI2DOF_MAX'  : 5,
    'BPVVDCHI2_MIN' : 25,
    'PT_MIN'        : '250*MeV'
    },
    "XDD" : { # making X for DD
    'VCHI2DOF_MAX'  : 25,
    'BPVVDCHI2_MIN' : 25,
    'PT_MIN'        : '0*MeV'
    },
     "J" : { # cuts on J/psi
    'VCHI2DOF_MAX'  : 12,
    'ADAMASS_MAX'   : '100*MeV',
    },
    "B2KX" : { # B -> K X
    'SUMPT_MIN'     : '0*MeV',
    'VCHI2DOF_MAX'  : 25,
    'BPVIPCHI2_MAX' : 50,
    'BPVLTIME_MIN'  : '0.2*ps',
    'AM_MIN'        : '4800*MeV',
    'AM_MAX'        : '5800*MeV',
    'HAD_MINIPCHI2_MIN' : 25,
    'PT_MIN'        : '1000*MeV'
    },
    "B2HHX" : { # B -> Kpi X, KK X
    'SUMPT_MIN'     : '0*MeV',
    'VCHI2DOF_MAX'  : 25,
    'BPVIPCHI2_MAX' : 50,
    'BPVLTIME_MIN'  : '0.2*ps',
    'AM_MIN'        : '4800*MeV',
    'AM_MAX'        : '5800*MeV',
    'HAD_MINIPCHI2_MIN' : 9,
    'PT_MIN'        : '1000*MeV'
    },
    "Prescales" : { "SS" : 0.1},
    'GECNTrkMax'   : 250
    }
    }


KS02MuMu = {
    'BUILDERTYPE' : 'K0s2MuMuLinesConf',
    'WGs'     : [ 'RD' ],
    'STREAMS' : [ 'Dimuon' ],
    'CONFIG'  : {
    'NoMuIDLinePrescale'    : 1e-03,
    'NoMuIDLinePostscale'   : 1,
    'K0s2mmLinePrescale'    : 1,
    'K0s2mmLinePostscale'   : 1,
    'K0s2mmSBLinePrescale'  : 0.1,
    'K0s2mmSBLinePostscale'  : 1,
    'minMuPT' : 0,  #MeV
    'minKsPT' : 0,  #MeV
    }
    }


RareStrange = {
    'BUILDERTYPE'  : 'RareStrangeLinesConf',
    'CONFIG'       : {
    'Postscale'           :1,
    'SigmaPMuMuPrescale' : 0.,
    'SigmaPMuMuDownPrescale' :1,
    'SigmaPEEPrescale' : 0.,
    'SigmaPEEDownPrescale' : 0.1,
    'SigmaPMuMuLFVPrescale' :0.,
    'SigmaPMuMuLFVDownPrescale' :1,
    'KPiPiPiPrescale' : 0.01,
    'KPiPiPiMassMeasPrescale' :1,
    'KPiMuMuPrescale' :1,
    'KPiMuMuLFVPrescale' :1,
    'KPiMuMuDownPrescale' :1,
    'KPiPiPiDownPrescale' : 0.1,
    'KPiPiPiMassMeasDownPrescale' :1,
    'KPiMuMuLFVDownPrescale' :1,
    'SigmaMinTauPs' : 6,
    'SigmaMinPt' : 500.,
    'SigmaMaxDOCA' : 2.,
    'SigmaMassWin' : 500.,
    'SigmaMinDIRA' : 0.9,
    'SigmaMaxIpChi2' : 36.,
    'SigmaVtxChi2' : 36,
    'SigmaMinTauPsDown' : 7,
    'SigmaMinPtDown' : 0.,
    'SigmaMaxDOCADown' : 10.,
    'SigmaMassWinDown' : 500.,
    'SigmaPEEMassWinDown' : 100.,
    'SigmaMinDIRADown' : 0.9,
    'SigmaMaxIpChi2Down' : 25.,
    'SigmaVtxChi2Down' : 25,
    'SigmaDauTrChi2Down': 9,
    'muonMinIpChi2' : 9.,
    'protonPIDp': 5.,
    'electronPIDe':2.,
    'electronMinIpChi2': 9,
    'muonMinIpChi2Down' : 9.,
    'electronMinIpChi2Down': 4.,
    'KMaxDOCA' : 3,
    'KMinPT' : 100 ,
    'KMassWin' : 100,
    'KMinDIRA' : 0.98,
    'KMaxIpChi2' : 25,
    'KVtxChi2': 25,
    'KMinVDChi2' : 36,
    'KDauMinIpChi2' : 9,
    'KDauTrChi2' : 3,
    'KMaxDOCADown' : 10.,
    'KMinPTDown' : 0. ,
    'KMassWinDown' : 100,
    'KMinDIRADown' : 0.98,
    'KVtxChi2Down': 25,
    'KMinVDChi2Down' : 49,
    'KDauMinIpChi2Down' : 5,
    'KVDPVMinDown' : 500,
    'KVDPVMaxDown' : 2500,
    #Kaons stuff
    'KMaxDOCAMassMeas' : 3.,
    'KMinPTMassMeas' : 300 ,
    'KMassWinMassMeas' : 50,
    'KMinDIRAMassMeas' : 0.9998,
    'KMaxIpChi2MassMeas' : 25,
    'KVtxChi2MassMeas': 20,
    'KMinVDChi2MassMeas' : 48,
    'KDauMinIpChi2MassMeas' : 4,
    #Kaons stuff downstream
    'KMaxDOCAMassMeasDown' : 9999.,
    'KMinPTMassMeasDown' : 200 ,
    'KMassWinMassMeasDown' : 100,
    'KMinDIRAMassMeasDown' : 0.999,
    'KVtxChi2MassMeasDown': 20,
    'KMinVDChi2MassMeasDown' : 36,
    'KDauMinIpChi2MassMeasDown' : 4,
    'KVDPVMinMassMeasDown' : 700,
    'KVDPVMaxMassMeasDown' : 2300
    },
    'WGs' : [ 'RD' ],
    'STREAMS' : [ 'Dimuon' ]
    }



## Kshort2MuMuMuMu = {
##     'BUILDERTYPE' : 'Kshort2MuMuMuMuConf' ,
##     'CONFIG'      : {
##     'MuonP'         : 3000. ,    #MeV
##     'MuonPT'        : 500.  ,    #MeV
##     'MuonMINIPCHI2' : 2     ,    #adminensional
##     'MuonTRCHI2'    : 5     ,    #adminensional
    
##     #4body
##     'MINIPCHI2_mumumumu' : 4. ,  #adminensional --  for the individual pions
##     'PT_mumumumu'        : 300 , #MeV
##     'MuonPIDmu_mumumumu' : -1,   #adimensional
##     'DPT_mumumumu'           : 2500.,     #MeV
##     'DVCHI2DOF_mumumumu'     : 8     ,    #adminensional         
##     'DMAXDOCA_mumumumu'      : 0.2  ,    #mm
##     'KsDauMAXIPCHI2_mumumumu' : 15    ,    #adimensinal
##     'DFDCHI2_mumumumu'       : 9     ,   #adimensional
##     'DIPCHI2_mumumumu'       : 20    ,    #adimensional
    
    
##     'DDIRA'         : 0.9999,    #adimensional
##     'MaxDimuonMass' : 260.  ,    #MeV
##     'MaxKsMass'     : 550.  ,    #MeV, comb mass window
##     'KsMotherMassCut' : 540.  ,    # MeV, mother mass cut
    
##     'Kshort2MuMuMuMuLinePrescale'    : 1 ,
##     'Kshort2MuMuMuMuLinePostscale'   : 1
##     },
##     'WGs'     : [ 'RD' ],
##     'STREAMS' : [ 'Dimuon' ]
##     }

Kshort2eePiPi = {
    'BUILDERTYPE' : 'Kshort2PiPieeConf' ,
    'CONFIG'      : {
    'ePT'        : 100.  ,    #MeV
    'eMINIPCHI2' : 50    ,    #adimensional
    'eTRCHI2'    : 3     ,    #adimensional
    'ePIDe'      : -4    ,    #adimensional
    'eGhostProb' : 0.3   ,    #adimensional
    
    'PionPT'        : 100.  ,    #MeV
    'PionMINIPCHI2' : 50    ,    #adimensional
    'PionTRCHI2'    : 3     ,    #adimensional
    'PionPIDK'      : 5     ,    #adimensional
    'PionGhostProb' : 0.3   ,    #adimensional
    
    #4body         
    'KsMAXDOCA'        : 0.3       , #mm
    'KsLifetime'       : 0.01*89.53, #adimensional
    'KsIP'             : 1         , #mm
    'MaxKsMass'        : 800.  ,    #MeV, comb mass high limit
    
    'Kshort2PiPieeLinePrescale'    : 1 ,
    'Kshort2PiPieeLinePostscale'   : 1
    },
    'WGs'     : [ 'RD' ],
    'STREAMS' : [ 'Dimuon' ]
    }


Kshort2PiPiMuMu = {
    'BUILDERTYPE' : 'Kshort2PiPiMuMuConf' ,
    'CONFIG'      : {
    'MuonP'         : 3000. ,    #MeV
    'MuonPT'        : 500.  ,    #MeV
    'MuonMINIPCHI2' : 2     ,    #adminensional
    'MuonTRCHI2'    : 5     ,    #adminensional
    
    'PionP'         : 2000. ,    #MeV
    'PionPT'        : 300.  ,    #MeV
    'PionMINIPCHI2' : 2     ,    #adminensional
    'PionTRCHI2'    : 5     ,    #adminensional
    
    
    #4body
    'MINIPCHI2_hhmumu' : 4. ,  #adminensional --  for the individual pions
    'PT_hhmumu'        : 300 , #MeV
    'MuonPIDmu_hhmumu' : -1,   #adimensional
    'DPT_hhmumu'           : 2500.,     #MeV
    'DVCHI2DOF_hhmumu'     : 8     ,    #adminensional         
    'DMAXDOCA_hhmumu'      : 0.2  ,    #mm
    'KsDauMAXIPCHI2_hhmumu' : 15    ,    #adimensinal
    'DFDCHI2_hhmumu'       : 9     ,   #adimensional
    'DIPCHI2_hhmumu'       : 20    ,    #adimensional
    
    
    'DDIRA'         : 0.9999,    #adimensional
    'MaxDimuonMass'    : 260.  ,    #MeV
    'MaxKsMass'                   : 550.  ,    #MeV, comb mass window
    'KsMotherMassCut'             : 540.  ,    # MeV, mother mass cut
    
    'Kshort2PiPiMuMuLinePrescale'    : 1 ,
    'Kshort2PiPiMuMuLinePostscale'   : 1
    },
    'WGs'     : [ 'RD' ],
    'STREAMS' : [ 'Dimuon' ]
   } 

K0s2Pi0MuMu = { 
    'BUILDERTYPE' : 'K0s2Pi0MuMuLinesConf',
    'WGs'         : [ 'RD' ],
    'STREAMS'     : [ 'Dimuon' ],
    'CONFIG'      : {
    'SignalLinePrescale'    : 1,
    'SignalLinePostscale'   : 1,
    'SidebandLinePrescale'  : 0.2,
    'SidebandLinePostscale' : 1,
    "muIpChi2"              : 36,
    "muTrChi2Dof"           : 5,
    "KSsignalminMass"       : 300,
    "KSsignalmaxMass"       : 600,
    "KSsidebminMass"        : 600,
    "KSsidebmaxMass"        : 1000,
    "KSdoca"                : 0.3,
    "KSdira"                : 0,
    "KSlife"                : 0.06*89.53,
    "KSip"                  : 0.9
    }
    }

K0S24X = {
    'BUILDERTYPE' : 'K0S24XConf',
    'CONFIG'      : {
     'MuonMINIPCHI2' : 36.  #adminensional
     , 'MuonTRCHI2'    : 5.   #adminensional
     , 'mmmm_MaxDimuonMass'   :  400. # MeV
     , 'mmmm_KS0_MassWin'     : 1000. #MeV
     , 'mmmm_KsMotherMass_min':  400. #MeV
     , 'mmmmL_DMAXDOCA'       :  0.6 # mm
     , 'mmmmD_DMAXDOCA'       : 10.  # mm
     , 'mmmm_DIRA'            :  0.  #adimensional
     , 'mmmmL_MinIP_dist'     : 20.  #mm
     , 'mmmmD_MinIP_dist'     : 20.  #mm
     , 'mmmm_lifetimeCut'     : 0.05*89.53*2.9979e-01 #
     , 'K0S24MuLLinePrescale' : 1 
     , 'K0S24MuLLinePostscale' : 1 
     , 'K0S24MuDLinePrescale'  : 1 
     , 'K0S24MuDLinePostscale' : 1
    },
    'WGs'     : [ 'RD' ],
    'STREAMS' : [ 'Dimuon' ]
    }


