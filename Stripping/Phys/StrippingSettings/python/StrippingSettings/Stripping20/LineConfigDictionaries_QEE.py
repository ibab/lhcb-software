"""
Line config dictionaries for the EW Stream. Mostly QEE WG lines


Dictionary names should have the same name as the corresponding
StrippingSelections file containing the line builder instance.

   e.g  StrippingPromptCharm linebuilder should be added as:

      PromptCharm = { ... }
"""

from GaudiKernel.SystemOfUnits import *
#import GaudiKernel.SystemOfUnits as units


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
    'STREAMS' : [ 'EW' ]
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
    'SingMuon48_Prescale'  :0.2,
    'pT'     : 20.,
    'pTlow'  : 15.,
    'pTvlow' :  5.,
    'SingMuon10_pT': 10.,
    'SingMuon48_pT': 4.8
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ] 
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
    'STREAMS' : [ 'EW' ]
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
    'STREAMS' : [ 'EW' ]
    }


## #Z02TauTauProng
## Z02TauTauProng = {
##     'BUILDERTYPE'  : 'Z02TauTauProngConf',
##     'CONFIG'       : {
##     'Z2TauTau_Prong_LinePrescale'  : 1.0,
##     'Z2TauTau_Prong_LinePostscale' : 1.0,
##     ###
##     'TAU_MASS_LOW'                 : '400.0',    # MeV/c²
##     'TAU_MASS_HIGH'                : '1777.0',   # MeV/c²
##     ###
##     'Z_MASS_LOW'                   : '10000.0',  # MeV/c²
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
    'STREAMS' : [ 'EW' ]
    }

#We
We = {
    'BUILDERTYPE'  : 'WeConf',
    'CONFIG'       : {
    'We_Prescale'    : 1.0,
    'WeLow_Prescale' : 0.1,
    'We_Postscale'   : 1.0,
    'PrsCalMin' : 50.,
    'ECalMin'   :  0.10,
    'HCalMax'   :  0.05,
    'pT'        : 20.,
    'pTlow'     : 15.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ] 
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
    'STREAMS' : [ 'EW' ]
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
    'STREAMS' : [ 'EW' ]
    }


#SingleTrackTIS
SingleTrackTIS = {
    'BUILDERTYPE'  : 'SingleTrackTISConf',
    'CONFIG'       : {
    'SingleTrackTIS_Prescale'    : 0.10,
    'SingleTrackTISLow_Prescale' : 0.01,
    'SingleTrackTIS_Postscale'   : 1.00,
    'pT'    : 20.,
    'pTlow' : 15.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ] 
    }

#DisplVertices
"""
DisplVertices = {
    'BUILDERTYPE'  : 'DisplVerticesLinesConf',
    'CONFIG'       : {
    ## Velo tracks filter
    "FilterVelo"              : { "Apply"                : True
                                  , "MinIP"                : 0.1*mm
                                  , "MinIPChi2"            : -1.0
                                  , "MinNumTracks"         : 4
                                  , "PVLocation"           : "Rec/Vertex/Primary" ## TODO check if this makes sense
                                  , "RejectSplashEvents"   : False
                                  , "RemoveBackwardTracks" : True
                                  }
    
    #==========        SELECTION CUT VALUES         ==========#
    
    ## Velo reconstruction
    , "RV2PWithVelo"            : { "MinRho"               :  0.4*mm
                                    , "MinMass"              :  3.0*GeV
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
    , "SingleSelection"         : { "PreScale"             :  1.0
                                    #       HighMass,         Medium,         HighFD
                                    , "MinRho"               : [  1.0*mm ,  2.*mm ,  4.*mm  ]
                                    , "MinMass"              : [ 8.*GeV,  6.5*GeV,  4.5*GeV ]
                                    , "MinSumPT"             :  3.0*GeV
                                    , "MinNumTracks"         : [  8            ,  7            ,  6             ]
                                    , "ApplyMatterVeto"      : True
                                    , "MaxFractE1Track"      :  0.8
                                    , "MaxFractTrwHitBefore" :  0.49
                                    }
    # prescaled
    , "SinglePSSelection"       : { "PreScale"             :  0.005
                                    , "MinRho"               :  0.5*mm
                                    , "MinMass"              :  3.0*GeV
                                    , "MinSumPT"             :  0.0*GeV
                                    , "MinNumTracks"         :  4
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
                                     , "MinMass"              :  5.5*GeV
                                     , "MinSumPT"             :  3.0*GeV
                                     , "MinNumTracks"         :  6
                                     , "ApplyMatterVeto"      : False
                                     , "MaxFractE1Track"      :  10.
                                     , "MaxFractTrwHitBefore" :  10.
                                     }
    
    #==========     HLT filters for all lines      ==========#
    , "HLT"                     : { "CharmHLT"     : "HLT_PASS('Hlt2CharmHadD02HH_D02KPiDecision')"
                                    , "HLTPS" : "HLT_PASS_RE('Hlt2DisplVertices(Single|SingleLoose|Double)PSDecision')"
                                    }
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }
"""
    
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
    'STREAMS' : [ 'EW' ]
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
    ,  'scale'            : 0.1
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
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
    'STREAMS' : [ 'Dimuon' ]
    }

#HighPtTopo
HighPtTopo = {
    'BUILDERTYPE'  : 'HighPtTopoJetsConf',
    'CONFIG'       : {
    'HighPtTopo_Prescale'    : 1.0,
    'HighPtTopoLow_Prescale' : 0.1,
    'HighPtTopo_Postscale'   : 1.0,
    'pT'        : 40000,
    'pTlow'     : 20000
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }

#StrangeBaryons
StrangeBaryons = {
    'BUILDERTYPE'  : 'StrippingStrangeBaryonsConf',
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
    'Lambda0MassWindow'            :       6., # < 6 for all six cases
    
    #Bachelor cuts
    'minCHI2IPPV_Pi_Bachelor_LLL' :      10., # > 10
    'minCHI2IPPV_Pi_Bachelor_DDD' :       4., # > 4
    'minCHI2IPPV_Bachelor'     :       3., # > 3 for the other 3 cases: Tight DDL & DDD + Loose DDL
    'minCHI2IPPV_K_Bachelor_D' :      10., # > 6
    
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


##PLEASE NOTE:
##Lines below here MUST be moved to a new stream if RICH RAW information is removed from the EW DST.

#LowMult
LowMult = {
    'BUILDERTYPE'  : 'LowMultConf',
    'CONFIG'       : {
    'LowMultPrescale'           : 1.0
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
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }

#DiPhotonDiMuon
DiPhotonDiMuon = {
    'BUILDERTYPE'  : 'PP2PPMuMuConf',
    'CONFIG'       : {
    'PP2PPMuMuLinePrescale'    : 1.0 
    ,  'PP2PPMuMuLinePostscale'   : 1.0
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }

