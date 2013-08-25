# Config dictionaries for calibration lines (2012 restripping)
"""
Line config dictionaries for the PID and Calibration streams
(all WGs, 2012 incremental restripping).

Dictionary names should have the same name as the corresponding
StrippingSelections file containing the line builder instance,
without the preceeding 'Stripping'.

   e.g  StrippingNoPIDDstarWithD02RSKPi linebuilder should be added as:

      NoPIDDstarWithD02RSKPi = { ... }
"""

from GaudiKernel.SystemOfUnits import MeV, GeV, cm, mm


#### NEW lines ####

##############################################################################
##
## Stripping line for selection of
##    B0 -> Lambda_c~- (-> p+ K- pi+) p+ pi- pi+
##    B+ -> Lambda_c~- (-> p+ K- pi+) p+ pi- pi+ pi+
## for proton detection asymmetry measurement.
##
## Responsible: Alex Pearce
## Test: Yes
##
##############################################################################
B2LcpXLc2Kpi = {
    'BUILDERTYPE' : 'B2LcpXLc2KpiBuilder',
    'WGs' : [ 'ALL' ],
    'STREAMS' : [ 'Calibration' ],
    'CONFIG' : {
    # Fraction of candidates to randomly throw away before stripping
    "Prescale": 1.0,
    # Fraction of candidates to randomly throw away after stripping
    "Postscale": 1.0,
    # B0 partial mass window
    "BMassMin": 2400*MeV,
    "BMassMax": 4400*MeV,
    # B0 mass minus Lambda_c partial mass window
    "BLcDeltaMassMin": 1500*MeV,
    "BLcDeltaMassMax": 3750*MeV,
    # Maximum B0 pairwise daughter distance of closest approach
    "BDOCACHI2": 50.0,
    # Minimum Sum of Lambda_c daughters' tranverse momentum
    "BPTSum": 5000.0*MeV,
    # Maximum B0 vertex fit quality per DOF
    "BVCHI2DOF": 15.0,
    # Minimum cosine of B0 pointing angle difference
    "BDIRA": 0.999,
    # Lambda_c mass window
    "LcPartialMassMin": 800.0*MeV,
    "LcPartialMassMax": 1350.0*MeV,
    # Maximum Lambda_c pairwise daughter distance of closest approach
    "LcPartialDOCACHI2": 10.0,
    # Minimum Sum of Lambda_c daughters' tranverse momentum
    "LcPartialPTSum": 1500.0*MeV,
    # Maximum Lambda_c vertex fit quality per DOF
    "LcPartialVCHI2DOF": 3.0,
    # Minimum Lambda_c flight distance fit quality per DOF
    "LcPartialFDCHI2": 20.0,
    # Minimum Lambda_c daughter momentum
    "TrackP": 2000.0*MeV,
    # Minimum final state track transverse momentum
    "TrackPT": 500.0*MeV,
    # Maximum final state track impact parameter chi^2
    "TrackIPCHI2": 20.0,
    # Minimum Kaon ANN PID
    "ProbNNk": 0.2,
    # Minimum Proton ANN PID
    "ProbNNp": 0.2,
    # Minimum Pion ANN PID
    "ProbNNpi": 0.2
    }
    }

##############################################################################
##
## Stripping lines for selection of dimuon and prompt charm candidates passing
## specific trigger conditions to study how well the offline tracking is
## able to find the tracks used to make the trigger decisions
##
## NB. We only want to run the "NoReco" lines from this module. These lines are
## probably better off going in a dedicated full DST stream, but for the
## incremental restripping, they will go in the CharmCompleteEvent stream.
##
## Responsible: Vladimir Gligorov, Conor Fitzpatrick
## Test: Yes
##
##############################################################################
TriggerValidation = {
  'BUILDERTYPE' : 'TriggerValidationConf',
  'WGs' : [ 'ALL' ],
  'STREAMS' : [ 'CharmCompleteEvent' ],
  'CONFIG' : {
  'D02Kpi_DaugPtMin': 800.,
  'D02Kpi_DaugPtMax': 1500.,
  'D02Kpi_DaugP': 5000.,
  'D02Kpi_DaugIPChi2': 9.,
  'D02Kpi_DaugTrkChi2': 3.,
  'D02Kpi_HighPIDK': 5.,
  'D02Kpi_LowPIDK': 0.,
  'D02Kpi_D0Pt': 2000.,
  'D02Kpi_D0MassWindowCentre': 1865.,
  'D02Kpi_D0KPiMassWindowWidthLow':  -100.,
  'D02Kpi_D0KPiMassWindowWidthHigh': 200.,
  'D02Kpi_D0P': 5000.,
  'D02Kpi_D0VtxChi2Ndof': 10.,
  'D02Kpi_D0FDChi2': 40.,
  'D02Kpi_D0BPVDira': 0.9999,
  'D02Kpi_D0DOCA': 0.07,
  'D02Kpi_Prescale':0., # only run NoReco lines
  'D02Kpi_Postscale':1.,
               
  'D2KPP_DaughterPT':250.,
  'D2KPP_DaughterP':2000.,
  'D2KPP_DaughterIPChi2':4.,
  'D2KPP_Daughter2IPChi2':10.,
  'D2KPP_KPIDK':7.,
  'D2KPP_piPIDK':3.,
  'D2KPP_PTSum':2800.,
  'D2KPP_DPt':1000.,
  'D2KPP_DIPChi2':15.,
  'D2KPP_DdcaFDChi2':100.,
  'D2KPP_DDIRA':0.,
  'D2KPP_DVXChi2NDOF':10.,
  'D2KPP_MinMassPosFit':1800.,
  'D2KPP_MaxMassPosFit':2040.,
  'D2KPP_Prescale':0.,  # only run NoReco lines
  'D2KPP_Postscale':1.,


  'Phi2KK_DaugPtMin':500.,
  'Phi2KK_PtMin':2000.,
  'Phi2KK_PIDK':5.,
  'Phi2KK_DaugP':0.,
  'Phi2KK_DaugTrkChi2':5.,
  'Phi2KK_DaugTrkGHP':0.05,
  'Phi2KK_VFASPFVCHI2':10.,
  'Phi2KK_ADAMASS':30.,
  'Phi2KK_ADOCACHI2CUT':30.,
  'Phi2KK_MIPCHI2':9.,
  'Phi2KK_Prescale':0., # only run NoReco lines
  'Phi2KK_Postscale':1.,
  
  'Jpsi2MM_DaugPtMin':0.,
  'Jpsi2MM_DaugP':0.,
  'Jpsi2MM_DaugTrkChi2':10.,
  'Jpsi2MM_VFASPFVCHI2':25.,
  'Jpsi2MM_ADAMASS':100.,
  'Jpsi2MM_ADOCACHI2CUT':30.,
  'Jpsi2MM_Prescale':0., # only run NoReco lines
  'Jpsi2MM_Postscale':1.,
  
  'Jpsi2MM_NoReco_Prescale' : 0.005,
  'Jpsi2MM_NoReco_Postscale' : 1.,
  'D02KPi_NoReco_Prescale' : 0.01,
  'D02KPi_NoReco_Postscale' : 1.,
  
  'HLT':"HLT_PASS_RE('Hlt1MBNoBiasDecision')",
  'HLT_Jpsi2MM_NoReco' : "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')",
  'HLT_D02KPi_NoReco' : "HLT_PASS_RE('Hlt2CharmHadD02HH_D02KPiWideMassDecision')"
  }
  }
  

#### Modified lines ####

##############################################################################
##
## Module for construction of Jpsi -> mu mu /  Upsilon -> mu mu /
## Z -> mu mu / B -> J/psi K to be used to measure tracking efficiency.
## Provides functions to build a Jpsi / Upsilon / Z / B selection
## with a long track and a muonTT track.
##
## Seeding station for Z candidates has been changed from M3 to M5
##
## Responsible: Ronan Wallace
## Test: Yes
##
##############################################################################
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
    'JpsiPrescale'                : 0, 
    'UpsilonPrescale'             : 0,
    'ZPrescale'                   : 1,   
    'BJpsiKPrescale'              : 0,
    'Postscale'                   : 1 
    }
    }
