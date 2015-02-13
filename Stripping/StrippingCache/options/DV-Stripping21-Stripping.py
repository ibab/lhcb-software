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

log.warning(100*"#")
log.warning(100*"#")
log.warning(100*"#")
log.warning("# some adjustment by Vanya Belyaev" )

for s in streams :

    _lines = []
    for l in s.lines :
        if 0 <= l.name().upper().find('D02KSKS') :
            log.warning("# VB: REMOVE %s" % l.name() )
            continue
        if 0 <= l.name().upper().find('BS2KSKS') :
            log.warning("# VB: REMOVE %s" % l.name() )
            continue
        if 0 <= l.name().upper().find('PROTONASYM') :
            log.warning("# VB: REMOVE %s" % l.name() )
            continue
        if 0 <= l.name().upper().find('MUNU') :
            log.warning("# VB: REMOVE %s" % l.name() )
            continue
        _lines.append ( l )
    s.lines = _lines
    
log.warning(100*"#")
log.warning(100*"#")
log.warning(100*"#")




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
# Get lines that needs MDST.DST and their required raw banks
#
mdstDstLines = [ line.name() for line in sc.activeLines(mdstStreams) if (line.MDSTFlag and line.prescale() > 0) ]
mdstDstRawEvts = { }
for stream in sc.activeStreams() :
    if stream.name() in mdstStreams :
        mdstDstRawEvts.update( stream.getRequiredRawEvents() )
# Locations to veto from MDST.DST
tesVETO = [ "/Event/Velo/RawEvent",
            "/Event/Tracker/RawEvent",
            "/Event/Rich/RawEvent",
            "/Event/Calo/RawEvent",
            "/Event/Muon/RawEvent",
            "/Event/Other/RawEvent" ]
tesVETO += ["/Event/"+name+"/#99" for name in dstStreams]
# Locations to add to MDST.DST
tesOPT = [ '/Event/'+stripTESPrefix+'#99' ] + [ '/Event/'+name+"#99" for name in mdstStreams ]

#
# Sequence of configuration to create the MDST.DST output
#
from Configurables import GaudiSequencer
mdstDstSeq = GaudiSequencer( "MDSTDSTSeq" )

#from Configurables import StoreExplorerAlg
#mdstDstSeq.Members += [ StoreExplorerAlg("ExploreTES") ]

from Configurables import AddressKillerAlg
mdstDstSeq.Members += [ AddressKillerAlg( name = "KillTESAddresses_MDSTDST" ) ]

from Configurables import FixInputCopyStream
mdstDstSeq.Members += [ FixInputCopyStream() ]

from Configurables import InputCopyStream
from GaudiConf     import IOHelper
mdstDstWriter = InputCopyStream( "MDSTDSTWriter",
                                 AlgDependentItemList = mdstDstRawEvts,
                                 AcceptAlgs = mdstDstLines,
                                 OptItemList = tesOPT,
                                 ItemList    = caloProtoReprocessLocs,
                                 TESVetoList = tesVETO )
mdstDstSeq.Members += IOHelper().outputAlgs( filename = '000000.MDST.DST.dst',
                                             writer = mdstDstWriter,
                                             writeFSR = True )


#
# Add stripping TCK
#
from Configurables import StrippingTCK
stck = StrippingTCK(HDRLocation = '/Event/Strip/Phys/DecReports', TCK=0x36112100)

#
#Configure DaVinci
#
from Configurables import DaVinci
DaVinci().EvtMax = -1 # Number of events
DaVinci().HistogramFile = "DVHistos.root"
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ stck ] )
DaVinci().appendToMainSequence( [ dstWriter.sequence() ] )
if(len(mdstDstLines)>0): DaVinci().appendToMainSequence( [ mdstDstSeq ] )
DaVinci().ProductionType = "Stripping"

# Change the column size of Timing table
from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
TimingAuditor().TIMER.NameSize = 60
