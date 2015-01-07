
from Gaudi.Configuration import *

from Configurables import CondDB
#CondDB(UseOracle = True)
#importOptions("$APPCONFIGOPTS/DisableLFC.py")

from Configurables import Brunel
Brunel().DataType = "2012"
Brunel().Simulation = True
#Brunel().InputType = "MDF"
Brunel().InputType = "DST"

from Configurables import LHCbApp
#LHCbApp().DDDBtag   = "default"
#LHCbApp().CondDBtag = "default"
LHCbApp().DDDBtag   = "dddb-20120831"
LHCbApp().CondDBtag = "sim-20121025-vc-md100"
#LHCbApp().SIMCONDDBtag = "sim-20121025-vc-md100"

Brunel().EvtMax = 300
Brunel().PrintFreq = 100

from Configurables import TrajOTProjector
Projector = TrajOTProjector("OTProjector", UseDrift = True)

from TrackFitter.ConfiguredFitters import *
Fitter = ConfiguredMasterFitter("TrackMonFitter")
Fitter.Projector.OT = Projector
Fitter.MaxNumberOutliers = 2

from Configurables import ( GaudiSequencer, TrackHitAdder, TrackEventFitter, OTModuleClbrMon, TrackMon )
GaudiSequencer("MoniOTSeq").Members = [
#  TrackHitAdder(TrackLocation = "Rec/Track/Best"),
#  TrackEventFitter(TracksInContainer = "Rec/Track/Best", Fitter = Fitter),
  OTModuleClbrMon(
    TrackType = 3, # Long tracks
    MinP = 3.2, # GeV
    MaxTrackChi2 = 100,
    MaxTrackUbChi2 = 2,
    ReadXMLs = False,
    Simulation = Brunel().Simulation,
    Apply_Calibration = False,
    Verbose = False,
    xmlFilePath = "/afs/cern.ch/user/l/lgrillo/databases_for_online/OT-test-withMC/results.xml",
    TrackLocation = "Rec/Track/Best",
    Fitter = Fitter,
    Projector = Projector
  ),
  TrackMon(
    TrackType = 3, # Long tracks
    MinP = 3.2, # GeV
    MaxTrackChi2 = 100,
    TrackLocation = "Rec/Track/Best",
    Fitter = Fitter,
    Projector = Projector
  )
]

Brunel().OutputType = "None"
Brunel().Histograms = "None"

Brunel().RecoSequence = [ "Decoding", "VELO", "TT", "IT", "OT", "Tr", "Vertex","PROTO" ]
#Brunel().RecoSequence = [ "Decoding", "VELO", "TT", "IT", "OT", "Tr", "Vertex"," " ]
from Configurables import RecMoniConf
RecMoniConf().setProp("MoniSequence", [ "OT" ] )



#from Configurables import ( CondDBAccessSvc, CondDB )
#CondDB().addLayer(CondDBAccessSvc("t0db",
#  ConnectionString = "sqlite_file:/afs/cern.ch/user/a/akozlins/public/OT/LHCBCOND/2011-05.v2-walk.db/LHCBCOND")
#)



'''
from Configurables import ApplicationMgr, DataOnDemandSvc
ApplicationMgr().ExtSvc += [ "DataOnDemandSvc", "ANNDispatchSvc" ]
DataOnDemandSvc().AlgMap["Trig/L0/L0DUReport"] = L0DUFromRawAlg(OutputLevel = 4)
DataOnDemandSvc().AlgMap["Hlt/DecReports"] = HltDecReportsDecoder(OutputLevel = 4)
'''
'''
from Configurables import LoKi__VoidFilter as VoidFilter
GaudiSequencer("BrunelSequencer").Members.append(
  VoidFilter("VoidFilter",
    Code = "SKIP(100)"
  )
)
'''

'''
from Configurables import L0DUFromRawAlg
GaudiSequencer("BrunelSequencer").Members.append(L0DUFromRawAlg(OutputLevel = 4))

from Configurables import LoKi__L0Filter as L0Filter
GaudiSequencer("BrunelSequencer").Members.append(
  L0Filter("filterSumEtPrev2",
    Preambulo = [ "from LoKiCore.functions import *" ],
    Code      = "L0_SUMET(-2) < 50"
  )
)
'''
'''
from Configurables import HltDecReportsDecoder
GaudiSequencer("BrunelSequencer").Members.append(HltDecReportsDecoder(OutputLevel = 4))

from Configurables import LoKi__HDRFilter as HDRFilter
GaudiSequencer("BrunelSequencer").Members.append(
  HDRFilter('Hlt1MBNoBias',
    Preambulo = [ "from LoKiCore.functions import *" ],
    Code = "HLT_PASS_RE('Hlt1MBNoBiasDecision')"
  )
)
'''
