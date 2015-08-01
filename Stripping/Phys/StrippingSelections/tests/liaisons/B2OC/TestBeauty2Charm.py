# $Id: $
# Test your line(s) of the stripping
#  
# NOTE: Please make a copy of this file for your testing, and do NOT change this one!
#

from Gaudi.Configuration import *
from Configurables import DaVinci
from StrippingConf.Configuration import StrippingConf, StrippingStream

# Tighten Trk Chi2 to <3
from CommonParticles.Utils import DefaultTrackingCuts
DefaultTrackingCuts().Cuts  = { "Chi2Cut" : [ 0, 3 ],
                                "CloneDistCut" : [5000, 9e+99 ] }

#
#Raw event juggler to split Other/RawEvent into Velo/RawEvent and Tracker/RawEvent
#
from Configurables import RawEventJuggler
juggler = RawEventJuggler( DataOnDemand=True, Input=2.0, Output=4.0 )


# Specify the name of your configuration
confname='Beauty2Charm' #FOR USERS


# NOTE: this will work only if you inserted correctly the 
# default_config dictionary in the code where your LineBuilder 
# is defined.
from StrippingSelections import buildersConf
confs = buildersConf()
from StrippingSelections.Utils import lineBuilder, buildStreamsFromBuilder
streamall = buildStreamsFromBuilder(confs,confname)

MyLines = [

    # Bc+ -> D+ D0
    'StrippingBc2DD0D2KSHD02HHBeauty2CharmLine',
    'StrippingBc2DD0D2KSHD02KHHHBeauty2CharmLine',
    'StrippingBc2DD0D2KSHD02KSHHBeauty2CharmLine',
    'StrippingBc2DD0D2KstKSD02HHBeauty2CharmLine',
    'StrippingBc2DD0D2KstKSD02KHHHBeauty2CharmLine',
    'StrippingBc2DD0D2KstKSD02KSHHBeauty2CharmLine',
    'StrippingBc2DD0D2KstHHD02HHBeauty2CharmLine',
    'StrippingBc2DD0D2KstHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DD0D2KstHHD02KSHHBeauty2CharmLine',
    'StrippingBc2DD0D2KstKst0D02HHBeauty2CharmLine',
    'StrippingBc2DD0D2KstKst0D02KHHHBeauty2CharmLine',
    'StrippingBc2DD0D2KstKst0D02KSHHBeauty2CharmLine',
    'StrippingBc2DD0D2KHHD02HHBeauty2CharmLine',
    'StrippingBc2DD0D2KHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DD0D2KHHD02KSHHBeauty2CharmLine',
    'StrippingBc2DD0D2KSHHHD02HHBeauty2CharmLine',
    'StrippingBc2DD0D2KSHHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DD0D2KSHHHD02KSHHBeauty2CharmLine',

    # Bc+ -> D*+ D0
    'StrippingBc2DstD0Dst2DGammaD2KSHHHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KSHHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KSHHHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KSHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KSHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KSHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02KHHHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02KHHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02KHHHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KHHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KHHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KHHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KHHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02HHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02HHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02HHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02KSHHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02KSHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02KSHHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KSHHHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KSHHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KSHHHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KSHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KSHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KSHD02KSHHBeauty2CharmLine',

    # Bc+ -> D*+ D*0
    'StrippingBc2DstDst0Dst2DGammaD2KSHHHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHHHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHHHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHHHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHHHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHHHDst02D0GammaD02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHDst02D0GammaD02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KHHHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KHHHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KHHHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KHHHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KHHHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KHHHDst02D0GammaD02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KHHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KHHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KHHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KHHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KHHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KHHDst02D0GammaD02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KHHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KHHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KHHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KHHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KHHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KHHDst02D0GammaD02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02HHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02HHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02HHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02HHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02HHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02HHDst02D0GammaD02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KSHHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KSHHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KSHHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KSHHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KSHHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KSHHDst02D0GammaD02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHHHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHHHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHHHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHHHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHHHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHHHDst02D0GammaD02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHDst02D0GammaD02HHBeauty2CharmLine'

]
## streamsel = StrippingStream("Bhadron")
## for stream in streamall:
##     for line in stream.lines:
##          if line.name() in MyLines:
##             streamsel.appendLines([line])
## streams = [streamsel]

streams = streamall

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
                                 TESVetoList = tesVETO )
mdstDstSeq.Members += IOHelper().outputAlgs( filename = '000000.MDST.DST.dst',
                                             writer = mdstDstWriter,
                                             writeFSR = True )


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

pFreq = 1000
DaVinci().EvtMax = 10000
DaVinci().PrintFreq = pFreq

from Configurables import StrippingReport
sr = StrippingReport(Selections = sc.selections(),
                     ReportFrequency = pFreq)

from Configurables import AlgorithmCorrelationsAlg
ac = AlgorithmCorrelationsAlg(Algorithms = sc.selections())

DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
#DaVinci().appendToMainSequence( [ ac ] )
DaVinci().appendToMainSequence( [ dstWriter.sequence() ] )
#if(len(mdstDstLines)>0): DaVinci().appendToMainSequence( [ mdstDstSeq ] )
DaVinci().ProductionType = "Stripping"
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
