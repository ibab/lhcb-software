"""
Line config dictionaries for 2.76 TeV data stripping.
"""

from GaudiKernel.SystemOfUnits import *

MiniBias = {
    'BUILDERTYPE' : 'MiniBiasConf',
    'CONFIG'   :{ "NoBiasLine_RE"       : "(HLT_PASS('Hlt1MBNoBiasDecision'))",
                  "NoBiasLine_Rate"     : 1,
                  "NoBiasLine_Limiter"  : "Hlt1MBNoBiasODINFilter",
                  "NoBiasLine_CondDB"   : False,
                  "L0AnyRLLine_RE"      : "(HLT_PASS('Hlt1L0AnyRateLimitedDecision'))",
                  "L0AnyRLLine_Rate"    : 1,
                  "L0AnyRLLine_Limiter" : "Hlt1L0AnyRateLimitedPostScaler",
                  "L0AnyPSLine_RE"      : "(HLT_PASS_RE('Hlt1L0Any.*Decision'))",
                  "L0AnyPSLine_PS"      : 0.01, # changed too
                }, 
    'WGs' : [ 'ALL' ],
    'STREAMS' : [ 'ALL' ]
    }

V0ForPID = {
    'BUILDERTYPE' : 'StrippingV0ForPIDConf',
    'STREAMS'     : [ 'ALL' ],
    'WGs'         : [ 'ALL' ],
    'CONFIG'      : {
    'TrackChi2'            :  5       ,          ## Track Chi2/ndof quality 
    'VertexChi2'           :  16      ,          ## Cut on Vertex chi2-quality
    'DeltaMassK0S'         :  50 * MeV,          ## Mass-window (half)-width for K0S 
    'DeltaMassLambda'      :  25 * MeV,          ## Mass-window (half)-width for Lambda 
    'MaxZ'                 : 220 * cm ,          ## Maximal allowed vertex Z-position
    'DaughtersIPChi2'      :  25      ,          ## minimal IP-chi2 for daughter particles  
    'LTimeFitChi2'         :  49      ,          ## Chi2 from LifeTime Fitter
    ##
    'WrongMassK0S'         :   9 * MeV,          ## Mass-window (half)-width for ppi hypthesis
    'WrongMassK0S_DD'      :  18 * MeV,          ## Mass-window (half)-width for ppi hypthesis (DD-case)
    ##
    'WrongMassLambda'      :  20 * MeV,          ## Mass-window (half)-width for pipi hypthesis
    'WrongMassLambda_DD'   :  40 * MeV,          ## Mass-window (half)-width for pipi hypthesis (DD-case)
    ##
    'CTauK0S'              :   1 * mm ,          ## Cut on c*tau for K0S 
    'CTauK0S_DD'           :  10 * mm ,          ## Cut on c*tau for K0S (DD-case)
    ##
    'CTauLambda0'          :   5 * mm ,          ## Cut on c*tau for Lambda0
    'CTauLambda0_DD'       :  20 * mm ,          ## Cut on c*tau for Lambda0 (DD-case)
    ##
    'Proton_IsMUONCut'     :  "(INTREE( (ABSID=='p+') & ISMUON ) )" , 
    ## 
    # Technicalities:
    ##
    'Monitor'              :  False    ,          ## Activate the monitoring ?
    ##
    "HLT"                  : "HLT_PASS_RE('Hlt1MB.*Decision')" ,  ## HLT-cut
    ##
    'Preambulo'       : [
    ## import c_light
    "from GaudiKernel.PhysicalConstants import c_light" ,
    ## define DD-category of K0S 
    "DD =    CHILDCUT ( ISDOWN , 1 ) & CHILDCUT ( ISDOWN , 2 ) " ,
    ## define LL-category of K0S 
    "LL =    CHILDCUT ( ISLONG , 1 ) & CHILDCUT ( ISLONG , 2 ) "
    ] ,
    'KS0LL_Prescale'           : 0.020 ,
    'KS0DD_Prescale'           : 0.024 , 
    'LamLL_Prescale_LoP'       : 0.140 , 
    'LamLL_Prescale_HiP'       : 1.000 ,
    'LamDD_Prescale'           : 0.100 , 
    'LamLLIsMUON_Prescale_LoP' : 1.000 ,
    'LamLLIsMUON_Prescale_HiP' : 1.000 ,
    'LamDDIsMUON_Prescale'     : 1.000   
     }
    }

# NoPID Inclusive Phi Lines
# A. Powell & S. Schleich
InclPhi={
    'BUILDERTYPE' : 'InclPhiConf',
    'CONFIG' : {  'KaonPT'              : 500      # MeV
                , 'KaonDLL'             : 15       # adimensional
                , 'PhiMassWindow'       : 30       # MeV
                , 'HighPtPrescale'      : 0.0030   # adimensional 
                , 'LowPtPrescale'       : 0.0040   # adimensional 
                , 'LDPrescale'          : 0.0002   # adimensional 
                },
    'STREAMS' : [ 'ALL' ],
    'WGs'    : ['ALL']
    }

#StrangeBaryons
StrangeBaryons = {
    'BUILDERTYPE'  : 'StrippingStrangeBaryonsConf',
    'CONFIG' : {
    #PID cuts
    'PreScale'    : 0.01,
    'checkPV'   : True,
    'HLT' : "HLT_PASS('Hlt1MBNoBiasDecision')",
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
    'STREAMS':[ 'ALL' ]
    }

#Z02MuMu
Z02MuMu = {
    'BUILDERTYPE'  : 'Z02MuMuConf',
    'CONFIG'       : {
    'Z02MuMu_Prescale'  : 1.0,
    'Z02MuMu_Postscale' : 1.0,
    'pT'    : 3.,
    'MMmin' : 40.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'ALL' ]
    }

#DY2MuMu
DY2MuMu = {
    'BUILDERTYPE'  : 'DY2MuMuConf',
    'CONFIG'       : {
    'DY2MuMu1LinePrescale'    : 0.05,
    'DY2MuMu1LineHltPrescale' : 0.50,
    'DY2MuMu2LinePrescale'    : 0.25,
    'DY2MuMu2LineHltPrescale' : 1.0,
    'DY2MuMu3LinePrescale'    : 1.0,
    'DY2MuMu4LinePrescale'    : 1.0,
    'DY2MuMuLinePostscale'    : 1.0,
    'DY1MinMass' :  3.2,
    'DY1MaxMass' :  5.,
    'DY2MinMass' :  5.,
    'DY2MaxMass' : 10.,
    'DY3MinMass' : 10.,
    'DY3MaxMass' : 20.,
    'DY4MinMass' : 20.,
    'DY4MaxMass' : 40.,
    'p'             : 10.,
    'pT1'           :  1.5,
    'pT2'           :  3.,
    'trkpchi2'      :  0.001,
    'pid'           : -3.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'ALL' ]
    }

#MuMuSS
MuMuSS = {
    'BUILDERTYPE'  : 'MuMuSSConf',
    'CONFIG'       : {
    'MuMuSSLine1Prescale' :  .1,
    'MuMuSSLine2Prescale' : 1.0,
    'MuMuSSLine3Prescale' : 1.0,
    'MuMuSSLine4Prescale' : 1.0,
    'MuMuSSLinePostscale' : 1.0,
    'MuMuSSLine1MinMass' :  3.2,
    'MuMuSSLine1MaxMass' :  5.,
    'MuMuSSLine2MinMass' :  5.,
    'MuMuSSLine2MaxMass' : 10.,
    'MuMuSSLine3MinMass' : 10.,
    'MuMuSSLine3MaxMass' : 20.,
    'MuMuSSLine4MinMass' : 20.,
    'p'   : 10.,
    'pT1' :  1.5,
    'pT2' :  3.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'ALL' ]
    }

#WMu
WMu = {
    'BUILDERTYPE'  : 'WMuConf',
    'CONFIG'       : {
    'WMu_Prescale'    : 1.0,
    'WMuLow_Prescale' : 0.1,
    'STNB_Prescale' : 0.2,
    'WMu_Postscale'   : 1.0,
    'SingMuon10_Prescale' : 0.01,
    'SingMuon48_Prescale'  : 0.4,
    'pT'     : 20.,
    'pTlow'  : 15.,
    'pTvlow' :  5.,
    'SingMuon10_pT': 10.,
    'SingMuon48_pT': 4.8
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'ALL' ] 
    }

## Micro-DST, Vanya BELYAEV
## Test:  Yes
PromptCharm = {
    'BUILDERTYPE' : 'StrippingPromptCharmConf', 
    'CONFIG'      : {
    ## PT-cuts 
    #
    'pT(D0)'           :  3.0 * GeV ,    ## pt-cut for  prompt   D0
    'pT(D0) for D*+'   :  2.0 * GeV ,    ## pt-cut for  D0 from  D*+ 
    'pT(D+)'           :  3.0 * GeV ,    ## pt-cut for  prompt   D+
    'pT(Ds+)'          :  3.0 * GeV ,    ## pt-cut for  prompt   Ds+
    'pT(Ds+) for Bc+'  :  1.0 * GeV ,    ## pt-cut for  Ds+ from Bc+
    'pT(Lc+)'          :  3.0 * GeV ,    ## pt-cut for  prompt   Lc+
    #
    # Selection of basic particles 
    #
    'TrackCuts'       : ' ( TRCHI2DOF < 5 ) & ( PT > 250 * MeV ) & ( TRGHOSTPROB < 0.5 ) & in_range  ( 2 , ETA , 5 ) ' , 
    'BasicCuts'       : ' & ( 9 < MIPCHI2DV() ) ' , 
    'KaonCuts'        : ' & ( 2 < PIDK  - PIDpi ) '                           , 
    'PionCuts'        : ' & ( 2 < PIDpi - PIDK  ) '                           , 
    'ProtonCuts'      : ' & ( 2 < PIDp  - PIDpi ) & ( 2 < PIDp - PIDK ) '     , 
    'SlowPionCuts'    : ' TRCHI2DOF < 5 '                                     ,
    'MuonCuts'        : ' ISMUON & ( PT > 650 * MeV ) & ( TRCHI2DOF < 5 ) '   , 
    #
    # Global Event cuts 
    #
    'CheckPV'         : True , 
    #
    # Technicalities:
    #
    'Preambulo'       : [
    # the D0 decay channels
    "pipi   = DECTREE ('[D0]cc -> pi- pi+   ') " ,
    "kk     = DECTREE ('[D0]cc -> K-  K+    ') " ,
    "kpi    = DECTREE ('[D0    -> K-  pi+]CC') " ,
    # number of kaons in final state (as CombinationCuts)
    "ak2    = 2 == ANUM( 'K+' == ABSID ) "       ,
    # shortcut for chi2 of vertex fit 
    'chi2vx = VFASPF(VCHI2) '                    ,
    # shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light" , 
    "ctau   = BPVLTIME ( 9 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<9
    # dimuons:
    "psi           =   ADAMASS ('J/psi(1S)') < 150 * MeV"  ,
    "psi_prime     =   ADAMASS (  'psi(2S)') < 150 * MeV"  ,
    ] ,
    ## monitoring ?
    'Monitor'     : False ,
    ## pescales 
    'D0Prescale'             : 1.0 ,
    'D*Prescale'             : 1.0 ,
    'DsPrescale'             : 1.0 ,
    'D+Prescale'             : 1.0 ,
    'LambdaCPrescale'        : 1.0 ,
    'LambdaC*Prescale'       : 1.0 ,
    'SigmaCPrescale'         : 1.0 ,
    'DiCharmPrescale'        : 1.0 , 
    'DiMu&CharmPrescale'     : 1.0 , 
    'Chi&CharmPrescale'      : 1.0 , 
    'Ds&PsiPrescale'         : 1.0 
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'ALL' ]
#    {
#      'Charm' : [    
#        'StrippingD02HHForPromptCharm',
#        'StrippingDstarForPromptCharm',
#        'StrippingDsForPromptCharm',
#        'StrippingDForPromptCharm',
#        'StrippingLambdaCForPromptCharm',
#        'StrippingSigmaCForPromptCharm',
#        'StrippingLambdaCstarForPromptCharm',
#        'StrippingDiMuonAndCharmForPromptCharm',
#        'StrippingChiAndCharmForPromptCharm',
#        'StrippingDsPsiForPromptCharm',
#        'StrippingCharmAndWForPromptCharm',
#        'StrippingDiMuonAndWForPromptCharm',
#        'StrippingChiAndWForPromptCharm'
#        ],
#      'CharmCompleteEvent' : [
#        'StrippingDiCharmForPromptCharm'
#        ]
#      }
    }

# Xsec: D(s)+ -> h+ h- h+
D2HHHForXSec = {
    'BUILDERTYPE' : 'StrippingD2HHHForXSecConf',
    'CONFIG'      : { 
                    'Daug_All_PT_MIN'           :  200.0 * MeV
                  , 'Daug_2of3_PT_MIN'          :  400.0 * MeV
                  , 'Daug_1of3_PT_MIN'          :  400.0 * MeV
                  , 'Daug_All_BPVIPCHI2_MIN'    :    1.0
                  , 'Daug_2of3_BPVIPCHI2_MIN'   :    4.0
                  , 'Daug_1of3_BPVIPCHI2_MIN'   :    4.0
                  , 'K_PIDK_MIN'                :    0.0
                  , 'Pi_PIDK_MAX'               :    3.0
                  , 'Comb_AM_MIN'               : 1580.0 * MeV
                  , 'Comb_AM_MAX'               : 2260.0 * MeV
                  , 'Comb_ADOCAMAX_MAX'         :    0.5 * mm
                  , 'D_VCHI2VDOF_MAX'           :   25.0
                  , 'D_acosBPVDIRA_MAX'         :   35.0 * mrad
                  , 'D_PVDispCut'               : "((BPVVDCHI2 > 16.0)|(BPVLTIME() > 0.150 * picosecond))"
                  , 'HltFilter'                 : None
                  #, 'HltFilter'                 : "HLT_PASS_RE('Hlt1MB.*')"
                  , 'PrescaleD2KPP'             :    1.0
                  , 'PrescaleD2KKP'             :    1.0
                  , 'PrescaleD2KKK'             :   -1.0
                  , 'PrescaleD2PPP'             :   -1.0
                  , 'PrescaleD2KPPDCS'          :   -1.0
                  , 'PostscaleD2KPP'            :    1.0
                  , 'PostscaleD2KKP'            :    1.0
                  , 'PostscaleD2KKK'            :   -1.0
                  , 'PostscaleD2PPP'            :   -1.0
                  , 'PostscaleD2KPPDCS'         :   -1.0
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'ALL' ] 
    }


# Xsec: D(s)+ -> phi pi+
D2PhiPiForXSec = {
    'BUILDERTYPE' : 'StrippingD2PhiPiForXSecConf',
    'CONFIG'      : {
                    'K_BPVIPCHI2_MIN'           :    1.0
                  , 'Pi_BPVIPCHI2_MIN'          :    1.0
                  , 'K_PIDK_MIN'                :    0.0
                  , 'Phi_M_MIN'                 : 1000.0 * MeV
                  , 'Phi_M_MAX'                 : 1040.0 * MeV
                  , 'D_AM_MIN'                  : 1770.0 * MeV
                  , 'D_AM_MAX'                  : 2070.0 * MeV
                  , 'D_VCHI2VDOF_MAX'           :   25.0
                  , 'D_acosBPVDIRA_MAX'         :   35.0 * mrad
                  , 'D_PVDispCut'               : "((BPVVDCHI2 > 16.0)|(BPVLTIME() > 0.150 * picosecond))"
                  , 'HltFilter'                 : None
                  #, 'HltFilter'                 : "HLT_PASS_RE('Hlt1MB.*')"
                  #
                  , 'PrescaleD2PhiPi'           :    1.0
                  , 'PostscaleD2PhiPi'          :    1.0
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'ALL' ] 
    }

# Xsec: Lc+ -> p K- pi+
Lambdac2PKPiForXSec = {
    'BUILDERTYPE' : 'StrippingLambdac2PKPiForXSecConf',
    'CONFIG'      : {
                     'Daug_All_PT_MIN'          :  200.0 * MeV
                   , 'Daug_2of3_PT_MIN'         :  400.0 * MeV
                   , 'Daug_1of3_PT_MIN'         : 1000.0 * MeV
                   , 'Daug_All_BPVIPCHI2_MIN'   :    1.0
                   , 'Daug_2of3_BPVIPCHI2_MIN'  :    1.0
                   , 'Daug_1of3_BPVIPCHI2_MIN'  :    4.0
                   , 'Proton_PIDpPIDpi_MIN'     :    0.0
                   , 'Proton_PIDpPIDK_MIN'      :    0.0
                   , 'K_PIDK_MIN'               :    0.0
                   , 'Pi_PIDK_MAX'              :    3.0
                   , 'Comb_ADAMASS_WIN'         :   90.0 * MeV
                   , 'Comb_ADOCAMAX_MAX'        :    0.5 * mm
                   , 'Lambdac_VCHI2VDOF_MAX'    :   25.0
                   , 'Lambdac_acosBPVDIRA_MAX'  :   35.0 * mrad
                   , 'Lambdac_PVDispCut'        : "((BPVVDCHI2 > 4.0)|(BPVLTIME() > 0.075 * picosecond))"
                   , 'HltFilter'                :  None
                   #, 'HltFilter'                 : "HLT_PASS_RE('Hlt1MB.*')"
                   , 'PrescaleLambdac2PKPi'     :    1.0
                   , 'PostscaleLambdac2PKPi'    :    1.0
                   , 'PrescaleLambdac2PKK'      :    1.0
                   , 'PostscaleLambdac2PKK'     :    1.0
                   , 'PrescaleLambdac2PPiPi'    :   -1.0
                   , 'PostscaleLambdac2PPiPi'   :   -1.0
                   , 'PrescaleLambdac2PPiKWS'   :   -1.0
                   , 'PostscaleLambdac2PPiKWS'  :   -1.0
    },
    'WGs'          : ['Charm'],
    'STREAMS'      : [ 'ALL' ] 
    }

# Xsec: D0 -> K- pi- pi+ pi+
D02K3PiForXSec = {
    'BUILDERTYPE' : 'StrippingD02K3PiForXSecConf',
    'CONFIG'      : {
                    'Daug_All_PT_MIN'           :  250.0 * MeV
                  , 'Daug_3of4_PT_MIN'          :  300.0 * MeV
                  , 'Daug_2of4_PT_MIN'          :  350.0 * MeV
                  , 'Daug_1of4_PT_MIN'          :  400.0 * MeV
                  , 'Daug_All_BPVIPCHI2_MIN'    :    1.0
                  , 'Daug_3of4_BPVIPCHI2_MIN'   :    4.0
                  , 'Daug_2of4_BPVIPCHI2_MIN'   :    4.0
                  , 'Daug_1of4_BPVIPCHI2_MIN'   :    4.0
                  , 'K_PIDK_MIN'                :    0.0
                  #, 'Pi_PIDK_MAX'               :   0.0
                  , 'Comb_ADAMASS_WIN'          :  100.0 * MeV
                  , 'Comb_ADOCAMAX_MAX'         :    0.5 * mm
                  , 'D0_VCHI2VDOF_MAX'          :   25.0
                  , 'D0_acosBPVDIRA_MAX'        :   35.0 * mrad
                  , 'D0_PVDispCut'              : "((BPVVDCHI2 > 16.0)|(BPVLTIME() > 0.150 * picosecond))"
                  , 'Dstar_AMDiff_MAX'          :  160.0 * MeV
                  , 'Dstar_VCHI2VDOF_MAX'       :  100.0
                  #
                  , 'HltFilter'                 : None
                  #, 'HltFilter'                 : "HLT_PASS_RE('Hlt1MB.*')"
                  #
                  , 'PrescaleD02K3Pi'             :  1.0
                  , 'PrescaleDstar2D0Pi_D02K3Pi'  :  1.0
                  , 'PostscaleD02K3Pi'            :  1.0
                  , 'PostscaleDstar2D0Pi_D02K3Pi' :  1.0
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'ALL' ] 
    }

# Xsec: D0 -> K- pi+ (geometrical selection)
D02KPiGeoForXSec = {
    'BUILDERTYPE'  : 'StrippingD02KPiGeoForXSecConf',
    'CONFIG'       : {
                    'D0_ADAMASS_WIN'      : 250.0 * MeV
                  , 'D0_AMAXDOCA_MAX'     :  10.0 * mm
                  , 'D0_ADMASS_WIN'       : 125.0 * MeV
                  , 'D0_NU_2PT_MIN'       :  14.0
                  , 'D0_BPVVDSIGN_MIN'    :   1.0 * mm
                  , 'Dstar_AMDiff_MAX'    : 160.0 * MeV
                  , 'Dstar_VCHI2VDOF_MAX' : 100.0
                  , 'Dstar_MDiff_MAX'     : 155.0 * MeV
                  , 'HltFilter'           : None
                  #, 'HltFilter'           : "HLT_PASS_RE('Hlt1MB.*')"
                  #
                  , 'PrescaleD02HH'             :   1.0
                  , 'PrescaleDstar2D0Pi_D02HH'  :   1.0
                  , 'PostscaleD02HH'            :   1.0
                  , 'PostscaleDstar2D0Pi_D02HH' :   1.0
    },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'ALL' ] 
    }

# Xsec: D0 -> h+ h-
D02HHForXSec = { 
    'BUILDERTYPE' : 'StrippingD02HHForXSecConf',
    'CONFIG'      : {
                    'Daug_PT_MIN'               : 250.0 * MeV
                  , 'Daug_BPVIPCHI2_MIN'        :   4.0
                  , 'K_PIDK_MIN'                :   0.0
                  , 'Pi_PIDK_MAX'               :   3.0
                  , 'D0_ADAMASS_WIN'            :  80.0*MeV
                  , 'D0_VCHI2VDOF_MAX'          :  25.0
                  , 'D0_acosBPVDIRA_MAX'        :  35.0 * mrad
                  , 'D0_PVDispCut'              : "((BPVVDCHI2 > 16.0)|(BPVLTIME() > 0.150 * picosecond))"
                  , 'Dstar_AMDiff_MAX'          : 160.0*MeV
                  , 'Dstar_VCHI2VDOF_MAX'       : 100.0
                  #
                  , 'HltFilter'          : None
                  #, 'HltFilter'           : "HLT_PASS_RE('Hlt1MB.*')"
                  #
                  , 'PrescaleD02HH'             :   1.0
                  , 'PrescaleDstar2D0Pi_D02HH'  :   1.0
                  , 'PostscaleD02HH'            :   1.0
                  , 'PostscaleDstar2D0Pi_D02HH' :   1.0
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'ALL' ] 
    }

#LowMult
LowMult = {
    'BUILDERTYPE'  : 'LowMultConf',
    'CONFIG'       : {
    'LowMultPrescale'           : 1.0,
    'LowMultWSPrescale'       : 0.1,
    'LowMultHHIncPrescale'    : 0.1,
    'LowMultLMRPrescale'      : 0.2,
    'LowMultPrescale_ps'      : 0.005,
    'LowMultNoFilterPrescale' : 0.1,
    'LowMultPostscale'        : 1.0,
    # Final-state particles
    'H_PTmin'         : 100.0 * MeV,
    'H_Pmin'          : 5000.0 * MeV,
    'H_TrkChi2max'    : 3.0,
    'K_PIDKmin'       : 0.0,
    'P_PIDPmin'       : 0.0,
    'KKInc_K_Pmin'    : 10000.0 * MeV,
    'KKInc_K_PIDKmin' : 5.0,
    # D0 -> KPi
    'D2KPi_APTmin'        : 0.0 * MeV,
    'D2KPi_ADAMASSmax'    : 80.0 * MeV,
    'D2KPi_ADOCAmax'      : 0.5 * mm,
    'D2KPi_APmin'         : 10000.0 * MeV,
    'D2KPi_VtxChi2DoFmax' : 15.0,
    # D+- -> KPiPi
    'D2KPiPi_APTmin'        : 0.0 * MeV,
    'D2KPiPi_ADAMASSmax'    : 80.0 * MeV,
    'D2KPiPi_ADOCAmax'      : 0.5 * mm,
    'D2KPiPi_APmin'         : 10000.0 * MeV,
    'D2KPiPi_VtxChi2DoFmax' : 15.0,
    # D0 -> K3Pi
    'D2K3Pi_APTmin'        : 0.0 * MeV,
    'D2K3Pi_ADAMASSmax'    : 80.0 * MeV,
    'D2K3Pi_ADOCAmax'      : 0.7 * mm,
    'D2K3Pi_APmin'         : 10000.0 * MeV,
    'D2K3Pi_VtxChi2DoFmax' : 15.0,
    # 'ChiC' -> HH (H = K, Pi)
    'ChiC2HH_APTmin'        : 0.0 * MeV,
    'ChiC2HH_APTmax'        : 5000.0 * MeV,
    'ChiC2HH_AMmin'         : 2850.0 * MeV,
    'ChiC2HH_AMmax'         : 4500.0 * MeV,
    'ChiC2HH_ADOCAmax'      : 0.5 * mm,
    'ChiC2HH_APmin'         : 10000.0 * MeV,
    'ChiC2HH_VtxChi2DoFmax' : 15.0,
    #'ChiC' -> PP
    'ChiC2PP_APTmin'        : 0.0 * MeV,
    'ChiC2PP_APTmax'        : 5000.0 * MeV,
    'ChiC2PP_AMmin'         : 2850.0 * MeV,
    'ChiC2PP_AMmax'         : 3650.0 * MeV,
    'ChiC2PP_ADOCAmax'      : 0.5 * mm,
    'ChiC2PP_APmin'         : 10000.0 * MeV,
    'ChiC2PP_VtxChi2DoFmax' : 15.0,
    # 'ChiC' -> HHHH (H = K, Pi)
    'ChiC2HHHH_APTmin'        : 0.0 * MeV,
    'ChiC2HHHH_APTmax'        : 5000.0 * MeV,
    'ChiC2HHHH_AMmin'         : 2850.0 * MeV,
    'ChiC2HHHH_AMmax'         : 4500.0 * MeV,
    'ChiC2HHHH_ADOCAmax'      : 0.7 * mm,
    'ChiC2HHHH_APmin'         : 10000.0 * MeV,
    'ChiC2HHHH_VtxChi2DoFmax' : 15.0,
    # Low-mass resonance -> HH (H = K, Pi)
    'LMR2HH_APTmin'        : 500.0 * MeV,
    'LMR2HH_APTmax'        : 1500.0 * MeV,
    'LMR2HH_AMmin'         : 450.0 * MeV,
    'LMR2HH_AMmax'         : 1500.0 * MeV,
    'LMR2HH_ADOCAmax'      : 0.1 * mm,
    'LMR2HH_APmin'         : 15000.0 * MeV,
    'LMR2HH_VtxChi2DoFmax' : 3.0,
    # Phi resonance -> KK
    'PHI2KK_APTmin'        : 0.0 * MeV,
    'PHI2KK_APTmax'        : 1500.0 * MeV,
    'PHI2KK_AMmin'         : 990.0 * MeV,
    'PHI2KK_AMmax'         : 1050.0 * MeV,
    'PHI2KK_ADOCAmax'      : 0.1 * mm,
    'PHI2KK_APmin'         : 4000.0 * MeV,
    'PHI2KK_VtxChi2DoFmax' : 3.0
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'ALL' ]
    }
