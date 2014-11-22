################################################################################
##                          S T R I P P I N G  2 1                            ##
##                                                                            ##
##  Configuration for RD WG                                                   ##
##  Contact person: Pablo Ruiz Valls pruizval@cern.ch                         ##
################################################################################

#
# Rare Decay Lines
#

from GaudiKernel.SystemOfUnits import *

################################################################################
##                              EWP SubGroup Lines                            ##
################################################################################
## StrippingB2XMuMu.py                                                        ##
## StrippingB2XMuMu Lines, going to fullDST                                   ##
## -----------------------------------                                        ##
## StrippingB2XMuMuLine                                                       ##
## Authors: patrick.haworth.owen@cern.ch, konstantinos.petridis@cern.ch       ##
################################################################################

B2XMuMu = {
    'BUILDERTYPE'  :  'B2XMuMuConf',
    'CONFIG' : {
          'RelatedInfoTools'      : [
                 {   "Type" : "RelInfoConeVariables"
                     , "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM']
                     , 'Location':'ConeIsoInfo'}
                 , {'Type' : 'RelInfoVertexIsolation'
                    , 'Location':'VtxIsoInfo'}
                 , {'Type': 'RelInfoVertexIsolationBDT'
                    , 'Location':'VtxIsoBDTInfo' }
                 ,  {"Type" : "RelInfoBs2MuMuIsolations"
                     , "Variables" : ['BSMUMUCDFISO', 'BSMUMUOTHERBMAG', 'BSMUMUOTHERBANGLE', 'BSMUMUOTHERBBOOSTMAG', 'BSMUMUOTHERBBOOSTANGLE', 'BSMUMUTRACKPLUSISO', 'BSMUMUTRACKMINUSISO', 'BSMUMUOTHERBTRACKS']
                     , "Location"  : "BSMUMUVARIABLES"
                     }
                 ]
          , 'KpiVXCHI2NDOF'      : 9.0          # dimensionless
          , 'MuonPID'            : -3.0         # dimensionless
          , 'DimuonUPPERMASS'    : 7100.0       # MeV
          , 'Pi0MINPT'           : 800.0        # MeV
          , 'Pi0ForOmegaMINPT'   : 500.0       #  MeV
          , 'DplusLOWERMASS'     : 1600.0      #  MeV
          , 'DplusUPPERMASS'     : 2300.0      #  MeV
          , 'KstarplusWINDOW'    : 300.0       #  MeV
          , 'KsWINDOW'           : 30.0        #  MeV
          , 'LambdaWINDOW'       : 30.0        #  MeV
          , 'LongLivedPT'        : 0.0         #  MeV , used to be 500.0 MeV
          , 'LongLivedTau'        : 2          #  ps

           # A1 cuts
          , 'A1_Comb_MassLow'  :    0.0
          , 'A1_Comb_MassHigh' : 5550.0
          , 'A1_MassLow'       :    0.0
          , 'A1_MassHigh'      : 5500.0
          , 'A1_MinIPCHI2'     :    4.0
          , 'A1_FlightChi2'    :   25.0
          , 'A1_VtxChi2'       :   10.0
          , 'A1_Dau_MaxIPCHI2' :    9.0
          # From Bd2KstarMuMu line
          ,'UseNoPIDsHadrons'          : True,

          # B cuts
          'B_Comb_MassLow'      : 4800.0,
          'B_Comb_MassHigh'     : 7100.0,
          'B_MassLow'           : 4900.0,
          'B_MassHigh'          : 7000.0,
          'B_VertexCHI2'        :    8.0,
          'B_IPCHI2'            :   16.0,
          'B_DIRA'              : 0.9999,
          'B_FlightCHI2'        :  121.0,
          'B_Dau_MaxIPCHI2'     :    9.0,

          # Daughter cuts
          'Dau_VertexCHI2'      :   12.0,
          'Dau_DIRA'            :   -0.9,
          # Kstar cuts
          'Kstar_Comb_MassLow'  :    0.0,
          'Kstar_Comb_MassHigh' : 6200.0,
          'Kstar_MassLow'       :    0.0,
          'Kstar_MassHigh'      : 6200.0,
          'Kstar_MinIPCHI2'     :    0.0,
          'Kstar_FlightChi2'    :    9.0,
          'Kstar_Dau_MaxIPCHI2' :    9.0,

          #Omega cuts
          'Omega_MassWin'       :   100, #MeV
          'Omega_CombMassWin'   :   200, #MeV
          'OmegaChi2Prob'       : 0.00001,  #dimensionless

          #K1->OmegaK cuts
          'K12OmegaK_MassLow'   :   300, #MeV
          'K12OmegaK_MassHigh'  :  2100, #MeV
          'K12OmegaK_CombMassLow'   :   400, #MeV
          'K12OmegaK_CombMassHigh'  :  2000, #MeV
          'K12OmegaK_VtxChi2'   : 10 ,
          

          #JPsi (dimu) cuts
          'Dimu_FlightChi2'     :   9.0,
          'Dimu_Dau_MaxIPCHI2'  :   9.0,

          #Track cuts
          'Track_GhostProb'     :    0.35,

          #Hadron cuts
          'Hadron_MinIPCHI2'    :    6.0,

          #Muon cuts
          'Muon_MinIPCHI2'      :    9.0,
          'Muon_IsMuon'         :    True,
          'MuonNoPIDs_PIDmu'    :    0.0,

          #Wrong sign combinations
          'DimuonWS'            :   True,
          'HadronWS'            :   True,

          #GEC
          'SpdMult'             :  600,

          'HLT_FILTER' : "(HLT_PASS('Hlt1TrackAllL0Decision')|HLT_PASS('Hlt1TrackMuonDecision'))&(HLT_PASS_RE('Hlt2DiMuon.*Decision') | HLT_PASS_RE('Hlt2Topo.*Decision') | HLT_PASS_RE('Hlt2SingleMuon.*Decision'))",

          'DECAYS'              :  [
                     "B0 -> J/psi(1S) phi(1020)",
                     "[B0 -> J/psi(1S) K*(892)0]cc",
                     "B0 -> J/psi(1S) rho(770)0",
                     "[B+ -> J/psi(1S) rho(770)+]cc",
                     "B0 -> J/psi(1S) f_2(1950)",
                     "B0 -> J/psi(1S) KS0",
                     "[B0 -> J/psi(1S) D~0]cc",
                     "[B+ -> J/psi(1S) K+]cc",
                     "[B+ -> J/psi(1S) pi+]cc",
                     "[B+ -> J/psi(1S) K*(892)+]cc",
                     "[B+ -> J/psi(1S) D+]cc",
                     "[B+ -> J/psi(1S) D*(2010)+]cc",
                     "[Lambda_b0 -> J/psi(1S) Lambda0]cc",
                     "[Lambda_b0 -> J/psi(1S) Lambda(1520)0]cc",
                     "B0 -> J/psi(1S) pi0",
                     "[B+ -> J/psi(1S) a_1(1260)+]cc",
                     "[B+ -> J/psi(1S) K_1(1270)+]cc",
                     "[B+ -> J/psi(1S) K_2(1770)+]cc",
                     "B0 -> J/psi(1S) K_1(1270)0",
                     "[B+ -> J/psi(1S) K_1(1400)+]cc",
                     "B0 -> J/psi(1S) K_1(1400)0"
                     ]

          },
    'STREAMS' : ['Leptonic'],
    'WGs' : ['RD'],
    }

###############################################################################
## StrippingB2XMuMuInclusive.py                                               ##
## Inclusive StrippingB2XMuMu Lines, going to fullDST                         ##
## -----------------------------------                                        ##
## StrippingB2XMuMuInclDiMuHighQ2Line                                        ##
## StrippingB2XMuMuInclDiMuLowQ2Line                                         ##
## StrippingB2XMuMuInclDiMuCCbarLine                                        ##
## Authors: patrick.haworth.owen@cern.ch, konstantinos.petridis@cern.ch       ##
################################################################################

B2XMuMuIncl = {
    'BUILDERTYPE'  :  'B2XMuMuInclusiveConf',
    'CONFIG' : {
         # Incl (dimu) cuts
        'IPCHI2_LOWQ2'     :   9.0,
        'FlightChi2_LOWQ2'     :   100.0,
        'DIRA_LOWQ2'           :   -0.90,
        'VertexCHI2_LOWQ2'     :     2.0,
        'FlightChi2_HIGHQ2'     :   100.0,
        'DIRA_HIGHQ2'           :    0.99,
        'VertexCHI2_HIGHQ2'     :     4.0,
        'CCbarPrescale'  :    0.07,
        'LOWERMASS_LOWQ2'       :  1400.0, # MeV
        'UPPERMASS_LOWQ2'       :  2500.0, # MeV
        'LOWERMASS_CCbar'       :  2500.0, # MeV
        'UPPERMASS_CCbar'       :  3870.0, # MeV
        'LOWERMASS_HIGHQ2'       :  3870.0, # MeV
        'UPPERMASS_HIGHQ2'       :  5800.0, # MeV
        'CORRM_MIN'       :     0.0, # MeV
        'CORRM_MAX'       : 15000.0, # MeV
        # Track cuts
        'Track_CHI2nDOF'      :    3.0,
        'Track_GhostProb'     :    0.35,
         # Muon cuts
        'Muon_MinIPCHI2_LOWQ2'   :    16.0,
        'Muon_PIDmu_LOWQ2'       :    2.0,
        'Muon_PIDmuK_LOWQ2'       :    2.0,
        'Muon_PT_LOWQ2'          :    800,
        'Muon_MinIPCHI2_HIGHQ2'  :    16.0,
        'Muon_PIDmu_HIGHQ2'      :    0.0,
        'Muon_PIDmuK_HIGHQ2'     :    0.0,
        'Muon_PT_HIGHQ2'         :    250,
        # Wrong sign combinations
        'WS'            :   False,
        # GEC
        'SpdMult'             :  600,
        'HLT_FILTER'             :  "HLT_PASS_RE('Hlt2DiMuonDetachedDecision')|HLT_PASS_RE('Hlt2DiMuonDetachedHeavyDecision')|HLT_PASS_RE('Hlt2SingleMuonDecision')",
        },
    'STREAMS' : ['Dimuon'],
    'WGs' : ['RD'],
}

################################################################################
## StrippingB2KstTauTau.py                                                    ##
## B->K*tautau Lines                                                          ##
## -----------------------------------                                        ##
## StrippingB2KstTauTau_Line                                                  ##
## StrippingB2KstTauTau_Line                                                  ##
## StrippingB2KstTauMu_Line                                                   ##
## StrippingB2KstTauMu_Line                                                   ##
## Authors: G. Mancinelli, J. Serrano                                         ##
################################################################################
B2KstTauTau =  {
    'BUILDERTYPE'       :       'B2KstTauXConf',
    'WGs'    : [ 'RD' ],
    'CONFIG'    : {
          'PT_MU'                         : '1000', # MeV
          'TRACKCHI2_MU'                  : '3',    # dimensionless
          #
          'VCHI2_B'                       : '100', # dimensionless
          'VCHI2_B_Mu'                    : '150', # dimensionless
          'FDCHI2_B'                      : '80',
          'FD_B_Mu'                       : '3',
          'MASS_LOW_B'                    : '2000', # MeV
          'MASS_HIGH_B'                   : '10000', # MeV
          #
          'MASS_LOW_Kst'                  : '700', # MeV
          'MASS_HIGH_Kst'                 : '1100', # MeV
          'VCHI2_Kst'                     : '15',
          'PT_Kst'                        : '1000',
          #
          'B2KstTauTau_LinePrescale'               : 1,
          'B2KstTauTau_LinePostscale'              : 1,
          'B2KstTauMu_LinePrescale'                : 1,
          'B2KstTauMu_LinePostscale'               : 1,
          'B2KstTauTau_SameSign_LinePrescale'      : 0.5,
          'B2KstTauTau_SameSign_LinePostscale'     : 1,
          'B2KstTauMu_SameSign_LinePrescale'       : 0.5,
          'B2KstTauMu_SameSign_LinePostscale'      : 1,
          },
    'STREAMS'     : ['Bhadron']
      }

################################################################################
## StrippingBu2LLK.py                                                         ##
## B --> ll K Lines                                                           ##
## ------------------------                                                   ##
## StrippingBu2LLK_eeLine                                                     ##
## StrippingBu2LLK_ee2Line                                                    ##
## StrippingBu2LLK_meLine                                                     ##
## StrippingBu2LLK_mmLine                                                     ##
## Authors: Patrick Koppenburg, Alex Shires, Thomas Blake, Luca Pescatore     ##
################################################################################
Bu2LLK = {
    'BUILDERTYPE' : 'Bu2LLKConf',
    'CONFIG'     : {
          'BFlightCHI2'            : 100
          ,  'BDIRA'               : 0.9995
          ,  'BIPCHI2'             : 25
          ,  'BVertexCHI2'         : 9
          ,  'DiLeptonPT'          : 0
          ,  'DiLeptonFDCHI2'      : 16
          ,  'DiLeptonIPCHI2'      : 0
          ,  'LeptonIPCHI2'        : 9
          ,  'LeptonPT'            : 300
          ,  'KaonIPCHI2'          : 9
          ,  'KaonPT'              : 400
          ,  'UpperMass'           : 5500
          ,  'PIDe'                : 0
          ,  'Bu2eeKLinePrescale'  : 1
          ,  'Bu2mmKLinePrescale'  : 1
          ,  'Bu2meKLinePrescale'  : 1
          ,  'RelatedInfoTools'      : [
                 { "Type" : "RelInfoConeVariables"
                   , 'Location': "ConeIsoInfo" }
                 , {'Type' : 'RelInfoVertexIsolation'
                    , 'Location': "VertexIsoInfo"  }
                 , {'Type' : 'RelInfoVertexIsolationBDT'
                    , 'Location': "VertexIsoBDTInfo"  }
                 ]
          },
    'WGs'     : [ 'RD' ],
    'STREAMS' : ['Leptonic']
        }


################################################################################
##                              Radiative SubGroup Lines                      ##
################################################################################
## StrippingBeauty2XGamma.py                                                  ##
## Inclusive Radiative Lines, going to muDST                                  ##
## -----------------------------------                                        ##
## StrippingB2XGamma_2piLine                                                  ##
## StrippingB2XGamma_2piCNVLine                                               ##
## StrippingB2XGamma_piKsLine                                                 ##
## StrippingB2XGamma_3piLine                                                  ##
## StrippingB2XGamma_3pi_VanyaLine                                            ##
## StrippingB2XGamma_3pi_altLine                                              ##
## StrippingB2XGamma_3piCNVLine                                               ##
## StrippingB2XGamma_2pipi0RLine                                              ##
## StrippingB2XGamma_2pipi0MLine                                              ##
## StrippingB2XGamma_2piKsLine                                                ##
## StrippingB2XGamma_4piLine                                                  ##
## StrippingB2XGamma_4piVanyaLine                                             ##
## StrippingB2XGamma_4pi_2rhoLine                                             ##
## StrippingB2XGamma_3piKsLine                                                ##
## StrippingB2XGamma_3pipi0RLine                                              ##
## StrippingB2XGamma_3pipi0MLine                                              ##
## StrippingB2XGamma_2pi2KsLine                                               ##
## StrippingB2XGamma_LambdapiLine                                             ##
## StrippingB2XGamma_Lambda2piLine                                            ##
## StrippingB2XGamma_Lambda3piLine                                            ##
## Authors: pablo.ruiz.valls@cern.ch, albert.puig@cern.ch                     ##
################################################################################

B2XGamma = {
    'WGs' : ['RD'],
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
        'B2XGTrkChi2DOF': 3.,
        'B2XGTrkGhostProb' : 0.4,
        'B2XGTrkMinIPChi2' : 16.,
        'B2XGTrkMinPT'     : 300.,
        'B2XGTrkMinP'      : 1000,

        # PHOTON CUTS
        'B2XGGammaPTMin'       : 2000.,
        'B2XGGammaCL'          : 0.,
        'B2XGGammaCNVPTMin'       : 1000.,

        # TRACK COMBINATION CUTS
        'B2XGResMinPT'     : 150.,
        'B2XGResMinMass'   : 0.,
        'B2XGResMaxMass'   : 7900.,
        'B2XGResVtxChi2DOF': 10.,
        'B2XGResSumPtMin'  : 1500.,
        'B2XGResDocaMax'   : 100.5,
        'B2XGResBPVVDCHI2Min'  : 0.,
        'B2XGResIPCHI2Min' : 0.0,

        # B HADRON CUTS
        'B2XGBMinPT'       : 200.,
        'B2XGBMinM2pi'     : 3280.,
        'B2XGBMinM3pi'     : 2900.,
        'B2XGBMinM4pi'     : 2560.,
        'B2XGBMinMLambda'  : 2560.,
        'B2XGBMaxM'        : 9000.,
        'B2XGBSumPtMin' : 5000,
        'B2XGBMinBPVDIRA'  : 0.0,
        'B2XGBMaxCorrM'    : 73000.,
        'B2XGBVtxChi2DOF'  : 9.,
        'B2XGBVtxMaxIPChi2': 9.
            },
    'STREAMS' : ['Leptonic'],
    }

################################################################################
## StrippingBeauty2XGammaExclusive.py                                         ##
## Exclusive Radiative lines                                                  ##
## ----------------------------------                                         ##
## StrippingB2XGammaExclBs2PhiGammaLine                                       ##
## StrippingB2XGammaExclBd2KstGammaLine                                       ##
## Authors: Fatima Soomro, Albert Puig, Pablo Ruiz Valls                      ##
################################################################################
B2XGammaExcl = {
    'WGs'         : ['RD'],
    'BUILDERTYPE' : 'Beauty2XGammaExclusiveConf',
    'CONFIG'      : {
         'TrIPchi2'             : 25.       # Dimensionless
         ,'TrChi2'              : 5.        # dimensionless

         ,'PhiMassWin'          : 15.       # MeV
         ,'KstMassWin'          : 100.      # MeV
         ,'PhiVCHI2'            : 9.       # dimensionless
         ,'KstVCHI2'            : 9.       # dimensionless

         ,'photonPT'            : 2600.     # MeV
         ,'B_PT'                : 3000.     # MeV
         
         ,'BsMassWin'           : 1000.     # MeV
         ,'B0MassWin'           : 1000.     # MeV
         ,'BsDirAngle'          : 0.0      # radians
         ,'B0DirAngle'          : 0.02      # radians
         ,'BsPVIPchi2'          : 15.       # Dimensionless
         ,'B0PVIPchi2'          : 15.       # Dimensionless

         # Pre- and postscales
         ,'Bs2PhiGammaPreScale'               : 1.0
         ,'Bs2PhiGammaPostScale'              : 1.0
         ,'Bd2KstGammaPreScale'               : 1.0
         ,'Bd2KstGammaPostScale'              : 1.0
         },
    'STREAMS'   : {
               'Radiative' : ['StrippingB2XGammaExclBs2PhiGammaLine'],
               'Calibration' : ['StrippingB2XGammaExclBd2KstGammaLine'],
                   },
    }


################################################################################
## StrippingLb2L0Gamma.py                                                     ##
## Radiative Lambda_b Lines                                                   ##
## --------------------------                                                 ##
## StrippingLb2L0GammaLine                                                    ##
## StrippingLb2L0GammaConvertedLine                                           ##
## Author: albert.puig@cern.ch                                                ##
################################################################################
Lb2L0Gamma = {
    'WGs'         : ['RD'],
    'BUILDERTYPE' : 'StrippingLb2L0GammaConf',
    'CONFIG'      : {# Prescales
    'Lb2L0GammaPrescale'          : 1.0,
    'Lb2L0GammaConvertedPrescale' : 1.0,
                                       # Trigger cuts
    'L0'                          : ['Photon', 'Electron', 'Hadron'],
    'HLT'                         : [],
                                       # Track cuts
    'Track_Chi2ndf_Max'           : 3.0,
    'Track_MinChi2ndf_Max'        : 2.0,
    'Track_GhostProb_Max'         : 0.4,
    'TrackLL_IPChi2_Min'          : 16.0,
    'Pion_P_Min'                  : 2000.0,
    'Proton_P_Min'                : 7000.0, # Can increase
    'Pion_Pt_Min'                 : 300.0,
    'Proton_Pt_Min'               : 800.0, # Can increase
                                       # Lambda0 cuts
    'Lambda0_VtxChi2_Max'         : 9.0,
    'Lambda0LL_IP_Min'            : 0.05,
    'Lambda0LL_MassWindow'        : 20.0,
    'Lambda0DD_MassWindow'        : 30.0,
    'Lambda0_Pt_Min'              : 1000.0,
                                       # Photon cuts
    'Photon_PT_Min'               : 2500.0,
    'Photon_CL_Min'               : 0.2,
    'PhotonCnv_PT_Min'            : 1000.0,
    'PhotonCnv_MM_Max'            : 100.0,
    'PhotonCnv_VtxChi2_Max'       : 9.0,
                                       # Lambda_b cuts
    'Lambdab_VtxChi2_Max'         : 9.0,
    'Lambdab_Pt_Min'              : 1000.0,
    'Lambdab_SumPt_Min'           : 5000.0,
    'Lambdab_IPChi2_Max'          : 9.0,
    'Lambdab_MTDOCAChi2_Max'      : 7.0,
    'Lambdab_MassWindow'          : 1100.0,
         },
    'STREAMS' : ['Leptonic']
    }




################################################################################
## StrippingBs2gammagamma.py                                                  ##
## Bs -> gamma gamma lines                                                    ##
## -----------------------------------------                                  ##
## StrippingBs2gammagamma_LLLine                                              ##
## StrippingBs2gammagamma_DDLine                                              ##
## StrippingBs2gammagamma_DoubleLine                                          ##
## StrippingBs2gammagamma_NoConvLine                                          ##
## StrippingBs2gammagamma_NoConvWideLine                                      ##
## Author: Sean Benson                                                        ##
################################################################################
Bs2GammaGamma = {
    'WGs'         : ['RD'],
    'BUILDERTYPE' : 'StrippingBs2gammagammaConf',
    'CONFIG'      : { 'gammaPT'             : 1250    # MeV/c
                      ,'gammaP'              : 11000   # MeV/c
                      ,'gammaCL'             : 0.0     # adimensional
                      ,'gammaConvPT'         : 1400    # MeV/c
                      ,'gammaConvIPCHI'      : 1.5     # adimensional
                      ,'gammaNonePT'         : 1700    # MeV/c
                      ,'gammaNoneP'          : 16000   # MeV/c
                      ,'gammaNoneCL'         : 0.42    # adimensional
                      ,'BsPT'                : 1000    # MeV/c
                      ,'BsVertexCHI2pDOF'    : 20      # adimensional
                      ,'BsLowMass'           : 4600    # MeV/cc
                      ,'BsNonePT'            : 2000    # MeV/c
                      ,'BsLowMassDouble'     : 4300    # MeV/cc
                      ,'BsLowMassNone'       : 4900    # MeV/cc
                      ,'BsHighMass'          : 5800    # MeV/cc
                      ,'BsHighMassNone'      : 6000    # MeV/cc
                      ,'BsHighMassDouble'    : 5800    # MeV/cc
                      },
    'STREAMS'     : ['Leptonic']
    }

################################################################################
## StrippingBd2eeKstarBDT.py                                                  ##
## Bd -> K* e+e- line                                                         ##
## -----------------------------------------                                  ##
## StrippingBd2eeKstarBDTLine                                                 ##
## StrippingBd2eeKstarBDTLine2                                                ##
## Authors: Jibo He, Marie-Helene Schune                                      ##
################################################################################
Bd2eeKstarBDT = {
    'WGs' : ['RD'],
    'BUILDERTYPE' : 'Bd2eeKstarBDTConf',
    'CONFIG' :  {
        'LinePrescale'            :    1.   ,
        'LinePostscale'           :    1.   ,
        #
        'ElectronPT'              :  200.   ,  # MeV
        'ElectronTrackCHI2pNDOF'  :    5.   ,
        'ElectronGhostProb'       :    0.5  ,
        'ElectronIPCHI2'          :    1.   ,
        'ElectronPIDepi'          :   -2.   ,
        #
        'eeCuts'                  : "(HASVERTEX) & (VFASPF(VCHI2)<16) & (((MM<1.5*GeV)) | ((MM>2.2*GeV) & (MM<4.2*GeV)))",
        #
        'KaonPT'                  :  400.   ,  # MeV
        'KaonP'                   : 3000.   ,  # MeV
        'KaonTrackCHI2pNDOF'      :    5.   ,
        'KaonGhostProb'           :    0.35 ,
        'KaonIPCHI2'              :    4.   ,
        'KaonPIDKpi'              :   -5.   ,
        #
        'PionPT'                  :  250.   ,  # MeV
        'PionP'                   : 2000.   ,  # MeV
        'PionTrackCHI2pNDOF'      :    5.   ,
        'PionGhostProb'           :    0.35 ,
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
        'BDTCutValue'             :   -0.95 ,
        'BDTWeightsFile'          : '$TMVAWEIGHTSROOT/data/Bd2eeKstar_BDTG_v1r0.xml',
        #
        'RelatedInfoTools'       : [ { "Type" : "RelInfoBs2MuMuTrackIsolations"
                                       , "RecursionLevel" : 2
                                       , "Locations" : { 'Phys/StdAllLooseElectrons' : [ 'Electron1ISO', 'Electron2ISO' ] ,
                                                         'Phys/StdAllLooseKaons'     :  'KaonISO',
                                                         'Phys/StdAllLoosePions'     :  'PionISO'
                                                         }
                                       , "tracktype"  : 3
                                       , "angle"      : 0.27
                                       , "fc"         : 0.60
                                       , "doca_iso"   : 0.13
                                       , "ips"        : 3.0
                                       , "svdis"      : -0.15
                                       , "svdis_h"    : 30.
                                       , "pvdis"      : 0.5
                                       , "pvdis_h"    : 40.
                                       , "makeTrackCuts" : False
                                       , "IsoTwoBody" : False
                                       } ],
        'RelatedInfoTools2'       : [ { "Type" : "RelInfoBs2MuMuTrackIsolations"
                                        , "RecursionLevel" : 2
                                        , "Locations" : { 'Phys/StdDiElectronFromTracks' : [ 'Electron1ISO', 'Electron2ISO' ] ,
                                                          'Phys/StdAllLooseKaons'     :  'KaonISO',
                                                          'Phys/StdAllLoosePions'     :  'PionISO'
                                                          }
                                        , "tracktype"  : 3
                                        , "angle"      : 0.27
                                        , "fc"         : 0.60
                                        , "doca_iso"   : 0.13
                                        , "ips"        : 3.0
                                        , "svdis"      : -0.15
                                        , "svdis_h"    : 30.
                                        , "pvdis"      : 0.5
                                        , "pvdis_h"    : 40.
                                        , "makeTrackCuts" : False
                                        , "IsoTwoBody" : False
                                        } ],
        },
    'STREAMS' : ['Bhadron']
}


################################################################################
##                              VRD SubGroup Lines                            ##
################################################################################
## StrippingDarkBoson.py                                                     ###
## Dark Boson lines, going to mixed DST formats                               ##
## ----------------------------------------------                             ##
## StrippingB2KpiX2MuMuDDDarkBosonLine                                        ##
## StrippingB2KpiX2MuMuSSDarkBosonLine                                        ##
## StrippingB2KpiX2MuMuDDSSDarkBosonLine                                      ##
## StrippingB2KKX2MuMuDDDarkBosonLine                                         ##
## StrippingB2KKX2MuMuSSDarkBosonLine                                         ##
## StrippingB2KKX2MuMuDDSSDarkBosonLine                                       ##
## StrippingB2KX2MuMuDDDarkBosonLine                                          ##
## StrippingB2KX2MuMuSSDarkBosonLine                                          ##
## StrippingB2KX2MuMuDDSSDarkBosonLine                                        ##
## StrippingB2KX2PiPiSSDarkBosonLine                                          ##
## StrippingB2KX2PiPiDDSSDarkBosonLine                                        ##
## StrippingB2RhoX2MuMuDarkBosonLine                                          ##
## StrippingB2RhoX2MuMuDDDarkBosonLine                                        ##
## StrippingB2KpiX2EESSDarkBosonLine                                          ##
## StrippingB2KKX2EESSDarkBosonLine                                           ##
## StrippingB2KX2KKDDDarkBosonLine                                            ##
## StrippingB2KX2EESSDarkBosonLine                                            ##
## StrippingB2KX2KKSSDarkBosonLine                                            ##
## StrippingB2JKDarkBosonLine                                                 ##
## StrippingB2JKstDarkBosonLine                                               ##
## StrippingB2KKX2EEDarkBosonLine                                             ##
## StrippingB2KKX2MuMuDarkBosonLine                                           ##
## StrippingB2KpiX2EEDarkBosonLine                                            ##
## StrippingB2KpiX2MuMuDarkBosonLine                                          ##
## StrippingB2KX2EEDarkBosonLine                                              ##
## StrippingB2KX2KKDarkBosonLine                                              ##
## StrippingB2KX2KKDDSSDarkBosonLine                                          ##
## StrippingB2KX2MuMuDarkBosonLine                                            ##
## StrippingB2KX2PiPiDarkBosonLine                                            ##
## StrippingB2KX2PiPiDDDarkBosonLine                                          ##
## StrippingB2RhoX2MuMuDarkBosonLine                                          ##
## Authors: rvazquez@cern.ch, mwill@mit.edu                                  ###
################################################################################
DarkBoson = {
    'WGs' : ['RD'],
    'BUILDERTYPE' : 'DarkBosonConf',
    'CONFIG' : {
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
    },
    'STREAMS' : {
        'Dimuon' : [
           'StrippingB2KpiX2MuMuDDDarkBosonLine',
           'StrippingB2KpiX2MuMuDDSSDarkBosonLine',
           'StrippingB2KKX2MuMuDDDarkBosonLine',
           'StrippingB2KKX2MuMuDDSSDarkBosonLine',
           'StrippingB2KX2MuMuDDDarkBosonLine',
           'StrippingB2KX2MuMuDDSSDarkBosonLine',
           'StrippingB2KX2PiPiDDSSDarkBosonLine',
           'StrippingB2RhoX2MuMuDDDarkBosonLine',
           'StrippingB2KX2KKDDDarkBosonLine',
           'StrippingB2KX2KKDDSSDarkBosonLine',
           'StrippingB2KX2PiPiDDDarkBosonLine',
           ],
        'Leptonic' : [
           'StrippingB2KpiX2MuMuSSDarkBosonLine',
           'StrippingB2KKX2MuMuSSDarkBosonLine',
           'StrippingB2KX2MuMuSSDarkBosonLine',
           'StrippingB2KX2PiPiSSDarkBosonLine',
           'StrippingB2RhoX2MuMuDarkBosonLine',
           'StrippingB2KpiX2EESSDarkBosonLine',
           'StrippingB2KKX2EESSDarkBosonLine',
           'StrippingB2KX2EESSDarkBosonLine',
           'StrippingB2KX2KKSSDarkBosonLine',
           'StrippingB2JKDarkBosonLine',
           'StrippingB2JKstDarkBosonLine',
           'StrippingB2KKX2EEDarkBosonLine',
           'StrippingB2KKX2MuMuDarkBosonLine',
           'StrippingB2KpiX2EEDarkBosonLine',
           'StrippingB2KpiX2MuMuDarkBosonLine',
           'StrippingB2KX2EEDarkBosonLine',
           'StrippingB2KX2KKDarkBosonLine',
           'StrippingB2KX2MuMuDarkBosonLine',
           'StrippingB2KX2PiPiDarkBosonLine',
           ],
        },
    }

################################################################################
## StrippingB2XTau.py                                                         ##
## B -> Tau inclusive Lines                                                   ##
## StrippingB2XTau_DD_Line                                                    ##
## StrippingB2XTau_DD_SameSign_Line                                           ##
## StrippingB2XTau_DPi_Line                                                   ##
## StrippingB2XTau_DPi_SameSign_Line                                          ##
## StrippingB2XTau_TauMu_piSS_TOSLine                                         ##
## StrippingB2XTau_TauMu_SameSign_TOSLine                                     ##
## StrippingB2XTau_TauMu_TOSLine                                              ##
## StrippingB2XTau_TauTau_piSS_TOSLine                                        ##
## StrippingB2XTau_TauTau_SameSign_TOSLine                                    ##
## StrippingB2XTau_TauTau_TOSLine                                             ##
## Authors: V. Gligorov, Ch. Elsasser, G. Mancinelli, J. Serrano              ##
################################################################################
B2XTau = {
    'WGs' : ['RD'],
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
         'B2DPi_LinePrescale'            : 1,
         'B2DPi_LinePostscale'           : 1,
         'B2TauTau_SameSign_TOSLinePrescale'      : 0.5,
         'B2TauTau_SameSign_TOSLinePostscale'     : 1,
         'B2DD_SameSign_LinePrescale'             : 0.5,
         'B2DD_SameSign_LinePostscale'            : 1,
         'B2TauMu_SameSign_TOSLinePrescale'       : 0.5,
         'B2TauMu_SameSign_TOSLinePostscale'      : 1,
         'B2DPi_SameSign_LinePrescale'            : 0.5,
         'B2DPi_SameSign_LinePostscale'           : 1,
         'RelatedInfoTools'      : [
              { "Type" : "RelInfoBstautauMuonIsolationBDT"
                ,"RecursionLevel" : 0
                , "Variables" : ['BSTAUTAUMUONISOBDTFIRSTVALUE', 'BSTAUTAUMUONISOBDTSECONDVALUE','BSTAUTAUMUONISOBDTTHIRDVALUE']
                , "Location"  : "MuonIsolationBDT"  
                },
              #2
              { "Type" : "RelInfoBstautauMuonIsolation"
                ,"RecursionLevel" : 0
                , "Variables" : ['BSTAUTAUMUONISOFIRSTVALUE', 'BSTAUTAUMUONISOSECONDVALUE']
                , "Location"  : "MuonIsolation"  
                },
              #3
              { "Type" : "RelInfoBstautauTauIsolationBDT"
                ,"RecursionLevel" : 0
                , "Variables" : ['BSTAUTAUTAUISOBDTFIRSTVALUETAUP', 'BSTAUTAUTAUISOBDTSECONDVALUETAUP','BSTAUTAUTAUISOBDTTHIRDVALUETAUP','BSTAUTAUTAUISOBDTFIRSTVALUETAUM', 'BSTAUTAUTAUISOBDTSECONDVALUETAUM','BSTAUTAUTAUISOBDTTHIRDVALUETAUM']
                , "Location"  : "TauIsolationBDT"  
                },
              #4
              { "Type" : "RelInfoBstautauTauIsolation"
                ,"RecursionLevel" : 0
                , "Variables" : ['BSTAUTAUTAUISOFIRSTVALUETAUP', 'BSTAUTAUTAUISOSECONDVALUETAUP','BSTAUTAUTAUISOFIRSTVALUETAUM', 'BSTAUTAUTAUISOSECONDVALUETAUM']
                , "Location"  : "TauIsolation"  
                },
              #5
              
              { "Type" : "RelInfoBstautauTrackIsolationBDT" 
                ,"RecursionLevel" : 0
                , "Variables" : ['BSTAUTAUTRACKISOBDTFIRSTVALUETAUPPIM', 'BSTAUTAUTRACKISOBDTSECONDVALUETAUPPIM','BSTAUTAUTRACKISOBDTTHIRDVALUETAUPPIM','BSTAUTAUTRACKISOBDTFIRSTVALUETAUPPIP1', 'BSTAUTAUTRACKISOBDTSECONDVALUETAUPPIP1','BSTAUTAUTRACKISOBDTTHIRDVALUETAUPPIP1','BSTAUTAUTRACKISOBDTFIRSTVALUETAUPPIP2', 'BSTAUTAUTRACKISOBDTSECONDVALUETAUPPIP2','BSTAUTAUTRACKISOBDTTHIRDVALUETAUPPIP2','BSTAUTAUTRACKISOBDTFIRSTVALUETAUMPIP','BSTAUTAUTRACKISOBDTSECONDVALUETAUMPIP','BSTAUTAUTRACKISOBDTTHIRDVALUETAUMPIP','BSTAUTAUTRACKISOBDTFIRSTVALUETAUMPIM1','BSTAUTAUTRACKISOBDTSECONDVALUETAUMPIM1','BSTAUTAUTRACKISOBDTTHIRDVALUETAUMPIM1','BSTAUTAUTRACKISOBDTFIRSTVALUETAUMPIM2', 'BSTAUTAUTRACKISOBDTSECONDVALUETAUMPIM2','BSTAUTAUTRACKISOBDTTHIRDVALUETAUMPIM2']
                , "Location"  : "TrackIsolationBDT"  
                },
              
              #6
              { "Type" : "RelInfoBstautauTrackIsolation" 
                ,"RecursionLevel" : 0
                , "Variables" : ['BSTAUTAUTRACKISOFIRSTVALUETAUPPIM', 'BSTAUTAUTRACKISOFIRSTVALUETAUPPIP1','BSTAUTAUTRACKISOFIRSTVALUETAUPPIP2', 'BSTAUTAUTRACKISOFIRSTVALUETAUMPIP','BSTAUTAUTRACKISOFIRSTVALUETAUMPIM1', 'BSTAUTAUTRACKISOFIRSTVALUETAUMPIM2']
                , "Location"  : "TrackIsolation"  
                },
              #7
              { "Type" : "RelInfoBstautauCDFIso" 
                ,"RecursionLevel" : 0
                #, "Variables" : ['BSTAUTAUCDFISO']
                , "Location"  : "CDFIso"  
                }#,
              #8
              #{ "Type" : "RelInfoBstautauZVisoBDT" 
              #  ,"RecursionLevel" : 0
              #  , "Variables" : ['ZVISOTAUP','ZVISOTAUM']
              #  , "Location"  : "ZVisoBDT"
              #  }
              ]
         },
    'STREAMS' : ['BhadronCompleteEvent']
    }

################################################################################
## StrippingZVTOP.py                                                          ##
## B->TauTau & High Masses Lines                                              ##
## StrippingB2XTauZVTOP_High_Line                                             ##
## StrippingB2XTauZVTOP_TauTau_Line                                           ##
## Authors: G. Mancinelli, J Cogan                                            ##
################################################################################
B2XTauZVTOP = {
    'WGs' : ['RD'],
    'BUILDERTYPE' : 'ZVTOP_Conf',
    'CONFIG' :  {
         'B2TauTau_LinePrescale'      : 1,
         'B2TauTau_LinePostscale'     : 1,
         'B2TauTauSS_LinePrescale'      : 1,
         'B2TauTauSS_LinePostscale'     : 1,
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
         'HltFilter'               : "HLT_PASS_RE('Hlt2(Topo2BodyBBDT|Topo3BodyBBDT|Topo4BodyBBDT).*Decision')",
         'RelatedInfoTools'      : [
             { "Type" : "RelInfoBstautauMuonIsolationBDT"
               ,"RecursionLevel" : 0
               , "Variables" : ['BSTAUTAUMUONISOBDTFIRSTVALUE', 'BSTAUTAUMUONISOBDTSECONDVALUE','BSTAUTAUMUONISOBDTTHIRDVALUE']
               , "Location"  : "MuonIsolationBDT"  
               },
             { "Type" : "RelInfoBstautauMuonIsolation"
               ,"RecursionLevel" : 0
               , "Variables" : ['BSTAUTAUMUONISOFIRSTVALUE', 'BSTAUTAUMUONISOSECONDVALUE']
               , "Location"  : "MuonIsolation"  
               },
             { "Type" : "RelInfoBstautauTauIsolationBDT"
               ,"RecursionLevel" : 0
               , "Variables" : ['BSTAUTAUTAUISOBDTFIRSTVALUETAUP', 'BSTAUTAUTAUISOBDTSECONDVALUETAUP','BSTAUTAUTAUISOBDTTHIRDVALUETAUP','BSTAUTAUTAUISOBDTFIRSTVALUETAUM', 'BSTAUTAUTAUISOBDTSECONDVALUETAUM','BSTAUTAUTAUISOBDTTHIRDVALUETAUM']
               , "Location"  : "TauIsolationBDT"  
               },
             { "Type" : "RelInfoBstautauTauIsolation"
               ,"RecursionLevel" : 0
               , "Variables" : ['BSTAUTAUTAUISOFIRSTVALUETAUP', 'BSTAUTAUTAUISOSECONDVALUETAUP','BSTAUTAUTAUISOFIRSTVALUETAUM', 'BSTAUTAUTAUISOSECONDVALUETAUM']
               , "Location"  : "TauIsolation"  
               },
             { "Type" : "RelInfoBstautauTrackIsolationBDT" 
               ,"RecursionLevel" : 0
               , "Variables" : ['BSTAUTAUTRACKISOBDTFIRSTVALUETAUPPIM', 'BSTAUTAUTRACKISOBDTSECONDVALUETAUPPIM','BSTAUTAUTRACKISOBDTTHIRDVALUETAUPPIM','BSTAUTAUTRACKISOBDTFIRSTVALUETAUPPIP1', 'BSTAUTAUTRACKISOBDTSECONDVALUETAUPPIP1','BSTAUTAUTRACKISOBDTTHIRDVALUETAUPPIP1','BSTAUTAUTRACKISOBDTFIRSTVALUETAUPPIP2', 'BSTAUTAUTRACKISOBDTSECONDVALUETAUPPIP2','BSTAUTAUTRACKISOBDTTHIRDVALUETAUPPIP2','BSTAUTAUTRACKISOBDTFIRSTVALUETAUMPIP','BSTAUTAUTRACKISOBDTSECONDVALUETAUMPIP','BSTAUTAUTRACKISOBDTTHIRDVALUETAUMPIP','BSTAUTAUTRACKISOBDTFIRSTVALUETAUMPIM1','BSTAUTAUTRACKISOBDTSECONDVALUETAUMPIM1','BSTAUTAUTRACKISOBDTTHIRDVALUETAUMPIM1','BSTAUTAUTRACKISOBDTFIRSTVALUETAUMPIM2', 'BSTAUTAUTRACKISOBDTSECONDVALUETAUMPIM2','BSTAUTAUTRACKISOBDTTHIRDVALUETAUMPIM2']
               , "Location"  : "TrackIsolationBDT"  
               },
             { "Type" : "RelInfoBstautauTrackIsolation" 
               ,"RecursionLevel" : 0
               , "Variables" : ['BSTAUTAUTRACKISOFIRSTVALUETAUPPIM', 'BSTAUTAUTRACKISOFIRSTVALUETAUPPIP1','BSTAUTAUTRACKISOFIRSTVALUETAUPPIP2', 'BSTAUTAUTRACKISOFIRSTVALUETAUMPIP','BSTAUTAUTRACKISOFIRSTVALUETAUMPIM1', 'BSTAUTAUTRACKISOFIRSTVALUETAUMPIM2']
               , "Location"  : "TrackIsolation"  
               },
             { "Type" : "RelInfoBstautauCDFIso" 
               ,"RecursionLevel" : 0
               #, "Variables" : ['BSTAUTAUCDFISO']
               , "Location"  : "CDFIso"  
               }#,
             #{ "Type" : "RelInfoBstautauZVisoBDT" 
             #  ,"RecursionLevel" : 0
             #  , "Variables" : ['ZVISOTAUP','ZVISOTAUM']
             #  , "Location"  : "ZVisoBDT"
             #  }
             ]
         },
    'STREAMS' : ['BhadronCompleteEvent']
    }

################################################################################
## StrippingBs2MuMuLines.py                                                   ##
## Bs->MuMu Lines                                                             ##
## StrippingBs2MuMusBd2JPsiKstLine                                            ##
## StrippingBs2JPsiPhiLine                                                    ##
## StrippingBs2MuMusBu2JPsiKLine                                              ##
## StrippingBs2MuMusNoMuIDLine                                                ##
## StrippingBs2MuMusNoMuIDLooseLine                                           ##
## StrippingBs2MuMusSSLine                                                    ##
## StrippingBs2MuMusWideMassLine                                              ##
## StrippingBs2MuMusLTUBLine                                                  ##
## StrippingBs2MuMusBs2KKLTUBLine                                             ##
## Authors: Marc-Olivier Bettler                                              ##
################################################################################

Bs2MuMus = {
    'WGs' : ['RD'],
    'BUILDERTYPE' : 'Bs2MuMuLinesConf',
    'CONFIG' : {
           'DefaultLinePrescale'    : 1,
           'DefaultPostscale'   : 1,
           'Bs2mmWideLinePrescale'  : 1,
           'LooseLinePrescale'      : 0.,
           'BuPrescale'    : 1,
           'BsPrescale'    : 1,
           'BdPrescale'    : 1,
           'JPsiLinePrescale'       : 1,
           'JPsiLooseLinePrescale'  : 0.1,
           'JPsiPromptLinePrescale' : 0.005,
           'SSPrescale'             : 1 ,
           'Bs2mmLTUBLinePrescale'  : 1 ,
           'Bs2KKLTUBLinePrescale' : 1 ,
           'MuIPChi2_loose'        :  9,
           'MuTrChi2_loose'        : 10,
           'BIPChi2_loose'         : 64,
           'BFDChi2_loose'         : 100,
           'BPVVDChi2'            : 121,
           'daugt_IPChi2'            : 9
           },
    'STREAMS' : {
            'Leptonic' : ['StrippingBs2MuMusNoMuIDLine'
                          ,'StrippingBs2MuMusWideMassLine'
                          ,'StrippingBs2MuMusNoMuIDLooseLine'
                          ,'StrippingBs2MuMusBu2JPsiKLine'
                          ,'StrippingBs2MuMusBs2JPsiPhiLine'
                          ,'StrippingBs2MuMusBd2JPsiKstLine'
                          ,'StrippingBs2MuMusSSLine'
                          ,'StrippingBs2MuMusLTUBLine'
                          ,'StrippingBs2MuMusBs2KKLTUBLine'],
            'Dimuon' : ['StrippingBs2MuMusNoMuIDLine'
                        ,'StrippingBs2MuMusWideMassLine']

            }
    }

################################################################################
## StrippingBu2MuNu.py                                                        ##
## Stripping Line for BMuNu (tight muons with high pT and IP)                 ##
## StrippingBu2MuNuLine                                                       ##
## Authors: Xabier Cid Vidal                                                  ##
################################################################################

Bu2MuNu = {
    'WGs' : ['RD'],
    'BUILDERTYPE' : 'Bu2MuNuConf',
    'CONFIG': {
          'PrescaleSignal'  : 1.0,
          'PostscaleSignal' : 1.0,
          'PrescaleControl'  : 0.015,
          'PostscaleControl' : 1.0,
          'pTminMu'     : 5.,
          'pTmaxMu'     : 40.,
          'pmaxMu'      : 500.,
          'IPchi2Mu'    : 400.,
          'TrChi2Mu'    : 3,
          'useNN'        : True,
          'maxNTracks'  : 150,
          'pTminMuControl': 2.5,
          'IPchi2MuControl': 100.
          },
    'STREAMS' : ['Leptonic'],
    }

################################################################################
## StrippingD23MuLines.py                                                     ##
## D+ -> mu l+ l- lines                                                       ##
## StrippingD23MuLinesD23MuLine                                               ##
## StrippingD23MuLinesD2MueeLine                                              ##
## StrippingD23MuLinesD23PiLine                                               ##
## Author: Oliver Gruenberg                                                   ##
################################################################################

D23MuLines = {
    'WGs' : ['RD'],
    'BUILDERTYPE' : 'D23MuLinesConf',
    'CONFIG' : {
         'Postscale'            :1,
         'D23MuPrescale'        :1,
         'D2MueePrescale'       :1,
         'D23PiPrescale'        :0.01,
         },
    'STREAMS' : ['Leptonic'],
    }

################################################################################
## StrippingB23MuLines.py                                                     ##
## B+ -> mu l+ l- lines                                                       ##
## StrippingB23MuLinesB23MuLine                                               ##
## StrippingB23MuLinesB2MueeLine                                              ##
## StrippingB23MuLinesB2DMuLine                                               ##
## StrippingB23MuLinesB23PiLine                                               ##
## Author: Oliver Gruenberg                                                   ##
################################################################################

B23MuLines = {
    'WGs' : ['RD'],
    'BUILDERTYPE' : 'B23MuLinesConf',
    'CONFIG' : {
        'Postscale'            :1,
        'B23MuPrescale'        :1,
        'B2MueePrescale'       :1,
        'B2DMuPrescale'        :1,
        'B23PiPrescale'        :1,
        },
    'STREAMS' : ['Leptonic'],
        }

################################################################################
## StrippingB24pLines.py                                                      ##
## B0 -> p+ p+ p- p- lines                                                    ##
## StrippingB24pLinesB24pLine                                                 ##
## StrippingB24pLinesB2JpsiKpiLine                                            ##
## Author: Oliver Gruenberg                                                   ##
################################################################################ 
B24pLines = {
    'WGs' : ['RD'],
    'BUILDERTYPE' : 'B24pLinesConf',
    'CONFIG' :{
         'Postscale'             :1,
         'B24pPrescale'          :1,
         'B2JpsiKpiPrescale'     :1,
         },
    'STREAMS' : ['Bhadron'],
    }


################################################################################
## StrippingBLVLines.py                                                       ##
## Baryon-lepton number violating lines                                       ##
## StrippingBLVLinesLa2KmuLine                                                ##
## StrippingBLVLinesLb2KmuLine                                                ##
## StrippingBLVLinesLb2DmuLine                                                ##
## StrippingBLVLinesLb2DsmuLine                                               ##
## StrippingBLVLinesB2LcmuLine                                                ##
## StrippingBLVLinesLb2LcpiLine                                               ##
## StrippingBLVLinesB2DpiLine                                                 ##
## StrippingBLVLinesBs2DspiLine                                               ##
## StrippingBLVLinesB2LcpLine                                                 ##
## Author: Oliver Gruenberg                                                   ##
################################################################################

BLVLines = {
    'WGs' : ['RD'],
    'BUILDERTYPE' : 'BLVLinesConf',
    'CONFIG' : {
           'Postscale'           :1,
           ### Signal Ch.
           'La2KmuPrescale'      :1,
           'Lb2KmuPrescale'      :1,
           'Lb2DmuPrescale'      :1,
           'Lb2DsmuPrescale'     :1,
           'B2LcmuPrescale'      :1,
           ### Control Ch.
           'Lb2LcpiPrescale'     :0.1,
           'B2DpiPrescale'       :0.1,
           'Bs2DspiPrescale'     :0.1,
           'B2LcpPrescale'       :0.2,
            },
    'STREAMS' : ['Bhadron'],
            }
################################################################################
## StrippingK0s2Pi0MuMuLines.py                                               ##
## K0s-->Pi0MuMu lines                                                        ##
## StrippingK0s2Pi0MuMuSidebandLine                                           ##
## StrippingK0s2Pi0MuMuSignalLine                                             ##
## Authors: Xabier Cid Vidal,Diego Martinez Santos                            ##
################################################################################
K0s2Pi0MuMu = {
    'WGs' : ['RD'],
    'BUILDERTYPE' : 'K0s2Pi0MuMuLinesConf',
    'CONFIG' : {
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
               },
    'STREAMS' : ['Leptonic'],
    }

################################################################################
## StrippingK0s2MuMuLines.py                                                  ##
## K0s-->MuMu lines                                                           ##
## StrippingK0s2MuMuNoMuIDLine                                                ##
## StrippingK0s2MuMuK0s2mmLine                                                ##
## StrippingK0s2MuMuK0s2mmSBLine                                              ##
## Authors: Xabier Cid Vidal,Diego Martinez Santos                            ##
################################################################################
K0s2MuMu = {
    'WGs' : ['RD'],
    'BUILDERTYPE' : 'K0s2MuMuLinesConf',
    'CONFIG' : {'NoMuIDLinePrescale'    : 1e-03,
                'NoMuIDLinePostscale'   : 1,
                'K0s2mmLinePrescale'  : 1,
                'K0s2mmLinePostscale'  : 1,
                'K0s2mmSBLinePrescale'  : 0.1,
                'K0s2mmSBLinePostscale'  : 1,
                'minMuPT' : 0,  #MeV
                'minKsPT' : 0,  #MeV
                },
    'STREAMS' : ['Leptonic'],
    }

################################################################################
## StrippingKshort2MuMuMuMu.py                                                ##
## Kshort-->MuMuMuMu lines                                                    ##
## -----------------------------                                              ##
## StrippingKshort2MuMuMuMu_LongLine                                          ##
## StrippingKshort2MuMuMuMu_DownLine                                          ##
## Authors; Mike Sokoloff, Marc Olivier Bettler                               ##
################################################################################
Kshort2MuMuMuMu = {
    'WGs' : ['RD'],
    'BUILDERTYPE' : 'Kshort2MuMuMuMuConf',
    'CONFIG' : {'MuonP'         : 3000. ,    #MeV
                'MuonPT'        : 500.  ,    #MeV
                'MuonMINIPCHI2' : 2     ,    #adminensional
                'MuonTRCHI2'    : 5     ,    #adminensional


                                  #4body
                'MINIPCHI2_mumumumu' : 4. ,  #adminensional --  for the individual pions
                'PT_mumumumu'        : 300 , #MeV
                'MuonPIDmu_mumumumu' : -1,   #adimensional
                'DPT_mumumumu'           : 2500.,     #MeV
                'DVCHI2DOF_mumumumu'     : 8     ,    #adminensional
                'DMAXDOCA_mumumumu'      : 0.2  ,    #mm
                'KsDauMAXIPCHI2_mumumumu' : 15    ,    #adimensinal
                'DFDCHI2_mumumumu'       : 9     ,   #adimensional
                'DIPCHI2_mumumumu'       : 20    ,    #adimensional


                'DDIRA'         : 0.9999,    #adimensional
                'MaxDimuonMass'    : 260.  ,    #MeV
                'MaxKsMass'                   : 550.  ,    #MeV, comb mass window
                'KsMotherMassCut'             : 540.  ,    # MeV, mother mass cut

                'Kshort2MuMuMuMuLinePrescale'    : 1, 
                'Kshort2MuMuMuMuLinePostscale'   : 1
                },
    'STREAMS' : ['Leptonic']
    }
################################################################################
## StrippingKshort2PiPiMuMu.py                                                ##
## Kshort-->PiPiMuMu lines                                                    ##
## -----------------------------                                              ##
## StrippingKshort2PiPiMuMu_LongLine                                          ##
## StrippingKshort2PiPiMuMu_DownLine                                          ##
## Authors; Mike Sokoloff, Marc Olivier Bettler                               ##
################################################################################
Kshort2PiPiMuMu = {
    'WGs' : ['RD'],
    'BUILDERTYPE' : 'Kshort2PiPiMuMuConf',
    'CONFIG' : {
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
    'STREAMS' : ['Leptonic']
        }

################################################################################
## StrippingKshort2eePiPi.py                                                  ##
##  Kshort --> eePiPi Line                                                    ##
## StrippingKs2PiPiee_PiPiLine                                                ##
## Authors: Carla Marin                                                       ##
################################################################################

Ks2PiPiee = {
    'WGs' : ['RD'],
    'BUILDERTYPE' : 'Kshort2eePiPiConf',
    'CONFIG' : {
         'Kshort2eePiPiLinePrescale'    : 1 ,
         'Kshort2eePiPiLinePostscale'   : 1 ,
         'ePT'           : 100.          , #MeV
         'eMINIPCHI2'    : 16            , #adimensional
         'ePIDe'         : -4            , #adimensional
         'eGhostProb'    : 0.5           , #adimensional

         'PionMINIPCHI2' : 16            , #adimensional
         'PionPIDK'      : 5             , #adimensional
         'PionGhostProb' : 0.5           , #adimensional

         #4body
         'KsMAXDOCA'     : 1.            , #mm
         'KsLifetime'    : 0.01*89.53    , #0.01*10^-12s
         'KsIP'          : 1             , #mm
         'MaxKsMass'     : 800.          , #MeV, comb mass high limit
         },
    'STREAMS' : ['Leptonic'],
    }

################################################################################
## StrippingLc23MuLines.py                                                    ##
## Lambda_c -> mu ll lines                                                    ##
## StrippingLc23MuLinesLc23muLine                                             ##
## StrippingLc23MuLinesLc2mueeLine                                            ##
## StrippingLc23MuLinesLc2pmumuLine                                           ##
## StrippingLc23MuLinesLc2peeLine                                             ##
## StrippingLc23MuLinesLc2pKpiLine                                            ##
## Author: Oliver Gruenberg                                                   ##
################################################################################

Lc23MuLines = {
    'WGs' : ['RD'],
    'BUILDERTYPE' : 'Lc23MuLinesConf',
    'CONFIG' : {
         'Postscale'           :1,
         'Lc23muPrescale'      :1,
         'Lc2mueePrescale'     :1,
         'Lc2pmumuPrescale'    :1,
         'Lc2peePrescale'      :1,
         'Lc2pKpiPrescale'     :0.01,
         },
    'STREAMS' : ['Leptonic']
    }

################################################################################
## StrippingLFVLines.py                                                       ##
## Lepton flavor violation lines                                              ##
## StrippingLFVsB2eeLine                                                      ##
## StrippingLFVsB2eMuLine                                                     ##
## StrippingLFVsBu2KJPsieeLine                                                ##
## StrippingLFVsTau2eMuMuLine                                                 ##
## StrippingLFVsTau2PhiMuLine                                                 ##
## Authors: J. Albrecht, Archilli, F. Soomro, Dujany, Harrison                ##
################################################################################

LFV = {
    'BUILDERTYPE' : 'LFVLinesConf' ,
    'STREAMS' : [ 'Leptonic' ],
    'WGs'     : [ 'RD' ],
    'CONFIG'  : {
        'Postscale'           :1,
        'TauPrescale'         :1,
        'Tau2MuMuePrescale'         :1,
        'B2eMuPrescale'       :1,
        'B2eePrescale'        :1,
        'B2heMuPrescale'      :1,
        'B2pMuPrescale'       :1,
        'Bu2KJPsieePrescale'  :1,
        'B2TauMuPrescale'       :1,
        'B2hTauMuPrescale'      :1,
        'RelatedInfoTools'      : [
                { "Type" : "RelInfoBs2MuMuIsolations"
                  ,"RecursionLevel" : 0
                  , "Variables" : ['BSMUMUCDFISO', 'BSMUMUOTHERBMAG', 'BSMUMUOTHERBANGLE', 'BSMUMUOTHERBBOOSTMAG', 'BSMUMUOTHERBBOOSTANGLE', 'BSMUMUTRACKPLUSISO', 'BSMUMUTRACKMINUSISO', 'BSMUMUOTHERBTRACKS']
                  , "Location"  : "BSMUMUVARIABLES"
                  , "tracktype" : 3
                  , "angle"      : 0.27
                  , "fc"         : 0.60
                  , "doca_iso"   : 0.13
                  , "ips"        : 3.0
                  , "svdis"      : -0.15
                  , "svdis_h"    : 30.
                  , "pvdis"      : 0.5
                  , "pvdis_h"    : 40.
                  , "makeTrackCuts" : False
                  , "IsoTwoBody" : False
                  }
                ] #matches 'RelatedInfoTools'
        },
    }

################################################################################
## StrippingRareStrange.py                                                    ##
## Rare strange decays lines                                                  ##
## StrippingRareStrangeK3PiLine                                               ##
## StrippingRareStrangeKPiMuMuLine                                            ##
## StrippingRareStrangeKPiMuMuDownLine                                        ##
## StrippingRareStrangeKPiMuMuLFVLine                                         ##
## StrippingRareStrangeKPiMuMuLFVDownLine                                     ##
## StrippingRareStrangeKPiPiPiLine                                            ##
## StrippingRareStrangeKPiPiPiDownLine                                        ##
## StrippingRareStrangeKPiPiPiMassMeasLine                                    ##
## StrippingRareStrangeKPiPiPiMassMeasDownLine                                ##
## StrippingRareStrangeSigmaPEELine                                           ##
## StrippingRareStrangeSigmaPEEDownLine                                       ##
## StrippingRareStrangeSigmaPMuMuLine                                         ##
## StrippingRareStrangeSigmaPMuMuDownLine                                     ##
## StrippingRareStrangeSigmaPMuMuLFVLine                                      ##
## StrippingRareStrangeSigmaPMuMuLFVDownLine                                  ##
## Authors: J. Albrecht, Archilli, F. Soomro, Dujany, Harrison                ##
################################################################################

RareStrange = {
    'WGs'     : [ 'RD' ],
    'BUILDERTYPE' : 'RareStrangeLinesConf',
    'CONFIG' : {
         'Postscale'           :1,
        'SigmaPMuMuPrescale' : 1,
        'SigmaPMuMuDownPrescale' :1,
        'SigmaPEEPrescale' : 1,
        'SigmaPEEDownPrescale' : 0.1,
        'SigmaPMuMuLFVPrescale' :0.1,
        'SigmaPMuMuLFVDownPrescale' :0.1,
        'KPiPiPiPrescale' : 0.01,
        'KPiPiPiMassMeasPrescale' :1,
        'KPiMuMuPrescale' :1,
        'KPiMuMuLFVPrescale' :1,
        'KPiMuMuDownPrescale' :1,
        'KPiPiPiDownPrescale' : 0.1,
        'KPiPiPiMassMeasDownPrescale' :1,
        'KPiMuMuLFVDownPrescale' :1,
        'SigmaMinTauPs' : 6.,
        'SigmaMinPt' : 500.,
        'SigmaMaxDOCA' : 2.,
        'SigmaMassWin' : 500.,
        'SigmaMinDIRA' : 0.9,
        'SigmaMaxIpChi2' : 36.,
        'SigmaVtxChi2' : 36.,
        'SigmaMinTauPsDown' : 7.,
        'SigmaMinPtDown' : 0.,
        'SigmaMaxDOCADown' : 10.,
        'SigmaMassWinDown' : 500.,
        'SigmaPEEMassWinDown' : 100.,
        'SigmaMinDIRADown' : 0.9,
        'SigmaMaxIpChi2Down' : 25.,
        'SigmaVtxChi2Down' : 25.,
        'SigmaDauTrChi2Down': 9.,
        'muonMinIpChi2' : 9.,
        'protonPIDp': 5.,
        'electronPIDe':2.,
        'electronMinIpChi2': 9.,
        'muonMinIpChi2Down' : 9.,
        'electronMinIpChi2Down': 4.,
        'KMaxDOCA' : 3.,
        'KMinPT' : 100. ,
        'KMassWin' : 100.,
        'KMinDIRA' : 0.98,
        'KMaxIpChi2' : 25.,
        'KVtxChi2': 25.,
        'KMinVDChi2' : 36.,
        'KDauMinIpChi2' : 9.,
        'KDauTrChi2' : 3.,
        'KMaxDOCADown' : 10.,
        'KMinPTDown' : 0. ,
        'KMassWinDown' : 100.,
        'KMinDIRADown' : 0.98,
        'KVtxChi2Down': 25.,
        'KMinVDChi2Down' : 49.,
        'KDauMinIpChi2Down' : 5.,
        'KVDPVMinDown' : 500.,
        'KVDPVMaxDown' : 2500.,
        #Kaons stuff
          'KMaxDOCAMassMeas' : 2.,
        'KMinPTMassMeas' : 300. ,
        'KMassWinMassMeas' : 50.,
        'KMinDIRAMassMeas' : 0.9998,
        'KMaxIpChi2MassMeas' : 25.,
        'KVtxChi2MassMeas': 10.,
        'KMinVDChi2MassMeas' : 100.,
        'KDauMinIpChi2MassMeas' : 8., 
        #Kaons stuff downstream
        'KMaxDOCAMassMeasDown' : 9999.,
        'KMinPTMassMeasDown' : 250. ,
        'KMassWinMassMeasDown' : 100.,
        'KMinDIRAMassMeasDown' : 0.999,
        'KVtxChi2MassMeasDown': 20.,
        'KMinVDChi2MassMeasDown' : 64.,
        'KDauMinIpChi2MassMeasDown' : 4., 
        'KVDPVMinMassMeasDown' : 900.,
        'KVDPVMaxMassMeasDown' : 2200.
         },
    'STREAMS' : ['Leptonic']
    }

################################################################################
## StrippingTau2LambdaMuLines.py                                              ##
## tau -->KKMu stripping line                                                 ##
## StrippingTau2LambdaMuLine                                                  ##
## Author: Marcin Chrzaszcz                                                   ##
################################################################################
Tau2LambdaMu = {
    'WGs'     : [ 'RD' ],
    'BUILDERTYPE' : 'Tau2LambdaMuLinesConf',
    'CONFIG' : {
         'TauPrescale'         :1.,
         'TauPostscale'        :1.,
         'Tau2LambdaMuPrescale'  :1.
         },
    'STREAMS' : ['Leptonic']
    }

################################################################################
## StrippingTau23MuLines.py                                                   ##
## tau -->MuMuMu lines                                                        ##
## StrippingTau23MuTau23MuLine                                                ##
## StrippingTau23MuTau25MuLine                                                ##
## StrippingTau23MuTau2PMuMuLine                                              ##
## StrippingTau23MuDs2PhiPiLine                                               ##
## Author: Johannes Albrecht, Jon Harrison, Paul Seyfert                      ##
################################################################################
Tau23Mu = {
    'WGs'         : ['RD'],
    'BUILDERTYPE' : 'Tau23MuLinesConf',
    'CONFIG'      : {
         'TauPrescale'         :1.,
         'TauPostscale'        :1.,
         'Ds23PiTISPrescale'   :0.0,
         'Ds23PiPrescale'      :0.0,
         'Ds2PhiPiPrescale'    :1.,
         'Tau25Prescale'       :1.,
         'Tau2PMuMuPrescale' :1.
         },
    'STREAMS'     : { 'Leptonic' : ['StrippingTau23MuTau23MuLine','StrippingTau23MuDs2PhiPiLine','StrippingTau23MuTau2PMuMuLine','StrippingTau23MuDs23PiLine','StrippingTau23MuTau25MuLine']}
    }

################################################################################
## StrippingB2MuMuMuMuLines.py                                                ##
## B to 4 Mu lines                                                            ##
## StrippingB2MuMuMuMuLinesB2DetachedDimuonAndJpsi                            ##
## StrippingB2MuMuMuMuLinesB24Mu                                              ##
## Authors: Marc-Olivier Bettler, Francesco Dettori, Alexander Baranov        ##
################################################################################
B2MuMuMuMuLines = {
    'WGs'            : ['RD'],
    'BUILDERTYPE'    : 'B2MuMuMuMuLinesConf',
    'CONFIG'         : {
            'B2MuMuMuMuLinePrescale'    : 1,
            'B2MuMuMuMuLinePostscale'   : 1,
            'D2MuMuMuMuLinePrescale'    : 1,
            'D2MuMuMuMuLinePostscale'   : 1,
            'B2TwoDetachedDimuonLinePrescale'  : 1,
            'B2TwoDetachedDimuonLinePostscale' : 1,
            'B2JpsiKmumuLinePrescale'  : 1,
            'B2JpsiKmumuLinePostscale' : 1,
            'B2JpsiPhimumuLinePrescale'  : 1,
            'B2JpsiPhimumuLinePostscale' : 1,
            'B2DetachedDimuonAndJpsiLinePrescale' : 1,
            'B2DetachedDimuonAndJpsiLinePostscale': 1,
            'DetachedDiMuons': {
                 'AMAXDOCA_MAX'  : '0.5*mm',
                 'ASUMPT_MIN'    : '1000*MeV',
                 'VCHI2DOF_MAX'  : 16,
                 'BPVVDCHI2_MIN' : 16,
                 },
            'B2DetachedDiMuons': {
                'SUMPT_MIN'        : '2000*MeV',
                'VCHI2DOF_MAX'     : 6,
                'BPVIPCHI2_MAX'    : 16,
                'BPVVDCHI2_MIN'    : 50,
                'BPVDIRA_MIN'      : 0.0,
                'MASS_MIN'         : {'B':'4600*MeV'},
                'MASS_MAX'         : {'B':'6000*MeV'}
                },
            'B2DetachedDiMuonsAndJpsi': {
                'SUMPT_MIN'        : '2000*MeV',
                'VCHI2DOF_MAX'     : 6,
                'BPVIPCHI2_MAX'    : 16,
                'BPVVDCHI2_MIN'    : 50,
                'BPVDIRA_MIN'      : 0.0,
                'MASS_MIN'         : {'B':'4600*MeV'},
                'MASS_MAX'         : {'B':'7000*MeV'}
                }
            },
    'STREAMS' : { 'Bhadron' : ['StrippingB2MuMuMuMuLinesB2DetachedDimuonAndJpsiLine','StrippingB2MuMuMuMuLinesB24MuLine']}
    }
