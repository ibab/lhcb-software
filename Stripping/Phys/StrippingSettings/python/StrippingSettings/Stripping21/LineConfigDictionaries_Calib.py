from GaudiKernel.SystemOfUnits import MeV, GeV, cm, mm

D02KPiPi0 = {
   'BUILDERTYPE' : 'StrippingD02KPiPi0Conf',
   'CONFIG' : {     'TrackMinPT_M'         : 300       # MeV
                   ,'TrackMinPT_R'         : 600       # MeV
                   ,'TrackMinTrackProb'    : 0.000001  # unitless
                   ,'TrackMaxGhostProb'    : 0.3       # unitless
                   ,'TrackMinIPChi2'       : 16        # unitless
                   ,'Pi0MinPT_M'           : 2000      # MeV
                   ,'Pi0MinPT_R'           : 1000      # MeV
                   ,'ResPi0MinGamCL'       : 0.2       # unitless
                   ,'D0MinM'               : 1600      # MeV
                   ,'D0MaxM'               : 2100      # MeV
                   ,'D0MinVtxProb'         : 0.001     # unitless
                   ,'D0MaxIPChi2'          : 9         # unitless
                   ,'D0MinDIRA'            : 0.9999    # unitless
                   ,'D0MinVVDChi2'         : 64        # unitless
                   ,'MergedLinePrescale'   : 0.5       # unitless
                   ,'MergedLinePostscale'  : 1.        # unitless
                   ,'ResolvedLinePrescale' : 0.5       # unitless
                   ,'ResolvedLinePostscale': 1.        # unitless
                   },
   'STREAMS' : ['Calibration'],
   'WGs' : ['ALL']
}


MuIDCalib = {
    'BUILDERTYPE' : 'MuIDCalibConf',
    'WGs'         : [ 'ALL' ],
    'STREAMS'     : [ 'Calibration' ],
    'CONFIG'      : { 'PromptPrescale' : 0., #not used anymore 0.08 old prescale        
    'DetachedPrescale'         : 1.,
    'DetachedNoMIPPrescale'    : 1, #0.3,                                       
    'DetachedNoMIPHiPPrescale' : 0.,
    'DetachedNoMIPKPrescale'   : 1.,
    'FromLambdacPrescale'      : 1.,
    'KFromLambdacPrescale'     : 0.,
    'PiFromLambdacPrescale'    : 0.,
    'PFromLambdacPrescale'     : 0.,
    'KISMUONFromLambdacPrescale' : 0.,
    'PiISMUONFromLambdacPrescale': 0.,
    'PISMUONFromLambdacPrescale' : 0.
    
    }
    }

NoPIDDstarWithD02RSKPi = {
    'BUILDERTYPE' : 'NoPIDDstarWithD02RSKPiConf',
    'STREAMS'     : [ 'PID' ],
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
    , 'Prescale'         : 0.80           ## unitless
    , 'Postscale'        : 1.00           ## unitless
    ##
    , 'Monitor'          : False           ## Activate the monitoring?
    }
    }

V0ForPID = {
    'BUILDERTYPE' : 'StrippingV0ForPIDConf',
    'STREAMS'     : [ 'PID' ],
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

Jpsi2eeForElectronID = {
    'BUILDERTYPE'       : 'ElectronIDConf',
    'CONFIG'    : {
        'JpsiLinePrescale'            :   0.5   ,
        'JpsiLineHltFilter'           : None    ,

        'Both_PT'                 :  500.   ,  # MeV
        'Both_P'                  : 3000.   ,  # MeV
        'Both_TRCHI2DOF'          :    5.   ,
        'Both_MIPCHI2'            :    9.   ,
        
        'Tag_PT'                  : 1500.   ,  # MeV
        'Tag_P'                   : 6000.   ,  # MeV
        'Tag_PIDe'                :    5.   ,
        'Tag_MIPCHI2'             :    9.   ,

        'Probe_PT'                :  500.   ,  # MeV
        'Probe_P'                 : 3000.   ,  # MeV
        'Probe_MIPCHI2'           :    9.   ,
        
        'eeCombMinMass'           : 2100.   ,  # MeV         
        'eeCombMaxMass'           : 4300.   ,  # MeV   
        'eeVCHI2PDOF'             :    9.   ,  
        'eeMinMass'               : 2200.   ,  # MeV 
        'eeMaxMass'               : 4200.   ,  # MeV

        'JpsiLineCut'             : "(PT>2.*GeV) & (BPVDLS>50) ",      
        
        'Bu2JpsieeKLine_Prescale'  :  1,
        'Bu2JpsieeKLine_HltFilter' : None,
        'Bu2JpsieeKLine_KaonCut'   : "(TRCHI2DOF<4) & (PT>1.0*GeV) & (PIDK >0) & (BPVIPCHI2()>9)",
        'Bu2JpsieeKLine_JpsiCut'   : "(BPVDLS>5)",
        'Bu2JpsieeKLine_BuComCut'  : "in_range(4.1*GeV,AM,6.1*GeV)",
        'Bu2JpsieeKLine_BuMomCut'  : "in_range(4.2*GeV,M, 6.0*GeV) & (VFASPF(VCHI2PDOF)<9)"
        },
    'STREAMS' : [ 'PID' ],
    'WGs'    : ['ALL']
    }

TrackEffDownMuon = { 'NAME'        : 'TrackEffDownMuon',
    'WGs'         : ['ALL'],
    'BUILDERTYPE' : 'StrippingTrackEffDownMuonConf',
    'CONFIG'      : { 
				'MuMom':		2000.	# MeV
			,	'MuTMom':		200.	# MeV
			,	'TrChi2':		10.	# MeV
			,	'MassPreComb':		2000.	# MeV
			,	'MassPostComb':		200.	# MeV
			,	'Doca':			5.	# mm
			,	'VertChi2':		25.	# adimensional
                	,       'DataType':             '2011'  # for ConfiguredMuonIDs configuration
			,	'NominalLinePrescale':  0.2 # proposal: 0.2 to stay below 0.15% retention rate 
			,	'NominalLinePostscale': 1.
			,	'ValidationLinePrescale':0.0015 #0.5 in stripping15: 0.1 gives 1.42% retention rate , ValidationLine further prescaled
			,	'ValidationLinePostscale': 1.
			,	'HLT1TisTosSpecs': { "Hlt1TrackMuonDecision%TOS" : 0, "Hlt1SingleMuonNoIPL0Decision%TOS" : 0} #no reg. expression allowed(see selHlt1Jpsi )
			,	'HLT1PassOnAll': True
			,	'HLT2TisTosSpecs': { "Hlt2SingleMuon.*Decision%TOS" : 0} #reg. expression allowed
			,	'HLT2PassOnAll': False
                    },
    'STREAMS'     : { 'Calibration' : ['StrippingTrackEffDownMuonNominalLine','StrippingTrackEffDownMuonValidationLine']}
}
    
TrackEffVeloMuon = { 
    'BUILDERTYPE' : 'StrippingTrackEffVeloMuonConf',
    'WGs' : [ 'ALL' ],
    'STREAMS' : { 'Calibration': ['StrippingTrackEffVeloMuonLine1',
                                       'StrippingTrackEffVeloMuonLine2'] },
    'CONFIG' : {
			"TrChi2VeMu":		5.	# adimensional
		,	"TrChi2LongMu":		3.	# adimensional
                ,       "JpsiPt":               0.5     # GeV
                ,       "TrPt":                 100.    # MeV
                ,       "TrP":                  5.      # GeV
                ,       "LongP":                7.      # GeV
                ,       "MuDLL":                1.      # adimensional
                ,       "VertChi2":             2.      # adimensional
                ,       "MassPreComb":          1000.   # MeV
                ,       "MassPostComb":         400.    # MeV
                ,       "Prescale":             0.22    # adimensional
                ,       "Postscale":            1.      # adimensional
                ,       'HLT1TisTosSpecs': { "Hlt1TrackMuonDecision%TOS" : 0, "Hlt1SingleMuonNoIPDecision%TOS" : 0} #no reg. expression allowed(see selHlt1Jpsi )
                ,       'HLT1PassOnAll': True
                ,       'HLT2TisTosSpecs': { "Hlt2SingleMuon.*Decision%TOS" : 0} #reg. expression allowed
                ,       'HLT2PassOnAll': False
                        }
    }

TrackEffMuonTT = {
    'BUILDERTYPE' : 'StrippingTrackEffMuonTTConf',
    'WGs' : [ 'ALL' ],
  'CONFIG'      : {
                        'JpsiMassWin'                 : 500,
                        'UpsilonMassWin'              : 1500,
                        'ZMassWin'                    : 40000,
                        'BMassWin'                    : 500,
                        'JpsiMuonTTPT'                : 0,
                        'UpsilonMuonTTPT'             : 500,
                        'ZMuonTTPT'                   : 500,
                        'JpsiLongPT'                  : 1300,
                        'UpsilonLongPT'               : 1000,
                        'ZLongPT'                     : 10000,
                        'JpsiPT'                      : 1000,
                        'UpsilonPT'                   : 0,
                        'ZPT'                         : 0,
                        'JpsiLongMuonMinIP'           : 0.5,
                        'UpsilonLongMuonMinIP'        : 0,
                        'ZLongMuonMinIP'              : 0,
                        'JpsiMINIP'                   : 3,
                        'UpsilonMINIP'                : 10000, #this is a dummy                                   
                        'ZMINIP'                      : 10000, #this is a dummy                                   
                        'BJpsiKMINIP'                 : 10000, #this is a dummy                                   
                        'JpsiLongMuonTrackCHI2'       : 5,
                        'UpsilonLongMuonTrackCHI2'    : 5,
                        'ZLongMuonTrackCHI2'          : 5,
                        'VertexChi2'                  : 5,
                        'LongMuonPID'                 : 2,
                        'JpsiHlt1Triggers'            :  { "Hlt1TrackMuonDecision%TOS" : 0},
                        'UpsilonHlt1Triggers'         :  { "Hlt1SingleMuonHighPTDecision%TOS" : 0},
                        'ZHlt1Triggers'               :  { "Hlt1SingleMuonHighPTDecision%TOS" : 0},
                        'JpsiHlt2Triggers'            :  { "Hlt2SingleMuon.*Decision%TOS" : 0},
                        'UpsilonHlt2Triggers'         :  { "Hlt2SingleMuonLowPTDecision%TOS" : 0},
                        'ZHlt2Triggers'               :  { "Hlt2SingleMuonHighPTDecision%TOS" : 0},
                        'BJpsiKHlt2TriggersTUS'       :  { "Hlt2TopoMu2BodyBBDTDecision%TUS" : 0},
                        'BJpsiKHlt2TriggersTOS'       :  { "Hlt2TopoMu2BodyBBDTDecision%TOS" : 0},
                        'JpsiPrescale'                : 1,
                        'UpsilonPrescale'             : 1,
                        'ZPrescale'                   : 1,
                        'BJpsiKPrescale'              : 1,
                        'Postscale'                   : 1
                    },
    'STREAMS'     : { 'Calibration' : ['StrippingTrackEffMuonTT_JpsiLine1','StrippingTrackEffMuonTT_JpsiLine2','StrippingTrackEffMuonTT_UpsilonLine1','StrippingTrackEffMuonTT_UpsilonLine2','StrippingTrackEffMuonTT_ZLine1','StrippingTrackEffMuonTT_ZLine2','StrippingTrackEffMuonTT_BJpsiKLine1','StrippingTrackEffMuonTT_BJpsiKLine2']}
    }


DstarD02KShh_ForTrackingEff = {
    'BUILDERTYPE' : 'DstarD02KShh_ForTrackingEffBuilder',
    'CONFIG' : { 'LongTrackGEC'          :    150
                , 'Trk_PT_MIN'            :  600.0
                , 'Trk_P_MIN'             :10000.0
                , 'Trk_GHOST_MAX'         :    0.4
                , 'Pair_AMINDOCA_MAX'     :    0.1
                , 'Pair_BPVVD_MIN'        :    0.0
                , 'Pair_SumAPT_MIN'       : 2500.0
                , 'Pair_BPVCORRM_MAX'     : 3000.0
                , 'TrkPt_SlowPion'        :    0.0
                , 'TrkP_SlowPion'         : 3000.0
                , 'TrkChi2_SlowPion'      :    2.25
                , 'TrkChi2_MAX_Child_MAX' :    2.25
                , 'KAON_PIDK_MIN'         :    3.0
                , 'PION_PIDK_MAX'         :    0.0
                , 'IPCHI2_MAX_Child_MIN'  :   16.0
                , 'IPCHI2_PiSlow_MAX'     :    9.0
                , 'PairMaxDoca_Dstar'     :  100.0
                , 'PT_Dstar_MIN'          : 2500.0
                , 'DIRA_D0_MIN'           :    0.999
                , 'FDCHI2_D0_MIN'         :   80.0
                , 'VCHI2_D0_MAX'          :    4.0
                , 'M_MIN'                 :    0.0
                , 'M_MAX'                 : 1800.0
                , 'PhiM_MIN'              : 1000.0
                , 'PhiM_MAX'              : 1040.0
                , 'DeltaM_MIN'            :    0.0
                , 'DeltaM_MAX'            :  250.0
                , 'HLTFILTER'             : "(HLT_PASS_RE('Hlt2CharmHadD02HHXDst.*Decision')|HLT_PASS('Hlt2IncPhiDecision'))"
                , 'Hlt2TisTosSpec'        : { 'Hlt2CharmHadD02HHXDst.*Decision%TOS' : 0, 'Hlt2IncPhiDecision%TOS' : 0}
                , 'KKprescale'            :    1.0
                , 'PiPiprescale'          :    0.1
                , 'KPlusPiMinusprescale'  :    0.1
                , 'KMinusPiPlusprescale'  :    0.1
                , 'postscale'             :    1.0

    },
    'STREAMS' : [  'Calibration' ],
    'WGs' : [ 'ALL' ]
}

SigmacForPID = {
  'BUILDERTYPE' : 'SigmacForPIDConf',
  'CONFIG' :  {
     'pT(Lc+)'         :  3.0 * GeV , ## pt-cut for  prompt   Lc+
     #
     # Selection of basic particles
     #
     'TrackCuts'       : """
( TRCHI2DOF < 4     ) &
( PT > 250 * MeV    ) &
( TRGHOSTPROB < 0.5 ) & 
in_range  ( 2 , ETA , 5 )
""" ,
     'BasicCuts'       : ' & ( 9 < MIPCHI2DV() ) ' ,
     'KaonCuts'        : ' & ( 2 < PIDK  - PIDpi ) & ( -5 < PIDK  - PIDp ) ',
     'PionCuts'        : ' & ( 2 < PIDpi - PIDK  ) & ( -5 < PIDpi - PIDp  ) ',
     'SlowPionCuts'    : ' TRCHI2DOF < 5 ',
     #
     # Global Event cuts
     #
     'CheckPV'         : True ,
     #
     # Technicalities:
     #
     'Preambulo'       : [
        # shortcut for chi2 of vertex fit
        'chi2vx = VFASPF(VCHI2) ',
        # shortcut for the c*tau
        "from GaudiKernel.PhysicalConstants import c_light" ,
        ## use the embedded cut for chi2(LifetimeFit)<9
        "ctau_9   = BPVLTIME (   9 ) * c_light ", 
     ],
     ## monitoring ?
     'Monitor'         : False ,
     ## prescale
     'SigmaCPrescale'  : 1.0 ,
  },
  'STREAMS' : [ 'PID' ],
  'WGs' : [ 'ALL' ]
}


CharmFromBSemiForProtonPID = {
  'BUILDERTYPE' : 'CharmFromBSemiForProtonPIDAllLinesConf',
  'CONFIG' : {
      "prescale"              : 1 # adimensional
      ,"GEC_nLongTrk"         : 250 # adimensional
      ,"MAXGHOSTPROB"         : 0.35 # adimensional
      ,"Mu_PT"                : 1000 # MeV
      ,"Pi_PIDKMax"           : -2 # adimensional
      ,"K_PIDKMin"            : 8 # adimensional
      ,"H_PT"                 : 300 # MeV
      ,"LambdaC_MASSWIN"      : 80 # MeV
      ,"LambdaC_PTSUM"        : 2000 # MeV
      ,"LambdaC_DOCACHI2_MAX" : 6 # adimensional
      ,"LambdaC_VCHI2NDF"     : 3 # adimensional
      ,"LambdaC_FDCHI2"       : 20 # adimensional
      ,"LambdaC_DIRA"         : 0.99 # adimensional
      ,"LambdaC_DZ"           : 0.5 # mm
      ,"LambdaB_MASS_MIN"     : 3000 # MeV
      ,"LambdaB_MASS_MAX"     : 5000 # MeV
      ,"LambdaB_DOCACHI2_MAX" : 6 # adimensional
      ,"LambdaB_DIRA"         : 0.999 # adimensional
      ,"LambdaB_VCHI2NDF"     : 3 # adimentional
  },
  'STREAMS' : [ 'PID' ],
  'WGs' : [ 'ALL' ]
}
