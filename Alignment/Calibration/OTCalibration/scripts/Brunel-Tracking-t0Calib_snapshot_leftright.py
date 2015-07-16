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
theApp.EvtMax = 100000
theApp.PrintFreq = 100
theApp.Simulation = False
theApp.OutputType = "NONE"

#theApp.RecoSequence = [ "Decoding", "VELO", "TT", "IT", "OT", "Tr", "Vertex" ]
from Configurables import RecMoniConf
RecMoniConf().setProp("MoniSequence", [ "OT" ] )

theApp.DDDBtag = "dddb-20150526"
theApp.CondDBtag = "cond-20150601"

# Setup CondDB
#from Configurables import CondDB
from Configurables import ( CondDBAccessSvc, CondDB )
from Gaudi.Configuration import appendPostConfigAction

conddb = CondDB()
conddb.IgnoreHeartBeat = True
#conddb.UseDBSnapshot = True
#conddb.EnableRunChangeHandler = True
#conddb.Tags['ONLINE'] = 'fake'
conddb.Tags = {
       "DDDB"    : theApp.getProp('DDDBtag'),
       "LHCBCOND": theApp.getProp('CondDBtag'),
       "SIMCOND" : theApp.getProp('CondDBtag'),
       "ONLINE"  : 'fake'
}

def overrideONLINEWithSnapshot():
 ONLINE_2015 = CondDBAccessSvc("ONLINE_2015")
 ONLINE_2015.ConnectionString = "sqlite_file:ONLINE-2015.db/ONLINE"
 conddb.addLayer(ONLINE_2015)
appendPostConfigAction(overrideONLINEWithSnapshot)

#conddb.addLayer(CondDBAccessSvc("t0_db",ConnectionString = "sqlite_file:OTIS_t0_2015_07_21_154882.db/LHCBCOND"))

#import AllHlt1
#conddb.RunChangeHandlerConditions = AllHlt1.ConditionMap



# Further selection
theApp.VetoHltErrorEvents = False
from Configurables import L0Conf
L0Conf().EnsureKnownTCK=False

from Configurables import TrajOTProjector
Projector = TrajOTProjector("OTProjector", UseDrift = True)

from TrackFitter.ConfiguredFitters import *
Fitter = ConfiguredMasterFitter("TrackMonFitter")
Fitter.Projector.OT = Projector
Fitter.MaxNumberOutliers = 0

#from Configurables import TrackListRefiner, TrackSelector
#refiner = TrackListRefiner("RefineOTTracks")
#refiner.inputLocation = "Rec/Track/Best"
#refiner.outputLocation = "Rec/Track/OTTracks"
#refiner.Selector = TrackSelector(name="OTTrackSelector")
#refiner.Selector.MinNOTHits = 18

#from Configurables import TrackListRefiner, TrackSelector
#refiner = TrackListRefiner("RefineOTTracks", inputLocation = "Rec/Track/Best", outputLocation = "Rec/Track/OTTracks",
#Selector = TrackSelector())
#refiner.Selector.MinNOTHits = 18
#refiner.Selector.MinPCut = 5000

#from Configurables import TrackContainerCopy
#trackCopy = TrackContainerCopy("ContainerCopy",
 #                              inputLocations = ["Rec/Track/OTTracks"],
  #                             outputLocation = "Rec/Track/myOTTracks")

#OT t0 calibration
from Configurables import ( GaudiSequencer, TrackHitAdder, TrackEventFitter, OTModuleClbrMon, TrackMon )
GaudiSequencer("MoniOTSeq").Members = [
  TrackHitAdder(TrackLocation = "Rec/Track/Best"),
  TrackEventFitter(TracksInContainer = "Rec/Track/Best", Fitter = Fitter),
  #refiner,
  #trackCopy,
  OTModuleClbrMon(
    TrackType = 3, # Long tracks                                                                  
    MinP = 5., # GeV
    MaxTrackChi2 = 100,
    MaxTrackUbChi2 = 2,
    ReadXMLs = False,
    TrackLocation = "Rec/Track/Best",
    Fitter = Fitter,
    Projector = Projector,
    OTIS_LR_calibration = True
  )
]

NTupleSvc().Output = [ "FILE1 DATAFILE='clbr.root' TYP='ROOT' OPT='NEW'" ]
HistogramPersistencySvc().OutputFile = "clbr_hists.root"

