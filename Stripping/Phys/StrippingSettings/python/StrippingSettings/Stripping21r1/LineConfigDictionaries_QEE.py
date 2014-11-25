################################################################################
##                          S T R I P P I N G  2 1 r 1                            ##
##                                                                            ##
##  Configuration for QEE WG                                                  ##
##  Contact person: Ronan Wallace - ronan.wallace@cern.ch                     ##
################################################################################
"""
Line config dictionaries for the EW Stream. Mostly QEE WG lines


Dictionary names should have the same name as the corresponding
StrippingSelections file containing the line builder instance.

   e.g  StrippingPromptCharm linebuilder should be added as:

      PromptCharm = { ... }
"""

from GaudiKernel.SystemOfUnits import *
#import GaudiKernel.SystemOfUnits as units

#######################
#Group: Jets and Exotica
#DST: micro
#Raw: Muon
#######################
H24MuLines={
    'BUILDERTYPE'  : 'H24MuLineConf',
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'Leptonic' ],
    'CONFIG':{'DefaultPostscale'       : 1,
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

              }
}

####################################
#DST: Full
#Raw: Calo, Muon, Tracker, Velo, Rich
####################################
LLP2MuX= {
    'WGs'         : ['QEE'],
    'STREAMS'         : ['EW'],
    'BUILDERTYPE' : 'LLP2MuXConf',
    'CONFIG'      : { "MinPT"  : 12.00*GeV
                     ,"MinIP"  :  0.25*mm
                    }
}

#########
#Raw:Calo
#########
DisplVerticesLines = {
    'BUILDERTYPE' : 'DisplVerticesLinesConf',
    'CONFIG'      : {
        ## Velo GEC
          "VeloGEC"                 : { "Apply"                : True 
                                      , "MaxVeloRatio"         : 0.1
                                      , "MaxPhiVectorSize"     : 250.
                                      }
        ## Velo tracks filter
        , "FilterVelo"              : { "Apply"                : True
                                      , "MinNumTracks"         : 4
                                      , "PVLocation"           : "Rec/Vertex/Primary"
                                      , "RemoveBackwardTracks" : True  
                                      , "RemovePVTracks"       : False # use IP(CHI2) cut instead
                                      , "MinIP"                : 0.1*mm
                                      , "MinIPChi2"            : -1.0
                                      , "MinDOCABeamLine"      : -2.0
                                      , "RemoveVeloClones"     : True
                                      }
                                      
        , "JetID"                   : "( 0.8 > JMPF ) & ( 0.1 < JCPF ) & ( 900. < JMPT )"
                                      
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
        , "SinglePSSelection"       : { "PreScale"             :  0.005
                                      , "MinRho"               :  0.5*mm
                                      , "MinMass"              :  3.0*GeV
                                      , "MinSumPT"             :  0.0*GeV
                                      , "MinNumTracks"         :  5
                                      , "ApplyMatterVeto"      :  False
                                      , "MaxFractE1Track"      :  10.
                                      , "MaxFractTrwHitBefore" :  10.
                                      }
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
                                      , "ConeSize"             :  1.2
                                      , "MinDOCABL"            :  0.1*mm
                                      , "MinNumJets"           :  1  
                                      , "SingleJet"            :  True
                                      , "MinNJetMass"          :  0.0
                                      , "MinNJetTransvMass"    : None
                                      , "JetIDCut"             : "( JNWITHPVINFO >= 5 ) & ( JMPT > 1800. ) & ( (PT/M) > 2.5 )"
                                      } # Tuning: done
        , "JetSingleHighMassSelection" : { "PreScale"          :  1.0
                                      , "MinRho"               :  0.4 
                                      , "MinMass"              :  5.0*GeV
                                      , "MinSumPT"             :  7.0*GeV
                                      , "MinNumTracks"         :  5
                                      , "ApplyMatterVeto"      : True
                                      , "MaxFractE1Track"      :  0.8
                                      , "MaxFractTrwHitBefore" :  0.49
                                      ## Jet-related cuts
                                      , "ConeSize"             :  0.7  
                                      , "MinDOCABL"            :  -2.
                                      , "MinNumJets"           :  2
                                      , "SingleJet"            :  False
                                      , "MinNJetMass"          :  0.0
                                      , "MinNJetTransvMass"    : None
                                      , "JetIDCut"             : None
                                      } # Tuning: done
        ## jet sequence on top of Hlt2 candidates
        , "JetHltSingleLowMassSelection": { "PreScale"         :  1.0
                                      ## Jet-related cuts
                                      , "ConeSize"             :  1.2
                                      , "MinDOCABL"            :  0.1*mm
                                      , "MinNumJets"           :  1  
                                      , "SingleJet"            :  True
                                      , "MinNJetMass"          :  0.0
                                      , "MinNJetTransvMass"    : None
                                      , "JetIDCut"             : "( JNWITHPVINFO >= 5 ) & ( JMPT > 1800. ) & ( (PT/M) > 1.5 )"
                                      } # Tuning: done
        , "JetHltSingleHighMassSelection" : { "PreScale"       :  1.0 
                                      ## Jet-related cuts
                                      , "ConeSize"             :  0.7
                                      , "MinDOCABL"            :  -2.
                                      , "MinNumJets"           :  2
                                      , "SingleJet"            :  False
                                      , "MinNJetMass"          :  0.0
                                      , "MinNJetTransvMass"    : None
                                      , "JetIDCut"             : None
                                      } # Tuning: done

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
        , "HLTPS"                   : { "PreScale"             :  0.2 }
        , "HltEffCharmHLTSelection" : { "PreScale"             :  1.0
                                      , "MinRho"               :  0.6*mm
                                      , "MinMass"              :  6.*GeV
                                      , "MinSumPT"             :  3.0*GeV
                                      , "MinNumTracks"         :  6  
                                      , "ApplyMatterVeto"      : False
                                      , "MaxFractE1Track"      :  10.
                                      , "MaxFractTrwHitBefore" :  10.
                                      }

        , "HLTPS"                   : { "PreScale"             :  0.2 }
        , "HltEffCharmHLTSelection" : { "PreScale"             :  1.0
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
                                      ## For reviving Hlt2 candidates
                                      , "SignalLines"  : [ ( ("0x001c0028", "0x002f002c"), ["Hlt2DisplVerticesSingleDecision"] )
                                                         , ( ("0x00340032", "0x00730035"), ["Hlt2DisplVerticesHighFDSingleDecision", "Hlt2DisplVerticesHighMassSingleDecision", "Hlt2DisplVerticesLowMassSingleDecision", "Hlt2DisplVerticesSingleDownDecision"] )
                                                         , ( ("0x00750037", "0x007b0038"), ["Hlt2DisplVerticesSingleDecision","Hlt2DisplVerticesSingleDownDecision", "Hlt2DisplVerticesSingleHighFDPostScaledDecision", "Hlt2DisplVerticesSingleHighMassPostScaledDecision"] )
                                                         , ( ("0x007e0039", "0x0097003d"), ["Hlt2DisplVerticesSingleDecision","Hlt2DisplVerticesSingleDownDecision", "Hlt2DisplVerticesSingleHighFDPostScaledDecision", "Hlt2DisplVerticesSingleHighMassPostScaledDecision"] )
                                                         , ( ("0x00990042", "0x40000000"), ["Hlt2DisplVerticesSingleDecision","Hlt2DisplVerticesSingleDownDecision", "Hlt2DisplVerticesSingleHighFDDecision", "Hlt2DisplVerticesSingleHighMassDecision", "Hlt2DisplVerticesSingleVeryHighFDDecision"] )
                                                         ]
                                      }
        },
    'WGs'         : ['QEE'],
    'STREAMS'     : [ 'EW' ]
}


Ditau = {
  'WGs'         : [ 'QEE' ],
  'BUILDERTYPE' : 'DitauConf',
  'STREAMS'     : ['EW'],

  # Configuration for isolated muon + track (MuX)
  'CONFIG'  : {
    # All subconfig have to be nested inside 'CONFIG'
  
    # Configuration for isolated muon + track (MuX)
    'CONFIG_MuX':              { 
                                # Preambles
                                'prescale'  : 1.0,
                                'postscale' : 1.0,
                                'HLT'       : "HLT_PASS('Hlt2SingleMuonHighPTDecision')",  # PT > 10 GeV

                                # Cuts on all single child ( mu / pi )
                                'min_TRPCHI2'       : 0.01, # 0.01,
                                'max_BPVIP'         : 1 * mm, # 0.5 * mm,
                                'max_Cone_N'        : 2, # 2,  # Number of charged track(s) in a 0.1-Cone

                                # Muon only
                                'mu_min_PT'         : 10 * GeV, # 5
                                'mu_min_FracCone05E': 0.7,

                                # Track only (StdAllNoPIDsPions)
                                'pi_min_PT'         : 3 * GeV, # 3
                                'pi_min_FracCone05E': 0.1,

                                # Cut on ditau
                                'mother_min_MM'     : 11 * GeV, # M**2 >= 4*PT1*PT2
                               },
 

    # Configuration for isolated electron + track (EX)
    'CONFIG_EX' :             {

                               # Preambles
                               'prescale'  : 1.0,
                               'postscale' : 1.0,
                               'HLT'       : "HLT_PASS('Hlt2SingleTFElectronDecision') | HLT_PASS('Hlt2SingleTFVHighPtElectronDecision') ",  # PT10(tight) | PT15

                               # Cuts on all single child ( e / pi )
                               'min_TRPCHI2'     : 0.01,
                               'max_BPVIP'       : 1 * mm,
                               'max_Cone_N'      : 2,

                               # Electron only
                               'e_min_PT'          : 10. * GeV,
                               'e_min_FracCone05E' : 0.7,
                               'e_max_TRCHI2DOF'   : 20,
                               'e_min_CaloPrsE'    : 50. * MeV,
                               'e_min_CaloEcalE'   : 0.1,   # Fraction of P
                               'e_max_CaloHcalE'   : 0.05,  # Fraction of P

                               # Track only (StdAllNoPIDsPions)
                               'pi_min_PT'         : 2 * GeV,
                               'pi_min_FracCone05E': 0.1,

                               # Mother cuts
                               'mother_min_MM'     : 8 * GeV, # M**2 >= 4*PT1*PT2
                              },


    # Configuration for isolated muon + 3-prongs (Mu3P)
    #'CONFIG_Mu3P' : None,
  
    # Configuration for 3-prongs + 3-prongs (3P3P)
    #'CONFIG_3P3P' : None,

  }

}

InclbJets = {
    'WGs'         : ['QEE'],
    'BUILDERTYPE' : 'InclbJetsLinesConf',
    'CONFIG'      : { 'NrPVsCut'         : 1,
                      'NrSeedsCut'       : 2,
                      'TrkChi2Cut'       : 3.0,
                      'PrtMomCut'        : 2.0,
                      'PrtPtCut'         : 0.6,
                      'PrtIPSCut'        : 2.5,
                      'DZSVPVCut'        : 1.0,
                      'SumMomSVCut'      : 3.0,
                      'VtxChi2Cut'       : 20.0,
                      'scale'            : 0.05
                    },
    'STREAMS'     : [ 'EW' ]
    }

WmuAKTJets = {
    'BUILDERTYPE'  : 'WmuAKTJetsConf',
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ],
    'CONFIG':{'WmuAKTJets_Prescale'    : 1.0,
              'WmuAKTJets_Postscale'   : 1.0,
              'min_mu_pT'              : 10., # GeV/c
              'max_mu_pT'              : 200000., # GeV/c
              'dr_lepton_jet'          : 0.5,
              'min_jet_pT'             : 15. # GeV/c
              }
}

WeAKTJets = {
    'BUILDERTYPE'  : 'WeAKTJetsConf',
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ],
    'CONFIG':{'WeAKTJets_Prescale'    : 1.0,
              'WeAKTJets_Postscale'   : 1.0,
              'min_e_pT'              : 10., #GeV/c
              'max_e_pT'              : 200000., #GeV/c
              'dr_lepton_jet'         : 0.5,
              'PrsCalMin'             : 50.,
              'ECalMin'               : 0.10,
              'HCalMax'               : 0.05,
              'min_jet_pT'            : 15. #GeV/c
              }
}

HighPtGamma = {
    'BUILDERTYPE'  : 'HighPtGammaJetsConf',
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ],
    'CONFIG':{'Prescale'  : 1.0,
              'Postscale' : 1.0,
              'photonpT'  : 12.5 ,# GeV/c
              'photonIso' : 25,
              'minJetpT' : 12.5, # GeV/c
              'PrescaleLoose'  : 0.1,
              'PostscaleLoose' : 1.0,
              'photonpTLoose'  : 7.5,
              'photonIsoLoose' : 20,
              'minJetpTLoose' : 7.5 # GeV/c
              }
}

Dijets = {
    'BUILDERTYPE' : 'DijetsConf',
    'CONFIG'      : {
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
    'WGs'         : ['QEE'], 
    'STREAMS'         : ['EW']
}


Jets = {
    'BUILDERTYPE'       : 'JetsConf',
    # Prescale for the calibration line.
    'CONFIG':{
        "PRESCALE" : {"MB"         : 1.0,
                      "DIFF"       : 1.0,
                      "bJetPT15"   : 0.005,
                      "bJetPT50"   : 0.1,
                      "bJetPT90"   : 1.0,
                      "dibJetT6A"  : 0.05,
                      "dibJetT6PS" : 1.0,
                      "_3jets_Pt7_3sv" : 1.0,
                      "_4jets_Pt5_3sv" : 1.0,
                      "_4jets_Pt5_0sv_Prescaled" : 0.01
                      },

        # HLT properties.
        "HLT"   : {"LINETOPO"       : "Hlt2Topo", # Line to use for TOPO
                   "LINEMB"         : "Hlt1MBNoBiasDecision"} , # Line to use for MB
        # Track properties.
        "TRK"   : {"MAX_MULT"       : 2500,       # Multiplicity.
                   "MIN_P"          : 5*GeV,     # Momentum.
                   "MIN_PT"         : 500*MeV,   # Transverse momentum.
                   "MIN_MIPCHI2DV"  : 16,        # Impact parameter chi-squared.
                   "MAX_GHP"        : 0.4,
                   "MAX_PROBNNGHOST": 0.7},      # Ghost probability.
        # Secondary vertex properties.
        "SVR"   : {"MAX_CHI2DOCA"   : 8,         # Chi-squared of closest approach.
                   "MIN_BPVDIRA"    : 0,         # Cos of angle between P and flight.
                   "MIN_BPVVDCHI2"  : 100,       # Chi-squared distance with PV.
                   "MAX_CHI2"       : 8,         # Chi-squared.
                   "MAX_M"          : 7*GeV,     # Combined mass.
                   "MIN_SUM_PT"     : 2*GeV},    # Scalar sum of transverse momenta.
        # Fully reconstructed jet p roperties.
        "JET"   : {"JEC"            : False,     # If no STDJETS, apply JEC.
                   "R"              : 0.5,       # If no STDJETS, set jet radius.
                   "MIN_PT"         :  5*GeV,    # Transverse momentum.
                   "EXTLOW_PT"      : 7*GeV,    # Transverse momentum.
                   "VERYLOW_PT"     : 15*GeV,   # Transverse momentum.
                   "LOW_PT"         : 17*GeV,   # Transverse momentum.
                   "MEDIUM_PT"      : 50*GeV,   # Transverse momentum.
                   "HIGH_PT"        : 90*GeV},   # Transverse momentum.
        "DIJET" : {"MAX_COSDPHI"    : -0.8}      # Cos of transverse angle.
    },

    'STREAMS' : [ 'EW' ] ,
    'WGs'    : [ 'QEE' ]
}

##############################
#Group:Strange Baryons and Correlations.
#DST:Full
#Raw:Calo,Rich,Muon
##############################
SbarSCorrelations = {
    'BUILDERTYPE' : 'SbarSCorrelationsConf',
    'CONFIG' : { 'HLT' : "HLT_PASS('Hlt1MBNoBiasDecision')|HLT_PASS('Hlt1MBMicroBiasTStationDecision')|HLT_PASS('Hlt1MBMicroBiasVeloDecision')|HLT_PASS('Hlt1MBMicroBiasTStationRateLimitedDecision')|HLT_PASS('Hlt1MBMicroBiasVeloRateLimitedDecision')"
                ,  'LongTrackGEC'          :    1000 # 150 or 500 might be a better choice
                ,  'Trk_P_MIN'             : 5000 # to limit material interactions for Kaons
                ,  'isLong'                : '(ISLONG)'
                ,  'KAON_PIDK_MIN'         : 8 # DLL_KAON_vs_PION
                ,  'KAON_PIDKp_MIN'        : 0 # DLL_KAON_vs_PROTON
                ,  'KAON_ipChi2_MAX'                : 49 # IPCHI2 with respect to best PV (do not know if UPCHI2 or IP is used, but I guess IPCHI2 was used to tag the best PV )
#
                ,   'PION_ipChi2_MIN'       : 9
                ,   'PROTON_ipChi2_MIN'     : 9
                ,   'PION_P_MIN'            : 2000
                ,   'PROTON_P_MIN'          : 2000
#
                ,   'Fisher'                : 10
                ,   'Lambda_V_Chi2_Max'      : 9
                ,   'Lambda_Adamass'        : 50
                ,   'Lambda_ipChi2_MAX'     : 49
#
                , 'Phiprescale'             :    0.05
                , 'F2prescale'              :    1.0
                , 'LambdaCprescale'         :    1.0
                , 'postscale'             :    1.0
           },
    'WGs'         : ['QEE'],
    'STREAMS'     : [ 'EW' ]
}

StrangeBaryons= {
      'BUILDERTYPE' : 'StrippingStrangeBaryonsConf',
      'CONFIG'      : { #PID cuts
                        'PreScale'   : 1,
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
                        'Lambda0MassWindowPost'            :      6., # < 6 for all six cases


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
       'WGs'         : ['QEE'],
       'STREAMS'     : [ 'EW' ]
}


StrangeBaryonsNoPID= {
      'BUILDERTYPE' : 'StrippingStrangeBaryonsNoPIDConf',
      'CONFIG' :  { #PID cuts
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
                   'Lambda0MassWindowPost'            :   6., # < 6 for all six cases


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
      'WGs'         : ['QEE'],
      'STREAMS'     : [ 'EW' ]
}


##############################
#Group:Low multiplicity lines
#DST:Full
#Raw:Velo
##############################
LowMult = {
    'BUILDERTYPE' : 'LowMultConf',
    'CONFIG'      : {
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
    , 'LMR2HH_APTmin'        : 0.0 * MeV
    , 'LMR2HH_APTmax'        : 1500.0 * MeV
    , 'LMR2HH_AMmin'         : 450.0 * MeV
    , 'LMR2HH_AMmax'         : 1700.0 * MeV   
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
    'WGs'         : ['QEE'],
    'STREAMS'     : [ 'EW' ]
}

##############################
#Group:Electroweak lines
#DST:Full
#Raw:Muon,Tracker,Velo,Rich,Calo
##############################
DY2ee= {
    'BUILDERTYPE' : 'DY2eeConf',
    'CONFIG'      : { 'DY2eeLine3Prescale' : 1.0,
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
    'WGs'         : ['QEE'],
    'STREAMS'     : [ 'EW' ]
    }

DY2MuMu = {
    'WGs'         : ['QEE'],
    'BUILDERTYPE' : 'DY2MuMuConf',
    'CONFIG'      : { 'DY2MuMu1LinePrescale'    : 0.05,
                      'DY2MuMu1LineHltPrescale' : 0.5,
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
    'WGs'         : ['QEE'],
    'STREAMS'     : [ 'EW' ]
    }

We = {
    'BUILDERTYPE' : 'WeConf',
    'CONFIG'      : { 'We_Prescale'    : 1.0,
                      'WeLow_Prescale' : 0.1,
                      'We_Postscale'   : 1.0,
                      'PrsCalMin' : 50.,
                      'ECalMin'   :  0.10,
                      'HCalMax'   :  0.05,
                      'pT'        : 20.,
                      'pTlow'     : 15.
                    },
    'WGs'         : ['QEE'],
    'STREAMS'     : [ 'EW' ]
    }

WMu = {
    'BUILDERTYPE' : 'WMuConf',
    'CONFIG'      : { 'WMu_Prescale'    : 1.0,
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
    'WGs'         : ['QEE'],
    'STREAMS'     : [ 'EW' ]
    }

Z02ee = {
    'BUILDERTYPE' : 'Z02eeConf',
    'CONFIG'      : { 'Z02ee_Prescale'  : 1.0,
                      'Z02ee_Postscale' : 1.0,
                      'PrsCalMin'      : 50.,
                      'ECalMin'        :  0.1,
                      'HCalMax'        :  0.05,
                      'pT'             : 10.,
                      'MMmin'          : 40.
                    },
    'WGs'         : ['QEE'],
    'STREAMS'     : [ 'EW' ]
    }

Z02MuMu = {
    'BUILDERTYPE' : 'Z02MuMuConf',
    'CONFIG'      : { 'Z02MuMu_Prescale'  : 1.0,
                      'Z02MuMu_Postscale' : 1.0,
                      'pT'    : 3.,
                      'MMmin' : 40.
                    },
    'WGs'         : ['QEE'],
    'STREAMS'     : [ 'EW' ]
    }

SingleTrackTIS = {
    'BUILDERTYPE' : 'SingleTrackTISConf',
    'CONFIG'      : { 'SingleTrackTIS_Prescale'    : 0.1,
                      'SingleTrackTISLow_Prescale' : 0.01,
                      'SingleTrackTIS_Postscale'   : 1.00,
                      'pT'    : 20.,
                      'pTlow' : 15.
                    },
    'WGs'         : ['QEE'],
    'STREAMS'     : [ 'EW' ]
    }

SingleTrackTIS = {
    'BUILDERTYPE' : 'SingleTrackTISConf',
    'CONFIG'      : { 'SingleTrackTIS_Prescale'    : 0.1,
                      'SingleTrackTISLow_Prescale' : 0.01,
                      'SingleTrackTIS_Postscale'   : 1.00,
                      'pT'    : 20.,
                      'pTlow' : 15.
                    },
    'WGs'         : ['QEE'],
    'STREAMS'     : [ 'EW' ]
    }

A1MuMu = {
    'WGs'         : ['QEE'],
    'BUILDERTYPE' : 'A1MuMuConf',
    'CONFIG'      : { 'A1MuMu_LinePrescale'  : 1.0,
                      'A1MuMu_LinePostscale'              : 1.0,
                      'A1MuMu_checkPV'                    : False,
                      'DIMUON_LOW_MASS'                   : '5000.0',    # MeV/c2
                      #'DIMUON_HIGH_MASS'                 : not set,     # MeV/c2
                      'PT_MUON_MIN'                       : '2500.0',    # MeV/c
                      'P_MUON_MIN'                        : '2500.0',    # MeV/c (de facto no cut)
                      'TRACKCHI2_MUON_MAX'                : '10',        # dl
                      'PT_DIMUON_MIN'                     : '7500.0',    # MeV/c
                      'VCHI2_DIMUON_MAX'                  : '12'         # dl
                    },
    'STREAMS'     : [ 'Dimuon' ]
    }

##########
#DST:micro
##########
MuMuSS = {
    'WGs'         : ['QEE'],
    'BUILDERTYPE' : 'MuMuSSConf',
    'CONFIG'      : { 'MuMuSSLine1Prescale' : 0.1,
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
    'STREAMS'     : ['Leptonic']
    }

