# $Id: $
# Test your line(s) of the stripping
#  
# NOTE: Please make a copy of this file for your testing, and do NOT change this one!
#

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
juggler = RawEventJuggler( DataOnDemand=True, Input=2.0, Output=4.0 )


# Specify the name of your configuration
confname='B2DstMuNuIncl' #['JPsiForSL','B2DMuNuX'] #B2DMuNuX' #Hb2Charged2Body' #FOR USERS
#confname='Hb2Charged2Body' #FOR USERS


# NOTE: this will work only if you inserted correctly the 
# default_config dictionary in the code where your LineBuilder 
# is defined.
from StrippingSelections import buildersConf
confs = buildersConf()
from StrippingSelections.Utils import lineBuilder, buildStreamsFromBuilder
streams = buildStreamsFromBuilder(confs,confname)

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

from Configurables import StrippingReport
sr = StrippingReport(Selections = sc.selections())
sr.OnlyPositive = False

from Configurables import AlgorithmCorrelationsAlg
ac = AlgorithmCorrelationsAlg(Algorithms = sc.selections())

DaVinci().HistogramFile = 'StrippingHistos.root'
DaVinci().TupleFile = 'StrippingTuples.root'
DaVinci().EvtMax = -1
DaVinci().Lumi = True
DaVinci().PrintFreq = 10000
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().appendToMainSequence( [ ac ] )
DaVinci().appendToMainSequence( [ dstWriter.sequence() ] )
if(len(mdstDstLines)>0): DaVinci().appendToMainSequence( [ mdstDstSeq ] )
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


from Configurables import DecayTreeTuple
Tup = DecayTreeTuple("Tup")
Tup.Inputs = ["Phys/B2DstMuNuInclLine/Particles"]
Tup.Decay = "[B~0 -> ^(D*(2010)+ -> ^(D0 -> ^K- ^pi+) ^pi+) ^mu-]CC"
Tup.Branches = {"B"      : "[B~0 ->  (D*(2010)+ ->  (D0 ->  K-  pi+)  pi+)  mu-]CC",
                "Dst"    : "[B~0 -> ^(D*(2010)+ ->  (D0 ->  K-  pi+)  pi+)  mu-]CC",
                "D0"     : "[B~0 ->  (D*(2010)+ -> ^(D0 ->  K-  pi+)  pi+)  mu-]CC",
                "Slowpi" : "[B~0 ->  (D*(2010)+ ->  (D0 ->  K-  pi+) ^pi+)  mu-]CC",
                "mu"     : "[B~0 ->  (D*(2010)+ ->  (D0 ->  K-  pi+)  pi+) ^mu-]CC"}
DaVinci().appendToMainSequence( [ Tup ] )

TupWS = DecayTreeTuple("Tup")
TupWS.Inputs = ["Phys/B2DstMuNuInclWSLine/Particles"]
TupWS.Decay = "[B~0 -> ^(D*(2010)- -> ^(D0 -> ^K- ^pi+) ^pi-) ^mu-]CC"
TupWS.Branches = {"B"      : "[B~0 ->  (D*(2010)- ->  (D0 ->  K-  pi+)  pi-)  mu-]CC",
                "Dst"    : "[B~0 -> ^(D*(2010)- ->  (D0 ->  K-  pi+)  pi-)  mu-]CC",
                "D0"     : "[B~0 ->  (D*(2010)- -> ^(D0 ->  K-  pi+)  pi-)  mu-]CC",
                "Slowpi" : "[B~0 ->  (D*(2010)- ->  (D0 ->  K-  pi+) ^pi-)  mu-]CC",
                "mu"     : "[B~0 ->  (D*(2010)- ->  (D0 ->  K-  pi+)  pi-) ^mu-]CC"}
DaVinci().appendToMainSequence( [ TupWS ] )
