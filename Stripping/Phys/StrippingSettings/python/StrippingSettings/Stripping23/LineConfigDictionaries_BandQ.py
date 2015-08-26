################################################################################
##                          S T R I P P I N G  2 3                            ##
##                                                                            ##
##  Configuration for B&Q WG                                                  ##
##  Contact person: Roberta.Cardinale@cern.ch                                   ##
################################################################################

from GaudiKernel.SystemOfUnits import *

#########################################################
### StrippingBc2Ds1Gamma
### -----------------------------------------------------
### Defined in:                 StrippingBc2Ds1Gamma.py
### Proponent:                  marco.pappagallo@cern.ch
### Motivation:                 Stripping lines for radiative decays
### Documentation:              
#######################################################

Bc2Ds1Gamma = {
    'WGs'    : [ 'BandQ' ]
    ,'BUILDERTYPE'       : 'Bc2Ds1GammaConf'
    ,'CONFIG'    : {
    #'TrIPchi2'            : 4.        # Dimensionless Already implemented in the particle lists
    #                   'BcMassWin'           : 1500.     # MeV
    'MinBcMass'           : 4800.     # MeV
    ,'MaxBcMass'           : 7500.     # MeV  
    ,'BcPVIPchi2'          : 16.       # Dimensionless
    ,'photonPT'            : 1500.     # MeV
    ,'KaonProbNN'          : 0.1       # Dimensionless
    ,'pionProbNN'          : 0.1       # Dimensionless
    ,'TrGhostProb'         : 0.5       # Dimensionless
    ,'Ds1DeltaMassWin'     : 650       # MeV
    ,'MaxDs1VertChi2DOF'   : 10        # Dimensionless
    ,'Bc_PT'               : 1000      # MeV
    ,'CTAU_Bc'             : 75        # microMeter
    # Pre- and postscales
    ,'Bc2Ds1GammaPreScale'               : 1.0
    ,'Bc2Ds1GammaPostScale'              : 1.0
    },
    'STREAMS' : [ 'Radiative' ]
    }


#########################################################
### StrippingBc2JpsiH with BDT
### -----------------------------------------------------
### Defined in:                 StrippingBc2JpsiHBDT.py
### Proponent:                  Jibo.He@cern.ch
### Motivation:                 Reduce retention on Bc FullDST
#########################################################


Bc2JpsiHBDT =  {
    'WGs'         : ['BandQ'],
    'BUILDERTYPE' : 'Bc2JpsiHBDTConf',
    'CONFIG'      : {
    'LinePrescale'        :    1.   ,
    'LinePostscale'       :    1.   ,    
    'MuonCuts'            : "(MINTREE('mu+'==ABSID,PT)>500*MeV) & (MAXTREE('mu+'==ABSID,TRCHI2DOF)<3) & (MINTREE('mu+'==ABSID,PIDmu)>0.)",
    'JpsiCuts'            : '((MM>3.0*GeV) & (MM<3.2*GeV) & (VFASPF(VCHI2PDOF)<16))',
    'PionCuts'            : '((TRCHI2DOF<3) & (TRGHOSTPROB<0.6) & (PT>1.0*GeV))',
    'BcComCuts'           : '(in_range(5.8*GeV, AM, 7.0*GeV))',
    'BcMomCuts'           : """(VFASPF(VCHI2/VDOF)<16) 
                             & (in_range(6.0*GeV, DTF_FUN(M,True,strings( ['J/psi(1S)'])), 6.75*GeV))
                             & (BPVIPCHI2()<25) 
                            """,
    'BDTCutValue'         :  0.6 ,
    'BDTWeightsFile'      : '$TMVAWEIGHTSROOT/data/Bc2JpsiH_BDTG_v1r0.xml'    
    },
    'STREAMS'   : [ 'Dimuon' ]
    }


#########################################################
### StrippingBc2JpsiMuLine
### -----------------------------------------------------
### Defined in:                 StrippingBc2JpsiMuXNew.py
### Proponent:                  Lucio.Anderlini@cern.ch, Jibo.He@cern.ch
### Motivation:                 Used for Bc lifetime measurement w/ SL channel
### Documentation:              n/a              
### Note on RAW event:          Muon Raw event was used in the analysis
###                             to study the effect of shared hits between
###                             same-sign muons in the final state
#########################################################


Bc2JpsiMu = {
    'BUILDERTYPE' : 'Bc2JpsiMuXConf',
    'CONFIG' : {
    'LinePrescale'        :    1.   ,
    'LinePostscale'       :    1.   ,
   
    'MuonTRCHI2DOF'       :    5.   ,  # adimentional
    'MuonPT'              : 1400.   ,  # MeV
    'MuonP'               :   -5.   ,  # MeV, not applied now
    'MuMuParticleName'    : "'J/psi(1S)'", # Particle Name, like "'psi(2S)'"
    'MuMuMassWindow'      :  150.   ,  # MeV, 10 sigma, may decreased to 150
    'MuMuVtxCHI2'         :    9.   ,  # adimentional
    'MuMuPT'              :  -10.   ,  # MeV, not applied
   
    'MuonBcTRCHI2DOF'     :    5.   ,  # adimentional    
    'MuonBcPT'            : 2500.   ,  # MeV
    'MuonBcP'             :   -5.   ,  # MeV, not applied now
    'BcUpperMass'         : 6400.   ,  # MeV, Upper limit for partial rec.
    'BcLowerMass'         : 3200.   ,  # MeV, Lower limit for partial rec.
    'BcVtxCHI2'           :    9.   ,  # adimentional
    'BcPT'                : 6000.      # MeV, May incrase up to 5000 MeV if needed      
    },
    'STREAMS' : [ 'Dimuon' ] ,
    'WGs'    : [ 'BandQ' ]
    }


#########################################################
### StrippingBc3pppiForBc3h
### StrippingBc3ppkForBc3h
### StrippingBc3piForBc3h
### StrippingBc3kForBc3h
### StrippingBc3kpiForBc3h
### -----------------------------------------------------
### Defined in:                 StrippingBc3h.py
### Proponent:                  Ivan.Belyaev@cern.ch
### Motivation:                 Bc -> J/psi 3h
### Documentation:              n/a              
#########################################################


Bc3h = {
    'BUILDERTYPE' : 'Bc3hConf',
    'CONFIG'      : {
    #
    ## PV-requiremens
    #
    'CheckPV'   : True ,
    #
    ## Global filter
    # 
    'FILTER'    : None ,   ## VOID filter 
    'ODIN'      : None ,   ## ODIN filter 
    'L0DU'      : None ,   ## L0   filter 
    'HLT1'       : None ,   ## HLT  filter
    'HLT2'       : None ,   ## HLT  filter
    #
    ## c*tau cut for B-hadrons 
    #
    'CTAU'      : 140 * micrometer , 
    'CTAU_BC'   :  80 * micrometer , 
    #
    ## pions and kaons
    # 
    'PionCut'   : """
    ( CLONEDIST   > 5000   ) & 
    ( TRGHOSTPROB < 0.4    ) &
    ( PT          > 500 * MeV               ) & 
    in_range ( 2          , ETA , 4.9       ) &
    in_range ( 3.2 * GeV  , P   , 150 * GeV ) &
    HASRICH                  &
    ( PROBNNpi     > 0.15  ) &
    ( MIPCHI2DV()  > 9.    )
    """ ,
    #
    'KaonCut'   : """
    ( CLONEDIST   > 5000   ) & 
    ( TRGHOSTPROB < 0.4    ) & 
    ( PT          > 500 * MeV               ) & 
    in_range ( 2          , ETA , 4.9       ) &
    in_range ( 3.2 * GeV  , P   , 150 * GeV ) &
    HASRICH                  &
    ( PROBNNk      > 0.20  ) &
    ( MIPCHI2DV()  > 9.    ) 
    """ ,
    #
    'ProtonCut'   : """
    ( CLONEDIST   > 5000    ) & 
    ( TRGHOSTPROB < 0.4     ) & 
    ( PT          > 500 * MeV              ) & 
    in_range (  2        , ETA , 4.9       ) &
    in_range ( 10 * GeV  , P   , 150 * GeV ) &
    HASRICH                   &
    ( PROBNNp      > 0.15   ) &
    ( MIPCHI2DV()  > 4.     ) 
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
    "ctau      = BPVLTIME ( 25 ) * c_light "                   ,
    "ctau_9    = BPVLTIME (  9 ) * c_light "                   ,
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
    'Bc3piPrescale'    : 1.0 ,
    'Bc3kPrescale'     : 1.0 ,
    'Bc3kpiPrescale'   : 1.0 ,
    'Bc3kpiPrescale'   : 1.0 ,
    'Bc3pppiPrescale'  : 1.0 ,
    'Bc3ppkPrescale'   : 1.0 ,
    # =========================================================================
    },
    'WGs' : [ 'BandQ' ],
    'STREAMS' : { 'Bhadron' : [ 'StrippingBc3piForBc3h'   ,
                                'StrippingBc3kForBc3h'    ,
                                'StrippingBc3kpiForBc3h'  , 
                                'StrippingBc3pppiForBc3h' , 
                                'StrippingBc3ppkForBc3h'  ] } 
    }


#########################################################
### StrippingCcbar2PhiPhiLine
### StrippingCcbar2PhiPhiDetachedLine
### StrippingCcbar2PhiKKDetachedLine
### StrippingCcbar2PhiBs2JpsiPhiLine
### StrippingCcbar2PhiB2JpsiKLine
### StrippingCcbar2PhiPiPiDetachedLine
### StrippingCcbar2PhiBs2TriPhiLine
### -----------------------------------------------------
### Defined in:                 StrippingCcbar2PhiPhi.py
### Proponent:                  sergey.barsuk@cern.ch, Jibo.He@cern.ch
### Motivation:                 
###   Module for selecting Ccbar->PhiPhi, including two lines:
###   1. Prompt line, with tight PT, PID cuts, requiring Hlt Tis, since there 
###      is no lifetime unbiased phi trigger yet.
###   2. Detached line, with loose PT, PID cuts, but with IPS cuts on Kaons. 
### Documentation:              n/a
#########################################################


Ccbar2Phi = {
    'BUILDERTYPE' : 'Ccbar2PhiPhiConf',
    'CONFIG' : {
        'TRCHI2DOF'        :     5.  ,
        'KaonPIDK'         :     5.  ,
        'KaonPT'           :   650.  , # MeV
        'PhiVtxChi2'       :    16.  ,
        'PhiMassW'         :    12.  , 
        'CombMaxMass'      :  4100.  , # MeV, before Vtx fit
        'CombMinMass'      :  2750.  , # MeV, before Vtx fit
        'MaxMass'          :  4000.  , # MeV, after Vtx fit
        'MinMass'          :  2800.  , # MeV, after Vtx fit
        'Phi_TisTosSpecs'  : { "Hlt1Global%TIS" : 0 },
        'PionCuts'         :  "(PT>0.7*GeV) & (TRCHI2DOF<5) & (MIPCHI2DV(PRIMARY)>36.) & (PIDK<10)" ,
        'KaonCuts'         :  "(PT>0.5*GeV) & (TRCHI2DOF<5) & (MIPCHI2DV(PRIMARY)>25.) & (PIDK>5)",
        'LooseKaonCuts'    :  "(PT>0.5*GeV) & (TRCHI2DOF<5) & (MIPCHI2DV(PRIMARY)>9.)"
        },
    'STREAMS' : [ 'Charm' ] ,
    'WGs'     : [ 'BandQ' ]
    }


#########################################################
### StrippingCcbar2PpbarLine
### -----------------------------------------------------
### Defined in:                 StrippingCcbar2PpbarNew.py
### Proponent:                  sergey.barsuk@cern.ch, Jibo.He@cern.ch
### Motivation:                 Module for Charmonium->p pbar: Normal line
### Documentation:              n/a
#########################################################



Ccbar2Ppbar = {
    'BUILDERTYPE'       : 'Ccbar2PpbarConf',
    'CONFIG'    : { 'LinePrescale'     :    1.   ,
                    'LinePostscale'    :    1.   ,
                    
                    'SpdMult'          :   300.  , # dimensionless, Spd Multiplicy cut 
                    'ProtonPT'         :  1950.  , # MeV
                    'ProtonP'          :    10.  , # GeV
                    'ProtonTRCHI2DOF'  :     4.  ,
                    'ProtonPIDppi'     :    20.  , # CombDLL(p-pi)
                    'ProtonPIDpK'      :    15.  , # CombDLL(p-K)
                    'ProtonIPCHI2Cut'  :    ""   ,
                    'CombMaxMass'      :  4100.  , # MeV, before Vtx fit
                    'CombMinMass'      :  2750.  , # MeV, before Vtx fit
                    'MaxMass'          :  4000.  , # MeV, after Vtx fit
                    'MinMass'          :  2800.  , # MeV, after Vtx fit
                    'VtxCHI2'          :     9.  , # dimensionless
                    'CCCut'            :  " & (PT>6*GeV)"    
                   },
    'STREAMS' : [ 'Charm' ] ,
    'WGs'    : [ 'BandQ' ]
}

#########################################################
### StrippingCcbar2PpbarExclusiveLine
### -----------------------------------------------------
### Defined in:                 StrippingCcbar2PpbarNew.py
### Proponent:                  sergey.barsuk@cern.ch, Jibo.He@cern.ch
### Motivation:                 Module for Charmonium->p pbar: Exclusive
### Documentation:              n/a
#########################################################

Ccbar2PpbarExclusive = {
    'BUILDERTYPE'       : 'Ccbar2PpbarConf',
    'CONFIG'    : {
        'LinePrescale'     :     1.  ,
        'LinePostscale'    :     1.  ,
        
        'SpdMult'          :    20.  , # dimensionless, Spd Multiplicy cut 
        'ProtonPT'         :   550.  , # MeV
        'ProtonP'          :    -2.  , # MeV
        'ProtonTRCHI2DOF'  :     5.  ,
        'ProtonPIDppi'     :    20.  , # CombDLL(p-pi)
        'ProtonPIDpK'      :    15.  , # CombDLL(p-K)
        'ProtonIPCHI2Cut'  :    ""   ,
        'CombMaxMass'      :  1.0e+6 , # MeV, before Vtx fit
        'CombMinMass'      :     0.  , # MeV, before Vtx fit
        'MaxMass'          :  1.0e+6 , # MeV, after Vtx fit
        'MinMass'          :     0.  , # MeV, after Vtx fit
        'VtxCHI2'          :     9.  , # dimensionless
        'CCCut'            :  ""     
        },
    'STREAMS' : [ 'Charm' ] ,
    'WGs'    : [ 'BandQ' ]
    }


#########################################################
### StrippingCcbar2PpbarDetachedLine
### -----------------------------------------------------
### Defined in:                 StrippingCcbar2PpbarNew.py
### Proponent:                  sergey.barsuk@cern.ch, Jibo.He@cern.ch
### Motivation:                 Module for Charmonium->p pbar: Detached line
### Documentation:              n/a
#########################################################

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
    'WGs'    : [ 'BandQ' ]
    }

#########################################################
### StrippingDiMuonAndGammaForCharmAssociative
### StrippingDoubleDiMuonForCharmAssociative
### StrippingChiAndDiMuonForCharmAssociative
### StrippingDiChiForCharmAssociative
### StrippingDiMuonAndWForCharmAssociative
### StrippingChiAndWForCharmAssociative
### -----------------------------------------------------
### Defined in:                 StrippingCharmAssociative.py
### Proponent:                  Ivan.Belyaev@cern.ch
### Motivation:                 stripping of associative ``onium'' production
### Documentation:              
###     - dimuon + dimuon 
###     - dimuon + high-pt gamma
### 
###    Parasitic:
### 
###     - dimuon + ( dimuon + gamma ) [ mimic   Chi_(b,c) + dimuon  ] 
###     -       2x ( dimuon + gamma ) [ mimic 2xChi_(b,c)           ] 
### 
###    Accociative W+ production:
###     - dimuon             & W+  
###     - ( dimuon + gamma ) & W+      [ mimic Chi_(b,c)] 
###
#########################################################

CharmAssociative = {
    'BUILDERTYPE'  :'StrippingCharmAssociativeConf',
    'CONFIG'  : {
    'PhotonCuts'      : ' PT > 3.0 * GeV  '                                 , 
    'MuonCuts'        : ' ISMUON & ( PT > 650 * MeV ) & ( TRCHI2DOF < 5 ) ' ,
    #
    ## photons from chi_(c,b)
    #
    'GammaChi'        : ' ( PT > 400 * MeV ) & ( CL > 0.05 ) ' , 
    #
    ## W+- selection
    #
    'WCuts'           : " ( 'mu+'== ABSID ) & ( PT > 15 * GeV )" ,
    #
    ## Global Event cuts 
    #
    'CheckPV'         : True , 
    #
    ## Technicalities:
    #
    'Preambulo'       : [
    #
    ## shortcut for chi2 of vertex fit 
    'chi2vx = VFASPF(VCHI2) '                    ,
    #
    ## shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light" , 
    #
    ## dimuons:
    "psi             = ADAMASS ( 'J/psi(1S)'  ) < 125 * MeV"         ,
    "psi_prime       = ADAMASS (   'psi(2S)'  ) < 125 * MeV"         ,
    "mu2_tight       = ( chi2vx < 10    ) & ( MINTREE ( 'mu+' == ABSID , PT ) > 900 * MeV ) " ,
    "dimu_tight      = ( PT > 3.0 * GeV ) & mu2_tight " ,
    "psi_tight       = ( ADMASS ( 'J/psi(1S)' ) < 110 * MeV ) & dimu_tight " ,
    "psi_prime_tight = ( ADMASS (   'psi(2S)' ) < 110 * MeV ) & dimu_tight " ,
    "dimuon_heavy    = ( M > 4.9 * GeV ) & dimu_tight "                      ,
    "dimuon_tight    = psi_tight | psi_prime_tight | dimuon_heavy " ,
    ] ,
    ## monitoring ?
    'Monitor'     : False ,
    #
    ## pescales 
    'DiMuonAndGammaPrescale' : 1.0 ,
    'DoubleDiMuonPrescale'   : 1.0 ,
    'ChiAndDiMuonPrescale'   : 1.0 ,
    'DiChiPrescale'          : 1.0 ,
    'DiMuonAndWPrescale'     : 1.0 , 
    'ChiAndWPrescale'        : 1.0
    },
    'STREAMS' : [ 'Leptonic' ] ,
    'WGs'     : [ 'BandQ'    ]
    }


###############################################################################
### StrippingChiCJPsiGammaConvChicLine
### StrippingChiCJPsiGammaConvChibLine
### -----------------------------------------------------
###   DISABLES: StrippingChiCJPsiGammaConvChicSymLine (obsolete)
###   DISABLES: StrippingChiCJPsiGammaConvChibSymLine (obsolete)
### -----------------------------------------------------
### Defined in:                 StrippingChiCJPsiGammaConv.py
### Proponent:                  
### Motivation:                 Stripping line for HeavyBaryons studies
### Documentation:              n/a              
###############################################################################

ChiCJPsiGammaConv = {
    'WGs'               : ['BandQ'],
    'BUILDERTYPE'       : 'StrippingChiCJPsiGammaConvConf',
    'CONFIG'            : {'trackChi2'               :    3.0
                           , 'MuPTMin'               :    400 #MeV #Can be tightened to 600 MeV
                           , 'MuPMin'               :    8000 #MeV
                           , 'JPsiMassMin'           :    3.0 # GeV
                           , 'JPsiMassMax'           :    3.2 # GeV
                           , 'JPsiPTMin'             :     2 # GeV
                           , 'JPsi_PIDmu'            :    0. 
                           , 'JPsiVertexChi2'        :   25.0
                           , 'UpsilonMassMin'        :    9.0  #GeV
                           , 'UpsilonMassMax'        :    12.9 #GeV
                           , 'UpsilonVertexChi2'     :   25.0
                           , 'UpsilonPTMin'          :    0.9 # GeV
                           , 'Upsilon_PIDmu'         :    0. 
                           , 'eDLLe'                 :   0.0
                           , 'GammaEEMass'           :   100.0 #MeV
                           , 'GammaEEPt'             :   580.0 #MeV
                           , 'mMinChiCRaw'           :   2.9 #GeV
                           , 'mMaxChiCRaw'           :   4.8 #GeV
                           , 'mMinChiCFit'           :   3.0 #GeV
                           , 'mMaxChiCFit'           :   4.7 ##GeV
                           , 'mMinChiBRaw'           :   9.4 #GeV
                           , 'mMaxChiBRaw'           :   13.2 #GeV
                           , 'mMinChiBFit'           :   9.5 #GeV
                           , 'mMaxChiBFit'           :   13.0  #GeV
                           , 'PrescaleChi'          :   1.0
                           , 'PostscaleChi'         :   1.0
                           },
    'STREAMS': {'Dimuon':
                ['StrippingChiCJPsiGammaConvChibLine',
                 'StrippingChiCJPsiGammaConvChicLine']
                }
    }


#########################################################
### StrippingHeavyBaryonXib2JpsiXi
### StrippingHeavyBaryonXibzero2JpsiXistar
### StrippingHeavyBaryonOmegab2JpsiOmega
### -----------------------------------------------------
### Defined in:                 StrippingHeavyBaryons.py
### Proponent:                  raphael.marki@epfl.ch, Yasmine.Amhis@cern.ch
### Motivation:                 Stripping line for HeavyBaryons studies
### Documentation:              n/a              
#########################################################
HeavyBaryon = {
    'BUILDERTYPE'  : 'HeavyBaryonsConf',
    'CONFIG'       : {    'TRCHI2DOF'             :       4.,
                          'PionPIDK'              :       5.,
                          'JpsiMassWindow'        :      80.,
                          'KaonPIDK'              :      -5.,
                          'DLSForLongLived'       :       5.,
                          'XiMassWindow'          :      30.,
                          'OmegaMassWindow'       :      30.,
                          "XibminusMassWindow"    :     300.,
                          "XibzeroMassWindow"     :     500.,
                          "OmegabminusMassWindow" :     500.
                          },
    'STREAMS' : [ 'Dimuon' ],
    'WGs'    : ['BandQ']
    }


####################################################
### StrippingXicHHHXic2PKPiLine
### StrippingXicHHHXic2PKKLine
### StrippingXicHHHTheta2PKS0Line
###-------------------------------------------------
### Defined in: StrippingXic2HHH.py
### Proponent:  Yury.Shcheglov@cern.ch
### Motivation: Xic studies and pentaquark searches
##################################################

XicHHH = {
    'WGs'         : ['BandQ'],
    'BUILDERTYPE' : 'StrippingXic2HHHConf',
    'CONFIG'      : {'Daug_All_PT_MIN'         : 300.0 * MeV
                     , 'Daug_P_MIN'              : 3000.0 * MeV
                     , 'Daug_TRCHI2DOF_MAX'      : 4.0
                     , 'K_IPCHI2_MIN'            : 9.0 
                     , 'pion_IPCHI2_MIN'         : 9.0
                     , 'Comb_MASS_MIN'           : 2250.0 * MeV 
                     , 'Comb_MASS_MAX'           : 2800.0 * MeV 
                     , 'Comb_ADOCAMAX_MAX'       : 0.3 * mm   
                     , 'Xic_PT_MIN'              : 2000.0 * MeV
                     , 'Xic_VCHI2VDOF_MAX'       : 8.0
                     , 'Xic_BPVVDCHI2_MIN'       : 0.0
                     , 'Xic_BPVDIRA_MIN'         : 0.999
                     , 'Xic_BPVIPCHI2_MAX'       : 12.
                     , 'Xic_BPVLTIME_MAX'        : 0.005 * ns
                     , 'Xic_BPVLTIME_MIN'        : -0.005 * ns
                     , 'PrescaleXic2PKPi'        : 0.0
                     , 'PostscaleXic2PKPi'       : 0.0
                     , 'PrescaleXic2PKK'         : 1.0
                     , 'PostscaleXic2PKK'        : 1.0
                     , 'PrescaleXic2PV0'         : 1.0
                     , 'PostscaleXic2PV0'        : 1.0
                     , 'PrescaleXic2KLam'        : 0.0 
                     , 'PostscaleXic2KLam'       : 0.0 
                     , 'RelatedInfoTools' : [
    { "Type" : "RelInfoConeVariables", "ConeAngle" : 1.5, "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], "Location": "Cone1"},
    { "Type" : "RelInfoConeVariables", "ConeAngle" : 0.75,  "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], "Location": "Cone2"},
    { "Type" : "RelInfoConeVariables", "ConeAngle" : 0.5, "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], "Location": "Cone3"},
    { "Type" : "RelInfoConeVariables", "ConeAngle" : 0.25,  "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], "Location": "Cone4"},
    { "Type" : "RelInfoConeVariables", "ConeAngle" : 0.1,  "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], "Location": "Cone5"}
    ] 
                     },
    'STREAMS' : [
    'Charm']
    }

    


#########################################################
### StrippingCC2DD
### -----------------------------------------------------
### Defined in:                 StrippingCC2DD.py
### Proponent:                  Andrea.Bizzeti@fi.infn.it
### Motivation:                 Low PT 2x charm, psi(3770)
### Documentation:              https://indico.cern.ch/conferenceDisplay.py?confId=269979
#########################################################

CC2DD = {
    'WGs'  : ['BandQ'],
    'BUILDERTYPE'       : 'CC2DDConf',
    'CONFIG'   :  {
    ######## D0 / D~0 -> K pi cuts
    'D0MassWin'     : "60*MeV",
    'D0PT'          : "0*MeV",    ### no cut
    'D0VtxChi2Ndof' : 10,
    'D0Bpvdira'     : -10.,
    'D0Bpvdls'      : 4.,
    'D0daughterBpvIpChi2'    : 7.,
    'D0daughterPT'           : "600*MeV",
    'D0daughterP'            : "5*GeV",
    'D0daughterTrkChi2'      : 3,
    'D0daughterTrkGhostProb' : 0.3,
    ### ProbNN conditions
    'D0daughterKaonProbNNk'  : 0.1,
    'D0daughterPionProbNNpi' : 0.1,
    ######## Dplus/Dminus -> Kpipi cuts, used also for D_s+/D_s- ->KKpi
    'DpmMassWin'     : "60*MeV",
    'DpmPT'          : "0*MeV",    ### no cut
    'DpmVtxChi2Ndof' : 10,
    'DpmBpvdira'     : -10.,
    'DpmBpvdls'      : 4.,
    'DpmdaughterBpvIpChi2'    : 7.,
    'DpmdaughterPT'           : "500*MeV",
                   'DpmdaughterP'            : "5*GeV",
    'DpmdaughterTrkChi2'      : 3,
    'DpmdaughterTrkGhostProb' : 0.3,
    ### ProbNN conditions
    'DpmdaughterKaonProbNNk'  : 0.1,
    'DpmdaughterPionProbNNpi' : 0.1,
    ######## psi(3779) -> D D  cuts
    #                   'CCMassCut'     : "(AM<5000*MeV)",
    # no mass constraint
    'CCMassCut'      : "(AM>0)",
    'CCVtxChi2Ndof'  : 10,
    'CCMaxD0ChildPT' : "0*MeV",    ### no cut
    'CCMaxD0TreePT'  : "0*MeV",    ### no cut
    'CCMaxD0MinTreeIpChi2'   : "0.",    ## unused for the moment
    },
    'STREAMS' : [
    'CharmCompleteEvent' 
    ],
    }

CC2DDcontrol = {
    'WGs'  : ['BandQ'],
    'BUILDERTYPE'       : 'CC2DDConf',
    'CONFIG'   :  {
######## D0 / D~0 -> K pi cuts
    'D0MassWin'     : "60*MeV",
    'D0PT'          : "0*MeV",    ### no cut
    'D0VtxChi2Ndof' : 10,
                   'D0Bpvdira'     : -10.,
    'D0Bpvdls'      : 3.,
    'D0daughterBpvIpChi2'    : 3.,
    'D0daughterPT'           : "600*MeV",
                   'D0daughterP'            : "5*GeV",
    'D0daughterTrkChi2'      : 3,
    'D0daughterTrkGhostProb' : 0.3,
    ### ProbNN conditions
    'D0daughterKaonProbNNk'  : 0.0,
    'D0daughterPionProbNNpi' : 0.0,
    ######## Dplus/Dminus -> Kpipi cuts, used also for D_s+/D_s- ->KKpi
    'DpmMassWin'     : "60*MeV",
                   'DpmPT'          : "0*MeV",    ### no cut
                   'DpmVtxChi2Ndof' : 10,
                   'DpmBpvdira'     : -10.,
                   'DpmBpvdls'      : 3.,
                   'DpmdaughterBpvIpChi2'    : 3.,
                   'DpmdaughterPT'           : "500*MeV",
                   'DpmdaughterP'            : "5*GeV",
                   'DpmdaughterTrkChi2'      : 3,
                   'DpmdaughterTrkGhostProb' : 0.3,
### ProbNN conditions
                   'DpmdaughterKaonProbNNk'  : 0.0,
                   'DpmdaughterPionProbNNpi' : 0.0,
######## psi(3779) -> D D  cuts
#                   'CCMassCut'     : "(AM<5000*MeV)",
# no mass constraint
                   'CCMassCut'      : "(AM>0)",
                   'CCVtxChi2Ndof'  : 10,
                   'CCMaxD0ChildPT' : "0*MeV",    ### no cut
                   'CCMaxD0TreePT'  : "0*MeV",    ### no cut
                   'CCMaxD0MinTreeIpChi2'   : "0.",    ## unused for the moment
                   },
    'STREAMS' : [
    'Charm'
    ],
    }   




#########################################################
### StrippingXibcXibc2LcJpsi
### StrippingXibcX2JpsiProton
### -----------------------------------------------------
### Defined in:                 StrippingXibc.py
### Proponent:                  Lucio.Anderlini@cern.ch
### Motivation:                 Inclusive lines for Xibc searches
### Documentation:              
###   https://indico.cern.ch/event/257864/session/1/contribution/32/material/slides/0.pdf
#########################################################


Xibc={
    'WGs'                                   : ['BandQ'],
    'BUILDERTYPE'                           : 'XibcBuilder',
    'STREAMS'                               : {'Dimuon' : ['StrippingXibcXibc2LcJpsi'
                                                           ]
                                               },
    'CONFIG'                                : {
    'Pions4Lambdac_MINIPCHI2'             : 0.0
    , 'Pion4Lambdac_TRPCHI2'              : 0.015
    , 'Pion4Lambdac_ProbNNpi'             : 0.2
    , 'Pion4Lambdac_PT'                   : '250*MeV'
    , 'Protons4Lambdac_TRPCHI2'           : 0.05
    , 'Protons4Lambdac_minP'              : '0*GeV'
    , 'Kaon4Lambdac_TRPCHI2'              : 0.05
    , 'Protons4Lambdac_ProbNNp'           : 0.05
    , 'Protons4Lambdac_PT'                : '450*MeV'
    , 'Kaon4Lambdac_PT'                   : '450*MeV'
    , 'Kaon4Lambdac_ProbNNk'              : 0.02
    , 'Kaon4Lambdac_minP'                 : '0*GeV'
    , 'Muon4Jpsi_PIDmu'                   : 0
    , 'Muon4Jpsi_TRPCHI2'                 : 0.005
    , 'Lambdac_MassWindowLarge'           : '120*MeV'
    , 'Lambdac_MassWindowTight'           : '30*MeV'		
    , 'Lambdac_MinAPT'                    : '1500*MeV' 
    , 'Lambdac_MaxADOCA'                  : '0.5*mm'
    , 'Lambdac_BPVDIRA'                   : 0.98
    , 'Lambdac_minTAU'                    : -0.0001 	
    , 'Lambdac_ENDVERTEXCHI2'             : 5						
    , 'Jpsi_MassWindowLarge'              : '150*MeV'
    , 'Jpsi_MassWindowTight'              : '50*MeV'
    , 'Muon4Jpsi_PT'                      : '650*MeV'
    , 'Jpsi_ENDVERTEXCHI2'                : 10
    , 'Xibc_ENDVERTEXCHI2'                : 7
    , 'Xibc_MassWindow'                   : '1.5*GeV'
    , 'Xibc_MINPVIP'                      : 1e3			#DISABLED FOR INCLUSIVITY
    , 'Xibc2LcJpsiPrescale'               : 1.0
    ####### #
    , 'Protons4Lambda0_ProbNNp'           : 0.02
    , 'LongProtons4Lambda0_MINIPCHI2'     : 2
    , 'Protons4Lambda0_PT'                : '600*MeV'
    , 'Pions4Lambda0_ProbNNpi'            : 0.2
    , 'Pions4Lambda0_PT'                  : '0*MeV'
    , 'Pions4Lambda0_MINIPCHI2'           : 0.
    , 'Lambda0_MassWindowLarge'           : '180*MeV'
    , 'Lambda0_MassWindowTight'           : '30*MeV'
    , 'Lambda0_APT'                       : '700*MeV'
    , 'Lambda0_ENDVERTEXCHI2'             : 10	
    , 'Lambda0_DownProtonTrackPvalue'     : 0
    , 'Lambda0_DownPionTrackPvalue'       : 0
    , 'Lambda0_minFD'                     : '1*mm/GeV'
    , 'Pions4Ximinus_PT'                  : '0*MeV'
    , 'Pions4Ximinus_ProbNNpi'            : 0.2
    , 'Pions4Ximinus_TRPCHI2'             : 0
    , 'Pions4Ximinus_MINIPCHI2'           : 0
    , 'Ximinus_MassWindowLarge'           : '120*MeV'
    , 'Ximinus_MassWindowTight'           : '40*MeV'
    , 'Ximinus_APT'                       : '800*MeV'
    , 'Ximinus_FlightDistance'            : '0.1*mm/GeV'
    , 'Ximinus_ENDVERTEXCHI2'             : 20
    , 'Xic0_MassWindowLarge'              : '600*MeV'
    , 'Xic0_MassWindowTight'              : '100*MeV'
    , 'Pions4Xic0_PT'                     : '200*MeV'
    , 'Pions4Xic0_ProbNNpi'               : 0.2
    , 'Pions4Xic0_TRPCHI2'                : 0
    , 'Xic0_APT'                          : '0*MeV'
    , 'Xic0_ENDVERTEXCHI2'                : 20
    , 'Xibc0_MassWindow'                  : '1.5*GeV'
    , 'Xibc0_ENDVERTEXCHI2'               : 20
    ####### 
    , 'HighMassBaryon_MassLowEdge'        : '4.5*GeV'
    , 'HighMassBaryon_MinAPT'             : '1*GeV'
    , 'ProtonsForHighMassBaryon_TRPCHI2'  : 0.1
    , 'ProtonsForHighMassBaryon_PT'       : '1.5*GeV'
    , 'ProtonsForHighMassBaryon_P'        : '5*GeV'
    , 'ProtonsForHighMassBaryon_ProbNNp'  : 0.1
    , 'JpsiForHighMassBaryon_PT'          : '1.5*GeV'
    , 'JpsiForHighMassBaryon_MassWin'     : '40*MeV'
    , 'JpsiForHighMassBaryon_MuonPIDmu'   : 0
    , 'JpsiProtonForHighMassBaryonCosth'  : 1 #disabled
    #######
    , "JpsiKp_CtrlLine_Prescale"          : 1.0
    , "JpsiKp_MinTAU"                     : '0.10*ps'
    , "JpsiKp_MassMin"                    : 5200 #MeV/c2
    , "JpsiKp_MassLbThreshold"            : 5750 #MeV/c2
    , "JpsiKp_MassMax"                    : 8000 #MeV/c2
    , "JpsiKp_MaxVertexChi2"              : 10
    , "JpsiKp_Jpsi_MinPT"                 : 700 #MeV/c
    , "JpsiKp_Jpsi_MassWin"               : 40  #MeV/cc
    , "JpsiKp_mu_MaxTrackGhostProb"       : 0.4
    , "JpsiKp_mu_MinPIDmu"                : 0
    , "JpsiKp_p_MinPt"                    : 300 #MeV
    , "JpsiKp_p_MinProbNNp"               : 0.05
    , "JpsiKp_p_MaxTrackGhostProb"        : 0.4
    , "JpsiKp_p_MinTrackPvalue"           : 0.1
    , "JpsiKp_p_MinP"                     : 2000 #MeV/c
    , "JpsiKp_K_MinPT"                    : 300 #MeV
    , "JpsiKp_K_MinProbNNk"               : 0.02
    , "JpsiKp_K_MaxTrackGhostProb"        : 0.4
    , "JpsiKp_K_MinTrackPvalue"           : 0.1
    , "JpsiKp_K_MinP"                     : 2000 #MeV/c
    ########
    , 'GlobalGhostProb_Max'               : 0.4
    , 'LongTrackGEC'                      : 150 
    }
    }


DiMuonInherit={
    'WGs'        : ['BandQ'],
    'STREAMS'    : {'Dimuon' :[
    "StrippingFullDSTDiMuonPsi2MuMuTOSLine",
    "StrippingFullDSTDiMuonJpsi2MuMuTOSLine",
    "StrippingFullDSTDiMuonPsi2MuMuDetachedLine",
    "StrippingFullDSTDiMuonJpsi2MuMuDetachedLine",
    "StrippingFullDSTDiMuonDiMuonNoPVLine",
    'StrippingFullDSTDiMuonDiMuonHighMassLine',
    'StrippingFullDSTDiMuonDiMuonHighMassSameSignLine'
    ],
                    'Leptonic' :[
    "StrippingMicroDSTDiMuonDiMuonSameSignLine",
    "StrippingMicroDSTDiMuonDiMuonIncLine"
    ]
                    },
    
    'BUILDERTYPE': 'DiMuonInherit',
    'CONFIG'     : {
    #### Definition of all the DiMuon lines ############
    ###############
    "Debug" : False,
    "Lines" : {
    "VirtualBase" : {
    "Prescale"      : 0.0,
    "Inherit"       : None,
    "checkPV"       : False,
    "maxPV"         : None,
    "TOScut"        : None,
    "RequiredRawEvents" : None,
    "InputDiMuon"   : "StdLooseDiMuon",
    "Cuts"          : {
    "MuonPt"        : "MINTREE('mu+'==ABSID,PT) > 650.0 *MeV",
    "JpsiVtx"       : "VFASPF(VCHI2PDOF)< 20.0"
    }
    },
    
    "FullDSTDiMuonDiMuonHighMassLine" : {
    "Prescale"      : 1.0,
    "Inherit"       : "VirtualBase",
    "Cuts"          : {
    "MuonP"         : "MINTREE('mu+'==ABSID,P ) > 8000.0 *MeV",
    "MuonPt"        : "MINTREE('mu+'==ABSID,PT) > 1000.0 *MeV",
    "Mass"          : "MM > 8500 *MeV",
    "MuonPIDmu"     : "MINTREE('mu+'==ABSID,PIDmu) > 0.0"
    }
    },
    
    "FullDSTDiMuonDiMuonHighMassSameSignLine" : {
    "Prescale"      : 0.2,
    "Inherit"       : "VirtualBase",
    #"Inherit"       : "FullDSTDiMuonDiMuonHighMassLine",
    "InputDiMuon"   : "StdLooseDiMuonSameSign",
    #"MuonPIDmu"     : None #remove the cut 
    #      "Cuts"          : None
    "Cuts"          : {
    "MuonP"         : "MINTREE('mu+'==ABSID,P ) > 8000.0 *MeV",
    "MuonPt"        : "MINTREE('mu+'==ABSID,PT) > 1000.0 *MeV",
    "Mass"          : "MM > 8500 *MeV",
    #"MuonPIDmu"     : "MINTREE('mu+'==ABSID,PIDmu) > 0.0"
    }
    },
    
    "FullDSTDiMuonDiMuonNoPVLine" : {
    "Prescale"      : 1.0,
    "Inherit"       : "VirtualBase",
    "maxPV"         : 0.5,
    "Cuts"          : {
    "BPVVDZ"        : "BPVVDZ < -1 *mm",
    #"MuonPIDmu"     : None, #remove the cut 
    "Mass"          : "MM > 2900",
    #"MuonPIDmu"     : "MINTREE('mu+'==ABSID,PIDmu) > 0.0"
    }
    },
      
    "FullDSTDiMuonJpsi2MuMuDetachedLine" : {
    "Prescale"      : 1.0,
    "Inherit"       : "VirtualBase",
    "checkPV"       : True,
    "RequiredRawEvents" : ["Trigger", "Muon", "Calo", "Rich", "Velo", "Tracker"],
    "InputDiMuon"   : "StdLooseJpsi2MuMu",
    "Cuts"          : {
    "MuonPt"        : "MINTREE('mu+'==ABSID,PT) > 500.0 *MeV",# replace!
    "Mass"          : "(MM > 2996.916) & (MM < 3196.916)",
    "Detachement"   : "((BPVDLS>3) | (BPVDLS<-3))",
    "MuonPIDmu"     : "MINTREE('mu+'==ABSID,PIDmu) > 0.0"
    }
    },

    "FullDSTDiMuonPsi2MuMuDetachedLine" : {
    "Prescale"      : 1.0,
    "Inherit"       : "FullDSTDiMuonJpsi2MuMuDetachedLine",
    "RequiredRawEvents" : ["Trigger", "Muon", "Calo", "Rich", "Velo", "Tracker"],
    "InputDiMuon"   : "StdLooseDiMuon",
    "Cuts"          : {
    "Mass"          :  "ADMASS('psi(2S)') < 100.0 *MeV"
    }
    },
    
    "FullDSTDiMuonJpsi2MuMuTOSLine" : {
    "Prescale"      : 1.0,
    "Inherit"       : "VirtualBase",
    "InputDiMuon"   : "StdLooseJpsi2MuMu",
    #"TOScut"        : { "L0.*Mu.*Decision%TOS":0,
    #                    "Hlt1DiMuonHighMassDecision%TOS" : 0,
    #                    "Hlt2DiMuonJPsiHighPTDecision%TOS" : 0 
    #                    },
    
    "Cuts"          : {
    "MuonP"         : "MINTREE('mu+'==ABSID,P) > 10 *GeV",
    "JpsiPt"        : "PT > 3 *GeV",
    "Mass"          : "(MM > 3010) & (MM < 3170)",
    "MuonPIDmu"     : "MINTREE('mu+'==ABSID,PIDmu) > 0.0",
    "TOSL0"         : "TOS('L0.*Mu.*Decision', 'L0TriggerTisTos')",
    "TOSHlt1"       : "TOS('Hlt1DiMuonHighMassDecision', 'Hlt1TriggerTisTos')",
    "TOSHlt2"       : "TOS('Hlt2DiMuonJPsiHighPTDecision', 'Hlt2TriggerTisTos')"
    }   
    },
    
    "FullDSTDiMuonPsi2MuMuTOSLine" : {
    "Prescale"      : 1.0,
    "Inherit"       : "FullDSTDiMuonJpsi2MuMuTOSLine",
    "InputDiMuon"   : "StdLooseDiMuon",
    #"TOScut"        : { "L0.*Mu.*Decision%TOS":0,
    #                    "Hlt1DiMuonHighMassDecision%TOS" : 0,
    #                    "Hlt2DiMuonPsi2SHighPTDecision%TOS" : 0 
    #},
    "Cuts"          : {
    "Mass"        : "ADMASS('psi(2S)') < 100.0",
    "MuonPt"      : "MINTREE('mu+'==ABSID,PT) > 1 *GeV",
    "MuonPIDmu"     : "MINTREE('mu+'==ABSID,PIDmu) > 0.0",
    "TOSL0"         : "TOS('L0.*Mu.*Decision', 'L0TriggerTisTos')",
    "TOSHlt1"       : "TOS('Hlt1DiMuonHighMassDecision', 'Hlt1TriggerTisTos')",
    "TOSHlt2"       : "TOS('Hlt2DiMuonPsi2SHighPTDecision', 'Hlt2TriggerTisTos')"
    }   
    },
    
    
    ## MicroDST lines
    
    "MicroDSTDiMuonDiMuonIncLine" : {
    "Prescale"      : 1.0,
    "Inherit"       : "VirtualBase",
    "InputDiMuon"   : "StdLooseDiMuon",
    "Cuts"          : {
    "Mass"          : "MM > 3000*MeV",
    }   
    },
    
    "MicroDSTDiMuonDiMuonSameSignLine" : {
    "Prescale"              : 0.5,
    "Inherit"               : "MicroDSTDiMuonDiMuonIncLine",
        "InputDiMuon"           : "StdLooseDiMuonSameSign",
    "Cuts"                  : None
    }
    
    
    }
    }
    }

#########################################################
### StrippingD02KpiForPromptCharm  
### StrippingDstarForPromptCharm                 
### StrippingDForPromptCharm                    
### StrippingDsForPromptCharm                   
### StrippingLambdaCForPromptCharm              
### StrippingLambdaC2pKKForPromptCharm          
### StrippingSigmaCForPromptCharm               
### StrippingLambdaCstarForPromptCharm          
### StrippingDiCharmForPromptCharm              
### StrippingDiMuonAndCharmForPromptCharm       
### StrippingDoubleDiMuonForPromptCharm         
### StrippingOmegaCstarForPromptCharm
### StrippingChiAndCharmForPromptCharm          
### StrippingChiAndWForPromptCharm              
### StrippingXicc+ForPromptCharm                
### StrippingXicc++ForPromptCharm               
### StrippingDsLamCForPromptCharm               
### StrippingCharmAndWForPromptCharm            
### StrippingDiMuonAndWForPromptCharm           
### -----------------------------------------------------
### Defined in:                 StrippingPromptCharm.py
### Proponent:                  Ivan.Belyaev@cern.ch
### Motivation:                 coherent stripping of stable charm hadrons
### Documentation: https://indico.cern.ch/conferenceDisplay.py?confId=270130
#########################################################

PromptCharm = {
    'WGs'           : ['BandQ'],
    'BUILDERTYPE'   : 'StrippingPromptCharmConf',
    'CONFIG'        : {
    #
    # use for simulation:
    'NOPIDHADRONS'   : False   , 
    #
    ## PT-cuts
    ## attention: with 1GeV pt-cut prescale is needed for D0,D+,D*+ and Ds
    #
    'pT(D0)'     :  1.0 * GeV ,    ## pt-cut for  prompt   D0
    'pT(D+)'     :  1.0 * GeV ,    ## pt-cut for  prompt   D+
    'pT(Ds+)'    :  1.0 * GeV ,    ## pt-cut for  prompt   Ds+
    'pT(Lc+)'    :  1.0 * GeV ,    ## pt-cut for  prompt   Lc+
    'pT(Xic0)'   :  1.0 * GeV ,    ## pt-cut for  prompt   Xic0/Omegac0
    'pT(Omgcc)'  :  2.0 * GeV ,    ## pt-cut for  prompt   Omegacc
    #
    'pT(D0->HH)' :  1.0 * GeV ,    ## pt-cut for  prompt   D0->KK,pipi models 
    #
    # Selection of basic particles
    #
    'TrackCut' : """
    ( CLONEDIST   > 5000      ) &
    ( TRGHOSTPROB < 0.5       ) &
    in_range ( 2  , ETA , 4.9 ) &
    HASRICH
    """ ,
    # 
    'PionCut'   : """
    ( PT          > 250 * MeV ) & 
    ( CLONEDIST   > 5000      ) & 
    ( TRGHOSTPROB < 0.5       ) &
    in_range ( 2          , ETA , 4.9       ) &
    in_range ( 3.2 * GeV  , P   , 150 * GeV ) &
    HASRICH                     &
    ( MIPCHI2DV()  > 9        )
    """ ,
    #
    'KaonCut'   : """
    ( PT          > 250 * MeV ) & 
    ( CLONEDIST   > 5000      ) & 
    ( TRGHOSTPROB < 0.5       ) &
    in_range ( 2          , ETA , 4.9       ) &
    in_range ( 3.2 * GeV  , P   , 150 * GeV ) &
    HASRICH                     &
    ( MIPCHI2DV()  > 9        )
    """ ,
    #
    'ProtonCut'   : """
    ( PT           > 250 * MeV ) & 
    ( CLONEDIST    > 5000      ) & 
    ( TRGHOSTPROB  < 0.5       ) & 
    in_range ( 2         , ETA , 4.9       ) &
    in_range ( 10 * GeV  , P   , 150 * GeV ) &
    HASRICH                      &
    ( MIPCHI2DV()  > 9         ) 
    """ ,
    ##
    'MuonCut'   : """
    ISMUON &
    in_range ( 2 , ETA , 4.9     ) &
    ( PT            >  550 * MeV ) &
    ( PIDmu - PIDpi >    0       ) &
    ( CLONEDIST     > 5000       )     
    """ ,
    ## cust for prompt kaon
    'PromptKaonCut'   : """
    ( CLONEDIST   > 5000         ) & 
    ( TRGHOSTPROB < 0.5          ) &
    in_range ( 2          , ETA , 4.9       ) &
    in_range ( 3.2 * GeV  , P   , 150 * GeV ) &
    HASRICH                     
    """ ,
    #
    ## PID-cuts for hadrons 
    #
    'PionPIDCut'   : " PROBNNpi > 0.1 " ,
    'KaonPIDCut'   : " PROBNNk  > 0.1 " ,
    'ProtonPIDCut' : " PROBNNp  > 0.1 " ,
    'PhotonCLCut'  : 0.05,
    ##
    #
    ## photons from chi_(c,b)
    #
    'GammaChi'        : " ( PT > 400 * MeV ) & ( CL > 0.05 ) " ,
    #
    ## W+- selection
    #
    'WCuts'           : " ( 'mu+'== ABSID ) & ( PT > 15 * GeV )" ,
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
    "ctau     = BPVLTIME (   9 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<9
    "ctau_9   = BPVLTIME (   9 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<9
    "ctau_16  = BPVLTIME (  16 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<16
    "ctau_25  = BPVLTIME (  25 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<25
    "ctau_100 = BPVLTIME ( 100 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<100
    "ctau_no  = BPVLTIME (     ) * c_light "  , ## no embedded cut for chi2(lifetimeFit)
    # dimuons:
    "psi           =   ADAMASS ('J/psi(1S)') < 150 * MeV"  ,
    "psi_prime     =   ADAMASS (  'psi(2S)') < 150 * MeV"  ,
    ] ,
    # Q Values:
    'QvalueXiCK'     :  500 * MeV ,
    'QvalueXiCprime' :  250 * MeV ,
    'QvalueXiCstar'  :  150 * MeV ,
    'QvalueXiCPiK'   :  500 * MeV ,
    'QvalueXiCprimeK':  500 * MeV ,
    'QvalueOmegaCC'  : 4100 * MeV ,
    ## monitoring ?
    'Monitor'     : False ,
    ## pescales
    'D0Prescale'             : 0.1,#0.05 ,
    'D+Prescale'             : 0.2,#0.05 ,
    'D*Prescale'             : 0.3,#0.1 ,
    'DsPrescale'             : 0.5 ,#0.5
    'LambdaCPrescale'          : 1.0 ,
    'XiC0Prescale'             : 1.0 ,
    'OmegaC0Prescale'          : 1.0 ,
    'LambdaCpKKPrescale'       : 1.0 ,
    'LambdaC*Prescale'         : 1.0 ,
    'OmegaC*Prescale'          : 1.0 ,
    'XiCprimePrescale'         : 1.0 ,
    'XiC*Prescale'             : 1.0 ,
    'OmegaC*2XiCPiKPrescale'   : 1.0 ,
    'OmegaC*2XiCprimeKPrescale': 1.0 ,
    'OmegaCCPrescale'          : 1.0 ,
    'SigmaCPrescale'           : 1.0 ,
    'Xic02LcPiPrescale'        : 1.0 ,
    #
    'OmegaCCKpiPrescale'       : 1.0 ,
    'OmegaCCKKPrescale'        : 1.0 ,
    ##
    'D02KKPrescale'          : 1.0 ,#0.1
    'D02pipiPrescale'        : 1.0, #0.1
    'D*CPPrescale'             : 1.0 ,
    ##
    'DiCharmPrescale'          : 1.0 ,
    'DiMu&CharmPrescale'       : 1.0 ,
    'DoubleDiMuPrescale'       : 1.0 ,
    'Chi&CharmPrescale'        : 1.0 ,
    'Charm&WPrescale'          : 1.0 ,
    'DiMuon&WPrescale'         : 1.0 ,
    'Chi&WPrescale'            : 1.0 ,
    ## ========================================================================
    },
    'STREAMS'     : { 'Charm'    : [ 'StrippingD02KpiForPromptCharm'              , 
                                     'StrippingDstarForPromptCharm'               , 
                                     'StrippingDForPromptCharm'                   , 
                                     'StrippingDsForPromptCharm'                  ,
                                     'StrippingLambdaCForPromptCharm'             ,
                                     'StrippingXiC0ForPromptCharm'                ,
                                     #'StrippingOmegaC0ForPromptCharm'             ,
                                     'StrippingLambdaC2pKKForPromptCharm'         ,
                                     'StrippingSigmaCForPromptCharm'              ,
                                     'StrippingLambdaCstarForPromptCharm'         ,
                                     'StrippingOmegaCstarForPromptCharm'          ,
                                     'StrippingXiCprimeForPromptCharm'            ,
                                     'StrippingXiCstarForPromptCharm'             ,
                                     'StrippingOmegaCstar2XiCPiKForPromptCharm'   ,
                                     'StrippingOmegaCstar2XiCprimeKForPromptCharm',
                                     'StrippingOmegaCCKpiForPromptCharm'          ,
                                     'StrippingOmegaCCKKForPromptCharm'           ,
                                     'StrippingXic02LcPiForPromptCharm'           ,
                                     'StrippingDiCharmForPromptCharm'             , ## ? 
                                     'StrippingChiAndCharmForPromptCharm'         ,
                                     'StrippingCharmAndWForPromptCharm'           ,
                                     'StrippingDiMuonAndCharmForPromptCharm'      ,
                                     ## for Eva
                                     'StrippingD02KKForPromptCharm'               ,   ## prescale ?
                                     'StrippingD02pipiForPromptCharm'             ,   ## prescale ?
                                     'StrippingDstarCPForPromptCharm'           ] ,
                      'CharmCompleteEvent' : [ 'StrippingOmegaC0ForPromptCharm'],
                      ## 
                      'Leptonic' : [ 'StrippingDoubleDiMuonForPromptCharm'   , ## Full DST ?
                                     'StrippingDiMuonAndWForPromptCharm'     , ## Full DST ? 
                                     'StrippingChiAndWForPromptCharm'        ] }
    }
