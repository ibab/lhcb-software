# $Id: $
# Test your line(s) of the stripping
#  
# NOTE: Please make a copy of this file for your testing, and do NOT change this one!
#

from Gaudi.Configuration import *
from Configurables import DaVinci
from StrippingConf.Configuration import StrippingConf, StrippingStream

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
    'StrippingBc2DD0D2KSHHHD02HHBeauty2CharmLine',
    'StrippingBc2DD0D2KSHHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DD0D2KSHHHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DD0D2KSHHHD02KSHHBeauty2CharmLine',
    'StrippingBc2DD0D2KSHD02HHBeauty2CharmLine',
    'StrippingBc2DD0D2KSHD02KHHHBeauty2CharmLine',
    'StrippingBc2DD0D2KSHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DD0D2KSHD02KSHHBeauty2CharmLine',
    'StrippingBc2DD0D2HHHD02HHBeauty2CharmLine',
    'StrippingBc2DD0D2HHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DD0D2HHHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DD0D2HHHD02KSHHBeauty2CharmLine',
    
    # Bc+ -> D*+ D0
    'StrippingBc2DstD0Dst2DPI0D2HHHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2HHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2HHHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2HHHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KSHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KSHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KSHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KSHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02KHHHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02KHHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02KHHHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02KHHHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KSHHHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KSHHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KSHHHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KSHHHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0DStar2DGammaD2HHHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0DStar2DGammaD2HHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0DStar2DGammaD2HHHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DstD0DStar2DGammaD2HHHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02HHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02HHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02HHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02HHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02KSHHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02KSHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02KSHHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02KSHHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KSHHHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KSHHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KSHHHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KSHHHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KSHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KSHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KSHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KSHD02KSHHBeauty2CharmLine',

    # Bc+ -> D+ D*0
    'StrippingBc2DDst0D2KSHHHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DDst0D2KSHHHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DDst0D2KSHHHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DDst0D2KSHHHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DDst0D2KSHHHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DDst0D2KSHHHDst02D0GammaD02HHBeauty2CharmLine',
    'StrippingBc2DDst0D2KSHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DDst0D2KSHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DDst0D2KSHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DDst0D2KSHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DDst0D2KSHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DDst0D2KSHDst02D0GammaD02HHBeauty2CharmLine',
    'StrippingBc2DDst0D2HHHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DDst0D2HHHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DDst0D2HHHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DDst0D2HHHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DDst0D2HHHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DDst0D2HHHDst02D0GammaD02HHBeauty2CharmLine',

    # Bc+ -> D*+ D*0
    'StrippingBc2DstDst0Dst2DPI0D2HHHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2HHHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2HHHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2HHHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2HHHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2HHHDst02D0GammaD02HHBeauty2CharmLine',
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
    'StrippingBc2DstDst0Dst2DGammaD2KSHHHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHHHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHHHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHHHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHHHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHHHDst02D0GammaD02HHBeauty2CharmLine',
    'StrippingBc2DstDst0DStar2DGammaD2HHHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0DStar2DGammaD2HHHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0DStar2DGammaD2HHHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DstDst0DStar2DGammaD2HHHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0DStar2DGammaD2HHHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0DStar2DGammaD2HHHDst02D0GammaD02HHBeauty2CharmLine',
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

## Use these lines to only run the lines listed above
streamsel = StrippingStream("Bhadron")
for stream in streamall:
    for line in stream.lines:
        if line.name() in MyLines:
            streamsel.appendLines([line])
streams = [streamsel]

## Use this instead to run everything in the B2OC module
#streams = streamall

for stream in streams:
    for line in stream.lines:
        line._prescale = 1.0

#streamsel.sequence().IgnoreFilterPassed = True # so that we get all events written out  

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
# Configuration of SelDSTWriter
# per-event an per-line selective writing of the raw event is active (selectiveRawEvent=True)
#
SelDSTWriterElements = { 'default' : stripDSTElements(pack=enablePacking) }

SelDSTWriterConf = { 'default' : stripDSTStreamConf(pack=enablePacking,selectiveRawEvent=True) }

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

pFreq = 1000
DaVinci().EvtMax = -1
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
DaVinci().ProductionType = "Stripping"

# change the column size of timing table
from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
TimingAuditor().TIMER.NameSize = 60

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

# database
DaVinci().DDDBtag   = "dddb-20150724"
DaVinci().CondDBtag = "cond-20150828"

# input file
DaVinci().DataType   = "2012"
DaVinci().InputType  = "DST"
DaVinci().Simulation = True

#from Gaudi.Configuration import * 
#from GaudiConf import IOHelper
#IOHelper('ROOT').inputFiles(["PFN:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/MC/2012/ALLSTREAMS.DST/00046547/0000/00046547_00000017_2.AllStreams.dst"],clear=True)

# 2012 mag Down
#DaVinci().DDDBtag   = "Sim08-20130326-1"
#DaVinci().CondDBtag = "Sim08-20130326-1-vc-md100"
#importOptions("/usera/jonesc/MC-bc2dd-14195001-MagDown.py")

# 2012 mag Up
DaVinci().DDDBtag   = "Sim08-20130326-1"
DaVinci().CondDBtag = "Sim08-20130326-1-vc-mu100"
#importOptions("/usera/jonesc/MC-bc2dd-14195001-MagUp.py")
