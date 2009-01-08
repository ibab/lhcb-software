##############################
#     HLT Lumi Lines
##############################
import GaudiPython
from Gaudi.Configuration import *
from Configurables import GaudiSequencer as Sequence
from Configurables import GaudiSequencer
from Configurables import DeterministicPrescaler as Prescale
from Configurables import LumiCountVertices, LumiCountTracks, LumiFromL0DU, LumiCountHltTracks
from Configurables import HltLumiOdinReader, LumiHistoMaker
from Configurables import RawEventDump, HltLumiSummaryDecoder, HltLumiWriter, bankKiller
from Configurables import LoKi__ODINFilter  as ODINFilter
from Configurables import LoKi__HDRFilter   as HltFilter
from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import Hlt1Member as Member
##from Configurables import HltDummySelection as Dummy
from Configurables import OdinTypesFilter 
from Configurables import LumiAnalyser
from Configurables import bankKiller

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
    return arg.keys()

# init is needed for ANNSvc
importOptions('$HLTCONFROOT/options/HltInit.py')
importOptions('$HLTCONFROOT/options/HltLumiInit.py')

# debugging options
debugOPL=4
debugging=False
fullReco=False

#
# -------------------- counting sequence ---------------------
#
seqName = 'LumiCount'
# list to collect the counters created using createCounter and createL0Counter
createdCounters = []
histoThresholds = []
histoMaxBins = []
lumiCountSequence = Sequence('Hlt'+seqName +'Sequence'
                             , ModeOR = True
                             , ShortCircuit = False
                             , MeasureTime = True
                             , OutputLevel = debugOPL )

if fullReco:
    from Configurables import HltTrackFilter, HltSelectionFilter
    createdCounters.extend( 
            combine( createCounter( LumiCountTracks,   seqName , lumiCountSequence),
                   {   'Velo'   : 'Hlt/Track/Velo'
                   , 'RZVelo'   : 'Hlt/Track/RZVelo'
                   #, 'Muon'    : 'Rec/Tracks/Muons' 
                   } ) 
            )
    histoThresholds.extend( [5, 5] )
    histoMaxBins.extend( [200, 200] )

    createdCounters.extend(
            combine( createCounter( LumiCountVertices, seqName, lumiCountSequence ),
                   {  'PV2D'  : 'Hlt/Vertex/PV2D'
                   } )
            )
    histoThresholds.extend( [1] )
    histoMaxBins.extend( [20] )

    # filter to get backward tracks (make sure it always passes by wrapping inside a sequence)
    lumiCountSequence.Members.append( Sequence('HltRZVeloBWSequence'
                                               , Members  = [ HltTrackFilter('HltPrepareRZVeloBW'
                                                                             , InputSelection   = 'TES:Hlt/Track/RZVelo'
                                                                             , AddInfo = False
                                                                             , FilterDescriptor = ['IsBackward,>,0.5']
                                                                             , OutputSelection     = 'RZVeloBW'
                                                                             , RequirePositiveInputs = False
                                                                             ) ]
                                               , MeasureTime = True
                                               , ModeOR = True
                                               , ShortCircuit = False
                                               ) )

    createdCounters.extend(
        combine( createCounter( LumiCountHltTracks,   seqName , lumiCountSequence),
                 { 'RZVeloBW' : 'RZVeloBW'
                   } )
        )
    histoThresholds.extend( [5] )
    histoMaxBins.extend( [200] )

pass # end of fullReco

createdCounters.extend(    
    combine( createL0Counter( seqName, lumiCountSequence ),
             { 'SPDMult'  : 'Spd(Mult)'
             , 'PUMult'   : 'PUHits(Mult)'
             , 'CaloEt'   : 'Sum(Et)' 
             } )
    )
histoThresholds.extend( [6, 3, 1000] )
histoMaxBins.extend( [600, 500, 6000] )


BXMembers = []
for i in [ 'NoBeam', 'BeamCrossing','SingleBeamRight','SingleBeamLeft'] :
    HistoMembers=[]
    if debugging: HistoMembers.append(HltLumiOdinReader('OdinReaderBefore'+i, OutputLevel = debugOPL ))
    HistoMembers.append( ODINFilter('Filter'+i, Code = 'ODIN_BXTYP == LHCb.ODIN.'+i, OutputLevel = debugOPL ))
    if debugging: HistoMembers.append(HltLumiOdinReader('OdinReaderAfter'+i, OutputLevel = debugOPL ))
    HistoMembers.append(LumiHistoMaker('Histo'+i,
                                       InputVariables = createdCounters,
                                       MaxBins = histoMaxBins,
                                       Thresholds = histoThresholds,
                                       OutputLevel = 3 ))

    BXMembers.append( Sequence('Hlt1Lumi'+i+'Sequence', 
                               Members = HistoMembers,
                               MeasureTime = True
                               ) )
BXTypes = Sequence( 'Hlt1LumiBXTypesSequence'
                  , Members = BXMembers
                  , ModeOR = True
                  , ShortCircuit = False
                  , MeasureTime = True)

lumiSeqMembers = []
if fullReco:
    importOptions('$HLTCONFROOT/options/HltRecoSequence.py')

    lumiRecoSequence  = Sequence( 'Hlt1LumiRecoSequence'
                                , Members = [ GaudiSequencer('HltRecoSequence') ]
                                , IgnoreFilterPassed = True
                                , MeasureTime = True )
    lumiSeqMembers += [lumiRecoSequence]
    
    pass # end of fullReco

lumiSeqMembers += [
    lumiCountSequence
    , BXTypes
    ]
lumiSequence = Line ( 'Lumi'
                      , ODIN = 'ODIN_TRGTYP == LHCb.ODIN.RandomTrigger'# 'RandomTrigger', 'TimingTrigger' or 'Reserve'
                      , algos = lumiSeqMembers 
                      )

#
# -------------------- analysis sequence ---------------------
#
analyseLumiSequence = sequenceAppender( Sequence('analyseLumiSequence', MeasureTime = True, OutputLevel = 3  ) )

if debugging:
    analyseLumiSequence( RawEventDump( DumpData = False, OutputLevel = debugOPL ) )
    analyseLumiSequence( HltLumiOdinReader('OdinDebugReader', OutputLevel = debugOPL ) )

# select only the right Trigger Type
analyseLumiSequence( OdinTypesFilter( 'OdinTriggerTypes', TriggerTypes=['RandomTrigger'] ) )
# decode summary data
analyseLumiSequence( HltLumiSummaryDecoder( OutputLevel = debugOPL ) ) 
# analyse summary data
analyseLumiSequence( LumiAnalyser( OutputLevel = debugOPL ) ) 


#
# -------------------- writing sequence ---------------------
#
writeLumiSequence = sequenceAppender(
    Sequence( 'writeLumiSequence'
              , ModeOR = True
              , ShortCircuit = False
              , MeasureTime = True
              , OutputLevel = 3
              , Members = 
              [ HltLumiWriter(OutputLevel = 3)
                , Sequence( 'LumiStripper' , MeasureTime = True, OutputLevel = 3 
                            , Members = 
                            [ bankKiller( BankTypes=[ 'ODIN','HltLumiSummary'],
                                          DefaultIsKill=True ,
                                          OutputLevel = 3 )
                              ])
                ] )
    )

#
# -------------------- prepare the sequences -------------------- 
#

lumiCountSequence = sequenceAppender(
    Sequence( 'LumiCountSequence'
              , ModeOR = False
              , ShortCircuit = True
              , MeasureTime = True
              , OutputLevel = 3
              , Members =
              [
    lumiSequence.sequencer(),
    ]
              )
    )

lumiCountAnalyseSequence = sequenceAppender(
    Sequence( 'LumiCountAnalyseSequence'
              , ModeOR = False
              , ShortCircuit = True
              , MeasureTime = True
              , OutputLevel = 3
              , Members =
              [
    lumiSequence.sequencer(),
    Sequence('analyseLumiSequence'),
    ]
              )
    )

lumiCountAnalyseWriteSequence = sequenceAppender(
    Sequence( 'LumiCountAnalyseWriteSequence'
              , ModeOR = False
              , ShortCircuit = True
              , MeasureTime = True
              , OutputLevel = 3
              , Members =
              [
    lumiSequence.sequencer(),
    Sequence('analyseLumiSequence'),
    Sequence('writeLumiSequence'),
    ]
              )
    )


lumiAnalyseSequence = sequenceAppender(
    Sequence( 'LumiAnalyseSequence'
              , ModeOR = False
              , ShortCircuit = True
              , MeasureTime = True
              , OutputLevel = 3
              , Members =
              [
    Sequence('analyseLumiSequence'),
    ]
              )
    )


#
# -------------------- enter the sequences in the topalg -------------------- 
#
#ApplicationMgr().TopAlg += [ Sequence('LumiCountAnalyseWriteSequence') ]


def configure(lines):
    seq={'count'  :lumiSequence.sequencer(),
         'analyse':Sequence('analyseLumiSequence'),
         'write'  :Sequence('writeLumiSequence'),
         }

    lumiSequenceMembers = [ seq[comp] for comp in seq.keys() if comp.upper() in lines.upper() ]
    return sequenceAppender(
        Sequence( lines
                  , ModeOR = False
                  , ShortCircuit = True
                  , MeasureTime = True
                  , OutputLevel = 3
                  , Members = lumiSequenceMembers
                  )
        )
