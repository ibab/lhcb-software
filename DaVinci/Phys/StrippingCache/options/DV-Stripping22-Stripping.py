"""
Options for building Stripping22. 
"""

stripping='stripping22'

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
#Fix for TrackEff lines
#
from Configurables import DecodeRawEvent
DecodeRawEvent().setProp("OverrideInputs",4.1)

#
# Build the streams and stripping object
#
from StrippingConf.Configuration import StrippingConf, StrippingStream
from StrippingSettings.Utils import strippingConfiguration
from StrippingArchive.Utils import buildStream
from StrippingArchive import strippingArchive

#get the configuration dictionary from the database
config  = strippingConfiguration(stripping)
#get the line builders from the archive
archive = strippingArchive(stripping)

stream = buildStream(stripping = config, streamName='ALL', archive = archive)

stream.lines=[l for l in stream.lines if (("TrackEffDown" not in l.name()))] #fix for Z02mumu bug

for l in stream.lines:
        if l.RequiredRawEvents and l._prescale==0:
                l.RequiredRawEvents=None

from Configurables import ProcStatusCheck
filterBadEvents = ProcStatusCheck()

sc = StrippingConf( Streams = [stream],
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents, 
                    TESPrefix = 'Strip' )

## Configure PV refitter
from Configurables import LoKi__PVReFitter
LoKi__PVReFitter("ToolSvc.LoKi::PVReFitter").CheckTracksByLHCbIDs = True

appMgr = ApplicationMgr()
appMgr.OutputLevel = 6
appMgr.ExtSvc += [ 'ToolSvc', 'AuditorSvc' ]

appMgr.HistogramPersistency = "ROOT"
ntSvc = NTupleSvc()
appMgr.ExtSvc += [ ntSvc ]

from Configurables import ( LHCbApp, PhysConf, AnalysisConf,
                            DstConf, LumiAlgsConf, DDDBConf )

#LHCbApp().DDDBtag   = "dddb-20150724"
#LHCbApp().CondDBtag = "cond-20150805"

# Can be enabled for next full stack release
PhysConf().OutputLevel     = appMgr.OutputLevel
#AnalysisConf().OutputLevel = appMgr.OutputLevel

datatype =  "2015"
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
