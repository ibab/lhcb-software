#  File to activate BLS hits checking
#
#  @author V.Talanov
#  @date 2009-10-30
#
from Gauss.Configuration import *
from Gaudi.Configuration import *

#-- Get instance(s) of BlsHitCHecker
#
from Configurables import BlsHitChecker

#-- An example how to activate monitoring for both BLS counters
#   Sets ALL possible options of BlsHitChecker to their defaults
#
BlsHitCheckerAll = BlsHitChecker( "BlsHitCheckerAll" )
GaudiSequencer( "DetectorsMonitor" ).Members += [ BlsHitCheckerAll ]
BlsHitCheckerAll.HistoDir = "BlsHitChecker/BlsHitCheckerAll"
BlsHitCheckerAll.OutputLevel = INFO
BlsHitCheckerAll.BlsAOn = TRUE
BlsHitCheckerAll.BlsCOn = TRUE
BlsHitCheckerAll.HistogramTitlePrefix = "BlsA+C: "
BlsHitCheckerAll.EntryXMin = -150.0 * SystemOfUnits.mm
BlsHitCheckerAll.EntryXMax = +150.0 * SystemOfUnits.mm
BlsHitCheckerAll.EntryXNbins = 300
BlsHitCheckerAll.EntryYMin = -150.0 * SystemOfUnits.mm
BlsHitCheckerAll.EntryYMax = +150.0 * SystemOfUnits.mm
BlsHitCheckerAll.EntryYNbins = 300
BlsHitCheckerAll.EntryZMin = -2200.0 * SystemOfUnits.mm
BlsHitCheckerAll.EntryZMax = -1900.0 * SystemOfUnits.mm
BlsHitCheckerAll.EntryZNbins = 300
BlsHitCheckerAll.EntryTimeOffset = +0.0 * SystemOfUnits.ns
BlsHitCheckerAll.EntryTimeMin = -50.0 * SystemOfUnits.ns
BlsHitCheckerAll.EntryTimeMax = +50.0 * SystemOfUnits.ns
BlsHitCheckerAll.EntryTimeNbins = 100
BlsHitCheckerAll.EventNumMin = 0.0
BlsHitCheckerAll.EventNumMax = 1000.0
BlsHitCheckerAll.EventNumNbins = 1000
BlsHitCheckerAll.EventNumTracksMin = 0.0
BlsHitCheckerAll.EventNumTracksMax = 50.0
BlsHitCheckerAll.EventNumTracksNbins = 50
BlsHitCheckerAll.TrackEnDepMin = 0.0
BlsHitCheckerAll.TrackEnDepMax = 50.0
BlsHitCheckerAll.TrackEnDepNbins = 50
BlsHitCheckerAll.TrackLengthMin = 0.0
BlsHitCheckerAll.TrackLengthMax = 7.0
BlsHitCheckerAll.TrackLengthNbins = 70

#-- Minimal set of options to active monitoring per each BLS
#
BlsHitCheckerBlsA = BlsHitChecker( "BlsHitCheckerBlsA" )
GaudiSequencer( "DetectorsMonitor" ).Members += [ BlsHitCheckerBlsA ]
BlsHitCheckerBlsA.HistoDir = "BlsHitChecker/BlsHitCheckerBlsA"
BlsHitCheckerBlsA.BlsAOn = TRUE
BlsHitCheckerBlsA.HistogramTitlePrefix = "BlsA: "

BlsHitCheckerBlsC = BlsHitChecker( "BlsHitCheckerBlsC" )
GaudiSequencer( "DetectorsMonitor" ).Members += [ BlsHitCheckerBlsC ]
BlsHitCheckerBlsC.HistoDir = "BlsHitChecker/BlsHitCheckerBlsC"
BlsHitCheckerBlsC.BlsCOn = TRUE
BlsHitCheckerBlsC.HistogramTitlePrefix = "BlsC: "

# BlsMoniSim.py
