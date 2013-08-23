# Config dictionaries for calibration lines (2012 restripping)
"""
Line config dictionaries for the PID and Calibration streams
(all WGs, 2012 incremental restripping).

Dictionary names should have the same name as the corresponding
StrippingSelections file containing the line builder instance.

   e.g  StrippingNoPIDDstarWithD02RSKPi linebuilder should be added as:

      StrippingNoPIDDstarWithD02RSKPi = { ... }
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
    "ProbNNk": 0.1,
    # Minimum Proton ANN PID
    "ProbNNp": 0.1,
    # Minimum Pion ANN PID
    "ProbNNpi": 0.1
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
