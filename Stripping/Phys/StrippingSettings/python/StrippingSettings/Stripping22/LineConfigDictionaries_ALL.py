"""
Line config dictionaries for Early Measurements data stripping.
"""

from GaudiKernel.SystemOfUnits import *




#-----#
# QEE #
#-----#

InclbJets = {
    'BUILDERTYPE' : 'InclbJetsConf',
    'WGs'         : [ 'QEE' ],
    'STREAMS'     : [ 'ALL'  ],
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
    'STREAMS'     : [ 'ALL' ],
    'CONFIG'      : {
        'LowMultRequiredRawEvents' : ["Velo","Calo","Rich","HC"],
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
    'STREAMS'     : [ 'ALL'  ],
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
    'STREAMS'     : [ 'ALL'  ],
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
    'STREAMS'     : [ 'ALL'  ],
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
    'STREAMS'     : [ 'ALL'  ],
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
    'STREAMS'           : [ 'ALLTURBO' ],
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
			,	'HLT2TisTosSpecs': { "Hlt2SingleMuon.*Decision%TOS" : 0, "Hlt2TrackEffDiMuonDownstream.*Decision%TUS" : 0 } #reg. expression allowed
			,	'ZHLT2TisTosSpecs': { "Hlt2SingleMuonHighPTDecision%TOS" : 0 } #reg. expression allowed
			,	'UpsilonHLT2TisTosSpecs': { "Hlt2SingleMuonLowPTDecision%TOS" : 0} #reg. expression allowed
			,	'HLT2PassOnAll': False
                    }
    }


#-------------#
# Calibration #
#-------------#
TrackEffDownMuonFS = {
    'NAME'        : 'TrackEffDownMuonFS',
    'WGs'         : ['ALL'],
    'BUILDERTYPE' : 'StrippingTrackEffDownMuonConf',
    'STREAMS'           : [ 'ALL' ],
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
			,	'NominalLinePrescale':  0.2 # proposal: 0.2 to stay below 0.15% retention rate 
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
			,	'HLT2TisTosSpecs': { "Hlt2SingleMuon.*Decision%TOS" : 0, "Hlt2TrackEffDiMuonDownstream.*Decision%TUS" : 0 } #reg. expression allowed
			,	'ZHLT2TisTosSpecs': { "Hlt2SingleMuonHighPTDecision%TOS" : 0 } #reg. expression allowed
			,	'UpsilonHLT2TisTosSpecs': { "Hlt2SingleMuonLowPTDecision%TOS" : 0} #reg. expression allowed
			,	'HLT2PassOnAll': False
                    }
    }

TrackEffMuonTT = {
    'NAME'        : 'TrackEffMuonTT',
    'WGs'         : ['ALL'],
    'STREAMS'           : [ 'ALL','ALLTURBO' ],
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
			'JpsiHlt2Triggers'            :  { "Hlt2SingleMuon.*Decision%TOS" : 0, "Hlt2TrackEffDiMuonMuonTT.*Decision%TUS" : 0},
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
    'STREAMS'           : [ 'ALL','ALLTURBO' ],
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
		,	'HLT2TisTosSpecs'	: { "Hlt2SingleMuon.*Decision%TOS" : 0, "Hlt2TrackEffDiMuonVeloMuon.*Decision%TUS" : 0 } #reg. expression allowed
		,	'ZHLT2TisTosSpecs'	: { "Hlt2SingleMuonHighPTDecision%TOS" : 0} #reg. expression allowed
		,	'UpsilonHLT2TisTosSpecs': { "Hlt2SingleMuonLowPTDecision%TOS" : 0} #reg. expression allowed
		,	'HLT2PassOnAll'		: False
         }
    }

NoPIDDstarWithD02RSKPi =  {
    'NAME'       : 'noPIDDstar',
    'WGs'        : ['ALL'],
    'STREAMS'           : [ 'ALL','ALLTURBO' ],
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
    , 'Prescale'         : 1.000          ## unitless
    , 'Postscale'        : 1.00           ## unitless
    ##
    , 'Monitor'          : False          ## Activate the monitoring?
      }
    }

PIDCalib = {
    'NAME'              : 'PIDCalib',
    'WGs'               : [ 'ALL' ],
    'BUILDERTYPE'       : 'PIDCalibLineBuilder',
    'STREAMS'           : [ 'ALL','ALLTURBO' ],
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
        'Prescale'      : 0.0
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
        'Prescale'      : 1.0
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
        , 'Prescale'  : 0.14
        , 'InputTES'    : [ 'Phys/StdNoPIDsDownPions/Particles', 
                                   'Phys/StdNoPIDsDownProtons/Particles' ]
      },

      'L02ppiDDHighPT' : {
        'CloneLine' : "L02ppiHighPT"
        , 'Prescale'  : 0.0
        , 'InputTES'    : [ 'Phys/StdNoPIDsDownPions/Particles', 
                                   'Phys/StdNoPIDsDownProtons/Particles' ]
      },

      'L02ppiDDVeryHighPT' : {
        'CloneLine' : "L02ppiVeryHighPT"
        , 'Prescale'  : 0.0
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
        'Prescale'      : 0.0
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
        'Prescale'      : 0.05
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
        , 'Prescale'   : 0.005
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


##### BandQ

DiMuonForXsection = {
    'BUILDERTYPE' : 'DiMuonForXsectionConf',
    'CONFIG' : {
    'TotalCuts'  :  """
    (MINTREE('mu+'==ABSID,PT)>0.65*GeV)
    & (VFASPF(VCHI2PDOF)<25)
    & ((ADMASS('J/psi(1S)')<150*MeV) | (ADMASS('psi(2S)')<150*MeV) | (MM>8.5*GeV))
    """,
    'Prescale'   :  1.,
    'CheckPV'    :  False
    },
    'STREAMS' : [ 'ALL','ALLTURBO' ] ,
    'WGs'    : [ 'BandQ' ]
    }

B2JpsiXforEM = {
    'BUILDERTYPE'       : 'B2JpsiXforEM_sConf',
    'CONFIG'    : {'ReFitPVsFlag'            :  False
                   ,       'TRCHI2DOF'                 :       5
                   ,       'BPVLTIME'                  :       0.2
                   ,       'JpsiMassWindow'            :       80
                   ,       'DaughterPT'                :       1000
                   ,       'VCHI2PDOF'                 :       10
                   ,       'BuVCHI2PDOF'               :       10.0 #ok
                   ,       'BdVCHI2PDOF'               :       10.0 #ok
                   ,       'BsVCHI2PDOF'               :       10.0 #ok
                   ,       'MinBsMass'                 :       5150.0
                   ,       'MaxBsMass'                 :       5550.0
                   ,       'MinBuMass'                 :       5150.0 #ok
                   ,       'MaxBuMass'                 :       5550.0 #ok
                   ,       'MinBdMass'                 :       5150.0
                   ,       'MaxBdMass'                 :       5550.0
                   ,       'CombMinBuMass'                 :       5050.0 #ok
                   ,       'CombMaxBuMass'                 :       5650.0 #ok
                   ,       'CombMinBdMass'                 :       5050.0
                   ,       'CombMaxBdMass'                 :       5650.0
                   ,       'CombMinBsMass'                 :       5050.0
                   ,       'CombMaxBsMass'                 :       5650.0
                   ,       'PIDKaon'                       : -2
                   , 'MinPhiMass'                  : 980.0
                   , 'MaxPhiMass'                  : 1050.0
                   , 'phiPT'                       : 500.0
                   , 'phiVCHI2'                    : 16.0
                   , 'MinKstarMass'                : 826.0
                   , 'MaxKstarMass'                : 966.0
                   , 'KstarPT'                     : 500.0
                   , 'KstarVCHI2'                  : 16.0
                   , 'PTKaon'                      : 500.     
                   },
    'STREAMS' : {'ALL' : ['StrippingB2JpsiXforEMBu2JpsiKDetachedLine',
                             'StrippingB2JpsiXforEMBd2JpsiKstarDetachedLine',
                             'StrippingB2JpsiXforEMBs2JpsiPhiDetachedLine'
                             ]},
    'WGs'               : ['BandQ']
    
    }


B2JpsiXforEMPVR = {
    'BUILDERTYPE'       : 'B2JpsiXforEM_sConf',
    'CONFIG'    : {'ReFitPVsFlag'            :  True 
                   ,       'TRCHI2DOF'                 :       5
                   ,       'BPVLTIME'                  :       0.2
                   ,       'JpsiMassWindow'            :       80
                   ,       'DaughterPT'                :       1000
                   ,       'VCHI2PDOF'                 :       10
                   ,       'BuVCHI2PDOF'               :       10.0 #ok
                   ,       'BdVCHI2PDOF'               :       10.0 #ok
                   ,       'BsVCHI2PDOF'               :       10.0 #ok
                   ,       'MinBsMass'                 :       5150.0
                   ,       'MaxBsMass'                 :       5550.0
                   ,       'MinBuMass'                 :       5150.0 #ok
                   ,       'MaxBuMass'                 :       5550.0 #ok
                   ,       'MinBdMass'                 :       5150.0
                   ,       'MaxBdMass'                 :       5550.0
                   ,       'CombMinBuMass'                 :       5050.0 #ok
                   ,       'CombMaxBuMass'                 :       5650.0 #ok
                   ,       'CombMinBdMass'                 :       5050.0
                   ,       'CombMaxBdMass'                 :       5650.0
                   ,       'CombMinBsMass'                 :       5050.0
                   ,       'CombMaxBsMass'                 :       5650.0
                   ,       'PIDKaon'                       : -2
                   , 'MinPhiMass'                  : 980.0
                   , 'MaxPhiMass'                  : 1050.0
                   , 'phiPT'                       : 500.0
                   , 'phiVCHI2'                    : 16.0
                   , 'MinKstarMass'                : 826.0
                   , 'MaxKstarMass'                : 966.0
                   , 'KstarPT'                     : 500.0
                   , 'KstarVCHI2'                  : 16.0
                   , 'PTKaon'                      : 500.     
                   },
    'STREAMS' : {'ALL' : ['StrippingB2JpsiXforEMPVRBu2JpsiKDetachedLine',
                             'StrippingB2JpsiXforEMPVRBd2JpsiKstarDetachedLine',
                             'StrippingB2JpsiXforEMPVRBs2JpsiPhiDetachedLine'
                             ]},
    'WGs'               : ['BandQ']
    
    }

#-------#
# Charm #
#-------#

D02HHForXSec = {
    'NAME': 'D02HHForXSec',
    'WGs': ['Charm'],
    'BUILDERTYPE': 'StrippingD02HHForXSecConf',
    'STREAMS': ['ALLTURBO'],
    'CONFIG': {
        # Minimum transverse momentum all D0 daughters must satisfy
        'Daug_All_PT_MIN': 250.0*MeV,
        # Minimum best primary vertex IP chi^2 all D0 daughters must satisfy
        'Daug_All_BPVIPCHI2_MIN': 16.0,
        # Minimum D0 daughter momentum
        'Daug_P_MIN': 3.0*GeV,
        # Maximum D0 daughter momentum
        'Daug_P_MAX': 100.0*GeV,
        # Minimum D0 daughter pseudorapidity
        'Daug_ETA_MIN': 2.0,
        # Maximum D0 daughter pseudorapidity
        'Daug_ETA_MAX': 5.0,
        # Minimum D0 daughter kaon DLLK
        'K_PIDK_MIN': 5.0,
        # Maximum D0 daughter pion DLLK
        'Pi_PIDK_MAX': 3.0,
        # D0 mass window around the nominal D0 mass before the vertex fit
        'D0_ADAMASS_WIN': 80.0*MeV,
        # Maximum D0 vertex chi^2 per vertex fit DoF
        'D0_VCHI2VDOF_MAX': 10.0,
        # Maximum angle between D0 momentum and D0 direction of flight
        'D0_acosBPVDIRA_MAX': 17.0*mrad,
        # Primary vertex displacement requirement, that the D0 is some sigma
        # away from the PV
        'D0_PVDispCut': '(BPVVDCHI2 > 49.0)',
        # Maximum delta mass value m(D*{0,+}) - m(D0)
        'Dstar_AMDiff_MAX': 160.0*MeV,
        # Maximum D*+ vertex chi^2 per vertex DoF (_not_ applied to D*0)
        'Dstar_VCHI2VDOF_MAX': 25.0,
        # Minimum D0 MVA discriminant value
        'D0_MVA_MIN': -0.3,
        # Path to the D0 MVA weights file
        # BDT is not applied if this is the empty string or None
        'D0_MVA_Weights': None,
        # Dictionary of LoKi functors defining the D0 MVA input variables
        # The keys must match those used when training the MVA
        'D0_MVA_Variables': {
            'D0_Loki_LV02': 'LV02',
            'D0_Loki_BPVLTIME': "BPVLTIME()",
            'D0_ENDVERTEX_CHI2': 'VFASPF(VCHI2/VDOF)',
            'D0_h1_PT': 'CHILD(PT, 1)',
            'D0_h2_PT': 'CHILD(PT, 2)',
            'D0_h1_ETA': 'CHILD(ETA, 1)',
            'D0_h2_ETA': 'CHILD(ETA, 2)'
        },
        # HLT filters, only process events firing triggers matching the RegEx
        'Hlt1Filter': None,
        'Hlt2Filter': None,
        # Fraction of candidates to randomly throw away before stripping
        'PrescaleD02HH': 1.0,
        'PrescaleDstar2D0Pi_D02HH': 1.0,
        'PrescaleDstar2D0Pi0Merged_D02HH': 1.0,
        'PrescaleDstar2D0Pi0Resolved_D02HH': 1.0,
        # Fraction of candidates to randomly throw after before stripping
        'PostscaleD02HH': 1.0,
        'PostscaleDstar2D0Pi_D02HH': 1.0,
        'PostscaleDstar2D0Pi0Merged_D02HH': 1.0,
        'PostscaleDstar2D0Pi0Resolved_D02HH': 1.0
    }
}

D02K3PiForXSec = {
    'NAME': 'D02K3PiForXSec',
    'WGs': ['Charm'],
    'BUILDERTYPE': 'StrippingD02K3PiForXSecConf',
    'STREAMS': ['ALLTURBO'],
    'CONFIG': {
        # Minimum transverse momentum all D0 daughters must satisfy
        'Daug_All_PT_MIN': 200.0*MeV,
        # Minimum transverse momentum at least 3 D0 daughters must satisfy
        'Daug_3of4_PT_MIN': 300.0*MeV,
        # Minimum transverse momentum at least 2 D0 daughters must satisfy
        'Daug_2of4_PT_MIN': 350.0*MeV,
        # Minimum transverse momentum at least 1 D0 daughter must satisfy
        'Daug_1of4_PT_MIN': 400.0*MeV,
        # Minimum best primary vertex IP chi^2 all D0 daughters must satisfy
        'Daug_All_BPVIPCHI2_MIN': 4.0,
        # Minimum best PV IP chi^2 at least 3 D0 daughters must satisfy
        'Daug_3of4_BPVIPCHI2_MIN': 4.0,
        # Minimum best PV IP chi^2 at least 2 D0 daughters must satisfy
        'Daug_2of4_BPVIPCHI2_MIN': 4.0,
        # Minimum best PV IP chi^2 at least 1 D0 daughter must satisfy
        'Daug_1of4_BPVIPCHI2_MIN': 4.0,
        # Minimum D0 daughter momentum
        'Daug_P_MIN': 3.0*GeV,
        # Maximum D0 daughter momentum
        'Daug_P_MAX': 100.0*GeV,
        # Minimum D0 daughter pseudorapidity
        'Daug_ETA_MIN': 2.0,
        # Maximum D0 daughter pseudorapidity
        'Daug_ETA_MAX': 5.0,
        # Minimum D0 daughter kaon DLLK
        'K_PIDK_MIN': 5.0,
        # Maximum D0 daughter pion DLLK
        'Pi_PIDK_MAX': 3.0,
        # D0 mass window around the nominal D0 mass before the vertex fit
        'Comb_ADAMASS_WIN': 80.0*MeV,
        # Maximum distance of closest approach of D0 daughters
        'Comb_ADOCAMAX_MAX': 0.1*mm,
        # Maximum D0 vertex chi^2 per vertex fit DoF
        'D0_VCHI2VDOF_MAX': 10.0,
        # Maximum angle between D0 momentum and D0 direction of flight
        'D0_acosBPVDIRA_MAX': 17.0*mrad,
        # Primary vertex displacement requirement, either that the D0 is some
        # sigma away from the PV, or it has a minimum flight time
        'D0_PVDispCut': '((BPVVDCHI2 > 16.0)&(BPVLTIME() > 0.100*picosecond))',
        # Maximum delta mass value m(D*{0,+}) - m(D0)
        'Dstar_AMDiff_MAX': 160.0*MeV,
        # Maximum D*+ vertex chi^2 per vertex DoF (_not_ applied to D*0)
        'Dstar_VCHI2VDOF_MAX': 25.0,
        # Minimum D0 MVA discriminant value
        'D0_MVA_MIN': -0.3,
        # Path to the D0 MVA weights file
        # BDT is not applied if this is the empty string or None
        'D0_MVA_Weights': None,
        # Dictionary of LoKi functors defining the D0 MVA input variables
        # The keys must match those used when training the MVA
        'D0_MVA_Variables': {
            'D0_Loki_LV02': 'LV02',
            'D0_Loki_BPVLTIME': "BPVLTIME()",
            'D0_Loki_DOCAMAX': "DOCAMAX",
            'D0_DIRA_OWNPV': 'BPVDIRA',
            'D0_ENDVERTEX_CHI2': 'VFASPF(VCHI2/VDOF)',
            'D0_h1_PT': 'CHILD(PT, 1)',
            'D0_h2_PT': 'CHILD(PT, 2)',
            'D0_h3_PT': 'CHILD(PT, 3)',
            'D0_h4_PT': 'CHILD(PT, 4)',
            'D0_h1_ETA': 'CHILD(ETA, 1)',
            'D0_h2_ETA': 'CHILD(ETA, 2)',
            'D0_h3_ETA': 'CHILD(ETA, 3)',
            'D0_h4_ETA': 'CHILD(ETA, 4)'
        },
        # HLT filters, only process events firing triggers matching the RegEx
        'Hlt1Filter': None,
        'Hlt2Filter': None,
        # Fraction of candidates to randomly throw away before stripping
        'PrescaleD02K3Pi': 1.0,
        'PrescaleDstar2D0Pi_D02K3Pi': 1.0,
        'PrescaleDstar2D0Pi0Merged_D02K3Pi': 1.0,
        'PrescaleDstar2D0Pi0Resolved_D02K3Pi': 1.0,
        # Fraction of candidates to randomly throw after before stripping
        'PostscaleD02K3Pi': 1.0,
        'PostscaleDstar2D0Pi_D02K3Pi': 1.0,
        'PostscaleDstar2D0Pi0Merged_D02K3Pi': 1.0,
        'PostscaleDstar2D0Pi0Resolved_D02K3Pi': 1.0
    }
}

D2HHHForXSec = {
    'NAME': 'D2HHHForXSec',
    'WGs': ['Charm'],
    'BUILDERTYPE': 'StrippingD2HHHForXSecConf',
    'STREAMS': ['ALLTURBO'],
    'CONFIG': {
        # Minimum transverse momentum all D daughters must satisfy
        'Daug_All_PT_MIN': 200.0*MeV,
        # Minimum transverse momentum at least 2 D daughters must satisfy
        'Daug_2of3_PT_MIN': 400.0*MeV,
        # Minimum transverse momentum at least 1 D daughter must satisfy
        'Daug_1of3_PT_MIN': 1000.0*MeV,
        # Minimum best primary vertex IP chi^2 all D daughters must satisfy
        'Daug_All_BPVIPCHI2_MIN': 4.0,
        # Minimum best PV IP chi^2 at least 2 D daughters must satisfy
        'Daug_2of3_BPVIPCHI2_MIN': 10.0,
        # Minimum best PV IP chi^2 at least 1 D daughter must satisfy
        'Daug_1of3_BPVIPCHI2_MIN': 50.0,
        # Minimum D daughter momentum
        'Daug_P_MIN': 3.0*GeV,
        # Maximum D daughter momentum
        'Daug_P_MAX': 100.0*GeV,
        # Minimum D daughter pseudorapidity
        'Daug_ETA_MIN': 2.0,
        # Maximum D daughter pseudorapidity
        'Daug_ETA_MAX': 5.0,
        # Minimum D daughter kaon DLLK
        'K_PIDK_MIN': 5.0,
        # Maximum D daughter pion DLLK
        'Pi_PIDK_MAX': 3.0,
        # D+ mass window around the nominal D+ mass after the vertex fit
        # Lower bound of this, -10 MeV, is used before the hhh combination
        # vertex fit
        'Dp_ADAMASS_WIN': 80.0*MeV,
        # D_s+ mass window around the nominal D+ mass after the vertex fit
        # Upper bound of this, +10 MeV, is used before the hhh combination
        # vertex fit
        'Ds_ADAMASS_WIN': 80.0*MeV,
        # Maximum distance of closest approach of D daughters
        'Comb_ADOCAMAX_MAX': 0.5*mm,
        # Maximum D vertex chi^2 per vertex fit DoF
        'D_VCHI2VDOF_MAX': 25.0,
        # Maximum angle between D momentum and D direction of flight
        'D_acosBPVDIRA_MAX': 34.0*mrad,
        # Primary vertex displacement requirement, that the D is some sigma
        # away from the PV and it has a minimum flight time
        'D_PVDispCut': '((BPVVDCHI2 > 16.0)&(BPVLTIME() > 0.150*picosecond))',
        # Minimum D+ and D_s+ MVA discriminant value
        'Dp_MVA_MIN': -0.3,
        'Ds_MVA_MIN': -0.3,
        # Path to the D+ and D_s+ MVA weights files
        # BDT is not applied if this is the empty string or None
        'Dp_MVA_Weights': None,
        'Ds_MVA_Weights': None,
        # Dictionary of LoKi functors defining the D MVA input variables
        # The keys must match those used when training the MVA
        # Same input variables are used for both D+ and D_s+
        'D_MVA_Variables': {
            # Largest D daughter PT
            'ROOTex::Leading(Dp_h1_PT,Dp_h2_PT,Dp_h3_PT)': (
                "MAXTREE(ISBASIC & HASTRACK, PT)"
            ),
            # Smallest D daughter PT
            'ROOTex::ThirdLeading(Dp_h1_PT,Dp_h2_PT,Dp_h3_PT)': (
                "MINTREE(ISBASIC & HASTRACK, PT)"
            ),
            'Dp_DIRA_OWNPV': 'BPVDIRA',
            'TMath::Log10(TMath::Abs(Dp_Loki_BPVVDCHI2))': (
                'log10(abs(BPVVDCHI2))'
            ),
            'TMath::Log10(TMath::Abs(Dp_Loki_BPVLTIME))': (
                "log10(abs(BPVLTIME()))"
            ),
            'TMath::Log10(Dp_ENDVERTEX_CHI2/Dp_ENDVERTEX_NDOF)': (
                'log10(VFASPF(VCHI2/VDOF))'
            )
        },
        # HLT filters, only process events firing triggers matching the RegEx
        'Hlt1Filter': None,
        'Hlt2Filter': None,
        # Fraction of candidates to randomly throw away before stripping
        'PrescaleDp2KPP': 1.0,
        'PrescaleDp2KKP': 1.0,
        'PrescaleDp2KKK': -1.0,
        'PrescaleDp2PPP': 1.0,
        'PrescaleDp2KPPDCS': -1.0,
        'PrescaleDs2KPP': 1.0,
        'PrescaleDs2KKP': 1.0,
        'PrescaleDs2KKK': -1.0,
        'PrescaleDs2PPP': 1.0,
        'PrescaleDs2KPPDCS': 1.0,
        # Fraction of candidates to randomly throw after before stripping
        'PostscaleDp2KPP': 1.0,
        'PostscaleDp2KKP': 1.0,
        'PostscaleDp2KKK': -1.0,
        'PostscaleDp2PPP': 1.0,
        'PostscaleDp2KPPDCS': -1.0,
        'PostscaleDs2KPP': 1.0,
        'PostscaleDs2KKP': 1.0,
        'PostscaleDs2KKK': -1.0,
        'PostscaleDs2PPP': 1.0,
        'PostscaleDs2KPPDCS': 1.0
    }
}

D2PhiPiForXSec = {
    'NAME': 'D2PhiPiForXSec',
    'WGs': ['Charm'],
    'BUILDERTYPE': 'StrippingD2PhiPiForXSecConf',
    'STREAMS': ['ALLTURBO'],
    'CONFIG': {
        # Minimum transverse momentum all D_s+ and phi daughters must satisfy
        'Daug_All_PT_MIN': 200.0*MeV,
        # Minimum best primary vertex IP chi^2 all D_s+ and phi daughters must
        # satisfy
        'Daug_All_BPVIPCHI2_MIN': 4.0,
        # Minimum phi daughter momentum
        'Daug_P_MIN': 3.0*GeV,
        # Maximum phi daughter momentum
        'Daug_P_MAX': 100.0*GeV,
        # Minimum phi daughter pseudorapidity
        'Daug_ETA_MIN': 2.0,
        # Maximum phi daughter pseudorapidity
        'Daug_ETA_MAX': 5.0,
        # Minimum phi daughter kaon DLLK
        'K_PIDK_MIN': 5.0,
        # Maximum D_s+ daughter pion DLLK
        'Pi_PIDK_MAX': 3.0,
        # Phi mass window around the nominal phi mass before the vertex fit
        'Comb_Phi_ADAMASS_WIN': 20*MeV,
        # D+ mass window around the nominal D+ mass after the vertex fit
        # Lower bound of this, -10 MeV, is used before the hhh combination
        # vertex fit
        'Dp_ADAMASS_WIN': 80.0*MeV,
        # D_s+ mass window around the nominal D+ mass after the vertex fit
        # Upper bound of this, +10 MeV, is used before the hhh combination
        # vertex fit
        'Ds_ADAMASS_WIN': 80.0*MeV,
        # Maximum D+ vertex chi^2 per vertex fit DoF
        'D_VCHI2VDOF_MAX': 25.0,
        # Maximum angle between D+ momentum and D+ direction of flight
        'D_acosBPVDIRA_MAX': 35.0*mrad,
        # Primary vertex displacement requirement, either that the D+ is some
        # sigma away from the PV, or it has a minimum flight time
        'D_PVDispCut': '((BPVVDCHI2 > 16.0)&(BPVLTIME() > 0.150*picosecond))',
        # Minimum photon confidence level
        'Photon_CL_MIN': 0.25,
        # Maximum delta mass value m(D*_s+) - m(D_s+) (PDG dm: 143 MeV)
        'Dsstar_AMDiff_MAX': 170*MeV,
        # Minimum D+ and D_s+ MVA discriminant value
        'Dp_MVA_MIN': -0.3,
        'Ds_MVA_MIN': -0.3,
        # Path to the D+ and D_s+ MVA weights files
        # BDT is not applied if this is the empty string or None
        'Dp_MVA_Weights': None,
        'Ds_MVA_Weights': None,
        # Dictionary of LoKi functors defining the D+ MVA input variables
        # The keys must match those used when training the MVA
        # Same input variables are used for both D+ and D_s+
        'D_MVA_Variables': {
            # Largest D+ and phi daughter PT
            'ROOTex::Leading(phi_h1_PT,phi_h2_PT,Ds_pi_PT)': (
                "MAXTREE(ISBASIC & HASTRACK, PT)"
            ),
            # Smallest D+ and phi daughter PT
            'ROOTex::ThirdLeading(phi_h1_PT,phi_h2_PT,Ds_pi_PT)': (
                "MINTREE(ISBASIC & HASTRACK, PT)"
            ),
            'Ds_DIRA_OWNPV': 'BPVDIRA',
            'Ds_Loki_LV02': 'LV02',
            'TMath::Log10(TMath::Abs(Ds_Loki_BPVLTIME))': (
                "log10(abs(BPVLTIME()))"
            ),
            'TMath::Log10(TMath::Abs(Ds_Loki_BPVVDCHI2))': (
                'log10(abs(BPVVDCHI2))'
            ),
            'TMath::Log10(Ds_ENDVERTEX_CHI2/Ds_ENDVERTEX_NDOF)': (
                'log10(VFASPF(VCHI2/VDOF))'
            )
        },
        # HLT filters, only process events firing triggers matching the RegEx
        'Hlt1Filter': None,
        'Hlt2Filter': None,
        # Fraction of candidates to randomly throw away before stripping
        'PrescaleDp2PhiPi': 1.0,
        'PrescaleDs2PhiPi': 1.0,
        'PrescaleDsstar2DsGamma': 1.0,
        # Fraction of candidates to randomly throw after before stripping
        'PostscaleDp2PhiPi': 1.0,
        'PostscaleDs2PhiPi': 1.0,
        'PostscaleDsstar2DsGamma': 1.0
    }
}

Lambdac2PHHForXSec = {
    'NAME': 'Lambdac2PHHForXSec',
    'WGs': ['Charm'],
    'BUILDERTYPE': 'StrippingLambdac2PHHForXSecConf',
    'STREAMS': ['ALLTURBO'],
    'CONFIG': {
        # Minimum transverse momentum all Lc+ daughters must satisfy
        'Daug_All_PT_MIN': 200.0*MeV,
        # Minimum transverse momentum at least 2 Lc+ daughters must satisfy
        'Daug_2of3_PT_MIN': 400.0*MeV,
        # Minimum transverse momentum at least 1 Lc+ daughter must satisfy
        'Daug_1of3_PT_MIN': 1000.0*MeV,
        # Minimum best primary vertex IP chi^2 all Lc+ daughters must satisfy
        'Daug_All_BPVIPCHI2_MIN': 4.0,
        # Minimum best PV IP chi^2 at least 2 Lc+ daughters must satisfy
        'Daug_2of3_BPVIPCHI2_MIN': 4.0,
        # Minimum best PV IP chi^2 at least 1 Lc+ daughter must satisfy
        'Daug_1of3_BPVIPCHI2_MIN': 6.0,
        # Minimum Lc+ daughter momentum
        'Daug_P_MIN': 3.0*GeV,
        # Maximum Lc+ daughter momentum
        'Daug_P_MAX': 100.0*GeV,
        # Minimum Lc+ daughter pseudorapidity
        'Daug_ETA_MIN': 2.0,
        # Maximum Lc+ daughter pseudorapidity
        'Daug_ETA_MAX': 5.0,
        # Minimum Lc+ daughter proton DLLp
        'Proton_PIDpPIDpi_MIN': 10.0,
        # Minimum Lc+ daughter proton DLLp - DLLK
        'Proton_PIDpPIDK_MIN': 5.0,
        # Minimum Lc+ daughter kaon DLLK
        'K_PIDK_MIN': 5.0,
        # Maximum Lc+ daughter pion DLLK
        'Pi_PIDK_MAX': 3.0,
        # Lc+ mass window around the nominal Lc+ mass before the vertex fit
        'Comb_ADAMASS_WIN': 90.0*MeV,
        # Maximum distance of closest approach of Lc+ daughters
        'Comb_ADOCAMAX_MAX': 0.5*mm,
        # Maximum Lc+ vertex chi^2 per vertex fit DoF
        'Lambdac_VCHI2VDOF_MAX': 25.0,
        # Maximum angle between Lc+ momentum and Lc+ direction of flight
        'Lambdac_acosBPVDIRA_MAX': 35.0*mrad,
        # Primary vertex displacement requirement, that the Lc+ is some sigma
        # away from the PV and it has a minimum flight time
        'Lambdac_PVDispCut': (
            '((BPVVDCHI2 > 4.0) &'
            '(BPVLTIME() > 0.075*picosecond))'
        ),
        # Maximum delta mass value m(Sigma_c) - m(Lambda_c+) (PDG dm: 167 MeV)
        'Sigmac_AMDiff_MAX': 185.0*MeV,
        # Maximum Sigma_c vertex chi^2 per vertex DoF
        'Sigmac_VCHI2VDOF_MAX': 25.0,
        # Minimum Lc+ MVA discriminant value
        'Lambdac_MVA_MIN': -0.3,
        # Path to the Lc+ MVA weights file
        # BDT is not applied if this is the empty string or None
        'Lambdac_MVA_Weights': None,
        # Dictionary of LoKi functors defining the Lc+ MVA input variables
        # The keys must match those used when training the MVA
        'Lambdac_MVA_Variables': {
            # Largest Lc+ daughter PT
            'ROOTex::Leading(Lc_h1_PT,Lc_h2_PT,Lc_p_PT)': (
                "MAXTREE(ISBASIC & HASTRACK, PT)"
            ),
            # Smallest Lc+ daughter PT
            'ROOTex::ThirdLeading(Lc_h1_PT,Lc_h2_PT,Lc_p_PT)': (
                "MINTREE(ISBASIC & HASTRACK, PT)"
            ),
            'Lc_DIRA_OWNPV': 'BPVDIRA',
            'Lc_Loki_LV02': 'LV02',
            'TMath::Log10(TMath::Abs(Lc_Loki_BPVVDCHI2))': (
                'log10(abs(BPVVDCHI2))'
            ),
            'TMath::Log10(TMath::Abs(Lc_Loki_BPVLTIME))': (
                "log10(abs(BPVLTIME()))"
            ),
            'TMath::Log10(Lc_ENDVERTEX_CHI2/Lc_ENDVERTEX_NDOF)': (
                'log10(VFASPF(VCHI2/VDOF))'
            )
        },
        # HLT filters, only process events firing triggers matching the RegEx
        'Hlt1Filter': None,
        'Hlt2Filter': None,
        # Fraction of candidates to randomly throw away before stripping
        'PrescaleLambdac2PKPi': 1.0,
        'PrescaleLambdac2PKK': 1.0,
        'PrescaleLambdac2PPiPi': 1.0,
        'PrescaleLambdac2PPiKWS': -1.0,
        'PrescaleSigmacz2LambdacPi': 1.0,
        'PrescaleSigmacpp2LambdacPi': 1.0,
        # Fraction of candidates to randomly throw after before stripping
        'PostscaleLambdac2PKPi': 1.0,
        'PostscaleLambdac2PKK': 1.0,
        'PostscaleLambdac2PPiPi': 1.0,
        'PostscaleLambdac2PPiKWS': -1.0,
        'PostscaleSigmacz2LambdacPi': 1.0,
        'PostscaleSigmacpp2LambdacPi': 1.0
    }
}

Xic2PKPiForXSec = {
    'NAME': 'Xic2PKPiForXSec',
    'WGs': ['Charm'],
    'BUILDERTYPE': 'StrippingXic2PKPiForXSecConf',
    'STREAMS': ['ALLTURBO'],
    'CONFIG': {
        # Minimum transverse momentum all Xic+ daughters must satisfy
        'Daug_All_PT_MIN': 200.0*MeV,
        # Minimum transverse momentum at least 2 Xic+ daughters must satisfy
        'Daug_2of3_PT_MIN': 400.0*MeV,
        # Minimum transverse momentum at least 1 Xic+ daughter must satisfy
        'Daug_1of3_PT_MIN': 1000.0*MeV,
        # Minimum best primary vertex IP chi^2 all Xic+ daughters must satisfy
        'Daug_All_BPVIPCHI2_MIN': 4.0,
        # Minimum best PV IP chi^2 at least 2 Xic+ daughters must satisfy
        'Daug_2of3_BPVIPCHI2_MIN': 4.0,
        # Minimum best PV IP chi^2 at least 1 Xic+ daughter must satisfy
        'Daug_1of3_BPVIPCHI2_MIN': 4.0,
        # Minimum Xic+ daughter momentum
        'Daug_P_MIN': 3.0*GeV,
        # Maximum Xic+ daughter momentum
        'Daug_P_MAX': 100.0*GeV,
        # Minimum Xic+ daughter pseudorapidity
        'Daug_ETA_MIN': 2.0,
        # Maximum Xic+ daughter pseudorapidity
        'Daug_ETA_MAX': 5.0,
        # Minimum Xic+ daughter proton DLLp
        'Proton_PIDpPIDpi_MIN': 10.0,
        # Minimum Xic+ daughter proton DLLp - DLLK
        'Proton_PIDpPIDK_MIN': 5.0,
        # Minimum Xic+ daughter kaon DLLK
        'K_PIDK_MIN': 5.0,
        # Maximum Xic+ daughter pion DLLK
        'Pi_PIDK_MAX': 3.0,
        # Xic+ mass window around the nominal Xic+ mass before the vertex fit
        'Comb_ADAMASS_WIN': 90.0*MeV,
        # Maximum distance of closest approach of Xic+ daughters
        'Comb_ADOCAMAX_MAX': 0.5*mm,
        # Maximum Xic+ vertex chi^2 per vertex fit DoF
        'Xic_VCHI2VDOF_MAX': 25.0,
        # Maximum angle between Xic+ momentum and Xic+ direction of flight
        'Xic_acosBPVDIRA_MAX': 35.0*mrad,
        # Primary vertex displacement requirement, that the Xic+ is some sigma
        # away from the PV and it has a minimum flight time
        'Xic_PVDispCut': (
            '((BPVVDCHI2 > 16.0) &'
            '(BPVLTIME() > 0.150*picosecond))'
        ),
        # Minimum Xic MVA discriminant value
        'Xic_MVA_MIN': -0.3,
        # Path to the Xic MVA weights file
        # BDT is not applied if this is the empty string or None
        'Xic_MVA_Weights': None,
        # Dictionary of LoKi functors defining the Xic MVA input variables
        # The keys must match those used when training the MVA
        'Xic_MVA_Variables': {
        },
        # HLT filters, only process events firing triggers matching the RegEx
        'Hlt1Filter': None,
        'Hlt2Filter': None,
        # Fraction of candidates to randomly throw away before stripping
        'PrescaleXic2PKPi': 1.0,
        # Fraction of candidates to randomly throw after before stripping
        'PostscaleXic2PKPi': 1.0
    }
}

Xic2PKKPiForXSec = {
    'NAME': 'Xic2PKKPiForXSec',
    'WGs': ['Charm'],
    'BUILDERTYPE': 'StrippingXic2PKKPiForXSecConf',
    'STREAMS': ['ALLTURBO'],
    'CONFIG': {
        # Minimum transverse momentum all Xic0 daughters must satisfy
        'Daug_All_PT_MIN': 250.0*MeV,
        # Minimum transverse momentum at least 3 Xic0 daughters must satisfy
        'Daug_3of4_PT_MIN': 300.0*MeV,
        # Minimum transverse momentum at least 2 Xic0 daughters must satisfy
        'Daug_2of4_PT_MIN': 400.0*MeV,
        # Minimum transverse momentum at least 1 Xic0 daughter must satisfy
        'Daug_1of4_PT_MIN': 800.0*MeV,
        # Minimum best primary vertex IP chi^2 all Xic0 daughters must satisfy
        'Daug_All_BPVIPCHI2_MIN': 4.0,
        # Minimum best PV IP chi^2 at least 3 Xic0 daughters must satisfy
        'Daug_3of4_BPVIPCHI2_MIN': 4.0,
        # Minimum best PV IP chi^2 at least 2 Xic0 daughters must satisfy
        'Daug_2of4_BPVIPCHI2_MIN': 4.0,
        # Minimum best PV IP chi^2 at least 1 Xic0 daughter must satisfy
        'Daug_1of4_BPVIPCHI2_MIN': 4.0,
        # Minimum Xic0 daughter momentum
        'Daug_P_MIN': 3.0*GeV,
        # Maximum Xic0 daughter momentum
        'Daug_P_MAX': 100.0*GeV,
        # Minimum Xic0 daughter pseudorapidity
        'Daug_ETA_MIN': 2.0,
        # Maximum Xic0 daughter pseudorapidity
        'Daug_ETA_MAX': 5.0,
        # Minimum Xic0 daughter proton DLLp
        'Proton_PIDpPIDpi_MIN': 10.0,
        # Minimum Xic0 daughter proton DLLp - DLLK
        'Proton_PIDpPIDK_MIN': 5.0,
        # Minimum Xic0 daughter kaon DLLK
        'K_PIDK_MIN': 5.0,
        # Maximum Xic0 daughter pion DLLK
        'Pi_PIDK_MAX': 3.0,
        # Xic0 mass window around the nominal Xic0 mass before the vertex fit
        'Comb_ADAMASS_WIN': 90.0*MeV,
        # Maximum distance of closest approach of Xic0 daughters
        'Comb_ADOCAMAX_MAX': 0.5*mm,
        # Maximum Xic0 vertex chi^2 per vertex fit DoF
        'Xic_VCHI2VDOF_MAX': 25.0,
        # Maximum angle between Xic0 momentum and Xic0 direction of flight
        'Xic_acosBPVDIRA_MAX': 35.0*mrad,
        # Primary vertex displacement requirement, that the Xic0 is some sigma
        # away from the PV, and it has a minimum flight time
        'Xic_PVDispCut': (
            '((BPVVDCHI2 > 4.0) &'
            '(BPVLTIME() > 0.075*picosecond))'
        ),
        # Minimum Xic0 MVA discriminant value
        'Xic_MVA_MIN': -0.3,
        # Path to the Xic0 MVA weights file
        # BDT is not applied if this is the empty string or None
        'Xic_MVA_Weights': None,
        # Dictionary of LoKi functors defining the Xic0 MVA input variables
        # The keys must match those used when training the MVA
        'Xic_MVA_Variables': {
        },
        # HLT filters, only process events firing triggers matching the RegEx
        'Hlt1Filter': None,
        'Hlt2Filter': None,
        # Fraction of candidates to randomly throw away before stripping
        'PrescaleXic2PKKPi': 1.0,
        # Fraction of candidates to randomly throw after before stripping
        'PostscaleXic2PKKPi': 1.0
    }
}


##################
## Semileptonic ##
##################

B2DMuNuX = {
        'NAME'        : 'B2DMuNuX',
        'WGs'         : ['Semileptonic'],
        'BUILDERTYPE' : 'B2DMuNuXAllLinesConf',
        'CONFIG'      : {
            "prescaleFakes": 0.02
            ,"GEC_nLongTrk" : 250
            ,"TTSpecs"      : {}
            ,"HLT1"   : "HLT_PASS_RE('Hlt1.*Decision')"
            ,"HLT2"   : "HLT_PASS_RE('Hlt2.*Decision')"
            ,"Monitor"      : False
            ,"UseNoPIDsInputs":False
            ,"TRGHOSTPROB"   : 0.35
            ,"TRCHI2"        : 3.0
            ,"MuonPIDmu"     : 0.0
            ,"MuonPT"        : 1000*MeV
            ,"MuonIPCHI2"    : 9.00
            ,"MuonP"         : 6.0*GeV
            ,"HadronPT"      : 250.0*MeV
            ,"HadronP"       : 2.0*GeV
            ,"HadronIPCHI2"  : 4.0
            ,"ProtonPIDp"    : 0.0
            ,"ProtonPIDpK"   : -999.
            ,"ProtonP"       : 8.0*GeV
            ,"KaonPIDK"      : -2.0
            ,"KaonP"         : 5.0*GeV
            ,"PionPIDK"      : 20.0
            ,"ElectronPIDe"  : 0.0
            ,"ElectronPT"    : 1200*MeV
            ,"D_BPVDIRA"     : 0.99
            ,"D_FDCHI2"      : 25.0
            ,"D_MassWin"     : 80.0*MeV
            ,"D_AMassWin"    : 90.0*MeV
            ,"D_VCHI2DOF"    : 6.0
            ,"D_DocaChi2Max" : 20
            ,"B_DIRA"         : 0.999
            ,"B_VCHI2DOF"      : 9.0
            ,"B_D_DZ"         : -0.05*mm
            ,"B_MassMin"       : 2.2*GeV
            ,"B_MassMax"       : 8.0*GeV
            ,"B_DocaChi2Max"  : 10
            },
        'STREAMS'     : ['ALL']
     }
