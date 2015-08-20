"""
Options for building Stripping20. 
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
from StrippingArchive.Utils import buildStreams, cloneLinesFromStream
from StrippingArchive import strippingArchive

stripping='stripping20'
#get the configuration dictionary from the database
config  = strippingConfiguration(stripping)
#get the line builders from the archive
archive = strippingArchive(stripping)

streams = buildStreams(stripping = config, archive = archive)

from Configurables import ProcStatusCheck
filterBadEvents = ProcStatusCheck()

# Charm control stream, for 10% of events from CharmToBeSwum
for stream in streams : 
    if stream.name() == 'CharmToBeSwum' : charmToBeSwumStream = stream

_CharmControl = StrippingStream("CharmControl")
_CharmControl.appendLines( cloneLinesFromStream( charmToBeSwumStream, 'CharmControl', prescale = 0.1 ) )
streams.append( _CharmControl )

sc = StrippingConf( Streams = streams,
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents, 
                    TESPrefix = 'Strip' )

# Add cone variables to Bhadron and Charm streams
from Configurables import AddExtraInfo, ConeVariables

bhadronOutputLocations = []
charmOutputLocations = []

for stream in streams :
    if stream.name() == 'Bhadron' : bhadronOutputLocations = [ "/Event/" + x for x in stream.outputLocations() ]
    if stream.name() == 'Charm' : charmOutputLocations = [ "/Event/" + x for x in stream.outputLocations() ]

bhadron_extra = AddExtraInfo('BhadronExtraInfo')
bhadron_extra.Inputs = bhadronOutputLocations

cv1   = ConeVariables('BhadronExtraInfo.ConeVariables1', ConeAngle = 1.5, ConeNumber = 1,
                       Variables = ['angle', 'mult', 'ptasy'] )
cv2   = ConeVariables('BhadronExtraInfo.ConeVariables2', ConeAngle = 1.7, ConeNumber = 2,
                       Variables = ['angle', 'mult', 'ptasy'] )
cv3   = ConeVariables('BhadronExtraInfo.ConeVariables3', ConeAngle = 1.0, ConeNumber = 3,
                       Variables = ['angle', 'mult', 'ptasy'] )

bhadron_extra.addTool( cv1 , 'BhadronExtraInfo.ConeVariables1')
bhadron_extra.addTool( cv2 , 'BhadronExtraInfo.ConeVariables2')
bhadron_extra.addTool( cv3 , 'BhadronExtraInfo.ConeVariables3')
bhadron_extra.Tools = [ 'ConeVariables/BhadronExtraInfo.ConeVariables1',
                        'ConeVariables/BhadronExtraInfo.ConeVariables2', 
                        'ConeVariables/BhadronExtraInfo.ConeVariables3', 
                         ]

charm_extra = AddExtraInfo('CharmExtraInfo')
charm_extra.Inputs = charmOutputLocations

charm_cv1   = ConeVariables('CharmExtraInfo.ConeVariables1', ConeAngle = 1.5, ConeNumber = 1,
                            Variables = ['angle', 'mult', 'ptasy'] )

charm_extra.addTool( charm_cv1 , 'CharmExtraInfo.ConeVariables1')
charm_extra.Tools = [ 'ConeVariables/CharmExtraInfo.ConeVariables1' ]

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
