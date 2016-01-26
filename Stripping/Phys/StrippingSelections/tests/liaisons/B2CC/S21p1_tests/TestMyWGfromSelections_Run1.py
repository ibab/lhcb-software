# $Id: $
# Test your line(s) of the stripping
#  
# NOTE: Please make a copy of this file for your testing, and do NOT change this one!
#

#use CommonParticlesArchive
from CommonParticlesArchive import CommonParticlesArchiveConf
CommonParticlesArchiveConf().redirect("stripping21r0p1")

from Gaudi.Configuration import *
from Configurables import DaVinci
from StrippingConf.Configuration import StrippingConf

# Tighten Trk Chi2 to <3
from CommonParticles.Utils import DefaultTrackingCuts
DefaultTrackingCuts().Cuts  = { "Chi2Cut" : [ 0, 3 ],
                                "CloneDistCut" : [5000, 9e+99 ] }

#
#Raw event juggler to split Other/RawEvent into Velo/RawEvent and Tracker/RawEvent
#
from Configurables import RawEventJuggler
juggler = RawEventJuggler( DataOnDemand=True, Input=2.0, Output=4.2 )

#
#Fix for TrackEff lines
#
from Configurables import DecodeRawEvent
DecodeRawEvent().setProp("OverrideInputs",4.2)

# Specify the name of your configuration
my_wg='B2CC' #FOR LIAISONS


# NOTE: this will work only if you inserted correctly the 
# default_config dictionary in the code where your LineBuilder 
# is defined.
from StrippingSelections import buildersConf
confs = buildersConf()
from StrippingSelections.Utils import lineBuilder, buildStreams
streams = buildStreams( confs, WGs=my_wg )

leptonicMicroDSTname   = 'Leptonic'
charmMicroDSTname      = 'Charm'
pidMicroDSTname        = 'PID'
bhadronMicroDSTname    = 'Bhadron'
mdstStreams = [ leptonicMicroDSTname,charmMicroDSTname,pidMicroDSTname,bhadronMicroDSTname ]
dstStreams  = [ "BhadronCompleteEvent", "CharmCompleteEvent", "CharmToBeSwum", "Dimuon",
                "EW", "Semileptonic", "Calibration", "MiniBias", "Radiative" ]

stripTESPrefix = 'Strip'

from Configurables import ProcStatusCheck

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


# Items that might get lost when running the CALO+PROTO ReProcessing in DV
caloProtoReprocessLocs = [ "/Event/pRec/ProtoP#99", "/Event/pRec/Calo#99" ]

# Make sure they are present on full DST streams
SelDSTWriterConf['default'].extraItems += caloProtoReprocessLocs

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
DaVinci().EvtMax = 1000
DaVinci().PrintFreq = 100
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
#DaVinci().appendToMainSequence( [ ac ] )
DaVinci().appendToMainSequence( [ dstWriter.sequence() ] )
DaVinci().ProductionType = "Stripping"
DaVinci().DataType  = "2012"
DaVinci().InputType = "DST"

# change the column size of timing table
from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
TimingAuditor().TIMER.NameSize = 60

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

# database
#DaVinci().DDDBtag  = "dddb-20120831"
#DaVinci().CondDBtag = "cond-20121008"
DaVinci().CondDBtag = "cond-20150409-1"
DaVinci().DDDBtag = "dddb-20150928" 

# RedoCalo
importOptions("$STRIPPINGSELECTIONSROOT/tests/users/DV-RedoCaloPID-Stripping21.py")

# input file
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco14_Run125113.py")
