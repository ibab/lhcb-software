###############################################################################
# File for running Brunel with default options (2008 real data,.mdf in,.dst out)
###############################################################################
# Syntax is:
#   gaudirun.py Brunel-Default.py <someDataFiles>.py
###############################################################################

from Configurables import Brunel

from Gaudi.Configuration import *

###############################################################################
# Set here any steering options.
# Available steering options and defaults are documented in
# $BRUNELROOT/python/Brunel/Configuration.py
###############################################################################

# Just instantiate the configurable...
theApp = Brunel()
theApp.DataType   = "2015"
theApp.InputType  = "MDF"
theApp.EvtMax = 100
theApp.PrintFreq = 100
theApp.Simulation = False
theApp.OutputType = "NONE"

#theApp.RecoSequence = [ "Decoding", "VELO", "TT", "IT", "OT", "Tr", "Vertex" ]
from Configurables import RecMoniConf
RecMoniConf().setProp("MoniSequence", [ "OT" ] )

theApp.DDDBtag = "dddb-20150526"
theApp.CondDBtag = "cond-20150601"

# Setup CondDB
from Configurables import CondDB
conddb = CondDB()
conddb.IgnoreHeartBeat = True
#if running online
#conddb.UseDBSnapshot = True
#conddb.EnableRunChangeHandler = True
#conddb.Tags['ONLINE'] = 'fake'
#import AllHlt1
#conddb.RunChangeHandlerConditions = AllHlt1.ConditionMap

# Further selection
#theApp.VetoHltErrorEvents = False
#from Configurables import L0Conf
#L0Conf().EnsureKnownTCK=False

from Configurables import TrajOTProjector
Projector = TrajOTProjector("OTProjector", UseDrift = True)

from TrackFitter.ConfiguredFitters import *
Fitter = ConfiguredMasterFitter("TrackMonFitter")
Fitter.Projector.OT = Projector
Fitter.MaxNumberOutliers = 0

#OT t0 calibration
from Configurables import ( GaudiSequencer, TrackHitAdder, TrackEventFitter, OTModuleClbrMon, TrackMon )
GaudiSequencer("MoniOTSeq").Members = [
  TrackHitAdder(TrackLocation = "Rec/Track/Best"),
  TrackEventFitter(TracksInContainer = "Rec/Track/Best", Fitter = Fitter),
  OTModuleClbrMon(
    TrackType = 3, # Long tracks                                                                  
    MinP = 3.2, # GeV
    MaxTrackChi2 = 100,
    MaxTrackUbChi2 = 2,
    ReadXMLs = False,
    TrackLocation = "Rec/Track/Best",
    Fitter = Fitter,
    Projector = Projector
  )
]

NTupleSvc().Output = [ "FILE1 DATAFILE='clbr.root' TYP='ROOT' OPT='NEW'" ]
HistogramPersistencySvc().OutputFile = "clbr_hists.root"
