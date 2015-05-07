"""
Line config dictionaries for Early Measurements data stripping.
"""

from GaudiKernel.SystemOfUnits import GeV





#-----#
# QEE #
#-----#

InclbJets = {
    'BUILDERTYPE' : 'InclbJetsConf',
    'WGs'         : [ 'QEE' ],
    'STREAMS'     : [ 'EW'  ],
    'CONFIG'      : {
        'NrPVsCut'      : 1,
        'NrSeedsCut'    : 2,
        'TrkChi2Cut'    : 3.0,
        'PrtMomCut'     : 2.0,
        'PrtPtCut'      : 0.6,
        'PrtIPSCut'     : 2.5,
        'DZSVPVCut'     : 1.0,
        'SumMomSVCut'   : 1.0,
        'VtxChi2Cut'    : 20.0,
        'scale'         : 0.05
    },
}

LowMultINC = {
    'BUILDERTYPE' : 'LowMultINCLines',
    'WGs'         : [ 'QEE' ],
    'STREAMS'     : [ 'BhadronCompleteEvent' ],
    'CONFIG'      : {
        'LowMultRequiredRawEvents' : None, #["Velo","Heschel"], Please we need to add this!
        #
        'PrescaleL0DiHadron'  : 0,
        'DecisionL0DiHadron'  : None,#"L0_CHANNEL('DiHadron,lowMult')",
        'PrescaleL0DiEM'      : 0,
        'DecisionL0DiEM'      : None,#"L0_CHANNEL('DiEM,lowMult')",
        'PrescaleL0Photon'    : 0,
        'DecisionL0Photon'    : None,#"L0_CHANNEL('Photon,lowMult')",
        'PrescaleL0Muon'      : 0,
        'DecisionL0Muon'      : None,#"L0_CHANNEL('Muon,lowMult')",
        'PrescaleL0DiMuon'    : 0,
        'DecisionL0DiMuon'    : None,#"L0_CHANNEL('DiMuon,lowMult')",
        'PrescaleL0Electron'  : 0,
        'DecisionL0Electron'  : None,#"L0_CHANNEL('Electron,lowMult')",
        #
        #
        #
        #
        'PrescaleLowMultL2pPi'     : 1,
        'DecisionLowMultL2pPi'     : "HLT_PASS_RE('Hlt2LowMultL2pPiDecision') | HLT_PASS_RE('Hlt2LowMultL2pPiWSDecision')",
        'PrescaleLowMultD2KPi'     : 1,
        'DecisionLowMultD2KPi'     : "HLT_PASS_RE('Hlt2LowMultD2KPiDecision') | HLT_PASS_RE('Hlt2LowMultD2KPiWSDecision')",
        'PrescaleLowMultD2KPiPi'   : 1,
        'DecisionLowMultD2KPiPi'   : "HLT_PASS_RE('Hlt2LowMultD2KPiPiDecision') | HLT_PASS_RE('Hlt2LowMultD2KPiPiWSDecision')",
        'PrescaleLowMultD2KKPi'    : 1,
        'DecisionLowMultD2KKPi'    : "HLT_PASS_RE('Hlt2LowMultD2KKPiDecision') | HLT_PASS_RE('Hlt2LowMultD2KKPiWSDecision')",
        'PrescaleLowMultD2K3Pi'    : 1,
        'DecisionLowMultD2K3Pi'    :"HLT_PASS_RE('Hlt2LowMultD2K3PiDecision') | HLT_PASS_RE('Hlt2LowMultD2K3PiWSDecision')",
        'PrescaleLowMultChiC2HH'   : 1,
        'DecisionLowMultChiC2HH'   :"HLT_PASS_RE('Hlt2LowMultChiC2HHDecision') | HLT_PASS_RE('Hlt2LowMultChiC2HHWSDecision')",
        'PrescaleLowMultChiC2HHHH' : 1,
        'DecisionLowMultChiC2HHHH' :"HLT_PASS_RE('Hlt2LowMultChiC2HHHHDecision') | HLT_PASS_RE('Hlt2LowMultChiC2HHHHWSDecision')",
        'PrescaleLowMultChiC2PP'   : 1,
        'DecisionLowMultChiC2PP'   :"HLT_PASS_RE('Hlt2LowMultChiC2PPDecision') | HLT_PASS_RE('Hlt2LowMultChiC2PPWSDecision')",
        #
        'PrescaleLowMultLMR2HH'           : 1,
        'DecisionLowMultLMR2HH'           :"HLT_PASS_RE('Hlt2LowMultLMR2HHDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHWSDecision')",
        'PrescaleLowMultLMR2HHHH'         : 1,
        'DecisionLowMultLMR2HHHH'         :"HLT_PASS_RE('Hlt2LowMultLMR2HHHHDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHHHWSDecision')",
        'PrescaleLowMultLMR2HH_mediumPS'  : 1,
        'DecisionLowMultLMR2HH_mediumPS'  :"HLT_PASS_RE('Hlt2LowMultLMR2HH_mediumPSDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHWS_mediumPSDecision')",
        'PrescaleLowMultLMR2HHHH_mediumPS': 1,
        'DecisionLowMultLMR2HHHH_mediumPS': "HLT_PASS_RE('Hlt2LowMultLMR2HHHH_mediumPSDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHHHWS_mediumPSDecision')",
        'PrescaleLowMultLMR2HH_heavyPS'   : 1,
        'DecisionLowMultLMR2HH_heavyPS'   : "HLT_PASS_RE('Hlt2LowMultLMR2HH_heavyPSDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHWS_heavyPSDecision')",
        'PrescaleLowMultLMR2HHHH_heavyPS' : 1,
        'DecisionLowMultLMR2HHHH_heavyPS' : "HLT_PASS_RE('Hlt2LowMultLMR2HHHH_heavyPSDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHHHWS_heavyPSDecision')",
        # Unbiased hadron line
        'PrescaleLowMultHadron_noTrFilt' : 1,
        'DecisionLowMultHadron_noTrFilt' : "HLT_PASS_RE('Hlt2LowMultHadron_noTrFiltDecision')",
        #
        #
        #
        'PrescaleLowMultDiMuon'             : 1,
        'DecisionLowMultDiMuon'             : "HLT_PASS('Hlt2LowMultDiMuonDecision')",
        'PrescaleLowMultDiMuon_PS'          : 1,
        'DecisionLowMultDiMuon_PS'          : "HLT_PASS('Hlt2LowMultDiMuon_PSDecision')",
        'PrescaleLowMultMuon'               : 1,
        'DecisionLowMultMuon'               : "HLT_PASS('Hlt2LowMultMuonDecision')",
        #
        'PrescaleLowMultDiElectron'         : 1,
        'DecisionLowMultDiElectron'         : "HLT_PASS_RE('Hlt2LowMultDiElectronDecision')",
        'PrescaleLowMultDiElectron_noTrFilt': 1,
        'DecisionLowMultDiElectron_noTrFilt': "HLT_PASS_RE('Hlt2LowMultDiElectron_noTrFiltDecision')",
        #
        'PrescaleLowMultDiPhoton'         : 1,
        'DecisionLowMultDiPhoton'         : "HLT_PASS_RE('Hlt2LowMultDiPhotonDecision')",
        'PrescaleLowMultDiPhoton_HighMass': 1,
        'DecisionLowMultDiPhoton_HighMass': "HLT_PASS_RE('Hlt2LowMultDiPhoton_HighMassDecision')",
        'PrescaleLowMultPi0'              : 1,
        'DecisionLowMultPi0'              : "HLT_PASS_RE('Hlt2LowMultPi0Decision')",
        #
        'PrescaleLowMultTMP'         : 0,
        'DecisionLowMultTMP'         : None,
    }
}

MuMuSS = {
    'BUILDERTYPE' : 'MuMuSSConf',
    'WGs'         : [ 'QEE' ],
    'STREAMS'     : [ 'EW'  ],
    'CONFIG'      : { 
        'MuMuSSLine1Prescale' : 0.1,
        'MuMuSSLine2Prescale' : 1.0,
        'MuMuSSLine3Prescale' : 1.0,
        'MuMuSSLine4Prescale' : 1.0,
        'MuMuSSLinePostscale' : 1.0,  
        'MuMuSSLine1MinMass'  : 3.2  * GeV,
        'MuMuSSLine1MaxMass'  :  5.  * GeV,
        'MuMuSSLine2MinMass'  :  5.  * GeV,
        'MuMuSSLine2MaxMass'  : 10.  * GeV,
        'MuMuSSLine3MinMass'  : 10.  * GeV,
        'MuMuSSLine3MaxMass'  : 20.  * GeV,
        'MuMuSSLine4MinMass'  : 20.  * GeV,
        'p'     : 10. * GeV,
        'pT1'   : 1.5 * GeV,
        'pT2'   :  3. * GeV
    },
}

SingleTrackTIS = {
    'BUILDERTYPE' : 'SingleTrackTISConf',
    'WGs'         : [ 'QEE' ],
    'STREAMS'     : [ 'EW'  ],
    'CONFIG'      : { 
        'SingleTrackTIS_Prescale'    : 0.1,
        'SingleTrackTISLow_Prescale' : 0.01,
        'SingleTrackTIS_Postscale'   : 1.00,
        'pT'    : 20. * GeV,
        'pTlow' : 15. * GeV,
    },
}


WMu = {
    'BUILDERTYPE' : 'WMuConf',
    'WGs'         : [ 'QEE' ],
    'STREAMS'     : [ 'EW'  ],
    'CONFIG'      : { 
        'WMu_Prescale'        : 1.0,
        'WMuLow_Prescale'     : 0.1,
        'STNB_Prescale'       : 0.2,
        'WMu_Postscale'       : 1.0,
        'SingMuon10_Prescale' : 0.01,
        'SingMuon48_Prescale' : 0.4,
        'pT'                  : 20. * GeV,
        'pTlow'               : 15. * GeV,
        'pTvlow'              :  5. * GeV,
        'SingMuon10_pT'       : 10. * GeV,
        'SingMuon48_pT'       : 4.8 * GeV,
    },
}


Z02MuMu = {
    'BUILDERTYPE' : 'Z02MuMuConf',
    'WGs'         : [ 'QEE' ],
    'STREAMS'     : [ 'EW'  ],
    'CONFIG'      : { 
        'Z02MuMu_Prescale'  : 1.0,
        'Z02MuMu_Postscale' : 1.0,
        'pT'    : 3.  * GeV,
        'MMmin' : 40. * GeV,
    },
}




#-------------#
# Calibration #
#-------------#

TrackEffDownMuon = {
    'NAME'        : 'TrackEffDownMuon',
    'WGs'         : ['ALL'],
    'BUILDERTYPE' : 'StrippingTrackEffDownMuonConf',
    'CONFIG'      : { 
				'MuMom':		2000.	# MeV
			,	'MuTMom':		200.	# MeV
			,	'ZMuMom':		0.	# MeV
			,	'ZMuTMom':		20000.	# MeV
			,	'UpsilonMuMom':		0.	# MeV
			,	'UpsilonMuTMom':	500.	# MeV
			,	'ZMuMinEta':		2.	# adimensional
			,	'ZMuMaxEta':		4.5	# adimensional
			,	'TrChi2':		10.	# MeV
			,	'MassPreComb':		2000.	# MeV
			,	'MassPostComb':		200.	# MeV
			,	'ZMassPreComb':		100000.	# MeV
			,	'ZMassPostComb':	1500.	# MeV
			,	'UpsilonMassPreComb':	100000.	# MeV
			,	'UpsilonMassPostComb':	00.	# MeV
			,	'Doca':			5.	# mm
			,	'VertChi2':		25.	# adimensional
			,	'SeedingMinP':		1500.	# MeV
            ,   'DataType':             '2015'  # for ConfiguredMuonIDs configuration
			,	'NominalLinePrescale':  1. # proposal: 0.2 to stay below 0.15% retention rate 
			,	'NominalLinePostscale': 1.
			,	'ValidationLinePrescale':0.0015 #0.5 in stripping15: 0.1 gives 1.42% retention rate , ValidationLine further prescaled
			,	'ValidationLinePostscale': 1.
			,	'ZLinePrescale':  1. # proposal: 0.2 to stay below 0.15% retention rate 
			,	'ZLinePostscale':  1.  
			,	'UpsilonLinePrescale':  1. # proposal: 0.2 to stay below 0.15% retention rate 
			,	'UpsilonLinePostscale':  1. 
            ,   'JpsiHlt1Filter' : 'Hlt1.*Decision'
            ,   'JpsiHlt2Filter' : 'Hlt2.*Decision'
			,	'HLT1TisTosSpecs': { "Hlt1TrackMuonDecision%TOS" : 0, "Hlt1SingleMuonNoIPL0Decision%TOS" : 0} #no reg. expression allowed(see selHlt1Jpsi )
			,	'ZHLT1TisTosSpecs': { "Hlt1SingleMuonHighPTDecision%TOS" : 0} #no reg. expression allowed(see selHlt1Jpsi )
			,	'UpsilonHLT1TisTosSpecs': { "Hlt1SingleMuonHighPTDecision%TOS" : 0} #no reg. expression allowed(see selHlt1Jpsi )
			,	'HLT1PassOnAll': True
			,	'HLT2TisTosSpecs': { "Hlt2SingleMuon.*Decision%TOS" : 0, "Hlt2TrackEffDiMuonDownstream.*Decision%TOS" : 0 } #reg. expression allowed
			,	'ZHLT2TisTosSpecs': { "Hlt2SingleMuonHighPTDecision%TOS" : 0 } #reg. expression allowed
			,	'UpsilonHLT2TisTosSpecs': { "Hlt2SingleMuonLowPTDecision%TOS" : 0} #reg. expression allowed
			,	'HLT2PassOnAll': False
                    }
    }

TrackEffMuonTT = {
    'NAME'        : 'TrackEffMuonTT',
    'WGs'         : ['ALL'],
    'BUILDERTYPE' : 'StrippingTrackEffMuonTTConf',
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
            'JpsiHlt1Filter'              : 'Hlt1.*Decision',
            'JpsiHlt2Filter'              : 'Hlt2.*Decision',
			'JpsiHlt1Triggers'            :  { "Hlt1TrackMuonDecision%TOS" : 0},
            'Hlt1PassOnAll'               : True,
			'UpsilonHlt1Triggers'         :  { "Hlt1SingleMuonHighPTDecision%TOS" : 0},
			'ZHlt1Triggers'               :  { "Hlt1SingleMuonHighPTDecision%TOS" : 0},
			'JpsiHlt2Triggers'            :  { "Hlt2SingleMuon.*Decision%TOS" : 0, "Hlt2TrackEffDiMuonMuonTT.*Decision%TOS" : 0},
			'UpsilonHlt2Triggers'         :  { "Hlt2SingleMuonLowPTDecision%TOS" : 0},
			'ZHlt2Triggers'               :  { "Hlt2SingleMuonHighPTDecision%TOS" : 0},
			'BJpsiKHlt2TriggersTUS'       :  { "Hlt2TopoMu2BodyBBDTDecision%TUS" : 0},
			'BJpsiKHlt2TriggersTOS'       :  { "Hlt2TopoMu2BodyBBDTDecision%TOS" : 0},
			'JpsiPrescale'                : 1,
			'UpsilonPrescale'             : 1,
			'ZPrescale'                   : 1,
			'BJpsiKPrescale'              : 1,
			'Postscale'                   : 1
                    }
    }

TrackEffVeloMuon = {
    'NAME'        : 'TrackEffVeloMuon',
    'WGs'         : ['ALL'],
    'BUILDERTYPE' : 'StrippingTrackEffVeloMuonConf',
    'CONFIG'      : {
			"TrChi2VeMu":		5.	# adimensional
		,	"TrChi2LongMu":		3.	# adimensional
		,	"JpsiPt":		0.5	# GeV
		,	"ZPt":			0.5	# GeV
		,	"UpsilonPt":		0.5	# GeV
		,	"TrPt":			100.	# MeV
		,	"TrP":			5.	# GeV
		,	"ZTrPt":		20000.	# MeV
		,	"ZTrP":			0.	# MeV
		,	"UpsilonTrPt":		500.	# MeV
		,	"UpsilonTrP":		0.	# MeV
		,	"LongP":		7.	# GeV
		,	"ZTrMinEta":		2.0	# adimensional
		,	"ZTrMaxEta":		4.5	# adimensional
		,	"MuDLL":		1.	# adimensional
		,	"VertChi2":		2.	# adimensional
		,	"ZVertChi2":		10000.	# adimensional, dummy
		,	"UpsilonVertChi2":	10000.	# adimensional, dummy
		,	"MassPreComb":		1000.	# MeV
		,	"ZMassPreComb":		100000. # MeV
		,	"UpsilonMassPreComb":	100000. # MeV
		,	"MassPostComb":		400.	# MeV
		,	"ZMassPostComb":	40000.	# MeV
		,	"UpsilonMassPostComb":	1500.	# MeV
		,	"Prescale":		1.	# adimensional
		,	"ZPrescale":		1.	# adimensional
		,	"UpsilonPrescale":	1.	# adimensional
		,	"Postscale":		1.	# adimensional
		,	"ZPostscale":		1.	# adimensional
		,	"UpsilonPostscale":	1.	# adimensional
        ,   'JpsiHlt1Filter' : 'Hlt1.*Decision'
        ,   'JpsiHlt2Filter' : 'Hlt2.*Decision'
		,	'HLT1TisTosSpecs'	: { "Hlt1TrackMuonDecision%TOS" : 0, "Hlt1SingleMuonNoIPDecision%TOS" : 0 } #no reg. expression allowed(see selHlt1Jpsi )
		,	'ZHLT1TisTosSpecs'	: { "Hlt1SingleMuonHighPTDecision%TOS" : 0 } #no reg. expression allowed(see selHlt1Jpsi )
		,	'UpsilonHLT1TisTosSpecs': { "Hlt1SingleMuonHighPTDecision%TOS" : 0 } #no reg. expression allowed(see selHlt1Jpsi )
		,	'HLT1PassOnAll'		: True
		,	'HLT2TisTosSpecs'	: { "Hlt2SingleMuon.*Decision%TOS" : 0, "Hlt2TrackEffDiMuonVeloMuon.*Decision%TOS" : 0 } #reg. expression allowed
		,	'ZHLT2TisTosSpecs'	: { "Hlt2SingleMuonHighPTDecision%TOS" : 0} #reg. expression allowed
		,	'UpsilonHLT2TisTosSpecs': { "Hlt2SingleMuonLowPTDecision%TOS" : 0} #reg. expression allowed
		,	'HLT2PassOnAll'		: False
         }
    }

NoPIDDstarWithD02RSKPi =  {
    'NAME'       : 'noPIDDstar',
    'WGs'        : ['ALL'],
    'BUILDERTYPE': 'NoPIDDstarWithD02RSKPiConf',
    'CONFIG'     :{
    
      'DaugPt'           : 250 * MeV      ## GeV
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
    , 'Prescale'         : 0.800          ## unitless
    , 'Postscale'        : 1.00           ## unitless
    ##
    , 'Monitor'          : False          ## Activate the monitoring?
      }
    }

PIDCalib = {
    'NAME'              : 'PIDCalib',
    'BUILDERTYPE'       : 'DiMuonConf',
    'WGs'               : [ 'ALL' ],
    'BUILDERTYPE'       : 'PIDCalibLineBuilder',
    'STREAMS'           : [ 'PID' ],
    'CONFIG'            : {
      'Jpsi2MuMu'       : { # Replaces old StrippingMuIDCalib_JpsiKFromBNoPIDNoMip
        'Prescale'      : 1.0
        , 'CheckPV'     : True
        , 'RawEvent'    : ['Muon']
        , 'MDST.DST'    : True
        , 'HLT1'        : "HLT_PASS_RE('Hlt1.*Decision')" 
        , 'HLT2'        : "HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
        , 'DecayDescriptor'    : "J/psi(1S) -> mu+ mu-"
        , 'TagAndProbeIndices' : [ 1, 2 ]
        , 'InputTES'    : [ 'Phys/StdNoPIDsMuons/Particles' ]
        , 'DaughtersCuts'       : {
            'mu+' : "(ISLONG) & (P>3*GeV) & (PT>800*MeV) & (MIPCHI2DV(PRIMARY)>10) & (TRCHI2DOF<3)"
          }
        , 'TagAndProbeCut'     :
            "(ACHILDCUT(ISMUON, #tag)) & (ACHILD(P, #tag) > 6*GeV) & (ACHILD(PT, #tag) > 1.5*GeV) & (ACHILD(MIPCHI2DV(PRIMARY), #tag) > 25)"
        , 'CombinationCut'     :
            "(in_range(3096-200, AM, 3096+200)) & (ACHI2DOCA(1,2) < 10)"
        , 'MotherCut'          :
            "(VFASPF(VCHI2)<5) & (BPVVDCHI2 > 225)"
      },


      'Bu2KMuMu'       : {
        'Prescale'      : 1.0
        , 'CheckPV'     : True
        , 'RawEvent'    : ['Muon']
        , 'MDST.DST'    : True
        , 'HLT1'        : "HLT_PASS_RE('Hlt1.*Decision')" 
        , 'HLT2'        : "HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
        , 'DecayDescriptor'    : "[B+ -> mu+ mu- K+]cc"
        , 'TagAndProbeIndices' : [ 1, 2 ]
        , 'InputTES'    : [ 'Phys/StdNoPIDsKaons/Particles', 'Phys/StdNoPIDsMuons/Particles' ]
        , 'DaughtersCuts'       : {
            'mu+' : "(MIPCHI2DV(PRIMARY)>10)",
            'K+'  : "(MIPCHI2DV(PRIMARY)>25)"
          }
        , 'TagAndProbeCut'     :
            "(ACHILDCUT(ISMUON, #tag)) & (ACHILD(P, #tag) > 6*GeV) & (ACHILD(PT, #tag) > 1.5*GeV) & (ACHILD(MIPCHI2DV(PRIMARY), #tag) > 25)"
        , 'Combination12Cut'   :
            "(in_range(3096-200, AM, 3096+200)) & (ACHI2DOCA(1,2) < 100)"
        , 'CombinationCut'     :
            "(in_range(5279-500, AM, 5279+500)) & (ACHI2DOCA(1,3) < 100) & (ACHI2DOCA(2,3) < 100)"
        , 'MotherCut'          :
#            "(VFASPF(VCHI2)<10)"
            "(VFASPF(VCHI2)<100) & (BPVVDCHI2 > 25) & (BPVIPCHI2()<25)"
      },


      'L02ppi' : {
        'Prescale'      : .14
        , 'CheckPV'     : True
        , 'RawEvent'    : ['Muon']
        , 'MDST.DST'    : False
        , 'HLT1'        : "HLT_PASS_RE('Hlt1.*Decision')" 
        , 'HLT2'        : "HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
        , 'DecayDescriptor'    : "[Lambda0 -> p+ pi-]cc"
        , 'InputTES'    : [ 'Phys/StdAllNoPIDsPions/Particles', 
                                   'Phys/StdAllNoPIDsProtons/Particles' ]
        , 'DaughtersCuts'       : {
            'p+'               : "( 2.0 * GeV < P ) &  ( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 35 )",
            'pi-'              : "( 2.0 * GeV < P ) &  ( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 35 )"
          }
        , 'CombinationCut'     :
            "(AM < 1.5 * GeV) & (ACHI2DOCA(1,2) < 20)"
        , 'MotherCut'          : (
            "   ( ADMASS ( 'Lambda0') < 25 ) " + 
            " & ( in_range ( 0 , VFASPF ( VCHI2 ) , 16 ) )" + 
            " & ( VFASPF ( VZ ) < 2200 ) "  +
            " & ( in_range ( 0 , BPVLTFITCHI2() , 49 ) )" + 
            " & ( BPVLTIME() * c_light > 5  ) " +
            " & ( ADWM( 'KS0' , WM( 'pi+' , 'pi-') ) > 20 )"
          )
      },

      'L02ppiHighPT' : {
        'CloneLine' : "L02ppi"
        , 'Prescale'  : 1.0
        , 'DaughtersCuts'       : {
            'p+'               : "(PT > 3.5*GeV) & ( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )",
            'pi-'              : "( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )"
          }
      },

      'L02ppiVeryHighPT' : {
        'CloneLine' : "L02ppi"
        , 'Prescale'  : 1.0
        , 'DaughtersCuts'       : {
            'p+'               : "(PT > 10*GeV) & ( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )",
            'pi-'              : "( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )"
          }
      },

      'L02ppiIsMuon' : {
        'CloneLine' : "L02ppi"
        , 'Prescale'  : 1.0
        , 'DaughtersCuts'       : {
            'p+'               : "(ISMUON) & ( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )",
            'pi-'              : "( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )"
          }
      },

      'L02ppiDD' : {
        'CloneLine' : "L02ppi"
        , 'Prescale'  : .14
        , 'InputTES'    : [ 'Phys/StdNoPIDsDownPions/Particles', 
                                   'Phys/StdNoPIDsDownProtons/Particles' ]
      },

      'L02ppiDDHighPT' : {
        'CloneLine' : "L02ppiHighPT"
        , 'Prescale'  : 1.0
        , 'InputTES'    : [ 'Phys/StdNoPIDsDownPions/Particles', 
                                   'Phys/StdNoPIDsDownProtons/Particles' ]
      },

      'L02ppiDDVeryHighPT' : {
        'CloneLine' : "L02ppiVeryHighPT"
        , 'Prescale'  : 1.0
        , 'InputTES'    : [ 'Phys/StdNoPIDsDownPions/Particles', 
                                   'Phys/StdNoPIDsDownProtons/Particles' ]
      },

      'Jpsi2ee'       : { 
        'Prescale'      : 1.0
        , 'CheckPV'     : True
        , 'RawEvent'    : ['Calo']
        , 'MDST.DST'    : True
        , 'HLT1'        : "HLT_PASS_RE('Hlt1.*Decision')" 
        , 'HLT2'        : "HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
        , 'DecayDescriptor'    : "J/psi(1S) -> e+ e-"
        , 'TagAndProbeIndices' : [ 1, 2 ]
        , 'InputTES'           : [ 'Phys/StdNoPIDsElectrons/Particles' ]
        , 'DaughtersCuts'       : {
            'e+' : "( P > 3*GeV ) & ( PT > 500*MeV ) & (BPVIPCHI2()> 9.0)  " 
          }
        , 'TagAndProbeCut'     :
            "(ACHILD(PIDe, #tag) > 5.0) & (ACHILD(PT, #tag) > 1500*MeV) & (ACHILD(P, #tag) > 6*GeV) "
        , 'CombinationCut'     :
            "(APT > 1.2*GeV) & (in_range(2100, AM, 4300)) & (ACHI2DOCA(1,2) < 18)"
        , 'MotherCut'          :
            "(VFASPF(VCHI2)<9) & (PT > 2*GeV) & (BPVVDCHI2 > 50)"
      },

      'Bu2Kee'       : {
        'Prescale'      : 1.0
        , 'CheckPV'     : True
        , 'RawEvent'    : ['Calo']
        , 'MDST.DST'    : True
        , 'HLT1'        : "HLT_PASS_RE('Hlt1.*Decision')" 
        , 'HLT2'        : "HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
        , 'DecayDescriptor'    : "[B+ -> e+ e- K+]cc"
        , 'TagAndProbeIndices' : [ 1, 2 ]
        , 'InputTES'           : [ 'Phys/StdTightKaons/Particles', 'Phys/StdNoPIDsElectrons/Particles' ]
        , 'DaughtersCuts'       : {
            'e+'  : "( P  > 3*GeV )   & ( PT > 500*MeV ) & (BPVIPCHI2()> 9.0) ",
            'K+'  : "( PT > 1.0*GeV ) & (BPVIPCHI2()> 9.0) & (PIDK > 0)"
          }
        , 'TagAndProbeCut'     :
            "(ACHILD(PIDe, #tag) > 5.0) & (ACHILD(PT, #tag) > 1500) & (ACHILD(P, #tag) > 6*GeV) "
        , 'Combination12Cut'   :
            "(in_range(2100, AM, 4300)) & (ACHI2DOCA(1,2) < 9)"
        , 'CombinationCut'     :
            "(in_range(5279-1200, AM, 5279+1000)) & (ACHI2DOCA(1,3) < 18) & (ACHI2DOCA(2,3) < 18)"
        , 'MotherCut'          :
            "(VFASPF(VCHI2)<18) & (BPVVDCHI2 > 25)"
      },

      'Bu2Jpsi_mumuK' :  {
        'Prescale'       : 1.0
        , 'CheckPV'      : True
        , 'RawEvent'    : ['Calo']
        , 'MDST.DST'    : True
        , 'HLT1'        : "HLT_PASS_RE('Hlt1.*Decision')" 
        , 'HLT2'        : "HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
        , 'DecayDescriptor'    : "[B+ -> J/psi(1S) K+]cc"
        , 'DecayDescriptorJpsi': "J/psi(1S) -> mu+ mu-"
        , 'InputTES'    : [ 'Phys/StdNoPIDsKaons/Particles' ]
        , 'InputTESJpsi': [ 'Phys/StdNoPIDsMuons/Particles' ]
        , 'DaughtersCuts'       : {
            'mu+' : "(BPVIPCHI2()> 10.0) ",
            'K+'  : "(MIPCHI2DV(PRIMARY)>25)"
          }
        , 'TagAndProbeCut'     :
            "(ACHILDCUT(ISMUON, #tag)) & (ACHILD(P, #tag) > 6*GeV) & (ACHILD(PT, #tag) > 1.5*GeV) & (ACHILD(MIPCHI2DV(PRIMARY), #tag) > 25)"
        , 'JpsiCombinationCut' :
            "(ADAMASS('J/psi(1S)')<200*MeV)"
        , 'JpsiMotherCut'      :
            "(VFASPF(VCHI2/VDOF)<5) & (BPVVDCHI2 > 225) "
        , 'CombinationCut'     :
            "ADAMASS('B+') < 500.*MeV"
        , 'MotherCut'          :
            "(BPVIPCHI2()<25)"
      },

      'Bu2Jpsi_eeK' :  {
        'Prescale'       : 1.0
        , 'CheckPV'      : True
        , 'RawEvent'    : ['Muon']
        , 'MDST.DST'    : True
        , 'HLT1'        : "HLT_PASS_RE('Hlt1.*Decision')" 
        , 'HLT2'        : "HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
        , 'DecayDescriptor'    : "[B+ -> J/psi(1S) K+]cc"
        , 'DecayDescriptorJpsi': "J/psi(1S) -> e+ e-"
        , 'InputTES'           : [ 'Phys/StdTightKaons/Particles' ]
        , 'InputTESJpsi'       : [ 'Phys/StdNoPIDsElectrons/Particles' ]
        , 'DaughtersCuts'       : {
            'e+'  : "( P  > 3*GeV )   & ( PT > 500*MeV ) & (BPVIPCHI2()> 9.0) ",
            'K+'  : "( PT > 1.0*GeV ) & (BPVIPCHI2()> 9.0) & ( PIDK > 0 )",
            'J/psi(1S)' : "(BPVDLS > 5)"

          }
        , 'TagAndProbeCut'     :
            "(ACHILD(PIDe, #tag) > 5.0) & (ACHILD(PT, #tag) > 1500) & (ACHILD(P, #tag) > 6*GeV) "
        , 'JpsiCombinationCut' :
            "(in_range(2100, AM, 4300))"
        , 'JpsiMotherCut'      :
            "(VFASPF(VCHI2)< 9.0) & (in_range(2200.0 *MeV, MM, 4200.0 *MeV)) "
        , 'CombinationCut'     :
            "in_range(4.1*GeV,AM,6.1*GeV)"
        , 'MotherCut'          :
            "(VFASPF(VCHI2PDOF)<9) & in_range(4.2*GeV,M, 6.0*GeV)"
      },


      'Ks02pipi'      :{
        'Prescale'      : 0.024
        , 'CheckPV'     : True
        , 'RawEvent'    : ['Muon']
        , 'MDST.DST'    : False
        , 'HLT1'        : "HLT_PASS_RE('Hlt1.*Decision')" 
        , 'HLT2'        : "HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
        , 'DecayDescriptor'       : "KS0 -> pi+ pi-"
        , 'InputTES'              : ['Phys/StdNoPIDsPions/Particles']
        , 'DaughtersCuts'         : {
            'pi+' : " ( 2.0 * GeV < P ) & ( MIPCHI2DV(PRIMARY) > 25 )"
          }
        , 'CombinationCut'        : " ( ADAMASS('KS0') < 100 ) & (ACHI2DOCA(1,2) < 20 )"
        , 'MotherCut'             : (
            "( ADMASS ( 'KS0') < 50 ) & "+
            " in_range ( 0 , VFASPF ( VCHI2 ) , 16 ) & " + 
            " ( VFASPF ( VZ ) < 2200 ) & " + 
            " (BPVVDCHI2 > 25) & " +
            " ( ADWM( 'Lambda0' , WM( 'p+' , 'pi-') ) > 9 ) & " + 
            " ( ADWM( 'Lambda0' , WM( 'pi+' , 'p~-') ) > 9 )"
          )
      },
    
      'Ks02pipiDD' : {
        'CloneLine'    : 'Ks02pipi'
        , 'Prescale'   : 0.024
        , 'CheckPV'     : True
        , 'RawEvent'    : ['Muon']
        , 'MDST.DST'    : False
        , 'HLT1'        : "HLT_PASS_RE('Hlt1.*Decision')" 
        , 'HLT2'        : "HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
        , 'InputTES'   : ['Phys/StdNoPIDsDownPions/Particles']
        , 'MotherCut'             : (
            "( ADMASS ( 'KS0') < 50 ) & "+
            " in_range ( 0 , VFASPF ( VCHI2 ) , 16 ) & " + 
            " ( VFASPF ( VZ ) < 2200 ) & " + 
            " (BPVVDCHI2 > 25) & " +
            " ( ADWM( 'Lambda0' , WM( 'p+' , 'pi-') ) > 18 ) & " + 
            " ( ADWM( 'Lambda0' , WM( 'pi+' , 'p~-') ) > 18 )"
        )
      },
    
      'Lc2pKpi' : {
        'Prescale':   1.0
        , 'CheckPV'     : True
        , 'RawEvent'    : ['Muon']
        , 'MDST.DST'    : False
        , 'HLT1'        : "HLT_PASS_RE('Hlt1.*Decision')" 
        , 'HLT2'        : "HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
        , 'InputTES' : [  'Phys/StdNoPIDsKaons/Particles' , 
                          'Phys/StdNoPIDsPions/Particles' , 
                          'Phys/StdNoPIDsProtons/Particles' 
                       ]
        , 'DecayDescriptor' : "[Lambda_c+ -> K- p+ pi+]cc"
        , 'DaughtersCuts' : {
          'K+'    : '( PT>250*MeV ) & ( P>2*GeV ) & ( TRPCHI2>0.0001 ) & ( MIPCHI2DV(PRIMARY)>8. ) ' 
          , 'p+'  : '( PT>250*MeV ) & ( P>2*GeV ) & ( TRPCHI2>0.0001 ) & ( MIPCHI2DV(PRIMARY)>8. ) ' 
          , 'pi+' : '( PT>250*MeV ) & ( P>2*GeV ) & ( TRPCHI2>0.0001 ) & ( MIPCHI2DV(PRIMARY)>8. ) '
          }
        , 'Combination12Cut' : "( ACHI2DOCA(1,2) < 25 )"
        , 'CombinationCut'   : "( ADAMASS('Lambda_c+')<150*MeV ) & ( APT>1.*GeV ) & ( ADOCACHI2CUT(50, '') )"
        , 'MotherCut'        : (
            "( M > 2.240*GeV ) & " +
            " ( M<2.330*GeV ) & " +
            " ( VFASPF(VCHI2/VDOF)<8 ) & " +
            " ( BPVDIRA>0.99999 ) & " +
            " ( MIPCHI2DV(PRIMARY)<4. ) & " +
            " in_range( 0.85*GeV, M13, 0.95*GeV ) & " +
            " ( (WM( 'K-' , 'pi+' , 'pi+' )>1.89*GeV) | (WM( 'K-' , 'pi+' , 'pi+' )<1.80*GeV) )"
        )
    }
  }
}
