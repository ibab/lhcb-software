################################################################################
##                          S T R I P P I N G  2 3                            ##
##                                                                            ##
##  Configuration for RD WG                                                   ##
##  Contact person: Pablo Ruiz Valls pruizval@cern.ch                         ##
################################################################################

from GaudiKernel.SystemOfUnits import *

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

Beauty2XGamma = {
    'WGs' : ['RD'],
    'BUILDERTYPE' : 'Beauty2XGammaConf',
    'CONFIG'      : {
    'B2XG2piPrescale'        : 1.,
    'B2XG2piCNVLLPrescale'   : 1.,
    'B2XG2piCNVDDPrescale'   : 1.,
    'B2XGpiKsPrescale'       : 1.,

    'B2XG3piPrescale'        : 1.,
    'B2XG3piCNVLLPrescale'   : 1.,
    'B2XG3piCNVDDPrescale'   : 1.,
    'B2XG2pipi0MPrescale'    : 1.,
    'B2XG2pipi0RPrescale'    : 1.,
    'B2XG2piKsPrescale'      : 1.,
    'B2PhiOmega2pipipi0MPrescale' : 1.,
    'B2PhiOmega2pipipi0RPrescale' : 1.,

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
    'B2XGTrkChi2DOF': 3., # was nothing
    'TrackGhostProb' : 0.4, # was 0.5
    'B2XGTrkMinIPChi2' : 16., # was 5
    'B2XGTrkMinPT'     : 300., # was 100
    'B2XGTrkMinP'      : 1000, # P was 2000, old stripping has 1 GeV...

              # PHOTON CUTS
    'B2XGGammaPTMin'       : 2000.,
    'B2XGGammaCL'          : 0.,# was 0.25
    'B2XGGammaCNVPTMin'       : 1000.,

              # Neutral pion cuts
    'Pi0MPMin': 4000.,
              #'Pi0MPTMin': 700., # was 1200
    'Pi0RPMin': 4000.,
    'Pi0MPTMin': 700.,
    'Pi0RPTMin': 700., # basic cut for phi/omega -> pi pi pi0 lines, recut harder for rest with below
    'Pi0RPTReCut' : 1700.,
    'Pi0MPTReCut' : 1200.,

              # TRACK COMBINATION CUTS
    'B2XGResMinPT'     : 150., #was 0
    'B2XGResMinMass'   : 0., #was 520
    'B2XGResMaxMass'   : 7900., # was 4940 (3500+4*(mK-mpi)). Now is maxMLambda+(mp-mpi)+(mK-mpi) to allow for Kp decays if StdLooseNoPIDPions also covers proton tracks
    'B2XGResVtxChi2DOF': 10., # was 25
    'B2XGResSumPtMin'  : 1500.,
    'B2XGResDocaMax'   : 100.5, # NOT USED
    'B2XGResBPVVDCHI2Min'  : 0., # FD avoid if possible. was 64
    'B2XGResIPCHI2Min' : 0.0,
    'B2XGPhiOmegaMinMass'   : 700., #
    'B2XGPhiOmegaMaxMass'   : 1300., # 2020

              # B HADRON CUTS
    'B2XGBMinPT'       : 200., #0
    'B2XGBMinM2pi'     : 3280., # was 3280
    'B2XGBMinM3pi'     : 2900., # was 2900
    'B2XGBMinM4pi'     : 2560., # was 2560
    'B2XGBMinMLambda'  : 2560., # was 2900
    'B2XGBMaxM'        : 9000., # was 8540
    'B2XGBSumPtMin' : 5000,  #SUMPT
    'B2XGBMinBPVDIRA'  : 0.0,  #BPVDIRA avoid if possible
    'B2XGBMaxCorrM'    : 73000., # NOT USED
    'B2XGBVtxChi2DOF'  : 9.,
    'B2XGBVtxMaxIPChi2': 9., # was 25
              },
    'STREAMS' : ['Leptonic'],
    }

Beauty2XGammaExclusive = {
    'WGs'         : ['RD'],
    'BUILDERTYPE' : 'Beauty2XGammaExclusiveConf',
    'CONFIG'      : {
    'TrIPchi2'             : 25.       # Dimensionless
    ,'TrChi2'              : 5.        # dimensionless
    #          ,'TrMaxPT'             : 1200.     # MeV

    ,'PhiMassWin'          : 15.       # MeV
    ,'KstMassWin'          : 100.      # MeV
    ,'PhiVCHI2'            : 9.       # dimensionless
    ,'KstVCHI2'            : 9.       # dimensionless

    ,'photonPT'            : 2600.     # MeV

    ,'B_PT'                : 3000.     # MeV

    ,'BsMassWin'           : 1000.     # MeV
    ,'B0MassWin'           : 1000.     # MeV
    ,'BsDirAngle'          : 0.02      # radians
    ,'B0DirAngle'          : 0.02      # radians
    ,'BsPVIPchi2'          : 15.       # Dimensionless
    ,'B0PVIPchi2'          : 15.       # Dimensionless
              # Pre- and postscales
    ,'Bs2PhiGammaPreScale'               : 1.0
    ,'Bs2PhiGammaPostScale'              : 1.0
    ,'Bd2KstGammaPreScale'               : 1.0
    ,'Bd2KstGammaPostScale'              : 1.0
              },
    'STREAMS' :  {
    'Radiative' : ['StrippingBeauty2XGammaExclusiveBs2PhiGammaLine'],
    'Calibration' : ['StrippingBeauty2XGammaExclusiveBd2KstGammaLine'],
              },
        }

Lb2L0Gamma = {
    'WGs'         : ['RD'],
    'BUILDERTYPE' : 'StrippingLb2L0GammaConf',
    'CONFIG'      : {# Prescales
    'Lb2L0GammaPrescale'          : 1.0,
    'Lb2L0GammaConvertedPrescale' : 1.0,
                                       # Trigger cuts
    'L0'                          : ['Photon', 'Electron', 'Hadron'],
    'HLT1'                         : [],
    'HLT2'                         : [],
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
                  'STREAMS' : ['Leptonic'],
                                   }

Bs2st2KKMuX = {
    'BUILDERTYPE' : 'Bs2st2KKMuXConf',
    'WGs'         : ['RD'],
    'STREAMS'     : ['Semileptonic'],
    'CONFIG'      : {
    'Bs2st2KKMuPrescale'    : 1.0,
    'Bs2st2KKMuWSPrescale'  : 1.0,
    'Bs2st2KKJpsiPrescale'  : 1.0,
    'Bs2st2KKJpsiWSPrescale': 1.0,

    'GEC_nLongTrk'     : 1000. ,# adimensional
    'HLT_FILTER'       : ''    ,

            # Kaon from B_s2*0 -> B K
    'K1PT'             : 500.  ,# MeV
    'K1PTLoose'        : 250.  ,# MeV
    'K1MinIPChi2'      : 9.    ,# adimensional
    'K1PIDK'           : 16.   ,# adimensional

            # Kaon from B -> K mu X
    'K2P'              : 3000. ,# MeV
    'K2PT'             : 1000. ,# MeV
    'K2MinIPChi2'      : 9.    ,# adimensional
    'K2PIDK'           : 0.    ,# adimensional

            # Muon from B -> K mu X
    'MuP'              : 3000. ,# MeV
    'MuPT'             : 1000. ,# MeV
    'MuMinIPChi2'      : 9.    ,# adimensional
    'MuPIDmu'          : 0.    ,# adimensional

            # J/psi from B -> K J/psi
    'JpsiMassWindow'   : 80.   ,# MeV

            # B -> K mu X
    'KMuVChi2Dof'      : 4.    ,# adimensional
    'KMuFdChi2'        : 100.  ,# adimensional
    'KMuMassMin'       : 1000. ,# MeV
    'KMuMassMax'       : 5500. ,# MeV

            # B -> K Jpsi
    'KJpsiVChi2Dof'    : 4.    ,# adimensional
    'KJpsiFdChi2'      : 25.   ,# adimensional
    'KJpsiMassMin'     : 5050. ,# MeV
    'KJpsiMassMax'     : 5500. ,# MeV

            # B_s2*0 -> B K
    'Bs2PT'            : 50.0   ,# MeV
    'DMKKMu'           : 713.677,# MeV (493.677 + 220)
    'DMKKJpsi'         : 1093.677,# MeV (493.677 + 600)
    'DZBPV'            : 1.0    ,# mm

    'RelatedInfoTools' : [
    {'Type' : 'RelInfoBs2MuMuTrackIsolations',
     'RecursionLevel' : 2,
     'Variables' : [
                     'BSMUMUTRACKPLUSISO',
                                      'BSMUMUTRACKPLUSISOTWO',
                                      'ISOTWOBODYMASSISOPLUS',
                                      'ISOTWOBODYCHI2ISOPLUS',
                                      'ISOTWOBODYISO5PLUS'
                                      ],
     'Locations' : {
    'Phys/StdLooseKaons' : 'KaonISO',
    'Phys/StdLooseMuons' : 'MuonISO'
                     },
     'tracktype'  : 3,
     'angle'      : 0.27,
     'fc'         : 0.60,
     'doca_iso'   : 0.13,
     'ips'        : 3.0,
     'svdis'      : -0.15,
     'svdis_h'    : 30.,
     'pvdis'      : 0.5,
     'pvdis_h'    : 40.,
     'makeTrackCuts' : False,
     'IsoTwoBody' : True
                  }
                ]
            }
        }

DarkBoson = {
    'BUILDERTYPE' : 'DarkBosonConf' ,
    'STREAMS' : {
    'Dimuon' : [
               'StrippingB2KpiX2MuMuDDDarkBosonLine',
               'StrippingB2KpiX2MuMuDDSSDarkBosonLine',
               'StrippingB2KKX2MuMuDDDarkBosonLine',
               'StrippingB2KKX2MuMuDDSSDarkBosonLine',
               'StrippingB2KX2MuMuDDDarkBosonLine',
               'StrippingB2KX2MuMuDDSSDarkBosonLine',
               'StrippingB2KpiX2PiPiDDDarkBosonLine',
               'StrippingB2KKX2PiPiDDDarkBosonLine',
               'StrippingB2KX2PiPiDDDarkBosonLine',
               'StrippingB2KX2PiPiDDSSDarkBosonLine',
               'StrippingB2RhoX2MuMuDDDarkBosonLine',
               'StrippingB2KpiX2KKDDDarkBosonLine',
               'StrippingB2KKX2KKDDDarkBosonLine',
               'StrippingB2KX2KKDDDarkBosonLine',
               'StrippingB2KX2KKDDSSDarkBosonLine',
               ],
    'Leptonic' : [
               'StrippingB2KpiX2MuMuDarkBosonLine',
               'StrippingB2KpiX2MuMuSSDarkBosonLine',
               'StrippingB2KKX2MuMuDarkBosonLine',
               'StrippingB2KKX2MuMuSSDarkBosonLine',
               'StrippingB2KX2MuMuDarkBosonLine',
               'StrippingB2KX2MuMuSSDarkBosonLine',
               'StrippingB2KpiX2PiPiDarkBosonLine',
               'StrippingB2KKX2PiPiDarkBosonLine',
               'StrippingB2KX2PiPiDarkBosonLine',
               'StrippingB2KX2PiPiSSDarkBosonLine',
               'StrippingB2RhoX2MuMuDarkBosonLine',
               'StrippingB2KpiX2KKDarkBosonLine',
               'StrippingB2KKX2KKDarkBosonLine',
               'StrippingB2KX2KKDarkBosonLine',
               'StrippingB2KX2KKSSDarkBosonLine',
               'StrippingB2KpiX2EEDarkBosonLine',
               'StrippingB2KpiX2EESSDarkBosonLine',
               'StrippingB2KKX2EEDarkBosonLine',
               'StrippingB2KKX2EESSDarkBosonLine',
               'StrippingB2KX2EEDarkBosonLine',
               'StrippingB2KX2EESSDarkBosonLine',
               'StrippingB2JKDarkBosonLine',
               'StrippingB2JKstDarkBosonLine',
               ]

    },
    'WGs'     : [ 'RD' ],
    'CONFIG'  : {
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
    'VCHI2DOF_MAX'  : 15,
    'BPVVDCHI2_MIN' : 25,
    'PT_MIN'        : '0*MeV'
        },
    "J" : { # cuts on J/psi
    'VCHI2DOF_MAX'  : 12,
    'ADAMASS_MAX'   : '100*MeV',
        },
    "B2KX" : { # B -> K X
    'SUMPT_MIN'     : '0*MeV',
    'VCHI2DOF_MAX'  : 15,
    'BPVIPCHI2_MAX' : 10,
    'BPVLTIME_MIN'  : '0.2*ps',
    'AM_MIN'        : '4800*MeV',
    'AM_MAX'        : '5800*MeV',
    'HAD_MINIPCHI2_MIN' : 25,
    'PT_MIN'        : '1000*MeV'
        },
    "B2HHX" : { # B -> Kpi X, KK X
    'SUMPT_MIN'     : '0*MeV',
    'VCHI2DOF_MAX'  : 15,
    'BPVIPCHI2_MAX' : 10,
    'BPVLTIME_MIN'  : '0.2*ps',
    'AM_MIN'        : '4800*MeV',
    'AM_MAX'        : '5800*MeV',
    'HAD_MINIPCHI2_MIN' : 9,
    'PT_MIN'        : '1000*MeV'
        },
    "Prescales" : { "SS" : 0.1, "DD" : 0.5, "KK": 0.25},
    "GECNTrkMax" : 250
        }
    }

Bu2MuNu = {
    'WGs'   : ['RD'],
    'BUILDERTYPE' : 'Bu2MuNuConf',
    'CONFIG': {'PrescaleSignal'  : 1.0,
               'PostscaleSignal' : 1.0,
               'PrescaleControl'  : 0.03,
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
    'STREAMS': [ 'Leptonic' ]
        }

B2XTau =  {
    'BUILDERTYPE'       :       'B2XTauConf',
    'WGs'    : [ 'RD' ],
    'CONFIG'    : {

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
    'MASS_LOW_Ds'                   : '1938', # MeV
    'MASS_HIGH_Ds'                  : '1998', # MeV
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
    'B2TauTau_SameSign_TOSLinePrescale'      : 1,
    'B2TauTau_SameSign_TOSLinePostscale'     : 1,
    'B2DD_SameSign_LinePrescale'             : 0.5,
    'B2DD_SameSign_LinePostscale'            : 1,
    'B2TauMu_SameSign_TOSLinePrescale'       : 0.5,
    'B2TauMu_SameSign_TOSLinePostscale'      : 1,
    'B2DPi_SameSign_LinePrescale'            : 0.5,
    'B2DPi_SameSign_LinePostscale'           : 1,
    'HLT_DECISIONS_HAD'    : {'Hlt2Topo(2|3|4)Body.*Decision%TOS' : 0},
    #  'HLT_DECISIONS_HAD'    : {'Hlt2(Topo2Body|Topo3Body|Topo4Body).*Decision%TOS' : 0},
    'HLT_DECISIONS_MUON'  : {"Hlt2(TopoMu|SingleMuon).*Decision%TOS": 0},
    'RelatedInfoTools'      : [
      #1
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
          },
      #8
    { "Type" : "RelInfoBstautauZVisoBDT"
      ,"RecursionLevel" : 0
      , "Variables" : ['ZVISOTAUP','ZVISOTAUM']
      , "Location"  : "ZVisoBDT"
          },

      ]
      },
    'STREAMS'     : ['Bhadron']
      }

ZVTOP = {
    'BUILDERTYPE': 'ZVTOP_Conf',
    'CONFIG'     :
     {
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
      #
    'RelatedInfoTools'      : [
      #1
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
          },
      #8
    { "Type" : "RelInfoBstautauZVisoBDT"
      ,"RecursionLevel" : 0
      , "Variables" : ['ZVISOTAUP','ZVISOTAUM']
      , "Location"  : "ZVisoBDT"
          }
      ]
    }

    ,
    'WGs'    : [ 'RD' ],
    'STREAMS'     : ['Bhadron']
    }

B2KstTauTau = {
    'BUILDERTYPE'       : 'B2KstTauXConf',
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
    'MASS_LOW_D'                    : '1840', # MeV
    'MASS_HIGH_D'                   : '1900', # MeV
    'MASS_LOW_Ds'                   : '1938', # MeV
    'MASS_HIGH_Ds'                  : '1998', # MeV
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
    'MINIPCHI2_K_KMM'     : '4',#'16'     ,
    'MINIPCHI2_PI_KMM'    : '4',#'16'     ,
    'DOCAMAX_KST_KMM'     : '0.15',#'0.15',
    'MINIPCHI2_KST_KMM'   : '3',#'9'      ,
    'FDCHI2OWNPV_KST_KMM' : '120' ,
    'MINIPCHI2_MU_KMM'    : '4',#'16'     ,
    'B_COSDIRA_KMM'       : '0.999',
    'VCHI2_B_Mu_KMM'                    : '100', # dimensionless
    'FD_B_Mu_KMM'                       : '3',
    'FD_Kst_Mu_KMM'                       : '3',
    'MASS_LOW_B_KMM'                    : '1500', # MeV
    'MASS_HIGH_B_KMM'                   : '10000', # MeV
    'PT_B_KMM'                        : '2000',
    'PT_MU_KMM'                         : '800', # MeV
    'PT_HAD_ALL_FINAL_STATE'        : '250',  # MeV
    'P_HAD_ALL_FINAL_STATE'         : '2000', # MeV
    'IPCHI2_HAD_ALL_FINAL_STATE'    : '16',    # dimensionless
    'TRACKCHI2_HAD_ALL_FINAL_STATE' : '3',    # dimensionless
    'TRGHOPROB_HAD_ALL_FINAL_STATE' : '0.3',    # dimensionless
      #
    'B2KstTauTau_LinePrescale'               : 1,
    'B2KstTauTau_LinePostscale'              : 1,
    'B2KstTauMu_LinePrescale'                : 1,
    'B2KstTauMu_LinePostscale'               : 1,
    'B2KstTauTau_SameSign_LinePrescale'      : 1,
    'B2KstTauTau_SameSign_LinePostscale'     : 1,
    'B2KstTauMu_SameSign_LinePrescale'       : 0.5,
    'B2KstTauMu_SameSign_LinePostscale'      : 1,
    'B2KstMuMu_LinePrescale'                : 1,
    'B2KstMuMu_LinePostscale'               : 1,
    'B2KstMuMu_SameSign_LinePrescale'         : 1,
    'B2KstMuMu_SameSign_LinePostscale'        : 1,
    'B2DDSL_LinePrescale'                    : 1,
    'B2DDSL_LinePostscale'                   : 1,
    'RelatedInfoTools'      : [
      #1
    { "Type" : "RelInfoBKsttautauMuonIsolationBDT"
      ,"RecursionLevel" : 0
      , "Variables" : [
           'BKSTTAUTAUMUONISOBDTFIRSTVALUETAUP', 'BKSTTAUTAUMUONISOBDTSECONDVALUETAUP','BKSTTAUTAUMUONISOBDTTHIRDVALUETAUP',
                  'BKSTTAUTAUMUONISOBDTFIRSTVALUETAUM', 'BKSTTAUTAUMUONISOBDTSECONDVALUETAUM','BKSTTAUTAUMUONISOBDTTHIRDVALUETAUM']
      , "Location"  : "B2KstTauTau_MuonIsolationBDT"
          },
    { "Type" : "RelInfoBKsttautauTauIsolationBDT"
      ,"RecursionLevel" : 0
      , "Variables" : [
         'BKSTTAUTAUTAUISOBDTFIRSTVALUETAUP', 'BKSTTAUTAUTAUISOBDTSECONDVALUETAUP','BKSTTAUTAUTAUISOBDTTHIRDVALUETAUP',
              'BKSTTAUTAUTAUISOBDTFIRSTVALUETAUM', 'BKSTTAUTAUTAUISOBDTSECONDVALUETAUM','BKSTTAUTAUTAUISOBDTTHIRDVALUETAUM',
              'BKSTTAUTAUTAUISOBDTFIRSTVALUEKST', 'BKSTTAUTAUTAUISOBDTSECONDVALUEKST','BKSTTAUTAUTAUISOBDTTHIRDVALUEKST']
      , "Location"  : "B2KstTauTau_TauIsolationBDT"
          },
    { "Type" : "RelInfoBKsttautauTrackIsolationBDT"
      ,"RecursionLevel" : 0
      , "Variables" : [
          'BKSTTAUTAUTRKISOBDTFIRSTVALUETAUPPIM','BKSTTAUTAUTRKISOBDTSECONDVALUETAUPPIM','BKSTTAUTAUTRKISOBDTTHIRDVALUETAUPPIM',
                'BKSTTAUTAUTRKISOBDTFIRSTVALUETAUPPIP1','BKSTTAUTAUTRKISOBDTSECONDVALUETAUPPIP1','BKSTTAUTAUTRKISOBDTTHIRDVALUETAUPPIP1',
                'BKSTTAUTAUTRKISOBDTFIRSTVALUETAUPPIP2','BKSTTAUTAUTRKISOBDTSECONDVALUETAUPPIP2','BKSTTAUTAUTRKISOBDTTHIRDVALUETAUPPIP2',
                'BKSTTAUTAUTRKISOBDTFIRSTVALUETAUMPIP','BKSTTAUTAUTRKISOBDTSECONDVALUETAUMPIP','BKSTTAUTAUTRKISOBDTTHIRDVALUETAUMPIP',
                'BKSTTAUTAUTRKISOBDTFIRSTVALUETAUMPIM1','BKSTTAUTAUTRKISOBDTSECONDVALUETAUMPIM1','BKSTTAUTAUTRKISOBDTTHIRDVALUETAUMPIM1',
                'BKSTTAUTAUTRKISOBDTFIRSTVALUETAUMPIM2','BKSTTAUTAUTRKISOBDTSECONDVALUETAUMPIM2','BKSTTAUTAUTRKISOBDTTHIRDVALUETAUMPIM2',
                'BKSTTAUTAUTRKISOBDTFIRSTVALUEKSTK','BKSTTAUTAUTRKISOBDTSECONDVALUEKSTK','BKSTTAUTAUTRKISOBDTTHIRDVALUEKSTK',
                'BKSTTAUTAUTRKISOBDTFIRSTVALUEKSTPI','BKSTTAUTAUTRKISOBDTSECONDVALUEKSTPI','BKSTTAUTAUTRKISOBDTTHIRDVALUEKSTPI']
      , "Location"  : "B2KstTauTau_TrackIsolationBDT"
          },
    { "Type" : "RelInfoBstautauCDFIso"
      ,"RecursionLevel" : 0
          #    , "Variables" : ['BKSTTAUTAUCDFISO']
      , "Location"  : "B2KstTauTau_CDFIso"
          }
      ]
      },
    'STREAMS'     : ['Bhadron']
      }

    
PhiToKSKS = {
    'NAME'              : 'PhiToKSKS',
    'WGs'               : ['RD'],
    'BUILDERTYPE'       : 'PhiToKSKSAllLinesConf',
    'CONFIG'    : {
    "prescale_PhiToKK" : 0.001,
    "prescale_PhiToKsKs" : 1.0,
    "prescale_PhiToMuMu" : 0.01,
    "prescale_JPsiToKK" : 0.002,
    "prescale_JPsiToKsKs" : 1.0,
    "prescale_D0ToKsKs" : 0.25,
    "prescale_EtaCToLL" : 1.0,

    "GEC_nLongTrk" : 250,
    "DoDTF" : True,
    "GHOSTPROB_MAX" : 0.35, #adimensional

    "K_PTMIN" : 200, #MeV
    "K_PIDK_MIN" : 7, #adimensional
    "K_IPCHI2_MAX" : 9, #adimensional

    "Mu_PTMIN" : 200, #MeV
    "Mu_PIDmu_MIN" : 0, #adimensional
    "Mu_IPCHI2_MAX" : 9, #adimensional

    "KS_LL_PTMIN" : 400, #MeV
    "KS_LL_MASS_WINDOW" : 20, #MeV
    "KS_LL_FD_MIN" : 10.0, #mm
    "KS_LL_FDCHI2_MIN" : 100, #adimensional
    "KS_LL_VCHI2NDOF_MAX" : 4,#adimensional
    "KS_LL_DIRA_MIN" : 0.9999, #adimensional

    "KS_DD_PTMIN" : 400, #MeV
    "KS_DD_MASS_WINDOW" : 20, #MeV
    "KS_DD_FD_MIN" : 10.0, #mm
    "KS_DD_FDCHI2_MIN" : 100, #adimensional
    "KS_DD_VCHI2NDOF_MAX" : 4,#adimensional
    "KS_DD_DIRA_MIN" : 0.999, #adimensional

    "Lambda_PTMIN" : 400, #MeV
    "Lambda_MASS_WINDOW" : 50, #MeV
    "Lambda_FD_MIN" : 10.0, #mm
    "Lambda_FDCHI2_MIN" : 100, #adimensional
    "Lambda_VCHI2NDOF_MAX" : 4,#adimensional
    "Lambda_DIRA_MIN" : 0.999, #adimensional

    "Phi_MASS_MAX" : 1100, # MeV
    "Phi_PT_MIN" : 800, # MeV
    "Phi_DOCACHI2_MAX" : 20, #adimensional
    "Phi_VCHI2NDOF_MAX" : 6, #adimensional
    "Phi_IPCHI2_MAX" : 9, #adimensional

    "D0_MASS_WIN" : 40, # MeV
    "D0_PT_MIN" : 800, # MeV
    "D0_DOCACHI2_MAX" : 20, #adimensional
    "D0_VCHI2NDOF_MAX" : 6, #adimensional
    "D0_IPCHI2_MAX" : 9, #adimensional

    "JPsi_MASS_WIN" : 120, # MeV
    "JPsi_PT_MIN" : 500, # MeV
    "JPsi_DOCACHI2_MAX" : 20, #adimensional
    "JPsi_VCHI2NDOF_MAX" : 6, #adimensional
    "JPsi_IPCHI2_MAX" : 9, #adimensional

    "etaC_MASS_WIN" : 200, # MeV
    "etaC_PT_MIN" : 400, # MeV
    "etaC_DOCACHI2_MAX" : 20, #adimensional
    "etaC_VCHI2NDOF_MAX" : 6, #adimensional
    "etaC_IPCHI2_MAX" : 9 #adimensional
    },
    'STREAMS' : ['Charm'] }

Bs2MuMuLines = {
    'NAME' : 'Bs2MuMuLines',
    'WGs'  : ['RD'],
    'BUILDERTYPE' : 'Bs2MuMuLinesConf',
    'CONFIG'   : {
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
    'daughter_IPChi2'            : 9,

    'TrackGhostProb'          : 0.45,

    'DOCA'                   : 0.3,
    'DOCA_loose'             :0.5,
    'DOCA_LTUB'              :0.3,

    'VCHI2_VDOF'             :9,
    'VCHI2_VDOF_LTUB'             :9,
    'VCHI2_VDOF_loose'             :25,

    'daughter_TrChi2'                  :3,
    'daughter_TrChi2_LTUB'                    :3,

    'B_Pt'                           :350,
    'B_Pt_LTUB'                      :500,

    'B_BPVIPChi2'                     :25,
    'B_BPVIPChi2_LTUB'                :25,

    'B_minimum_decaytime_LTUB'           :0.6,
    'muon_PT_LTUB'                          :40

            },
    'STREAMS' : {
    'Dimuon' : ['StrippingBs2MuMusNoMuIDLine',
                'StrippingBs2MuMusWideMassLine',
                'StrippingBs2MuMusBu2JPsiKLine'
                ],
    'Leptonic' : ['StrippingBs2MuMusWideMassLine',
                  'StrippingBs2MuMusBs2JPsiPhiLine',
                  'StrippingBs2MuMusBs2KKLTUBLine',
                  'StrippingBs2MuMusNoMuIDLine',
                  'StrippingBs2MuMusSSLine',
                  'StrippingBs2MuMusBd2JPsiKstLine',
                  'StrippingBs2MuMusLTUBLine',
                  'StrippingBs2MuMusBu2JPsiKLine'
                  ]
    }
    }
