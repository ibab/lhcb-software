"""
Line config dictionaries for Calibration channels

Stripping15 definitions.
Dictionaries have the name of the line builder instance.
"""
from GaudiKernel.SystemOfUnits import MeV, GeV, cm, mm

BeamGas = {
   'BUILDERTYPE' : 'BeamGasConf',
   'CONFIG' : {'Prescale' : 0.05, 'Postscale' : 1.0},
   'STREAMS' : ['Calibration'],
   'WGs' : ['ALL']
}

D02KPiPi0 = {
   'BUILDERTYPE' : 'StrippingD02KPiPi0Conf',
   'CONFIG' : { 'TrackMinPT'          : 300       # MeV
                   ,'TrackMinPT_R'       : 600       # MeV  (>= TrackMinPT else no effect)
                   ,'TrackMinTrackProb'  : 0.000001  # unitless
                   ,'TrackMinIPChi2'     : 16        # unitless
                   ,'Pi0MinPT_M'         : 2000      # MeV
                   ,'Pi0MinPT_R'         : 1000      # MeV
                   ,'ResPi0MinGamCL'     : 0.2       # unitless
                   ,'D0MinM'             : 1600      # MeV
                   ,'D0MaxM'             : 2100      # MeV
                   ,'D0MinVtxProb'       : 0.001     # unitless
                   ,'D0MaxIPChi2'        : 9         # unitless
                   ,'D0MinDIRA'          : 0.9999    # unitless
                   ,'D0MinVVDChi2'       : 64        # unitless
                   ,'MergedLinePrescale'   : 1.        # unitless
                   ,'MergedLinePostscale'  : 1.        # unitless
                   ,'ResolvedLinePrescale' : 1.        # unitless
                   ,'ResolvedLinePostscale': 1.        # unitless
                   },
   'STREAMS' : ['Calibration'],
   'WGs' : ['ALL']
}

# A. Powell

NoPIDDstarWithD02RSKPi = {
    'BUILDERTYPE' : 'NoPIDDstarWithD02RSKPiConf',
    'CONFIG' : {
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
    , 'Prescale'         : 0.482          ## unitless
    , 'Postscale'        : 1.00           ## unitless
    ##
    , 'Monitor'          : False           ## Activate the monitoring?
    },
    'STREAMS' : [ 'Calibration' ],
    'WGs'    : ['ALL']
    }

# NoPID Inclusive Phi Lines
# A. Powell & S. Schleich
InclPhi={
    'BUILDERTYPE' : 'InclPhiConf',
    'CONFIG' : {  'KaonPT'              : 500      # MeV
                , 'KaonDLL'             : 15       # adimensional
                , 'PhiMassWindow'       : 30       # MeV
                , 'HighPtPrescale'      : 0.0030   # adimensional 
                , 'LowPtPrescale'       : 0.0040   # adimensional 
                , 'LDPrescale'          : 0.0002   # adimensional 
                },
    'STREAMS' : [ 'Calibration' ],
    'WGs'    : ['ALL']
    }

# NoPID V0 Lines
# A. Powell
V0ForPID = {
    'BUILDERTYPE' : 'StrippingV0ForPIDConf',
    'CONFIG' : {
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
    'KS0LL_Prescale'           : 0.024 ,
    'KS0DD_Prescale'           : 0.026 , 
    'LamLL_Prescale_LoP'       : 0.151 , 
    'LamLL_Prescale_HiP'       : 1.000 ,
    'LamDD_Prescale'           : 0.099 , 
    'LamLLIsMUON_Prescale_LoP' : 1.000 ,
    'LamLLIsMUON_Prescale_HiP' : 1.000 ,
    'LamDDIsMUON_Prescale'     : 1.000   
     },
    'STREAMS' : [ 'Calibration' ],
    'WGs'    : ['ALL']
    }


Jpsi2eeForElectronID = {
    'BUILDERTYPE'	: 'ElectronIDConf',
    'CONFIG'	: {
        'LinePrescale'            :    1.   ,
        'LinePostscale'           :    1.   ,
        'HltFilter'               : None    ,

        'Both_PT'                 :  500.   ,  # MeV
        'Both_P'                  : 3000.   ,  # MeV
        'Both_TRCHI2DOF'          :    5.   ,
        'Both_MIPCHI2'            :   25.   ,
        
        'Tag_PT'                  : 1500.   ,  # MeV
        'Tag_P'                   : 6000.   ,  # MeV
        'Tag_PIDe'                :    5.   ,
        'Tag_MIPCHI2'             :   25.   ,

        'Probe_PT'                :  500.   ,  # MeV
        'Probe_P'                 : 3000.   ,  # MeV
        'Probe_MIPCHI2'           :   25.   ,

        'eeCombMinMass'           : 2100.   ,  # MeV         
        'eeCombMaxMass'           : 4300.   ,  # MeV   
        'eeVCHI2PDOF'             :    9.   ,  
        'eeMinMass'               : 2200.   ,  # MeV 
        'eeMaxMass'               : 4200.   ,  # MeV
        'eePT'                    : 2000.   , 
                
        'eeDLS'                   :   50.   ,
        'eeVZ'                    : -1.0e+9    # mm   
        },
    'STREAMS' : [ 'Calibration' ],
    'WGs'    : ['ALL']
    }


MuIDCalib = {
    'BUILDERTYPE' : 'MuIDCalibConf',
    'WGs'         : [ 'ALL' ],
    'STREAMS'     : [ 'Calibration' ],
    'CONFIG'      : {
    'PromptPrescale'           : 0.0,
    'DetachedPrescale'         : 1.,
    'DetachedNoMIPPrescale'    : 0.8,
    'DetachedNoMIPHiPPrescale' : 1.,
    'DetachedNoMIPKPrescale'   : 1.,
    'FromLambdacPrescale'      : 1.,
    'KFromLambdacPrescale'     : 1.,
    'PiFromLambdacPrescale'    : 1.,
    'PFromLambdacPrescale'     : 1.,
    'KISMUONFromLambdacPrescale' : 1.,
    'PiISMUONFromLambdacPrescale': 1.,
    'PISMUONFromLambdacPrescale' : 1. 
    }
    }


TrackEffDownMuon = { 
    'BUILDERTYPE' : 'StrippingTrackEffDownMuonConf',
    'WGs' : [ 'ALL' ],
    'STREAMS' : [ 'Calibration' ],
    'CONFIG' : {
    	'MuMom':		2000.	# MeV
        ,	'MuTMom':		200.	# MeV
        ,	'TrChi2':		10.	# MeV
        ,	'MassPreComb':		2000.	# MeV
        ,	'MassPostComb':		200.	# MeV
        ,	'Doca':			5.	# mm
        ,	'VertChi2':		25.	# adimensional
        ,       'DataType':             '2011'        
        ,	'NominalLinePrescale':  0.2 # proposal: 0.2 to stay below 0.15% retention rate 
        ,	'NominalLinePostscale': 1.
        ,	'ValidationLinePrescale':0.003 #0.5 in stripping15: 0.1 gives 1.42% retention rate
        ,	'ValidationLinePostscale': 1.
        ,	'HLT1TisTosSpecs': { "Hlt1TrackMuonDecision%TOS" : 0, "Hlt1SingleMuonNoIPL0Decision%TOS" : 0} #no reg. expression allowed(see selHlt1Jpsi )
        ,	'HLT1PassOnAll': True
        ,	'HLT2TisTosSpecs': { "Hlt2SingleMuon.*Decision%TOS" : 0} #reg. expression allowed
        ,	'HLT2PassOnAll': False
        } 
    }

TrackEffVeloMuon = { 
    'BUILDERTYPE' : 'StrippingTrackEffVeloMuonConf',
    'WGs' : [ 'ALL' ],
    'STREAMS' : [ 'Calibration' ],
    'CONFIG' : {
    "TrChi2Mu":		5.	# adimensional
    ,	"JpsiPt":		0.5	# GeV
    ,	"TrPt":			100.	# MeV
    ,	"TrP":			5.	# GeV
    ,	"LongP":		7.	# GeV
    ,	"MuDLL":		1.	# adimensional
    ,	"VertChi2":		2.	# adimensional
    ,	"MassPreComb":		1000.	# MeV
    ,	"MassPostComb":		400.	# MeV
    ,	"Prescale":		0.5	# adimensional
    ,	"Postscale":		1.	# adimensional
    ,	'HLT1TisTosSpecs': { "Hlt1TrackMuonDecision%TOS" : 0, "Hlt1SingleMuonNoIPDecision%TOS" : 0} #no reg. expression allowed(see selHlt1Jpsi )
    ,	'HLT1PassOnAll': True
    ,	'HLT2TisTosSpecs': { "Hlt2SingleMuon.*Decision%TOS" : 0} #reg. expression allowed
    ,	'HLT2PassOnAll': False
    }
    }

TrackEffMuonTT = {
    'BUILDERTYPE' : 'StrippingTrackEffMuonTTConf',
    'WGs' : [ 'ALL' ],
    'STREAMS' : [ 'Calibration' ],
    'CONFIG' : {
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
    'JpsiPrescale'                : 0.5,
    'UpsilonPrescale'             : 1,
    'ZPrescale'                   : 1,
    'BJpsiKPrescale'              : 1,
    'Postscale'                   : 1
    }
    }
