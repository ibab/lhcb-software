
from Gaudi.Configuration import *

#from Configurables import CondDB
#CondDB(UseOracle = True)
#importOptions("$APPCONFIGOPTS/DisableLFC.py")

from Configurables import Brunel
Brunel().DataType = "2012"
Brunel().Simulation = False
Brunel().InputType = "MDF"

Brunel().EvtMax = 100


from Configurables import LHCbApp
LHCbApp().DDDBtag   = "default"
LHCbApp().CondDBtag = "default"
#LHCbApp().DDDBtag = 'dddb-20130503-1'                                                                                                                             
#LHCbApp().CondDBtag = 'cond-20130710'

from Configurables import TrajOTProjector
Projector = TrajOTProjector("OTProjector", UseDrift = True)

from TrackFitter.ConfiguredFitters import *
Fitter = ConfiguredMasterFitter("TrackMonFitter")
Fitter.Projector.OT = Projector
Fitter.MaxNumberOutliers = 0

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

Brunel().OutputType = "None"
Brunel().Histograms = "None"

Brunel().RecoSequence = [ "Decoding", "VELO", "TT", "IT", "OT", "Tr", "Vertex" ]
from Configurables import RecMoniConf
RecMoniConf().setProp("MoniSequence", [ "OT" ] )

path_monolayer = "/afs/cern.ch/user/w/wouter/public/AlignDB/"
ddbs = []
ddbs.append(path_monolayer + "OTMonoGeometry.db/DDDB")
ddbs.append(path_monolayer + "OTMonoCatalog.db/LHCBCOND")
ddbs.append(path_monolayer + "OTMonoAlign20141225.db/LHCBCOND")

counter = 1
for db in ddbs:
     from Configurables import ( CondDB, CondDBAccessSvc )
     alignCond = CondDBAccessSvc( 'AlignDDDB' + str(counter) )
     alignCond.ConnectionString = 'sqlite_file:' + db
     CondDB().addLayer( alignCond )
     counter += 1

#from Configurables import ( CondDBAccessSvc, CondDB )
#CondDB().addLayer(CondDBAccessSvc("db_global",
#  ConnectionString = "sqlite_file:alignDB.early2012.2days.jpsi.db/LHCBCOND")
#)
