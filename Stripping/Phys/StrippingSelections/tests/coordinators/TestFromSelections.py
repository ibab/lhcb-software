# $Id: $
# Test your line(s) of the stripping
#  
# NOTE: Please make a copy of this file for your testing, and do NOT change this one!
#

from Gaudi.Configuration import *
from Configurables import DaVinci
from StrippingConf.Configuration import StrippingConf

from CommonParticles.Utils import DefaultTrackingCuts
DefaultTrackingCuts().Cuts  = { "Chi2Cut" : [ 0, 4 ],
                                "GhostProbCut" : [ 0,   0.4   ],
                                "CloneDistCut" : [5000, 9e+99 ] }

#
#Raw event juggler to split Other/RawEvent into Velo/RawEvent and Tracker/RawEvent
#
from Configurables import RawEventJuggler
juggler = RawEventJuggler( DataOnDemand=True, Input=0.3, Output=4.1 )

#
#Fix for TrackEff lines
#
from Configurables import DecodeRawEvent
DecodeRawEvent().setProp("OverrideInputs",4.1)

from Configurables import ConfigCDBAccessSvc
ConfigCDBAccessSvc().File = '$STRIPPINGSELECTIONSROOT/tests/data/config.cdb'


# NOTE: this will work only if you inserted correctly the 
# default_config dictionary in the code where your LineBuilder 
# is defined.
from StrippingSelections import buildersConf
confs = buildersConf()
from StrippingSelections.Utils import lineBuilder, buildStreams
streams = buildStreams( confs)

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

#
# Configure the dst writers for the output
#
enablePacking = True

from DSTWriters.microdstelements import *
from DSTWriters.Configuration import ( SelDSTWriter,
                                       stripDSTStreamConf,
                                       stripDSTElements,
                                       stripMicroDSTStreamConf,
                                       stripMicroDSTElements,
                                       stripCalibMicroDSTStreamConf )

#
# Configuration of MicroDST
# per-event an per-line selective writing of the raw event is active (selectiveRawEvent=True)
#
mdstStreamConf = stripMicroDSTStreamConf(pack=enablePacking, selectiveRawEvent=True)
mdstElements   = stripMicroDSTElements(pack=enablePacking)

#
# Configuration of SelDSTWriter
# per-event an per-line selective writing of the raw event is active (selectiveRawEvent=True)
#
SelDSTWriterElements = {
    'default'               : stripDSTElements(pack=enablePacking),
    charmMicroDSTname       : mdstElements,
    leptonicMicroDSTname    : mdstElements,
    pidMicroDSTname         : mdstElements,
    bhadronMicroDSTname     : mdstElements
    }

SelDSTWriterConf = {
    'default'                : stripDSTStreamConf(pack=enablePacking, selectiveRawEvent=True),
    charmMicroDSTname        : mdstStreamConf,
    leptonicMicroDSTname     : mdstStreamConf,
    bhadronMicroDSTname      : mdstStreamConf,
    pidMicroDSTname          : stripCalibMicroDSTStreamConf(pack=enablePacking, selectiveRawEvent=True)
    }

dstWriter = SelDSTWriter( "MyDSTWriter",
                          StreamConf = SelDSTWriterConf,
                          MicroDSTElements = SelDSTWriterElements,
                          OutputFileSuffix ='000000',
                          SelectionSequences = sc.activeStreams() )


#
# Add stripping TCK
#
from Configurables import StrippingTCK
stck = StrippingTCK(HDRLocation = '/Event/Strip/Phys/DecReports', TCK=0x36772300)

#
#Configure DaVinci
#

# Change the column size of Timing table
from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
TimingAuditor().TIMER.NameSize = 60

from Configurables import AuditorSvc, ChronoAuditor
AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )

from Configurables import StrippingReport
sr = StrippingReport(Selections = sc.selections())

from Configurables import AlgorithmCorrelationsAlg
ac = AlgorithmCorrelationsAlg(Algorithms = list(set(sc.selections())))

## Configure PV refitter
from GaudiKernel.SystemOfUnits import micrometer
from Configurables import LoKi__PVReFitter
LoKi__PVReFitter("ToolSvc.LoKi::PVReFitter").CheckTracksByLHCbIDs = True
LoKi__PVReFitter("ToolSvc.LoKi::PVReFitter").DeltaChi2 = 0.01
LoKi__PVReFitter("ToolSvc.LoKi::PVReFitter").DeltaDistance = 5*micrometer

## Configure the VeloTrack unpacker
from Configurables import UnpackTrack
unpackIt = UnpackTrack("unpackIt")
unpackIt.InputName = "pRec/Track/FittedHLT1VeloTracks"
unpackIt.OutputName = "Rec/Track/FittedHLT1VeloTracks"

DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().EvtMax = 10000
DaVinci().PrintFreq = 2000
DaVinci().appendToMainSequence( [unpackIt] )
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
#DaVinci().appendToMainSequence( [ ac ] )
DaVinci().appendToMainSequence( [ dstWriter.sequence() ] )
DaVinci().ProductionType = "Stripping"
DaVinci().DataType  = "2015"
DaVinci().InputType = "DST"

# change the column size of timing table
from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
TimingAuditor().TIMER.NameSize = 60

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

# database
DaVinci().DDDBtag  = "dddb-20150526"
DaVinci().CondDBtag = "cond-20150625"

# input file
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco15_NoBias.py")
