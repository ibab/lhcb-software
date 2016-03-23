################################################################################
##                          S T R I P P I N G  2 3                            ##
##                                                                            ##
##  Configuration for Calib WG                                                ##
##  Contact person: Michael Kolpin (Michael.Kolpin@cern.ch)                   ##
################################################################################

from GaudiKernel.SystemOfUnits import *

TrackEffDownMuon = {
    'WGs'         : ['ALL'],
    'BUILDERTYPE' : 'StrippingTrackEffDownMuonConf',
    'STREAMS'           : [ 'Calibration' ],
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
    'WGs'         : ['ALL'],
    'STREAMS'           : [ 'Calibration' ],
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
    'WGs'         : ['ALL'],
    'STREAMS'           : [ 'Calibration' ],
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

CharmFromBSemiForHadronAsy = {
        'WGs'         : ['ALL'],
        'BUILDERTYPE' : 'CharmFromBSemiForHadronAsyAllLinesConf',
        'CONFIG'      : {
            "prescale_LbRS" : 1.0
            ,"prescale_LbWS" : 0.2 
            ,"prescale_D0to3piRS" : 1.0
            ,"prescale_D0to3piWS" : 0.2
            ,"prescale_D0toK2piRS" : 1.0
            ,"prescale_D0toK2piWS" : 0.2 
            #### common to all modes
            ,"GEC_nLongTrk" : 250. # adimensional
            ,"GHOSTPROB_MAX" : 0.35 #adimensional
            ,"Mu_PT" : 800. # MeV
            ,"H_PT"  : 250. # MeV
            ,"Pi_PIDKMax" : 6. # adimensional
            ,"K_PIDKMin"  : 6. # adimensional
            ,"Slowpi_PIDKMax" : 10. # adimensional
            ,"Slowpi_PIDeMax" : 99. # adimensional
            ,"Slowpi_PTMin"   : 200. # MeV
            ##### specific to D0 modes
            ,"MuPi_SUMPT_MIN" : 1300. # MeV
            ,"MuPi_DOCACHI2_MAX" : 8.0 # adimensional
            ,"MuPi_CHI2NDOF_MAX" : 3.0 # adimensional
            ,"MuPi_DIRA_MIN" : -99. # adimensional
            ,"MuPi_FDCHI2_MIN" : 20. # adimensional
            ,"D0to3H_REQUIRE_TOS" : True # bool
            ,"D0to3H_DOCACHI2_MAX" : 10. # adimensional
            ,"D0to3H_VCHI2NDF_MAX" : 3.0 # adimensional
            ,"D0to3H_SUMPT_MIN" : 1800. # MeV
            ,"D0to3H_DZ" : 2.0 # mm
            ,"D0to3H_3pi_DeltaMass_MAX" : 350. # MeV
            ,"D0to3H_K2pi_DeltaMass_MAX" : 250. # MeV
            ,"D0to3H_3pi_MASS_MIN" : 900. # MeV
            ,"D0to3H_3pi_MASS_MAX" : 1400. # MeV
            ,"D0to3H_K2pi_MASS_MIN" : 1300. # MeV
            ,"D0to3H_K2pi_MASS_MAX" : 1800. # MeV
            ,"D0to3H_B_MASS_MIN" : 1800. # MeV
            ,"D0to3H_B_MASS_MAX" : 4900. # MeV
            ,"D0to3H_B_DIRA_MIN" : 0.99 # adimensional
            ,"D0to3H_B_VCHI2NDF_MAX" : 15. # adimensional
            ,"D0to3H_B_DOCACHI2_MAX" : 50. # adimensional
            #### specific to Lambda_c+ modes
            ,"PiPi_MASS_MAX" : 500.  # MeV
            ,"PiPi_DOCACHI2_MAX" : 15. # adimensional
            ,"PiPi_CHI2NDF" : 3. # adimensional
            ,"PiPi_SUMPT_MIN" : 600.  # MeV
            ,"MuPiPi_DOCACHI2_MAX" : 15. 
            ,"MuPiPi_CHI2NDF": 3.
            ,"MuPiPi_FDCHI2_MIN" : 20. # adimensional
            ,"Lc2Kpi_REQUIRE_TOS" : True # bool
            ,"Lc2Kpi_DOCACHI2_MAX" : 10. # adimensional
            ,"Lc2Kpi_VCHI2NDF_MAX" : 3.0 # adimensional
            ,"Lc2Kpi_SUMPT_MIN" : 1500.  # MeV
            ,"Lc2Kpi_FDCHI2_MIN" : 20. # adimensional
            ,"Lc2Kpi_MASS_MIN" : 800.  # MeV
            ,"Lc2Kpi_MASS_MAX" : 1350. # MeV
            ,"Lc2Kpi_DeltaMass_MAX" : 700. # MeV
            ,"Lc2Kpi_DZ" : 1.0 # mm
            ,"Lc2Kpi_B_MASS_MIN" : 2200. # MeV
            ,"Lc2Kpi_B_MASS_MAX" : 4300. # MeV
            ,"Lc2Kpi_B_FDCHI2_MIN" : 20. # adimensional
            ,"Lc2Kpi_B_DIRA_MIN" : 0.99 # adimensional
            ,"Lc2Kpi_B_DOCACHI2_MAX" : 50. # adimensional
            ,"Lc2Kpi_B_VCHI2NDF_MAX" : 15. # adimensional
            },
        'STREAMS'     : ["Calibration"]
    }

CharmFromBSemiForProtonPID = {
        'WGs'         : ['ALL'],
        'BUILDERTYPE' : 'CharmFromBSemiForProtonPIDAllLinesConf',
        'CONFIG'      : {
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
        "STREAMS":["PID"]
    }
        
D02KPiPi0 = {
    'WGs'         : ['ALL'],
    'BUILDERTYPE' : 'StrippingD02KPiPi0Conf',
    'CONFIG'      : {   'TrackMinPT_M'         : 300       # MeV
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
    'STREAMS'     : ['Calibration']
    }

DstarD02KShhForTrackingEff = {
        'BUILDERTYPE' : 'DstarD02KShh_ForTrackingEffBuilder',
        'CONFIG' : {'LongTrackGEC'          :    150
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
        'WGs' : ['ALL'],
        'STREAMS' : ['Calibration']
                }

ProtonAsym = {
    "WGs": ["ALL"],
    "BUILDERTYPE": "ProtonAsymBuilder",
    "CONFIG": {
        # Are the candidates to be stored on microDST, as opposed to full?
        "MicroDST":  True,

        # Prescales and postscales set the fraction of candidates to randomly
        # throw away before and after stripping, respectively
        "PrescaleBd2LcppipiLc2Kpi": 1.0,
        "PostscaleBd2LcppipiLc2Kpi": 1.0,
        "PrescaleBu2LcppipipiLc2Kpi": 1.0,
        "PostscaleBu2LcppipipiLc2Kpi": 1.0,
        "PrescaleLb2LcpiLc2Kpi": 1.0,
        "PostscaleLb2LcpiLc2Kpi": 1.0,
        "PrescaleLb2LcpipipiLc2Kpi": 1.0,
        "PostscaleLb2LcpipipiLc2Kpi": 1.0,
        "PrescaleSc2LcpipLc2Kpi": 1.0,
        "PostscaleSc2LcpipLc2Kpi": 1.0,
        "PrescaleSc2LcpimLc2Kpi": 1.0,
        "PostscaleSc2LcpimLc2Kpi": 1.0,
        "PrescaleLcst2LcpipiLc2Kpi": 1.0,
        "PostscaleLcst2LcpipiLc2Kpi": 1.0,

        # HLT filters require the events passing the line to have a positive
        # decision on the trigger line(s) matching the regex
        "HLTBd2LcppipiLc2Kpi": None,
        "HLTBu2LcppipipiLc2Kpi": None,
        "HLTLb2LcpiLc2Kpi": None,
        "HLTLb2LcpipipiLc2Kpi": None,
        "HLTSc2LcpipLc2Kpi": "HLT_PASS_RE('Hlt2.*CharmHadD02HHXDst.*Decision')",
        "HLTSc2LcpimLc2Kpi": "HLT_PASS_RE('Hlt2.*CharmHadD02HHXDst.*Decision')",
        "HLTLcst2LcpipiLc2Kpi": None,

        # Partial Lambda_c mass window, taken from TGenPhaseSpace studies
        "LcPartialMassMin": 700*MeV, #620 is the minimum -> cut a bit harder where most of bkg is
        "LcPartialMassMax": 1360*MeV,
        # Minimum sum of Lambda_c daughters' tranverse momentum
        "LcPartialPTSum": 1500.0*MeV,
        # Minimum Lambda_c flight distance fit quality per DOF
        "LcPartialFDCHI2": 36.0, #50
        # Maximum Lambda_c pairwise daughter distance of closest approach
        "LcPartialDOCACHI2": 25.0, #20
        # Maximum Lambda_c vertex fit quality per DOF
        "LcPartialVCHI2DOF": 10.0, #50

        # Partial Sigma_c mass window, taken from TGenPhaseSpace studies
        "ScPartialMassMin": 1000*MeV,
        "ScPartialMassMax": 1600*MeV,
        # Minimum Lambda_c vertex displacement wrt Sigma_c vertex along z
        "ScMinZ": 0.2*mm,
        # Sigma_c maximum vertex quality per DOF
        "ScVCHI2DOF": 10.0, #15
        # Maximum Sigma_c impact parameter chi2
        "ScBPVIPCHI2": 15.0, #20

        # Partial Lambda_c* mass window, taken from TGenPhaseSpace studies
        "LcstPartialMassMin": 1300*MeV,
        "LcstPartialMassMax": 1660*MeV,
        # Minimum Lambda_c vertex displacement wrt Lambda_c* vertex along z
        "LcstMinZ": 0.2*mm,
        # Lambda_c* maximum vertex quality per DOF
        "LcstVCHI2DOF": 10.0,
        # Maximum Lambda_c* impact parameter chi2
        "LcstBPVIPCHI2": 15.0, #20

        # Partial Lambda_b0 mass window, taken from TGenPhaseSpace studies
        "LbToLc1piPartialMassMin": 3000*MeV,
        "LbToLc3piPartialMassMin": 3500*MeV,
        "LbPartialMassMax": 4700*MeV,
        # Minimum transverse momentum of combined daughters
        "LbPTSum": 2000*MeV,
        # Lambda_b0 maximum vertex quality
        "LbVCHI2DOF": 20.0, #15.00
        # Minimum Lambda_c vertex displacement wrt Lambda_b vertex along z
        "LbMinZ": 0.2*mm,
        # Maximum Lambda_b impact parameter chi2
        "LbBPVIPCHI2": 15.0, #20
        # Minimum Lambda_b flight distance fit quality per DOF
        "LbFDCHI2": 36.0, #50    

        # Partial B0/B+ mass window, taken from TGenPhaseSpace studies
        "BPartialMassMin": 3500*MeV,
        "BPartialMassMax": 4350*MeV,
        # Minimum transverse momentum of combined daughters
        "BPTSum": 2000*MeV,
        # B0/B+ maximum vertex quality
        "BVCHI2DOF": 20.0, #15
        # Minimum Lambda_c vertex displacement wrt B0/B+ vertex along z
        "BMinZ": 0.2*mm,
        # Maximum B0/B+ impact parameter chi2
        "BBPVIPCHI2": 15.0, #20
        # Minimum B0/B+ flight distance fit quality per DOF
        "BFDCHI2": 36.0, #50

        # Minimum final state track momentum
        "TrackP": 1000.0*MeV, #3000
        # Minimum final state track transverse momentum
        "TrackPT": 100.0*MeV, #500
        # Maximum final state track impact parameter chi^2
        "TrackIPCHI2": 12.0, #20
        # Minimum Kaon ANN PID
        "ProbNNk": 0.2,
        # Minimum Pion ANN PID
        "ProbNNpi": 0.2,
        # Minimum Proton ANN PID
        "ProbNNp": 0.2,

        #Window for full reconstruction
        "LcMinWin": 2206*MeV,
        "LcMaxWin": 2366*MeV,

        "BuMinWin": 5000*MeV,
        "BuMaxWin": 6500*MeV,
        "LbMinWin": 5200*MeV,
        "LbMaxWin": 6500*MeV,
        "ScMinWin": 2053*MeV,
        "ScMaxWin": 2753*MeV,
        "LcstMinWin": 2292*MeV,
        "LcstMaxWin": 2892*MeV
             
    },
    "STREAMS": {
        "Calibration": [
            "StrippingBd2LcppipiLc2KpiProtonAsymLine",
            "StrippingBu2LcppipipiLc2KpiProtonAsymLine",
            "StrippingLb2LcpiLc2KpiProtonAsymLine",
            "StrippingLb2LcpipipiLc2KpiProtonAsymLine",
            "StrippingSc2LcpipLc2KpiProtonAsymLine",
            "StrippingSc2LcpimLc2KpiProtonAsymLine",
            "StrippingLcst2LcpipiLc2KpiProtonAsymLine",
            "StrippingBd2LcppipiLc2pKpiProtonAsymLine",
            "StrippingBu2LcppipipiLc2pKpiProtonAsymLine",
            "StrippingLb2LcpiLc2pKpiProtonAsymLine",
            "StrippingLb2LcpipipiLc2pKpiProtonAsymLine",
            "StrippingSc2LcpipLc2pKpiProtonAsymLine",
            "StrippingSc2LcpimLc2pKpiProtonAsymLine",
            "StrippingLcst2LcpipiLc2pKpiProtonAsymLine"
        ]
    }
}

SigmacForPID = {
  'BUILDERTYPE' : 'SigmacForPIDConf',
  'CONFIG'      : {
    #
    #
    ## PT-cuts
    #
    'pT(Lc+)'          :  3.0 * GeV ,    ## pt-cut for  prompt   Lc+
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
    'KaonCuts'        : ' & ( 2 < PIDK  - PIDpi ) & ( -5 < PIDK  - PIDp ) '   ,
    'PionCuts'        : ' & ( 2 < PIDpi - PIDK  ) & ( -5 < PIDpi - PIDp  ) '   ,
    'SlowPionCuts'    : ' TRCHI2DOF < 5 '                                     ,
    #
    # Global Event cuts
    #
    'CheckPV'         : True ,
    #
    # Technicalities:
    #
    'Preambulo'       : [
    # shortcut for chi2 of vertex fit
    'chi2vx = VFASPF(VCHI2) '                    ,
    # shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light" ,
    "ctau_9   = BPVLTIME (   9 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<9
    ] ,
    ## monitoring ?
    'Monitor'     : False ,
    ## prescale
    'SigmaCPrescale'         : 1.0 ,  
    ## ========================================================================
    },
  'STREAMS' : [ 'PID' ],
  'WGs' : [ 'ALL' ]
}

TrackEffD0ToK3Pi = {
        'WGs'         : ['ALL'],
        'BUILDERTYPE' : 'TrackEffD0ToK3PiAllLinesConf',
        'STREAMS':["Calibration"],
        'CONFIG'      : {
            "HLT2" : "HLT_PASS_RE('Hlt2.*CharmHad.*HHX.*Decision')",
            "TTSpecs" : {'Hlt2.*CharmHad.*HHX.*Decision%TOS':0},
            "VeloLineForTiming":False,
            "VeloFitter":"SimplifiedGeometry",
            "DoVeloDecoding": False,
            "RequireDstFirst":False,
            "Dst_MAX_M":2.035*GeV,
            "Dst_MAX_DTFCHI2":3.0,
            "Sc_MAX_M":2.5*GeV,
            "Sc_MAX_DTFCHI2":3.0,
            "D0_MIN_FD":5.0*mm,
            "LC_MIN_FD":2.0*mm,
            "D0_MAX_DOCA":0.05*mm,
            "VeloMINIP":0.05*mm,
            "Kaon_MIN_PIDK":7,
            "Pion_MAX_PIDK":4
            }
    }

TrackEffD0ToKPi = {
        'WGs'         : ['ALL'],
        'BUILDERTYPE' : 'TrackEffD0ToKPiAllLinesConf',
        'STREAMS':["Calibration"],
        'CONFIG'      : {
            "Monitor":False,
            "HLT1" :"HLT_PASS_RE('Hlt1TrackMVADecision')",
            "HLT2" :"HLT_PASS_RE('Hlt2TrackEff_D0.*Decision')",
            "TTSpecs" :{'Hlt1TrackMVADecision%TOS':0,'Hlt2TrackEff_D0.*Decision%TOS':0},
            "Tag_MIN_PT":1000.,
            "VeloMINIPCHI2":4.0,
            "Kaon_MIN_PIDK":0,
            "Pion_MAX_PIDK":20,
            "Dst_M_MAX":2100,
            "Dst_DTFCHI2_MAX":10
            }
    }



