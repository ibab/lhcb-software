# $Id: $
# Test your line(s) of the stripping by taking the dictionaries from StrippingSettings
#  
# NOTE: Please make a copy of this file for your testing, and do NOT change this one!
#

from Gaudi.Configuration import *
from Configurables import DaVinci
from StrippingConf.Configuration import StrippingConf

#
#Raw event juggler to split Other/RawEvent into Velo/RawEvent and Tracker/RawEvent
#
from Configurables import RawEventJuggler
juggler = RawEventJuggler( DataOnDemand=True, Input=0.3, Output=4.2 )

#
#Fix for TrackEff lines
#
from Configurables import DecodeRawEvent
DecodeRawEvent().setProp("OverrideInputs",4.2)

# Specify the name of your configuration
stripping='stripping24'

# NOTE: this will work only if you inserted correctly the 
# default_config dictionary in the code where your LineBuilder 
# is defined.
from StrippingSettings.Utils import strippingConfiguration
from StrippingSelections import buildersConf
conf=strippingConfiguration(stripping)
from StrippingSelections.Utils import lineBuilder, buildStreams
streams = buildStreams( conf )

leptonicMicroDSTname   = 'Leptonic'
charmMicroDSTname      = 'Charm'
pidMicroDSTname        = 'PID'
bhadronMicroDSTname    = 'Bhadron'
mdstStreams = [ leptonicMicroDSTname,charmMicroDSTname,pidMicroDSTname,bhadronMicroDSTname ]
dstStreams  = [ "BhadronCompleteEvent", "CharmCompleteEvent", "Dimuon",
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
#from Configurables import StrippingTCK
#stck = StrippingTCK(HDRLocation = '/Event/Strip/Phys/DecReports', TCK=0x36112100)

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

DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().EvtMax = 10000
DaVinci().PrintFreq = 100
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
#DaVinci().appendToMainSequence( [ ac ] )
DaVinci().appendToMainSequence( [ dstWriter.sequence() ] )
DaVinci().ProductionType = "Stripping"
DaVinci().DataType  = "2015"
DaVinci().InputType = "RDST"

# change the column size of timing table
from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
TimingAuditor().TIMER.NameSize = 60

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

# database
DaVinci().DDDBtag   = "dddb-20150724"
DaVinci().CondDBtag = "cond-20150828"

# input file
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco15a_Run164668.py")
