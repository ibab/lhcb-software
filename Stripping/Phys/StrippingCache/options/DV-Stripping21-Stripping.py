"""
Options for building Stripping21

New features:
- RAW EVENT: copy of the raw event (trigger excepted) is removed by default and replaced by selective writing of the needed raw banks
  on a per-event and per-line basis. In addition "Other/RawEvent" has been splitted into "Velo/RawEvent" and "Tracker/RawEvent".
  The raw banks needed are specified as an options of the StrippingLine class (e.g. StrippingLine.RequiredRawEvents = ["Velo"], default is an empty list).
  Valid raw banks are "Velo","Tracker","Muon","Rich","Calo"
  This reuquires the usage of RawEventJuggler to separate the Other RAW banks into Velo and Tracker
- MDST.DST: MDST.DST support for lines going to micro-DST is now granted on a per-line base. The support is enabled with line option (StrippingLine.MDSTFlag = True, default is False).
- Usage of CommonParticlesArchive
"""
#set stripping version
stripping="stripping21"

#use CommonParticlesArchive
from CommonParticlesArchive import CommonParticlesArchiveConf
CommonParticlesArchiveConf().redirect(stripping)

from Gaudi.Configuration import *
MessageSvc().Format = "% F%30W%S%7W%R%T %0W%M"

# Tighten Trk Chi2 to <3
from CommonParticles.Utils import DefaultTrackingCuts
DefaultTrackingCuts().Cuts  = { "Chi2Cut" : [ 0, 3 ],
                                "CloneDistCut" : [5000, 9e+99 ] }

#
#Raw event juggler to split Other/RawEvent into Velo/RawEvent and Tracker/RawEvent
#
from Configurables import RawEventJuggler
juggler = RawEventJuggler( DataOnDemand=True, Input=2.0, Output=4.0 )

#
# Build the streams and stripping object
#
from StrippingConf.Configuration import StrippingConf, StrippingStream
from StrippingSettings.Utils import strippingConfiguration
from StrippingArchive.Utils import buildStreams
from StrippingArchive import strippingArchive

config  = strippingConfiguration(stripping)
archive = strippingArchive(stripping)

streams = buildStreams(stripping = config, archive = archive)
leptonicMicroDSTname   = 'Leptonic'
charmMicroDSTname      = 'Charm'
pidMicroDSTname        = 'PID'
bhadronMicroDSTname    = 'Bhadron'
mdstStreams = [ leptonicMicroDSTname,charmMicroDSTname,pidMicroDSTname,bhadronMicroDSTname ]
dstStreams  = [ "BhadronCompleteEvent", "CharmCompleteEvent", "CharmToBeSwum", "Dimuon",
                "EW", "Semileptonic", "Calibration", "MiniBias", "Radiative" ]

stripTESPrefix = 'Strip'

from Configurables import ProcStatusCheck, GaudiSequencer
from PhysConf.Filters import LoKi_Filters
flts = LoKi_Filters(VOID_Code = "( TrSource(TrSOURCE('/Event/Rec/Track/Best', TrLONG))"\
                                " >> ( sum( TrPT,TrP < 1 * TeV ) > 1 * TeV ) )" ,
                    VOID_Preambulo = ["from LoKiTracks.decorators import *" ,
                                      "from LoKiCore.functions    import * ",
                                      "from GaudiKernel.SystemOfUnits import *"])
filterBadEvents = GaudiSequencer("BadEventFilter",
                                  ModeOR = True,
                                  Members = [ flts.sequencer("GECFilter"),
                                              ProcStatusCheck() ] )
streamFilter = { 'default'  : filterBadEvents,
                 'MiniBias' : ProcStatusCheck() }

sc = StrippingConf( Streams = streams,
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = streamFilter,
                    TESPrefix = stripTESPrefix,
                    Verbose = True,
                    DSTStreams = dstStreams,
                    MicroDSTStreams = mdstStreams )

appMgr = ApplicationMgr()
appMgr.OutputLevel = 6
appMgr.ExtSvc += [ 'ToolSvc', 'AuditorSvc' ]

appMgr.HistogramPersistency = "ROOT"
ntSvc = NTupleSvc()
appMgr.ExtSvc += [ ntSvc ]

from Configurables import ( LHCbApp, PhysConf, AnalysisConf,
                            DstConf, LumiAlgsConf, DDDBConf )

# Can be enabled for next full stack release
#PhysConf().OutputLevel     = appMgr.OutputLevel
#AnalysisConf().OutputLevel = appMgr.OutputLevel

datatype =  "2012"
PhysConf().DataType      = datatype
AnalysisConf().DataType  = datatype
LumiAlgsConf().DataType  = datatype
DDDBConf().DataType      = datatype

inputType = "DST"
LumiAlgsConf().InputType = inputType
PhysConf().InputType     = inputType

unPack = ["Reconstruction"]
PhysConf().EnableUnpack = unPack
DstConf().EnableUnpack  = unPack

lumiSeq = GaudiSequencer("LumiSeq")
LumiAlgsConf().LumiSequencer = lumiSeq

appMgr.TopAlg += [ PhysConf().initSequence(),
                   AnalysisConf().initSequence(),
                   sc.sequence(), lumiSeq ]

#from Configurables import DaVinci
#DaVinci().ProductionType = "Stripping"
#DaVinci().DataType   = datatype
#DaVinci().DDDBtag    = LHCbApp().DDDBtag
#DaVinci().CondDBtag  = LHCbApp().CondDBtag
#DaVinci().appendToMainSequence( [ sc.sequence() ] )
