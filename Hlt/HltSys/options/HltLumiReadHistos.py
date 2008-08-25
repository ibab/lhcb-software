##############################
#     HLT Lumi Lines
##############################
import GaudiPython
from Gaudi.Configuration import *
from Configurables import GaudiSequencer as Sequence
from Configurables import DeterministicPrescaler as Prescale
##from Configurables import HltDummySelection as Dummy
from Configurables import OdinTypesFilter 
from Configurables import HltTrackFilter, HltSelectionFilter
##from Configurables import LumiCountVertices, LumiCountTracks, LumiFromL0DU, LumiCountHltTracks, HltLumiWriter
from Configurables import HltLumiOdinReader, LumiHistoMaker
from Configurables import RawEventDump, HltLumiSummaryDecoder

####### create binders...
def sequenceAppender( seq ) :
    return lambda x : seq.Members.append( x )

def createCounter( counterKind, seqName, seq ) : 
    return lambda name, input  : seq.Members.append( counterKind( seqName + name
                                                                , InputSelection = input
                                                                , CounterName = name
                                                                , OutputContainer='Hlt/LumiSummary' ) )
def createL0Counter( seqName, seq ) :
    return lambda name, value : seq.Members.append(  LumiFromL0DU(seqName+name
                                                                 , InputSelection='Trig/L0/L0DUReport'
                                                                 , CounterName=name
                                                                 , ValueName=value
                                                                 , OutputContainer='Hlt/LumiSummary' ) )
####### operator, meet arguments...
def combine( op, arg ) :
    for key,value in arg.iteritems() : op(key,value)

# init is needed for ANNSvc
importOptions('$HLTSYSROOT/options/HltLumiInit.opts')

# debugging options
debugOPL=4
debugging=False

ApplicationMgr().TopAlg += [ Sequence('readLumiSequence', MeasureTime = True, OutputLevel = 3 ) ]

readLumiSequence = sequenceAppender( Sequence('readLumiSequence', MeasureTime = True, OutputLevel = 3  ) )

if debugging:
    readLumiSequence( RawEventDump( DumpData = False, OutputLevel = debugOPL ) )
    readLumiSequence(HltLumiOdinReader('OdinDebugReader', OutputLevel = debugOPL ) )

#readLumiSequence( OdinTypesFilter( TriggerTypes=['Reserve'],  # should become 'Random'
readLumiSequence( OdinTypesFilter( 'OdinTriggerTypes', TriggerTypes=['TimingTrigger'] ) )

readLumiSequence( HltLumiSummaryDecoder( OutputLevel = debugOPL ) ) 

BXMembers = []
for i in [ 'NoBeam', 'BeamCrossing','SingleBeamRight','SingleBeamLeft'] :

    HistoMembers=[]
    if debugging: HistoMembers.append(HltLumiOdinReader('OdinReaderBefore'+i, OutputLevel = debugOPL ))
    HistoMembers.append(OdinTypesFilter('Filter'+i, BXTypes = [i], OutputLevel = debugOPL ))
    if debugging: HistoMembers.append(HltLumiOdinReader('OdinReaderAfter'+i, OutputLevel = debugOPL ))
    HistoMembers.append(LumiHistoMaker('Histo'+i, OutputLevel = debugOPL ))
    
    BXMembers.append( Sequence('Hlt1Lumi'+i+'Sequence', 
                               Members = HistoMembers,
                               MeasureTime = True
                               ) )


    BXTypes = Sequence( 'Hlt1LumiBXTypesSequence'
                        , Members = BXMembers
                  , ModeOR = True
                  , ShortCircuit = False
                  , IgnoreFilterPassed = True
                  , MeasureTime = True)
readLumiSequence( BXTypes )

