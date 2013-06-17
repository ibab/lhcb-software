"""
Line config dictionaries for proton-ion data stripping.
"""

from GaudiKernel.SystemOfUnits import *
#from GaudiKernel.SystemOfUnits import as units

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


#WMu
WMu = {
    'BUILDERTYPE'  : 'WMuConf',
    'CONFIG'       : {
    'WMu_Prescale'    : 1.0,
    'WMuLow_Prescale' : 1.0,
    'STNB_Prescale' : 1.0,
    'WMu_Postscale'   : 1.0,
    'SingMuon10_Prescale' : 1.0,
    'SingMuon48_Prescale'  :1.0,
    'pT'     : 20.,
    'pTlow'  : 15.,
    'pTvlow' :  5.,
    'SingMuon10_pT': 10.,
    'SingMuon48_pT': 4.8
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'ALL' ] 
    }

#DY2MuMu
DY2MuMu = {
    'BUILDERTYPE'  : 'DY2MuMuConf',
    'CONFIG'       : {
    'DY2MuMu1LinePrescale'    : 1.0,
    'DY2MuMu1LineHltPrescale' : 1.0,
    'DY2MuMu2LinePrescale'    : 1.0,
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

#Z02TauTau
Z02TauTau = {
    'BUILDERTYPE'  : 'Z02TauTauConf',
    'CONFIG'       : {
    'Z02TauTauLinePrescale'  : 1.0,
    'Z02TauTauLinePostscale' : 1.0,
    'PrsCalMin' : 50.,
    'ECalMin'   :  0.1,
    'HCalMax'   :  0.05,
    'trkpchi2'  :  0.001,
    'mupT'      : 15.,
    'epT'       : 15.,
    'pipT'      :  5.,
    'MMmin'     : 20.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'ALL' ]
    }


## #Z02TauTauProng
## Z02TauTauProng = {
##     'BUILDERTYPE'  : 'Z02TauTauProngConf',
##     'CONFIG'       : {
##     'Z2TauTau_Prong_LinePrescale'  : 1.0,
##     'Z2TauTau_Prong_LinePostscale' : 1.0,
##     ###
##     'TAU_MASS_LOW'                 : '400.0',    # MeV/c2
##     'TAU_MASS_HIGH'                : '1777.0',   # MeV/c2
##     ###
##     'Z_MASS_LOW'                   : '10000.0',  # MeV/c2
##     #'Z_MASS_HIGH'                 : not set
##     'VCHI2_Z_MAX'                  : '100',       # dl
##     ###
##     'PT_HAD_MIN'                   : '2000',     # MeV/c
##     'TRACKCHI2_HAD_MAX'            : '5',        # dl
##     ###
##     'PT_TAU_MIN'                   : '2000',     # MeV/c
##     'VCHI2_TAU_MAX'                : '20'       # dl
##     },
##     'WGs' : [ 'RD' ],
##     'STREAMS' : [ 'EW' ]
##     }



#Z02ee
Z02ee = {
    'BUILDERTYPE'  : 'Z02eeConf',
    'CONFIG'       : {
    'Z02ee_Prescale'  : 1.0,
    'Z02ee_Postscale' : 1.0,
    'PrsCalMin'      : 50.,
    'ECalMin'        :  0.1,
    'HCalMax'        :  0.05,
    'pT'             : 10.,
    'MMmin'          : 40.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'ALL' ]
    }

#We
We = {
    'BUILDERTYPE'  : 'WeConf',
    'CONFIG'       : {
    'We_Prescale'    : 1.0,
    'WeLow_Prescale' : 1.0,
    'We_Postscale'   : 1.0,
    'PrsCalMin' : 50.,
    'ECalMin'   :  0.10,
    'HCalMax'   :  0.05,
    'pT'        : 20.,
    'pTlow'     : 15.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'ALL' ] 
    }

#DY2ee
DY2ee = {
    'BUILDERTYPE'  : 'DY2eeConf',
    'CONFIG'       : {
    'DY2eeLine3Prescale' : 1.0,
    'DY2eeLine4Prescale' : 1.0,
    'DY2eeLinePostscale' : 1.0,
    'DY3MinMass' : 10.,
    'DY3MaxMass' : 20.,
    'DY4MinMass' : 20.,
    'DY4MaxMass' : 40.,
    'ePID'      :  1.,
    'PrsCalMin' : 50.,
    'ECalMin'   :  0.1,
    'HCalMax'   :  0.05,
    'pT3'        : 3.,
    'pT4'        : 5.,
    'p3'        : 10.,
    'p4'        : 10.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'ALL' ]
    }

#MuMuSS
MuMuSS = {
    'BUILDERTYPE'  : 'MuMuSSConf',
    'CONFIG'       : {
    'MuMuSSLine1Prescale' :  1.0,
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


#SingleTrackTIS
SingleTrackTIS = {
    'BUILDERTYPE'  : 'SingleTrackTISConf',
    'CONFIG'       : {
    'SingleTrackTIS_Prescale'    : 1.0,
    'SingleTrackTISLow_Prescale' : 1.0,
    'SingleTrackTIS_Postscale'   : 1.00,
    'pT'    : 20.,
    'pTlow' : 15.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'ALL' ] 
    }

#DisplVertices
DisplVertices = {
    'BUILDERTYPE'  : 'DisplVerticesLinesConf',
    'CONFIG'       : {
    ## Velo tracks filter
    "FilterVelo"              : { "Apply"                : True
                                , "MinIP"                : 0.1*mm
                                , "MinIPChi2"            : -1.0
                                , "MinNumTracks"         : 4
                                , "PVLocation"           : "Rec/Vertex/Primary"
                                , "RejectSplashEvents"   : False
                                , "RemoveBackwardTracks" : True
                                }

    #==========        SELECTION CUT VALUES         ==========#

    ## Velo reconstruction
    , "RV2PWithVelo"            : { "MinRho"               :  0.4*mm
                                  , "MinMass"              :  3.0*GeV
                                  , "MinSumPT"             :  0.0*GeV
                                  , "MinNumTracks"         :  5
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
    , "SingleSelection"         : { "PreScale"             :  1.0
                                                             #       HighMass,         Medium,         HighFD
                                  , "MinRho"               : [  1.0*mm ,  2.*mm ,  4.*mm  ]
                                  , "MinMass"              : [ 8.*GeV,  6.5*GeV,  4.5*GeV ]
                                  , "MinSumPT"             :  3.0*GeV
                                  , "MinNumTracks"         : [  8            ,  7            ,  7             ]
                                  , "ApplyMatterVeto"      : True
                                  , "MaxFractE1Track"      :  0.8
                                  , "MaxFractTrwHitBefore" :  0.49
                                  }
    # prescaled
    , "SinglePSSelection"       : { "PreScale"             :  0.01 ##changed from 0.005
                                  , "MinRho"               :  0.5*mm
                                  , "MinMass"              :  3.0*GeV
                                  , "MinSumPT"             :  0.0*GeV
                                  , "MinNumTracks"         :  5
                                  , "ApplyMatterVeto"      :  False
                                  , "MaxFractE1Track"      :  10.
                                  , "MaxFractTrwHitBefore" :  10.
                                  }

    ## Double LLP line
    , "DoubleSelection"         : { "PreScale"             :  1.0
                                  , "MinRho"               :  0.6*mm
                                  , "MinMass"              :  3.0*GeV
                                  , "MinSumPT"             :  3.0*GeV
                                  , "MinNumTracks"         :  6
                                  , "ApplyMatterVeto"      :  False
                                  , "MaxFractE1Track"      :  0.8
                                  , "MaxFractTrwHitBefore" :  0.49
                                  ## Double only
                                  , "MinHighestMass"       : 3.0*GeV
                                  , "ApplyMatterVetoOne"   : True
                                  }
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
    , "SingleDownSelection"     : { "PreScale"             :  1.0
                                  , "MinRho"               :  2.0*mm
                                  , "MinMass"              :  3.0*GeV
                                  , "MinSumPT"             :  0.0*GeV
                                  , "MinNumTracks"         :  4
                                  , "ApplyMatterVeto"      : False
                                  , "MaxFractE1Track"      :  10.
                                  , "MaxFractTrwHitBefore" :  10.
                                  # Down only
                                  , "MinZ"                 :  170.*mm
                                  }
    #========== Other lines for efficiency studies ==========#
    , "HLTPS"             : { "PreScale"             :  1.0 }
    , "HltEffCharmHLTSelection"  : { "PreScale"             :  1.0
                                   , "MinRho"               :  0.6*mm
                                   , "MinMass"              :  6.*GeV
                                   , "MinSumPT"             :  3.0*GeV
                                   , "MinNumTracks"         :  6
                                   , "ApplyMatterVeto"      : False
                                   , "MaxFractE1Track"      :  10.
                                   , "MaxFractTrwHitBefore" :  10.
                                   }

    #==========     HLT filters for all lines      ==========#
    , "HLT"                     : { "CharmHLT"     : "HLT_PASS('Hlt2CharmHadD02HH_D02KPiDecision')"
                                  , "HLTPS"        : [ ( ("0x001c0028", "0x002f002c"), "HLT_PASS_RE('Hlt2DisplVerticesSinglePostScaledDecision')" )
                                                     , ( ("0x00340032", "0x00730035"), "HLT_PASS_RE('Hlt2DisplVerticesSinglePostScaledDecision')" )
                                                     , ( ("0x00750037", "0x007b0038"), "HLT_PASS_RE('Hlt2DisplVertices(Single|Double|SingleMV)PostScaledDecision')" )
                                                     , ( ("0x007e0039", "0x0097003d"), "HLT_PASS_RE('Hlt2DisplVertices(Single|Double|SingleMV)PostScaledDecision')" )
                                                     , ( ("0x00990042", "0x40000000"), "HLT_PASS_RE('Hlt2DisplVertices(Single|SingleLoose|Double)PSDecision')" )
                                                     ]
                                  }
    },                              
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'ALL' ]
    }

    
#HighPtJets
HighPtJets = {
    'BUILDERTYPE'  : 'HighPtJetsLinesConf',
    'CONFIG'       : {
    'MinWeightSeedCut' : 6
    ,  'ConeExtFactorCut' : 4.0
    ,  'MaxDeltaPhiCut'   : 6.5
    ,  'MaxDeltaEtaCut'   : 0.35
    ,  'Min1stJetPtCut'   : 10.0
    ,  'Min2ndJetPtCut'   : 0.0
    ,  'JetMultCut'       : 1
    ,  'MaxRPVCut'        : 0.5
    ,  'scale'            : 0.5
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'ALL' ]
    }

#InclbJets
InclbJets = {
    'BUILDERTYPE'  : 'InclbJetsLinesConf',
    'CONFIG'       : {
    'NrPVsCut'         : 1
    ,  'NrSeedsCut'       : 2
    ,  'TrkChi2Cut'       : 3.0
    ,  'PrtMomCut'        : 2.0
    ,  'PrtPtCut'         : 0.6
    ,  'PrtIPSCut'        : 2.5
    ,  'DZSVPVCut'        : 1.0
    ,  'SumMomSVCut'      : 3.0
    ,  'VtxChi2Cut'       : 20.0
    ,  'scale'            : 0.05
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'ALL' ]
    }

#A1MuMu
A1MuMu = {
    'BUILDERTYPE'  : 'A1MuMuConf',
    'CONFIG'       : {
    'A1MuMu_LinePrescale'  : 1.0,
    'A1MuMu_LinePostscale'              : 1.0,
    'A1MuMu_checkPV'                    : False,
    ###
    'DIMUON_LOW_MASS'                   : '5000.0',    # MeV/c2
    #'DIMUON_HIGH_MASS'                 : not set,     # MeV/c2
    ###
    'PT_MUON_MIN'                       : '2500.0',    # MeV/c
    'P_MUON_MIN'                        : '2500.0',    # MeV/c (de facto no cut)
    'TRACKCHI2_MUON_MAX'                : '10',        # dl
    ###
    'PT_DIMUON_MIN'                     : '7500.0',    # MeV/c
    'VCHI2_DIMUON_MAX'                  : '12'         # dl
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'ALL' ]
    }

#HighPtTopo
HighPtTopo = {
    'BUILDERTYPE'  : 'HighPtTopoJetsConf',
    'CONFIG'       : {
    'HighPtTopo_Prescale'    : 1.0,
    'HighPtTopoLow_Prescale' : 1.0,
    'HighPtTopo_Postscale'   : 1.0,
    'pT'        : 40000,
    'pTlow'     : 20000
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'ALL' ]
    }

#StrangeBaryons
StrangeBaryons = {
    'BUILDERTYPE' : 'StrippingStrangeBaryonsConf',
    'CONFIG' : {
    #PID cuts
    'PreScale'    : 0.01,
    'checkPV'   : True,
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
    'STREAMS':[ 'ALL' ]
    }


##PLEASE NOTE:
##Lines below here MUST be moved to a new stream if RICH RAW information is removed from the EW DST.

#LowMult
LowMult = {
    'BUILDERTYPE'  : 'LowMultConf',
    'CONFIG'       : {
    'LowMultPrescale'           : 1.0
    , 'LowMultWSPrescale'       : 1.0
    , 'LowMultHHIncPrescale'    : 1.0
    , 'LowMultLMRPrescale'      : 1.0
    , 'LowMultPrescale_ps'      : 1.0
    , 'LowMultNoFilterPrescale' : 1.0
    , 'LowMultPostscale'        : 1.0
    # Final-state particles
    , 'H_PTmin'         : 100.0 * MeV
    , 'H_Pmin'          : 5000.0 * MeV
    , 'H_TrkChi2max'    : 3.0
    , 'K_PIDKmin'       : 0.0
    , 'P_PIDPmin'       : 0.0
    , 'KKInc_K_Pmin'    : 10000.0 * MeV
    , 'KKInc_K_PIDKmin' : 5.0
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
    ,'PHI2KK_APTmin'        : 0.0 * MeV
    ,'PHI2KK_APTmax'        : 1500.0 * MeV
    ,'PHI2KK_AMmin'         : 990.0 * MeV
    ,'PHI2KK_AMmax'         : 1050.0 * MeV
    ,'PHI2KK_ADOCAmax'      : 0.1 * mm
    ,'PHI2KK_APmin'         : 4000.0 * MeV
    ,'PHI2KK_VtxChi2DoFmax' : 3.0
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'ALL' ]
    }

#DiPhotonDiMuon
DiPhotonDiMuon = {
    'BUILDERTYPE'  : 'PP2PPMuMuConf',
    'CONFIG'       : {
    'PP2PPMuMuLinePrescale'    : 1.0 
    ,  'PP2PPMuMuLinePostscale'   : 1.0
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'ALL' ]
    }

#HighPtGamma
HighPtGamma = {
    'BUILDERTYPE'  : 'HighPtGammaConf',    
    'CONFIG'       : {
    'Prescale'  : 1.0,
    'Postscale' : 1.0,
    'pT'        : 18.,
    'photonIso' : 30,

    'PrescaleLoose'  : 0.5,
    'PostscaleLoose' : 1.0,
    'pTLoose'        : 7.5,
    'photonIsoLoose' : 5
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'ALL' ]
    } 

#LLP2MuX
LLP2MuX = {
    'BUILDERTYPE'  : 'LLP2MuX',
    'CONFIG'       : {
    "MinPT"  : 12.00 * GeV,
    "MinIP"  :  0.25 * mm
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'ALL' ]
    }

DiMuonpA = {
    'BUILDERTYPE'  : 'DiMuonpAConf',
    'CONFIG'       : {
    'TotalCuts'  :  """
    (MINTREE('mu+'==ABSID,PT)>0.60*GeV)
    & (VFASPF(VCHI2PDOF)<25)
    & ((ADMASS('J/psi(1S)')<150*MeV) | (ADMASS('psi(2S)')<150*MeV) | (MM>8.4*GeV))
    """,
    'Prescale'   :  1.,
    'CheckPV'    :  False
    },
    'WGs' : [ 'BandQ' ],
    'STREAMS' : [ 'ALL' ]
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


#------------------------------------------------------------------------
#
# PID calibration lines (excludes the recently added Lambda_c proton
# calibration lines
#
#-----------------------------------------------------------------------

# PID: J/Psi -> mu mu
MuIDCalib = {
    'BUILDERTYPE' : 'MuIDCalibConf',
    'WGs'         : [ 'ALL' ],
    'STREAMS'     : [ 'ALL' ],
    'CONFIG'      : {
    'PromptPrescale'           : 0.0,
    'DetachedPrescale'         : 1.0,
    'DetachedNoMIPPrescale'    : 1.0,
    'DetachedNoMIPHiPPrescale' : 0.0,
    'DetachedNoMIPKPrescale'   : 1.0,
    'FromLambdacPrescale'      : 1.0,
    'KFromLambdacPrescale'     : 0.0,
    'PiFromLambdacPrescale'    : 0.0,
    'PFromLambdacPrescale'     : 0.0,
    'KISMUONFromLambdacPrescale' : 0.0,
    'PiISMUONFromLambdacPrescale': 0.0,
    'PISMUONFromLambdacPrescale' : 0.0
    }
    }

# PID: D* tagged D0 -> K- pi+
NoPIDDstarWithD02RSKPi = {
    'BUILDERTYPE' : 'NoPIDDstarWithD02RSKPiConf',
    'STREAMS'     : [ 'ALL' ],
    'WGs'         : [ 'ALL' ],
    'CONFIG'      : {
    'DaugPt'             : 250 * MeV      ## GeV
    , 'DaugP'            : 2.0 * GeV      ## GeV
    , 'DaugIPChi2'       : 16             ## unitless
    , 'DaugTrkChi2'      : 5              ## unitless
    , 'D0MassWin'        : 75 * MeV       ## MeV
    , 'D0Pt'             : 1.5 * GeV      ## GeV
    , 'D0VtxChi2Ndof'    : 13             ## unitless
    , 'D0FDChi2'         : 49             ## unitless
    , 'D0BPVDira'        : 0.9999         ## unitless
    , 'D0IPChi2'         : 30             ## unit
    , 'SlowPiPt'         : 150 * MeV      ## MeV
    , 'SlowPiTrkChi2'    : 5              ## unitless
    , 'DstarPt'          : 2.2 * GeV      ## GeV
    , 'DstarVtxChi2Ndof' : 13             ## unitless
    , 'DeltaM_Min'       : 130 * MeV      ## MeV
    , 'DeltaM_Max'       : 155 * MeV      ## MeV
    ##
    , 'DCS_WrongMass'    : 25 * MeV       ## MeV (3 sigma veto)
    , 'KK_WrongMass'     : 25 * MeV       ## MeV (3 sigma veto)
    , 'PiPi_WrongMass'   : 25 * MeV       ## MeV (3 sigma veto)
    ##
    , 'Prescale'         : 0.89           ## unitless
    , 'Postscale'        : 1.00           ## unitless
    ##
    , 'Monitor'          : False           ## Activate the monitoring?
    }
    }

# PID: Lambda0 -> p pi- and KS0 -> pi+ pi-
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
    'KS0LL_Prescale'           : 0.022 ,
    'KS0DD_Prescale'           : 0.027 , 
    'LamLL_Prescale_LoP'       : 0.180 , 
    'LamLL_Prescale_HiP'       : 1.000 ,
    'LamDD_Prescale'           : 0.140 , 
    'LamLLIsMUON_Prescale_LoP' : 1.000 ,
    'LamLLIsMUON_Prescale_HiP' : 1.000 ,
    'LamDDIsMUON_Prescale'     : 1.000   
     }
    }

# PID: J/Psi -> e+ e-
Jpsi2eeForElectronID = {
    'BUILDERTYPE'       : 'ElectronIDConf',
    'CONFIG'    : {
        'JpsiLinePrescale'        :   0.5   ,
        'JpsiLineHltFilter'       : None    ,

        'Both_PT'                 :  250.   ,  # MeV
        'Both_P'                  : 2000.   ,  # MeV
        'Both_TRCHI2DOF'          :    5.   ,
        'Both_MIPCHI2'            :    9.   ,
        
        'Tag_PT'                  : 1500.   ,  # MeV
        'Tag_P'                   : 6000.   ,  # MeV
        'Tag_PIDe'                :    5.   ,
        'Tag_MIPCHI2'             :    9.   ,

        'Probe_PT'                :  250.   ,  # MeV
        'Probe_P'                 : 2000.   ,  # MeV
        'Probe_MIPCHI2'           :    9.   ,
        
        'eeCombMinMass'           : 2100.   ,  # MeV         
        'eeCombMaxMass'           : 4300.   ,  # MeV   
        'eeVCHI2PDOF'             :    9.   ,  
        'eeMinMass'               : 2200.   ,  # MeV 
        'eeMaxMass'               : 4200.   ,  # MeV

        'JpsiLineCut'             : "(PT>2.*GeV) & (BPVDLS>50) ",      
        
        'Bu2JpsieeKLine_Prescale'  :  1,
        'Bu2JpsieeKLine_HltFilter' : None,
        'Bu2JpsieeKLine_KaonCut'   : "(TRCHI2DOF<4) & (TRGHOSTPROB<0.5) & (PT>1.0*GeV) & (PIDK >0) & (BPVIPCHI2()>9)",
        'Bu2JpsieeKLine_JpsiCut'   : "(BPVDLS>5)",
        'Bu2JpsieeKLine_BuComCut'  : "in_range(4.1*GeV,AM,6.1*GeV)",
        'Bu2JpsieeKLine_BuMomCut'  : "in_range(4.2*GeV,M, 6.0*GeV) & (VFASPF(VCHI2PDOF)<9)"
        },
    'STREAMS' : [ 'ALL' ],
    'WGs'    : ['ALL']
    }

# PID: inclusive phi -> K+ K-
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

# ChargedHyperon Lines
# Full-DST, Michael Sokoloff, Laurence Carson

ChargedHyperons  = {
    'BUILDERTYPE' : 'ChargedHyperonsConf',
    'CONFIG' : {
                  'TRCHI2DOFMax'    : 3.0
          , 'PionPIDK'        :  5.0
          , 'KaonPIDK'        :  -5.0
          , 'XiMassWindow'    :  20 * MeV
          , 'OmegaMassWindow' :  20 * MeV
          , 'LambdaLLMinDecayTime'  :  5.0 ##   (ps)
          , 'LambdaLLVtxChi2Max'    :   5.0
          , 'LambdaDDVtxChi2Max'    :   5.0
          , 'LambdaLLMassWin'  : 5.7 * MeV
          , 'LambdaDDMassWin'  : 5.7 * MeV
          , 'LambdaLLMinVZ'    : -100. * mm
          , 'LambdaLLMaxVZ'    :  500. * mm
          , 'LambdaDDMinVZ'    :  300. * mm
          , 'LambdaDDMaxVZ'    : 2275. * mm
          , 'TrGhostProbMax'   :  0.25
          , 'ProbNNkMin'       :  0.20
          , 'ProbNNpMinLL'     :  0.20
          , 'ProbNNpMinDD'     :  0.05
            },
    'STREAMS' : ['ALL'] ,
    'WGs'    : ['Charm']
}

