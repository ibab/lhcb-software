"""
Line config dictionaries for the EW Stream. Mostly QEE WG lines


Dictionary names should have the same name as the corresponding
StrippingSelections file containing the line builder instance.

   e.g  StrippingPromptCharm linebuilder should be added as:

      PromptCharm = { ... }
"""
from GaudiKernel.SystemOfUnits import GeV,MeV, mm
#LowMult
LowMult = {
    'BUILDERTYPE'  : 'LowMultConf',
    'CONFIG'       : {

    'LowMultPrescale'           : 1.0
    , 'LowMultZeroPrescale'     : 0.0
    , 'LowMultWSPrescale'       : 0.1
    , 'LowMultHHIncPrescale'    : 0.1
    , 'LowMultLMRPrescale'      : 0.2
    , 'LowMultPrescale_ps'      : 0.005
    , 'LowMultNoFilterPrescale' : 0.1
    , 'LowMultPostscale'        : 1.0
    # Final-state particles
    , 'H_PTmin'         : 100.0 * MeV
    , 'H_Pmin'          : 5000.0 * MeV
    , 'H_TrkChi2max'    : 3.0
    , 'K_PIDKmin'       : 0.0
    , 'P_PIDPmin'       : 0.0
    , 'KKInc_K_Pmin'    : 10000.0 * MeV
    , 'KKInc_K_PIDKmin' : 5.0
    , 'Pi0R_PTmin'      : 100.0 * MeV
    , 'Pi0M_PTmin'      : 200.0 * MeV
    , 'Pi0R_CLmin'      : 0.2
    , 'Pi0D_AMmin'      : 70.0 * MeV
    , 'Pi0D_AMmax'      : 210.0 * MeV
    , 'Ks_ADMASSmax'    : 50.0 * MeV
    # D0 -> KPi
    , 'D2KPi_APTmin'        : 0.0 * MeV
    , 'D2KPi_ADAMASSmax'    : 80.0 * MeV
    , 'D2KPi_ADOCAmax'      : 0.5 * mm
    , 'D2KPi_APmin'         : 10000.0 * MeV
    , 'D2KPi_VtxChi2DoFmax' : 15.0
    # D+- -> KPiPi
    , 'D2KPiPi_APTmin'        : 0.0 * MeV
    , 'D2KPiPi_ADAMASSmax'    : 80.0 * MeV
    , 'D2KPiPi_ADOCAmax'      : 0.5 * mm
    , 'D2KPiPi_APmin'         : 10000.0 * MeV
    , 'D2KPiPi_VtxChi2DoFmax' : 15.0
    # D0 -> K3Pi
    , 'D2K3Pi_APTmin'        : 0.0 * MeV
    , 'D2K3Pi_ADAMASSmax'    : 80.0 * MeV
    , 'D2K3Pi_ADOCAmax'      : 0.7 * mm
    , 'D2K3Pi_APmin'         : 10000.0 * MeV
    , 'D2K3Pi_VtxChi2DoFmax' : 15.0
    # D0 -> KPiPi0
    , 'D2KPiPi0_APTmin'        : 0.0 * MeV
    , 'D2KPiPi0_ADAMASSmax'    : 80.0 * MeV
    , 'D2KPiPi0_ADOCAmax'      : 0.5 * mm
    , 'D2KPiPi0_APmin'         : 10000.0 * MeV
    , 'D2KPiPi0_VtxChi2DoFmax' : 15.0
    # D0 -> KsPiPi
    , 'D2KsPiPi_APTmin'        : 0.0 * MeV
    , 'D2KsPiPi_ADAMASSmax'    : 80.0 * MeV
    , 'D2KsPiPi_ADOCAmax'      : 0.5 * mm
    , 'D2KsPiPi_APmin'         : 10000.0 * MeV
    , 'D2KsPiPi_VtxChi2DoFmax' : 15.0
    # D* -> D0 pi
    , 'Dstar2D0Pi_APTmin'        : 0.0 * MeV
    , 'Dstar2D0Pi_ADAMASSmax'    : 100.0 * MeV
    , 'Dstar2D0Pi_DeltaMmin'     : 135.0 * MeV
    , 'Dstar2D0Pi_DeltaMmax'     : 200.0 * MeV
    , 'Dstar2D0Pi_VtxChi2DoFmax' : 15.0
    # 'ChiC' -> HH (H = K, Pi)
    , 'ChiC2HH_APTmin'        : 0.0 * MeV
    , 'ChiC2HH_APTmax'        : 5000.0 * MeV
    , 'ChiC2HH_AMmin'         : 2850.0 * MeV
    , 'ChiC2HH_AMmax'         : 4500.0 * MeV
    , 'ChiC2HH_ADOCAmax'      : 0.5 * mm
    , 'ChiC2HH_APmin'         : 10000.0 * MeV
    , 'ChiC2HH_VtxChi2DoFmax' : 15.0
    # 'ChiC' -> PP
    , 'ChiC2PP_APTmin'        : 0.0 * MeV
    , 'ChiC2PP_APTmax'        : 5000.0 * MeV
    , 'ChiC2PP_AMmin'         : 2850.0 * MeV
    , 'ChiC2PP_AMmax'         : 3650.0 * MeV
    , 'ChiC2PP_ADOCAmax'      : 0.5 * mm
    , 'ChiC2PP_APmin'         : 10000.0 * MeV
    , 'ChiC2PP_VtxChi2DoFmax' : 15.0
    # 'ChiC' -> HHHH (H = K, Pi)
    , 'ChiC2HHHH_APTmin'        : 0.0 * MeV
    , 'ChiC2HHHH_APTmax'        : 5000.0 * MeV
    , 'ChiC2HHHH_AMmin'         : 2850.0 * MeV
    , 'ChiC2HHHH_AMmax'         : 4500.0 * MeV
    , 'ChiC2HHHH_ADOCAmax'      : 0.7 * mm
    , 'ChiC2HHHH_APmin'         : 10000.0 * MeV
    , 'ChiC2HHHH_VtxChi2DoFmax' : 15.0
    # Low-mass resonance -> HH (H = K, Pi)
    , 'LMR2HH_APTmin'        : 500.0 * MeV
    , 'LMR2HH_APTmax'        : 1500.0 * MeV
    , 'LMR2HH_AMmin'         : 450.0 * MeV
    , 'LMR2HH_AMmax'         : 1500.0 * MeV
    , 'LMR2HH_ADOCAmax'      : 0.1 * mm
    , 'LMR2HH_APmin'         : 15000.0 * MeV
    , 'LMR2HH_VtxChi2DoFmax' : 3.0
    # Phi resonance -> KK
    , 'PHI2KK_APTmin'        : 0.0 * MeV
    , 'PHI2KK_APTmax'        : 1500.0 * MeV
    , 'PHI2KK_AMmin'         : 990.0 * MeV
    , 'PHI2KK_AMmax'         : 1050.0 * MeV
    , 'PHI2KK_ADOCAmax'      : 0.1 * mm
    , 'PHI2KK_APmin'         : 4000.0 * MeV
    , 'PHI2KK_VtxChi2DoFmax' : 3.0
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : {
     'EW' : [
       'StrippingLowMultCEP_D2KPiPi0R_line',
       'StrippingLowMultCEP_D2KPiPi0M_line',
       'StrippingLowMultCEP_D2KPiPi0D_line',
       'StrippingLowMultCEP_D2KsPiPiLL_line',
       'StrippingLowMultCEP_D2KsPiPiDD_line',
       'StrippingLowMultCEP_Dstar2D0Pi_KPi_line',
       'StrippingLowMultCEP_Dstar2D0Pi_K3Pi_line',
       'StrippingLowMultCEP_Dstar2D0Pi_KPiPi0R_line', 
       'StrippingLowMultCEP_Dstar2D0Pi_KPiPi0M_line',
       'StrippingLowMultCEP_Dstar2D0Pi_KPiPi0D_line',
       'StrippingLowMultCEP_Dstar2D0Pi_KsPiPiLL_line',
       'StrippingLowMultCEP_Dstar2D0Pi_KsPiPiDD_line',
       'StrippingLowMultCEP_D2KPi_D2KPi_line',
       'StrippingLowMultCEP_D2K3Pi_D2K3Pi_line',
       'StrippingLowMultCEP_D2KsPiPiLL_D2KsPiPiLL_line',
       'StrippingLowMultCEP_D2KsPiPiDD_D2KsPiPiDD_line',
       'StrippingLowMultCEP_D2KsPiPiLL_D2KsPiPiDD_line',
       'StrippingLowMultCEP_D2KPi_D2KPiPi0R_line',
       'StrippingLowMultCEP_D2KPi_D2K3Pi_line',
       'StrippingLowMultCEP_D2KPi_D2KsPiPiLL_line',
       'StrippingLowMultCEP_D2KPi_D2KsPiPiDD_line',
       'StrippingLowMultCEP_D2K3Pi_D2KPiPi0R_line',
       'StrippingLowMultCEP_D2K3Pi_D2KsPiPiLL_line',
       'StrippingLowMultCEP_D2K3Pi_D2KsPiPiDD_line',
       'StrippingLowMultCEP_D2KmPipPip_D2KmPipPip_line',
       'StrippingLowMultCEP_D2KpPipPim_D2KpPipPim_line',
       'StrippingLowMultCEP_D2KmPipPip_D2KpPipPim_line',
       'StrippingLowMultCEP_D2KmPipPip_Dstar2D0Pi_KPi_line',
       'StrippingLowMultCEP_D2KmPipPip_Dstar2D0Pi_K3Pi_line',
       'StrippingLowMultCEP_D2KmPipPip_Dstar2D0Pi_KPiPi0R_line',
       'StrippingLowMultCEP_D2KmPipPip_Dstar2D0Pi_KsPiPiLL_line',
       'StrippingLowMultCEP_D2KmPipPip_Dstar2D0Pi_KsPiPiDD_line',
       'StrippingLowMultCEP_Dstar2D0Pi_KPi_Dstar2D0Pi_KPi_line',
       'StrippingLowMultCEP_Dstar2D0Pi_KPi_Dstar2D0Pi_K3Pi_line',
       'StrippingLowMultCEP_Dstar2D0Pi_KPi_Dstar2D0Pi_KPiPi0R_line',
       'StrippingLowMultCEP_Dstar2D0Pi_KPi_Dstar2D0Pi_KsPiPiLL_line',
       'StrippingLowMultCEP_Dstar2D0Pi_KPi_Dstar2D0Pi_KsPiPiDD_line']
     }
    }

#StrangeBaryonsNoPID
StrangeBaryonsNoPID = {
    'BUILDERTYPE'  : 'StrippingStrangeBaryonsNoPIDConf',
    'CONFIG'       : {
                   'PreScale'   : 1,
                   'checkPV'   : False,
                   'HLT' : "HLT_PASS('Hlt1MBNoBiasDecision')|HLT_PASS('Hlt1MBMicroBiasTStationDecision')|HLT_PASS('Hlt1MBMicroBiasVeloDecision')|HLT_PASS('Hlt1MBMicroBiasTStationRateLimitedDecision')|HLT_PASS('Hlt1MBMicroBiasVeloRateLimitedDecision')",
                   'ProtonPIDppi'              :       -5.,  #(PIDp-PIDpi) > -5                   
                   'PionPIDpiK'              :       0.,   #(PIDp-PIDK) > 0
                   
                   # Lambda Decay
                   'TRCHI2DOF'              :       4., # < 4 for all 6 cases
                   'minCHI2IPPV_pPi_LL'    :      20., # > 20
                   'minCHI2IPPV_pPi'              :       4., # > 4 for the rest of 4 cases
                   'CHI2VTX_L'                   :      15., # < 15 for all 6 cases
                   'L_FDCHI2_OWNPV_LL'    :     150., # > 150
                   'L_FDCHI2_OWNPV'              :     100., # > 100 for the other 5 cases

                   'L_FDCHI2_OWNPV_LL_Omega'    :     70., # > 150
                   'L_FDCHI2_OWNPV_Omega'              :     70., # > 100 for the other 5 cases
       
                   'minCHI2IPPV_L_LL'     :       9., # > 9
                   'minCHI2IPPV_L'               :       2., # > 2 for the other 5 cases
                   'Lambda0MassWindow'            :       30., # < 6 for all six cases
                   'Lambda0MassWindowPost'            :	  6., # < 6 for all six cases

                   
                   #Bachelor cuts

                   'minCHI2IPPV_Pi_Bachelor_LLL' :      10., # > 10
                   'minCHI2IPPV_Pi_Bachelor_DDD' :       4., # > 4
                   'minCHI2IPPV_Bachelor'     :       3., # > 3 for the other 3 cases: Tight DDL & DDD + Loose DDL
                   'minCHI2IPPV_K_Bachelor_D' :      3., # > 6
                   'minCHI2IPPV_K_Bachelor_L' :      3., # should have been 10 but use it for DDL case too , reduced to 3
                   
                   # Xi Decay
                   'CHI2VTX_Xi'                  :      25., # < 25 for all 6 cases
                   'Xi_FDCHI2_OWNPV_LLL'   :      30., # > 30
                   'Xi_FDCHI2_OWNPV_DDL'   :      15., # > 15
                   'Xi_FDCHI2_OWNPV'             :       5., # > 5 for the other 4 cases
                   'COS_L_Xi'                    :  0.9996, # > 0.9996 for all 6 cases
                   'XiMassWindow'              :      50., #?????????


                   # Omega Decay
                   'CHI2VTX_Omega'                  :      9., # < 25 for all 6 cases
                   'Omega_FDCHI2_OWNPV'             :      10., # > 5 for the other 4 cases
                   'COS_L_Omega'                    :      0.9996, # > 0.9996 for all 6 cases
                   'OmegaMassWindow'                :      50., #?????????
                 
                },
                'WGs' : [ 'QEE' ],
                'STREAMS' : [ 'EW' ]
               }


#Dijets
Dijets = {
    'BUILDERTYPE'  : 'DijetsConf',
    'CONFIG'       : {
    # Prescale for the calibration line.
    "PRESCALE" : 0.03,
    # HLT properties.
    "HLT"   : {"LINE" : "Hlt1TrackMuon"},   # Line to use.
    # Track properties.
    "TRK"   : {"MAX_MULT"      : 250,       # Multiplicity.
               "MIN_P"         : 5*GeV,     # Momentum.
               "MIN_PT"        : 500*MeV,   # Transverse momentum.
               "MIN_MIPCHI2DV" : 16,        # Impact parameter chi-squared.
               "MAX_GHP"       : 0.4},      # Ghost probability.
    # Muon properties.
    "MUO"   : {"MIN_PT"        : 2*GeV,     # Transverse momentum.
               "MIN_MIPCHI2DV" : 16},       # Impact parameter chi-squared.
    # Secondary vertex properties.
    "SVR"   : {"MAX_CHI2DOCA"  : 8,         # Chi-squared of closest approach.
               "MIN_BPVDIRA"   : 0,         # Cos of angle between P and flight.
               "MIN_BPVVDCHI2" : 100,       # Chi-squared distance with PV.
               "MAX_CHI2"      : 8,         # Chi-squared.
               "MAX_M"         : 7*GeV,     # Combined mass.
               "MIN_SUM_PT"    : 2*GeV},    # Scalar sum of transverse momenta.
    # Fully reconstructed jet properties.
    "JET"   : {"STDJETS"       : False,     # Flag to use StdJets container.
               "PF"            : True,      # Flag to use particle flow.
               "JEC"           : False,     # If no STDJETS, apply JEC.
               "R"             : 0.7,       # If no STDJETS, set jet radius.
               "MIN_PT"        : 19*GeV},   # Transverse momentum.
    # Pair of secondary vertices properties.
    "DISVR" : {"MAX_COSDPHI"   : 0,         # Cos of transverse angle.
               "MAX_COSDALPHA" : 0.99,      # Cos of opening angle.
               "MIN_M"         : 2*GeV,     # Combined mass.
               "MIN_SUM_PT"    : 10*GeV},   # Scalar sum of transverse momenta.
    # Pair of jets properties.
    "DIJET" : {"MAX_COSDPHI"   : -0.8}      # Cos of transverse angle.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'BhadronCompleteEvent' ]
   }

#H24Mu
H24MuLines = {
    'BUILDERTYPE'  : 'H24MuLineConf',
    'CONFIG'       : {
    'DefaultPostscale'       : 1,
    'PromptLinePrescale'     : 1,
    'SimpleLinePrescale'     : 1,
    'DetachedLinePrescale'   : 1,
    'LooseLinePrescale'      : 0.01,
    
    'MuTrackChi2DoF'         : 3,
    'MupTprompt'             : 350,  #MeV
    'MupTdetached'           : 150,  #MeV
    'MuGhostProb'            : 0.4,
    'MuMaxIPchi2'            : 4,
    'MuMinIPchi2'            : 1,
    'MuPIDdll'               : -3, # muon combDLL
    'MuNShared'              : 3, # muon NShared
    
    'A1maxMass'              : 2000, #MeV
    'A1Doca'                 : 0.3,   #mm
    'A1DocaTight'            : 0.1,   #mm
    'A1Vchi2'                : 10,
    'A1Vchi2Tight'           : 1,
    'A1Dira'                 : 0,
    'A1maxIPchi2'            : 36,
    'A1FDChi2'               : 4,

    'HmaxDOCA'               : 1, #mm
    'HmaxDOCATight'          : 0.25, #mm
    'HVchi2'                 : 15,
    'HVchi2Tight'            : 2,
    'HpT'                    : 1000, #MeV

    'MuTrackChi2DoF_loose'   : 10,
    'MupT_loose'             : 0,
    'MuMaxIPchi2_loose'      : 1000000,
    'A1maxMass_loose'        : 5000, #MeV
    'A1Doca_loose'           : 10, #mm
    'A1Vchi2_loose'          : 20,
    'HmaxDOCA_loose'         : 1000000, #mm
    'HpT_loose'              : 300, #MeV
    'HVchi2_loose'           : 50
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'Leptonic' ]
   }

#H24MuSameMassLine
H24MuSameMassLine = {
    'BUILDERTYPE'  : 'H24MuSameMassLineConf',
    'CONFIG'       : {
    'DefaultPostscale'       : 1,
    'PromptLinePrescale'     : 1,
    'SimpleLinePrescale'     : 0.1,
    'DetachedLinePrescale'   : 1,
    
    'MuTrackChi2DoF'         : 3,
    'MupTprompt'             : 400,  #MeV
    'MupTdetached'           : 200,  #MeV
    'MuGhostProb'            : 0.4,
    'MuMaxIPchi2'            : 2,
    'MuMinIPchi2'            : 1,
    'MuPIDdll'               : -3, # muon combDLL
    'MuNShared'              : 2, # muon NShared
    
    'A1maxMass'              : 3500, #MeV
    'A1Doca'                 : 0.2,   #mm
    'A1DocaTight'            : 0.1,   #mm
    'A1Vchi2'                : 5,
    'A1Vchi2Tight'           : 1,
    'A1Dira'                 : 0,
    'A1maxIPchi2'            : 16,
    'A1FDChi2'               : 4,

    'A1sDeltaMass'           : 200, #MeV
    'HmaxDOCA'               : 1, #mm
    'HmaxDOCATight'          : 0.25, #mm
    'HVchi2'                 : 10,
    'HVchi2Tight'            : 2,
    'HpT'                    : 1200, #MeV
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
   }

#WeAKTJets
WeAKTJets = {
    'BUILDERTYPE'  : 'WeAKTJetsConf',
    'CONFIG'       : {
    'WeAKTJets_Prescale'    : 0.1,
    'WeAKTJets_Postscale'   : 1.0,
    'min_e_pT'              : 10.,
    'max_e_pT'              : 200000.,
    #'max_e_pT'              : 20.,
    'dr_lepton_jet'         : 0.5,
    'PrsCalMin'             : 50.,
    'ECalMin'               : 0.10,
    'HCalMax'               : 0.05,
    'min_jet_pT'            : 15.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'BhadronCompleteEvent' ]
   }

#WmuAKTJets
WmuAKTJets = {
    'BUILDERTYPE'  : 'WmuAKTJetsConf',
    'CONFIG'       : {
    'WmuAKTJets_Prescale'    : 0.1,
    'WmuAKTJets_Postscale'   : 1.0,
    'min_mu_pT'              : 10.,
    'max_mu_pT'              : 200000.,
    #'max_mu_pT'              : 20.,
    'dr_lepton_jet'          : 0.5,
    'min_jet_pT'             : 15.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'BhadronCompleteEvent' ]
   }

#DisplVertices
DisplVertices = {                        
    'BUILDERTYPE'  : 'DisplVerticesLinesConf',
    'CONFIG'       : { 
    ## Velo GEC
    "VeloGEC"                 : { "Apply"                : True
                                , "MaxVeloRatio"         : 0.1
                                , "MaxPhiVectorSize"     : 500.
                                }
    ## Velo tracks filter
    , "FilterVelo"              : { "Apply"                : True
                                  , "MinIP"                : 0.1*mm
                                  , "MinIPChi2"            : -1.0
                                  , "MinNumTracks"         : 4
                                  , "PVLocation"           : "Rec/Vertex/Primary"
                                  , "RemoveBackwardTracks" : True
                                  , "RemovePVTracks"       : True
                                  , "RemoveVeloClones"     : True
                                  }

    #==========        SELECTION CUT VALUES         ==========#

    ## Velo reconstruction
    , "RV2PWithVelo"            : { "MinRho"               :  0.4*mm
                                  , "MinMass"              :  0.0*GeV
                                  , "MinSumPT"             :  0.0*GeV
                                  , "MinNumTracks"         :  4
                                  , "MaxFractE1Track"      :  10.
                                  , "MaxFractTrwHitBefore" :  10.
                                  , "ApplyMatterVeto"      :  False
                                  ## Reco only
                                  , "MaxChi2NonVeloOnly"   : 5.0
                                  , "UseVeloTracks"        : False
                                  , "ComputeMatterVeto"    : True
                                  , "FirstPVMaxRho"        : 0.3*mm
                                  , "FirstPVMinZ"          : -300.0*mm
                                  , "FirstPVMaxZ"          : 500.0*mm
                                  , "FirstPVMinNumTracks"  : 10
                                  }
    ## Single LLP line
    #, "SingleSelection"         : { "PreScale"             :  1.0
    #                                                         #       HighMass,         Medium,         HighFD
    #                              , "MinRho"               : [  1.0*mm ,  2.*mm ,  4.*mm  ]
    #                              , "MinMass"              : [ 8.*GeV,  6.5*GeV,  4.5*GeV ]
    #                              , "MinSumPT"             :  3.0*GeV
    #                              , "MinNumTracks"         : [  8            ,  7            ,  7             ]
    #                              , "ApplyMatterVeto"      : True
    #                              , "MaxFractE1Track"      :  0.8
    #                              , "MaxFractTrwHitBefore" :  0.49
    #                              }
    ## with the jet sequence
    , "JetSingleLowMassSelection" : { "PreScale"           :  1.0
                                    , "MinRho"               :  0.4
                                    , "MinMass"              :  0.0*GeV
                                    , "MinSumPT"             : 10.0*GeV
                                    , "MinNumTracks"         :  5
                                    , "ApplyMatterVeto"      : True
                                    , "MaxFractE1Track"      :  0.8
                                    , "MaxFractTrwHitBefore" :  0.49
                                    ## Jet-related cuts
                                    , "MinNumJets"           :  1
                                    , "SingleJet"            :  True
                                    , "MinNJetMass"          : 10.0*GeV
                                    , "MinNJetTransvMass"    : 33.0*GeV
                                    , "JetIDCut"             : "( JNWITHPVINFO >= 5 )"
                                    } # Tuning: done, 0.053 % 
   , "JetSingleHighMassSelection" : { "PreScale"          :  1.0
                                    , "MinRho"               :  0.4
                                    , "MinMass"              :  5.0*GeV
                                    , "MinSumPT"             :  7.0*GeV
                                    , "MinNumTracks"         :  5
                                    , "ApplyMatterVeto"      : True
                                    , "MaxFractE1Track"      :  0.8
                                    , "MaxFractTrwHitBefore" :  0.49
                                    ## Jet-related cuts
                                    , "MinNumJets"           :  2
                                    , "SingleJet"            :  False
                                    , "MinNJetMass"          :  13.0*GeV
                                    , "MinNJetTransvMass"    : None
                                    , "JetIDCut"             : None
                                    }
      ## jet sequence on top of Hlt2 candidates
      ## NOTE the only case where the mass is "wrong", is for a tightened JetID in the Hlt-based jet line (which will produce an error at config time)
      , "JetHltSingleLowMassSelection": { "PreScale"         :  1.0
                                           ## Jet-related cuts
                                        , "MinNumJets"           :  1
                                        , "SingleJet"            :  True
                                        , "MinNJetMass"          :  8.0*GeV
                                        , "MinNJetTransvMass"    : None
                                        , "JetIDCut"             : "( JNWITHPVINFO >= 3 ) & ( JMPT > 1800. )"
                                        } # Tuning: done - 0.04 %
      , "JetHltSingleHighMassSelection" : { "PreScale"       :  1.0
                                      ## Jet-related cuts
                                          , "MinNumJets"           :  2
                                          , "SingleJet"            :  False
                                          , "MinNJetMass"          :  0.0*GeV
                                          , "MinNJetTransvMass"    : None
                                          , "JetIDCut"             : None
                                          } # Tuning: fine like this - can cut harder on mass

        ## Double LLP line
        # , "DoubleSelection"         : { "PreScale"             :  1.0
        #                               , "MinRho"               :  0.6*mm
        #                               , "MinMass"              :  3.0*GeV
        #                               , "MinSumPT"             :  3.0*GeV
        #                               , "MinNumTracks"         :  6
        #                               , "ApplyMatterVeto"      :  False
        #                               , "MaxFractE1Track"      :  0.8
        #                               , "MaxFractTrwHitBefore" :  0.49
        #                               ## Double only
        #                               , "MinHighestMass"       : 3.0*GeV
        #                               , "ApplyMatterVetoOne"   : True
        #                               }

        ## Downstream reconstruction
        , "RV2PDown"                : { "MinRho"               :  2.0*mm
                                      , "MinMass"              :  3.0*GeV
                                      , "MinSumPT"             :  0.0*GeV
                                      , "MinNumTracks"         :  4
                                      , "MaxFractE1Track"      :  10.
                                      , "MaxFractTrwHitBefore" :  10.
                                      , "ApplyMatterVeto"      :  False
                                      ## Reco only
                                      , "MaxChi2NonVeloOnly"   : 5.0
                                      , "UseVeloTracks"        : False
                                      , "ComputeMatterVeto"    : False
                                      , "FirstPVMaxRho"        : 0.3*mm
                                      , "FirstPVMinZ"          : -300.0*mm
                                      , "FirstPVMaxZ"          : 500.0*mm
                                      , "FirstPVMinNumTracks"  : 10
                                      }
        # , "SingleDownSelection"     : { "PreScale"             :  1.0
        #                               , "MinRho"               :  2.0*mm
        #                               , "MinMass"              :  3.0*GeV
        #                               , "MinSumPT"             :  0.0*GeV
        #                               , "MinNumTracks"         :  4
        #                               , "ApplyMatterVeto"      : False
        #                               , "MaxFractE1Track"      :  10.
        #                               , "MaxFractTrwHitBefore" :  10.
        #                               # Down only
        #                               , "MinZ"                 :  170.*mm
        #                               }

        #========== Other lines for efficiency studies ==========#
        # , "HLTPS"             : { "PreScale"             :  1.0 }
        # , "HltEffCharmHLTSelection"  : { "PreScale"             :  1.0
        #                               , "MinRho"               :  0.6*mm
        #                               , "MinMass"              :  6.*GeV
        #                               , "MinSumPT"             :  3.0*GeV
        #                               , "MinNumTracks"         :  6
        #                               , "ApplyMatterVeto"      : False
        #                               , "MaxFractE1Track"      :  10.
        #                               , "MaxFractTrwHitBefore" :  10.
        #                               }

        #==========     HLT filters for all lines      ==========#
        , "HLT"                     : {# "CharmHLT"     : "HLT_PASS('Hlt2CharmHadD02HH_D02KPiDecision')"
        #                              , "HLTPS"        : [ ( ("0x001c0028", "0x002f002c"), "HLT_PASS_RE('Hlt2DisplVerticesSinglePostScaledDecision')" )
        #                                                 , ( ("0x00340032", "0x00730035"), "HLT_PASS_RE('Hlt2DisplVerticesSinglePostScaledDecision')" )
        #                                                 , ( ("0x00750037", "0x007b0038"), "HLT_PASS_RE('Hlt2DisplVertices(Single|Double|SingleMV)PostScaledDecision')" )
        #                                                 , ( ("0x007e0039", "0x0097003d"), "HLT_PASS_RE('Hlt2DisplVertices(Single|Double|SingleMV)PostScaledDecision')" )
        #                                                 , ( ("0x00990042", "0x40000000"), "HLT_PASS_RE('Hlt2DisplVertices(Single|SingleLoose|Double)PSDecision')" )
        #                                                 ]
                                      ## For reviving Hlt2 candidates
                                        "SignalLines"  : [ ( ("0x001c0028", "0x002f002c"), ["Hlt2DisplVerticesSingleDecision"] )
                                                         , ( ("0x00340032", "0x00730035"), ["Hlt2DisplVerticesHighFDSingleDecision","Hlt2DisplVerticesHighMassSingleDecision", "Hlt2DisplVerticesLowMassSingleDecision", "Hlt2DisplVerticesSingleDownDecision"] )
                                                         , ( ("0x00750037", "0x007b0038"), ["Hlt2DisplVerticesSingleDecision", "Hlt2DisplVerticesSingleDownDecision", "Hlt2DisplVerticesSingleHighFDPostScaledDecision", "Hlt2DisplVerticesSingleHighMassPostScaledDecision"] )
                                                         , ( ("0x007e0039", "0x0097003d"), ["Hlt2DisplVerticesSingleDecision", "Hlt2DisplVerticesSingleDownDecision", "Hlt2DisplVerticesSingleHighFDPostScaledDecision", "Hlt2DisplVerticesSingleHighMassPostScaledDecision"] )
                                                         , ( ("0x00990042", "0x40000000"), ["Hlt2DisplVerticesSingleDecision", "Hlt2DisplVerticesSingleDownDecision", "Hlt2DisplVerticesSingleHighFDDecision", "Hlt2DisplVerticesSingleHighMassDecision", "Hlt2DisplVerticesSingleVeryHighFDDecision"] )
                                                         ]
                                      }
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'BhadronCompleteEvent' ]
   }


