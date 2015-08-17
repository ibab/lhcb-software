# $Id: $
# Test your line(s) of the stripping by taking the dictionaries from StrippingSettings
#  
# NOTE: Please make a copy of this file for your testing, and do NOT change this one!
#

#set stripping version
stripping="stripping23"

#use CommonParticlesArchive
from CommonParticlesArchive import CommonParticlesArchiveConf
CommonParticlesArchiveConf().redirect(stripping)

from Gaudi.Configuration import *
MessageSvc().Format = "% F%30W%S%7W%R%T %0W%M"

from Gaudi.Configuration import *
from StrippingConf.Configuration import StrippingConf

# Tighten Trk Chi2 to <3
from CommonParticles.Utils import DefaultTrackingCuts
DefaultTrackingCuts().Cuts  = { "Chi2Cut" : [ 0, 3 ],
                                "CloneDistCut" : [5000, 9e+99 ] }

#
#Raw event juggler to split Other/RawEvent into Velo/RawEvent and Tracker/RawEvent
#
from Configurables import RawEventJuggler
juggler = RawEventJuggler( DataOnDemand=True, Input=0.3, Output=4.1 )

#
#Fix for TrackEff lines
#
#from Configurables import DecodeRawEvent
#DecodeRawEvent().setProp("OverrideInputs",4.1)

#from Configurables import ConfigCDBAccessSvc
#confDBAcc = ConfigCDBAccessSvc()
#confDBAcc.File = '/afs/cern.ch/user/s/sperazzi/public/Stripping/S23/DaVinciDev_v37r0/Phys/StrippingSelections/tests/data/config.cdb'

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

from Configurables import ProcStatusCheck

sc = StrippingConf( Streams = streams,
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = ProcStatusCheck(),
                    TESPrefix = stripTESPrefix,
                    ActiveMDSTStream = True,
                    Verbose = True,
                    DSTStreams = dstStreams,
                    MicroDSTStreams = mdstStreams )

## Configure PV refitter
from GaudiKernel.SystemOfUnits import micrometer
from Configurables import LoKi__PVReFitter
LoKi__PVReFitter("ToolSvc.LoKi::PVReFitter").CheckTracksByLHCbIDs = True
LoKi__PVReFitter("ToolSvc.LoKi::PVReFitter").DeltaChi2 = 0.01
LoKi__PVReFitter("ToolSvc.LoKi::PVReFitter").DeltaDistance = 5*micrometer

from Configurables import ApplicationMgr, AuditorSvc, SequencerTimerTool

# Initial IOV time
# http://www.onlineconversion.com/unix_time.htm
# values in ns (so multiply values from above link by 1e9)
from Configurables import EventClockSvc
EventClockSvc().EventTimeDecoder = "OdinTimeDecoder"

appMgr = ApplicationMgr()
appMgr.OutputLevel = 6
appMgr.ExtSvc += [ 'ToolSvc', 'AuditorSvc' ]

appMgr.HistogramPersistency = "ROOT"
ntSvc = NTupleSvc()
appMgr.ExtSvc += [ ntSvc ]

from Configurables import ( LHCbApp, PhysConf, AnalysisConf,
                            DstConf, LumiAlgsConf, DDDBConf, CondDB )

# Just to initialise
#condDB = CondDB()

LHCbApp().DDDBtag   = "dddb-20150724"
LHCbApp().CondDBtag = "cond-20150805"

# Can be enabled for next full stack release
#PhysConf().OutputLevel     = appMgr.OutputLevel
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
