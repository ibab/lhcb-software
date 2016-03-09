# $Id: $
# Test your line(s) of the stripping
#  
# NOTE: Please make a copy of this file for your testing, and do NOT change this one!
#

from Gaudi.Configuration import *
from Configurables import DaVinci
from StrippingConf.Configuration import StrippingConf

#
#Fix for TrackEff lines
#
#from Configurables import DecodeRawEvent
#DecodeRawEvent().setProp("OverrideInputs",4.2)

# Specify the name of your configuration
confname='Beauty2XGamma' #FOR USERS
    
# NOTE: this will work only if you inserted correctly the 
# default_config dictionary in the code where your LineBuilder 
# is defined.
from StrippingSelections import buildersConf
confs = buildersConf()
from StrippingSelections.Utils import lineBuilder, buildStreamsFromBuilder
#confs[confname]["CONFIG"]["SigmaPPi0CalPrescale"] = 0.5 ## FOR USERS, YOU ONLY NEED TO QUICKLY MODIFY CutName and NewValue (no need to recompile the package but please update the default_config before committing)
streams = buildStreamsFromBuilder(confs,confname)

#clone lines for CommonParticles overhead-free timing
print "Creating line clones for timing"
for s in streams:
    for l in s.lines:
        if "_TIMING" not in l.name():
            cloned = l.clone(l.name().strip("Stripping")+"_TIMING")
            s.appendLines([cloned])

#define stream names
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

#Configure DaVinci

DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().EvtMax = 10000
DaVinci().PrintFreq = 2000
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().appendToMainSequence( [ ac ] )
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

#
# Raw event juggler to split DAQ/RawEvent into FULL.DST format
#
from Configurables import GaudiSequencer, RawEventJuggler
jseq=GaudiSequencer("RawEventSplitSeq")
juggler=RawEventJuggler("rdstJuggler")
juggler.Sequencer=jseq
juggler.Input=0.3  # 2015 Online (Moore) format 
juggler.Output=4.2 # Reco15 format

#help(juggler)

#from Configurables import DaVinci
#DaVinci().prependToMainSequence( [jseq] )


# filter out events triggered exclusively by CEP lines
from Configurables import LoKi__HDRFilter   as HDRFilter
from DAQSys.Decoders import DecoderDB
Hlt2DecReportsDecoder=DecoderDB["HltDecReportsDecoder/Hlt2DecReportsDecoder"].setup()
HLTFilter2 = HDRFilter("LoKiHLT2Filter", Code =  "HLT_PASS_RE('Hlt2(?!Forward)(?!DebugEvent)(?!Lumi)(?!Transparent)(?!PassThrough)(?!LowMult).*Decision')"
                              , Location = Hlt2DecReportsDecoder.OutputHltDecReportsLocation)
otherseq=GaudiSequencer("filters")
otherseq.Members=[jseq,HLTFilter2]

DaVinci().EventPreFilters = [jseq,HLTFilter2]


#DaVinci().prependToMainSequence(HLTFilter2)

# input file
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco15a_Run164668.py")

