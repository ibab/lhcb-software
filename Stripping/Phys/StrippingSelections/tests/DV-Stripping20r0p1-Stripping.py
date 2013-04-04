"""
Options for building Stripping20r1. 
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
from StrippingSelections.Utils import buildStreams
from StrippingArchive import strippingArchive

stripping='stripping20r0p1'
config  = strippingConfiguration(stripping)

streams = buildStreams(stripping = config, WGs = [ 'BandQ' ] )

from Configurables import ProcStatusCheck
filterBadEvents = ProcStatusCheck()

sc = StrippingConf( Streams = streams,
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents, 
                    TESPrefix = 'Strip' )

enablePacking = True

from DSTWriters.microdstelements import *
from DSTWriters.Configuration import (SelDSTWriter,
                                      stripDSTStreamConf,
                                      stripDSTElements,
                                      stripMicroDSTStreamConf,
                                      stripMicroDSTElements,
                                      stripCalibMicroDSTStreamConf
                                      )

#
# Configuration of MicroDST
#
mdstStreamConf = stripMicroDSTStreamConf(pack=enablePacking)
mdstElements   = stripMicroDSTElements(pack=enablePacking)

leptonicMicroDSTname   = 'Leptonic'
charmMicroDSTname      = 'Charm'
pidMicroDSTname        = 'PID' 
bhadronMicroDSTname    = 'Bhadron' 

#
# Configuration of SelDSTWriter
#
SelDSTWriterElements = {
    'default'               : stripDSTElements(pack=enablePacking),
    charmMicroDSTname       : mdstElements,
    leptonicMicroDSTname    : mdstElements,
    pidMicroDSTname         : mdstElements,
    bhadronMicroDSTname     : mdstElements
    }


SelDSTWriterConf = {
    'default'                : stripDSTStreamConf(pack=enablePacking),
    charmMicroDSTname        : mdstStreamConf,
    leptonicMicroDSTname     : mdstStreamConf,
    bhadronMicroDSTname      : mdstStreamConf,
    pidMicroDSTname          : stripCalibMicroDSTStreamConf(pack=enablePacking)
    }


dstWriter = SelDSTWriter( "MyDSTWriter",
                          StreamConf = SelDSTWriterConf,
                          MicroDSTElements = SelDSTWriterElements,
                          OutputFileSuffix ='000000',
                          SelectionSequences = sc.activeStreams()
                          )


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
#charm_extra.MaxLevel = 1

charm_cv1   = ConeVariables('CharmExtraInfo.ConeVariables1', ConeAngle = 1.5, ConeNumber = 1,
                            Variables = ['angle', 'mult', 'ptasy'] )

charm_extra.addTool( charm_cv1 , 'CharmExtraInfo.ConeVariables1')
charm_extra.Tools = [ 'ConeVariables/CharmExtraInfo.ConeVariables1' ]


# Add stripping TCK 
from Configurables import StrippingTCK
stck = StrippingTCK(HDRLocation = '/Event/Strip/Phys/DecReports', TCK=0x32232001)

#
# DaVinci Configuration
#
from Configurables import DaVinci
DaVinci().EvtMax = 100000                        # Number of events
DaVinci().HistogramFile = "DVHistos.root"
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ bhadron_extra, charm_extra ] )
DaVinci().appendToMainSequence( [ stck ] )
DaVinci().appendToMainSequence( [ dstWriter.sequence() ] )
DaVinci().ProductionType = "Stripping"

# Change the column size of Timing table
from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
TimingAuditor().TIMER.NameSize = 60

# For tests
from Configurables import ApplicationMgr
ApplicationMgr().ExtSvc += [ 'AuditorSvc' ]
#AuditorSvc().Auditors += [ 'MemoryAuditor' ]
from Configurables import AuditorSvc, ChronoAuditor
AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )
from Configurables import StrippingReport
sr = StrippingReport(Selections = sc.selections(), OnlyPositive = False, EveryEvent = False, ReportFrequency=2000)
DaVinci().appendToMainSequence( [ sr ] )


DaVinci().DataType  = "2012"
DaVinci().InputType = "DST"

# change the column size of timing table
from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
TimingAuditor().TIMER.NameSize = 60

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

# database
DaVinci().DDDBtag  = "dddb-20120831"
DaVinci().CondDBtag = "cond-20121008"

# input file
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco14_Run125113.py")
