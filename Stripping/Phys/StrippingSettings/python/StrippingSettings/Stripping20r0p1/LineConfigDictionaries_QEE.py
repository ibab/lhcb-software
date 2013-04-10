"""
Line config dictionaries for the EW Stream. Mostly QEE WG lines


Dictionary names should have the same name as the corresponding
StrippingSelections file containing the line builder instance.

   e.g  StrippingPromptCharm linebuilder should be added as:

      PromptCharm = { ... }
"""

from GaudiKernel.SystemOfUnits import *

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
    'STREAMS' : {
    'EW' : ['StrippingLowMultCEP_PHI2KK_line']
    }
    }


#HighPtGamma
HighPtGamma = {
    'BUILDERTYPE'  : 'HighPtGammaConf',
    'CONFIG'       : {
    'Prescale'  : 1.0,
    'Postscale' : 1.0,
    'pT'        : 18.,
    'photonIso' : 30,

    'PrescaleLoose'  : 0.05,
    'PostscaleLoose' : 1.0,
    'pTLoose'        : 7.5,
    'photonIsoLoose' : 5
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }

#LLP2MuX
LLP2MuX = {
    'BUILDERTYPE'  : 'LLP2MuX',
    'CONFIG'       : { 
    "MinPT"  : 12.00 * GeV,
    "MinIP"  :  0.25 * mm
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }


#StrangeBaryons
StrangeBaryons = {
    'BUILDERTYPE' : 'StrippingStrangeBaryonsConf',
    'CONFIG' : { 
    #PID cuts
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
    'STREAMS':[ 'EW' ]
    }


