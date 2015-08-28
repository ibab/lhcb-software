"""
Options for building Stripping20r0p3. 
"""

from Gaudi.Configuration import *
MessageSvc().Format = "% F%30W%S%7W%R%T %0W%M"

# Tighten Trk Chi2 to <3
from CommonParticles.Utils import DefaultTrackingCuts
DefaultTrackingCuts().Cuts  = { "Chi2Cut" : [ 0, 3 ],
                                "CloneDistCut" : [5000, 9e+99 ] }

#
# Build the streams and stripping object
#
from StrippingConf.Configuration import StrippingConf, StrippingStream
from StrippingSettings.Utils import strippingConfiguration
from StrippingArchive.Utils import buildStreams
from StrippingArchive import strippingArchive

stripping='stripping20r0p3'
config  = strippingConfiguration(stripping)
archive = strippingArchive(stripping)

streams = buildStreams(stripping = config, archive = archive)

stripTESPrefix = 'Strip'
from Configurables import ProcStatusCheck
filterBadEvents = ProcStatusCheck()

sc = StrippingConf( Streams = streams,
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents, 
                    TESPrefix = stripTESPrefix )

appMgr = ApplicationMgr()
appMgr.OutputLevel = 6
appMgr.ExtSvc += [ 'ToolSvc', 'AuditorSvc' ]

appMgr.HistogramPersistency = "ROOT"
ntSvc = NTupleSvc()
appMgr.ExtSvc += [ ntSvc ]

from Configurables import ( LHCbApp, PhysConf, AnalysisConf,
                            DstConf, LumiAlgsConf, DDDBConf )

# Can be enabled for next full stack release
PhysConf().OutputLevel     = appMgr.OutputLevel
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
