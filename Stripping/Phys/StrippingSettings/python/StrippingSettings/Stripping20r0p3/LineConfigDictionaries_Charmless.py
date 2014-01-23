##
## Line config dictionaries for Charmless WG
##

from GaudiKernel.SystemOfUnits import *

##
## StrippingB2HHBDT.py
##
B2HHBDT = {
    'WGs' : ['Charmless'],
    'BUILDERTYPE' : 'B2HHBDTLines',
    'CONFIG'      : { 'PrescaleB2HHBDT' : 1.,
                      'MinPT'           : 1000,
                      'MinIP'           : 0.12,
                      'TrChi2'          : 3,
                      'TrGhostProb'     : 0.5,
                      'CombMassLow'     : 4600,
                      'CombMassHigh'    : 6400,
                      'DOCA'            : 0.1,
                      'BPT'             : 1200,
                      'BIP'             : 0.12,
                      'BTAU'            : 0.0006,
                      'MassLow'         : 4800,
                      'MassHigh'        : 6200,
                      'BDTCut'          : -0.3,
                      'BDTWeightsFile'  : "$TMVAWEIGHTSROOT/data/B2HHBDT.xml"
                    },
    'STREAMS'     : { 'Bhadron' : ['StrippingB2HHBDTLine']}
    }

##
## StrippingBu2rho0rhoPlus.py
##
Bu2rho0rhoPlus = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'StrippingBu2rho0rhoPlusConf',
    'CONFIG'      : { "isMC"                : False,
                      "trMinIPChi2PV"       : 16,
                      "trMinProbNNpi"       : 0.5,
                      "trMaxProbNNK"        : 0.5,
                      "trMaxChi2Dof"        : 3.0,
                      "trMaxGhostProb"      : 0.5,
                      "rhoCombMassMax"      : 1200,
                      "rhoCombMassMin"      : 200,
                      "rhoMothMassMax"      : 1100,
                      "rhoMothMassMin"      : 300,
                      "rho0DauMaxDocaChi2"  : 15,
                      "rho0MinDeltaZ"       : 0.0,
                      "rho0MinVChi2Dof"     : 9,
                      "rho0MinDira"         : 0.0,
                      "rho0MinVDisplChi2"   : 25,

                      "pi0ResMinP"          : 4000,
                      "pi0ResMinPT"         : 600,
                      "pi0ResDauMinCL"      : 0.2,
                      "pi0ResDauMinPT"      : 250,
                      "rhoPResMinPT"        : 1200,
                      "BuResCombMassWindow" : 750,
                      "BuResMinVChi2Dof"    : 9,
                      "BuResMothMassWindow" : 600,
                      "BuResMinDira"        : 0.99995,
                      "BuResMinPT"          : 3000,

                      "pi0MgdMinPT"         : 2200,
                      "BuMgdCombMassMin"    : 3900,
                      "BuMgdCombMassMax"    : 7150,
                      "BuMgdMinVChi2Dof"    : 9,
                      "BuMgdMothMassMin"    : 4000,
                      "BuMgdMothMassMax"    : 7000,
                      "BuMgdMinDira"        : 0.99994,
                      "PrescaleBu2rho0rhoPlusResolved" : 1.0,
                      "PrescaleBu2rho0rhoPlusMerged"   : 1.0
                    },
    'STREAMS'     : { 'Bhadron' : ['StrippingBu2rho0rhoPlusMergedLine',
                                   'StrippingBu2rho0rhoPlusResolvedLine']}
    }

##
## StrippingLb2PhipK.py
##
Lb2PhipK = {
    'WGs' : ['Charmless'],
    'BUILDERTYPE' : 'StrippingLb2PhipKConf',
    'CONFIG'      : { 'KaonPT'              : 400,      # MeV/c
                      'KaonIPCHI2'          : 4.0,      # adimensional
                      'PhiPT'               : 0,        # MeV/c
                      'PhiPTsq'             : 2.0,      # GeV*GeV/cc
                      'PhiVertexCHI2pDOF'   : 15,       # adimensional
                      'PhiMassWindow'       : 25,       # MeV/cc
                      'PhiMassMax'          : 1090,     # MeV/cc
                      'BsVertexCHI2pDOF'    : 15,       # adimensional
                      'BsMassWindow'        : 300,      # MeV/cc
                    },
    'STREAMS'     : { 'Bhadron' : ['StrippingLb2PhipKLine']}
    }

##
## StrippingBc2XK.py
##
Bc2XK = {
    'WGs' : ['Charmless'],
    'BUILDERTYPE' : 'Bc2XKConf',
    'CONFIG'      : { 'CheckPV'   : True ,
                      #
                      ## Global filter
                      # 
                      'FILTER'    : None ,   ## VOID filter 
                      'ODIN'      : None ,   ## ODIN filter 
                      'L0DU'      : None ,   ## L0   filter 
                      'HLT'       : None ,   ## HLT  filter
                      #
                      ## c*tau cut for B-hadrons 
                      #
                      'CTAU'      : 140 * micrometer ,
                      'CTAU_BC'   :  80 * micrometer ,
                      #
                      ## cuts for kstar, phi and bachelor kaons
                      # 
                      'KstarCut'   : """
                       ( PT          > 1000 * MeV               ) & 
                       in_range ( 2          , ETA , 4.9       ) &
                       in_range ( 5 * GeV  , P   , 500 * GeV ) &
                       ( MIPCHI2DV()  > 9.    ) &
                       ( CHILDCUT ( ( PT> 300*MeV) , 1 ) ) &
                       ( CHILDCUT ( ( PT> 300*MeV) , 2 ) ) 
                       """ ,
                      #
                      'PhiCut'   : """
                       ( PT          > 750 * MeV               ) & 
                       in_range ( 2          , ETA , 4.9       ) &
                       in_range ( 1 * GeV  , P   , 500 * GeV ) &
                       ( MIPCHI2DV()  > 9.    ) &
                       ( CHILDCUT ( ( PT> 300*MeV) , 1 ) ) &
                       ( CHILDCUT ( ( PT> 300*MeV) , 2 ) ) 
                       """ ,
                      #
                      'KaonCut'   : """
                       ( CLONEDIST   > 5000   ) & 
                       ( TRCHI2DOF   < 4      ) & 
                       ( TRGHOSTPROB < 0.4    ) & 
                       ( PT          > 750 * MeV               ) & 
                       in_range ( 2          , ETA , 4.9       ) &
                       in_range ( 3.2 * GeV  , P   , 150 * GeV ) &
                       HASRICH                  &
                       ( PROBNNk      > 0.20  ) &
                       ( MIPCHI2DV()  > 9.    ) 
                      """ ,
                      #
                      ## useful shortcuts:
                      #
                      'Preambulo' : [
                        ## shortcut for chi2 of vertex fit 
                        'chi2vx = VFASPF(VCHI2)   '                                ,
                        ## shortcut for the c*tau
                        "from GaudiKernel.PhysicalConstants import c_light"        ,
                        ## use the embedded cut for chi2(LifetimeFit)<25
                        #c_light was c_tau
                        "ctau      = BPVLTIME ( 25 ) * c_light "                   ,
                        "light_9    = BPVLTIME (  9 ) * c_light "                   ,
                        "ctau_16   = BPVLTIME ( 16 ) * c_light "                   ,
                        ## Combination mass-cut for beauty particles 
                        "mbp_acut  = in_range ( 5.050 * GeV , AM , 5.550 * GeV ) " ,
                        "mbc_acut  = in_range ( 6.000 * GeV , AM , 6.600 * GeV ) " ,
                        ## mass-cut for beauty particles 
                        "mbp_cut   = in_range ( 5.100 * GeV ,  M , 5.500 * GeV ) " ,
                        "mbc_cut   = in_range ( 6.050 * GeV ,  M , 6.550 * GeV ) " ,
                      ] ,
                      # =========================================================================
                      ## Prescales 
                      # =========================================================================
                      'Bc2KstKPrescale'  : 1.0 ,
                      'Bc2phiKPrescale'  : 1.0 ,
                      # =========================================================================
                    },
    'STREAMS'     : { 'Bhadron' : ['StrippingBc2KstKBc2XK',
                                   'StrippingBc2phiKBc2XK']}
    }

##
## StrippingB2pphh.py
##
B2pphh = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'B2pphhConf',
    'CONFIG'      : { 'MaxTrSIZE'          : 200 ,      ## GEC maximim recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG)
                      'CombMass12Max_kk'   : 4700.0,
                      'CombMass12Max_kpi'  : 5000.0,
                      'CombMass12Max_pipi' : 5350.0,
                      'd_achi2doca12'      : 20.0,
                      'd_achi2doca13'      : 20.0,
                      'd_achi2doca14'      : 20.0,
                      'd_achi2doca23'      : 20.0,
                      'd_achi2doca24'      : 20.0,
                      'd_achi2doca34'      : 20.0,
                      'p_PTSUM'            : 750.0,
                      'p_PSUM'             : 7000.0,
                      'p_PTMIN'            : 400.0,
                      'p_PMIN'             : 4.0,
                      'p_PROBNNpPROD'      : 0.05,
                      'CombMass123Max'     : 5600.0,
                      'CombMassMax'        : 5.60,
                      'CombMassMin'        : 5.00,
                      'MassMax'            : 5.55,
                      'MassMin'            : 5.05,
                      '4h_PTSUM'           : 3000.0,
                      'B_DIRA'             : .9999 ,
                      '4h_AMAXDOCA'        : 0.3 ,
                      'B_CHI2'             : 30.0 ,
                      'B_PT'               : 1000.,
                      'B_MIPDV'            : 0.2,
                      'PionCuts'           : "(TRCHI2DOF < 3.0) & HASRICH & (P > 1500*MeV) & (PT > 300*MeV) & (MIPCHI2DV(PRIMARY) > 6.0) & (TRGHOSTPROB < 0.35) & (PROBNNpi > 0.05)",
                      'KaonCuts'           : "(TRCHI2DOF < 3.0) & HASRICH & (P > 1500*MeV) & (PT > 300*MeV) & (MIPCHI2DV(PRIMARY) > 4.0) & (TRGHOSTPROB < 0.35) & (PROBNNk > 0.05)",
                      'ProtonCuts'         : "(TRCHI2DOF < 3.0) & HASRICH & (P > 1500*MeV) & (PT > 300*MeV) & (MIPCHI2DV(PRIMARY) > 2.0) & (TRGHOSTPROB < 0.35) & (PROBNNp > 0.05)",
                      'Prescaleppkk'       : 1.0,
                      'Prescalepppipi'     : 1.0,
                      'Prescaleppkpi'      : 1.0
                    },
    'STREAMS'     : { 'Bhadron' : ['StrippingB2pphh_kkLine',
                                   'StrippingB2pphh_kpiLine',
                                   'StrippingB2pphh_pipiLine']}
    }

##
## StrippingB2TwoBaryons.py
##
B2TwoBaryons = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'B2TwoBaryonLines',
    'CONFIG'      : { 'PrescaleB2PPbar'   : 1,
                      'MinPTB2PPbar'      : 900,
                      'MinIPChi2B2PPbar'  : 10,
                      #'TrChi2'            : 4,
                      'PIDppi'            : -1,
                      'PIDpk'             : -2,
                      'MaxPTB2PPbar'      : 2100,
                      'MaxIPChi2B2PPbar'  : 25,
                      'CombMassWindow'    : 200,
                      'VertexChi2B2PPbar' : 9,
                      'BPTB2PPbar'        : 1100,
                      'BIPChi2B2PPbar'    : 16,
                      'BDIRA'             : 0.9997,
                      'MaxGhostProb'      : 0.4,
                      'ExtraInfoTools'    : [ { "Type" : "ConeVariables"
                                              , "ConeAngle" : 0.6
                                              , "ConeNumber" : 1
                                              , "Variables" : ['angle', 'mult', 'ptasy']},
                                              { "Type" : "ConeVariables"
                                              , "ConeAngle" : 0.8
                                              , "ConeNumber" : 2
                                              , "Variables" : ['angle', 'mult', 'ptasy']},
                                              { "Type" : "ConeVariables"
                                              , "ConeAngle" : 1.0
                                              , "ConeNumber" : 3
                                              , "Variables" : ['angle', 'mult', 'ptasy']},
                                              {'Type' : 'VertexIsolation'}
                                            ]
                    },
    'STREAMS'     : { 'Bhadron' : ['StrippingB2TwoBaryonsB2PPbarLine']}
    }














