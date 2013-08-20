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

StrippingTrackEffMuonTT = {
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


