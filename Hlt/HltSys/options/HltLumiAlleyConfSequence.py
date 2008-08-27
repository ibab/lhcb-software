##############################
#     HLT Lumi Line(s)
##############################

# removed all "IgnoreFilterPassed = True" to get correct behaviour

import GaudiPython
from Gaudi.Configuration import *
from Configurables import GaudiSequencer as Sequence
from Configurables import DeterministicPrescaler as Prescale
from Configurables import DummyLumiAlley as Dummy
from Configurables import OdinTypesFilter 
from Configurables import HltTrackFilter, HltSelectionFilter
from Configurables import LumiCountVertices, LumiCountTracks, LumiFromL0DU, LumiCountHltTracks
from Configurables import HltLumiOdinReader, LumiHistoMaker
from Configurables import RawEventDump, HltLumiSummaryDecoder, HltLumiWriter, bankKiller

####### create binders...
def sequenceAppender( seq ) :
    return lambda x : seq.Members.append( x )

def createCounter( counterKind, seqName, seq ) : 
    return lambda name, input  :  seq.Members.append( counterKind( seqName + name
                                                                , InputSelection = input
                                                                , CounterName = name
                                                                , OutputContainer='Hlt/LumiSummary' ) )
def createL0Counter( seqName, seq ) :
    return lambda name, value :  seq.Members.append(  LumiFromL0DU(seqName+name
                                                                 , InputSelection='Trig/L0/L0DUReport'
                                                                 , CounterName=name
                                                                 , ValueName=value
                                                                 , OutputContainer='Hlt/LumiSummary' ) )
####### operator, meet arguments...
def combine( op, arg ) :
    for key,value in arg.iteritems() : op(key,value)

############# start building the lumi line(s)...

# HLT Reconstruction
importOptions('$HLTSYSROOT/options/HltRecoSequence.opts')
# init is needed for ANNSvc
importOptions('$HLTSYSROOT/options/HltLumiInit.opts')

# debugging options
debugOPL=4
debugging=False

# list to collect the counters created using createCounter and createL0Counter
createdCounters = []

LumiSequence = sequenceAppender( Sequence('HltLumiSequence'
                                          , MeasureTime = True ) )

LumiSequence( OdinTypesFilter( 'OdinTriggerTypes',
                               TriggerTypes=['TimingTrigger'],  # should become 'Random', now 'TimingTrigger' or 'Reserve'
                               BXTypes= ['ALL'] ) )
LumiSequence( Prescale( 'HltLumiPrescaler', AcceptFraction = 1.0 ) )

LumiSequence( Sequence( 'Hlt1LumiRecoSequence'
                      , Members = [ Sequence('HltRecoSequence')
                                    , Dummy( 'AlwaysPassReco' )         ## needed to pass this filter
                                    ]
                      , ModeOR = True
                      , ShortCircuit = False
                      , MeasureTime = True ) )

seqName = 'LumiCount'
LumiCountSequence = Sequence('Hlt'+seqName +'Sequence'
                            , ModeOR = True
                            , ShortCircuit = False
                            , MeasureTime = True)

combine( createCounter( LumiCountTracks,   seqName , LumiCountSequence),
         {   'Velo'   : 'Hlt/Track/Velo'
         , 'RZVelo'   : 'Hlt/Track/RZVelo'
         #, 'Muon'    : 'Rec/Tracks/Muons' 
         } )
createdCounters.extend(['Velo','RZVelo'])

combine( createCounter( LumiCountVertices, seqName, LumiCountSequence ),
       {  'PV2D'  : 'Hlt/Vertex/PV2D'
       #,  'PV3D'  : 'Hlt/Vertex/PV3D' 
       } )
createdCounters.extend(['PV2D'])

# filter to get backward tracks (make sure it always passes by wrapping inside a sequence)
LumiCountSequence.Members.append( Sequence('HltRZVeloBWSequence'
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

combine( createCounter( LumiCountHltTracks,   seqName , LumiCountSequence),
         { 'RZVeloBW' : 'RZVeloBW'
         } )
createdCounters.extend(['RZVeloBW'])

combine( createL0Counter( seqName, LumiCountSequence ),
         { 'SPDMult'  : 'Spd(Mult)'
         , 'PUMult'   : 'PUHits(Mult)'
         , 'CaloEt'   : 'Sum(Et)' 
         } )
createdCounters.extend(['SPDMult','PUMult','CaloEt' ])

LumiSequence( LumiCountSequence )

BXMembers = []
for i in [ 'NoBeam', 'BeamCrossing','SingleBeamRight','SingleBeamLeft'] :
    HistoMembers=[]
    if debugging: HistoMembers.append(HltLumiOdinReader('OdinReaderBefore'+i, OutputLevel = debugOPL ))
    HistoMembers.append(OdinTypesFilter('Filter'+i, BXTypes = [i], OutputLevel = debugOPL ))
    if debugging: HistoMembers.append(HltLumiOdinReader('OdinReaderAfter'+i, OutputLevel = debugOPL ))
    HistoMembers.append(LumiHistoMaker('Histo'+i, InputVariables = createdCounters, OutputLevel = 3 ))

    BXMembers.append( Sequence('Hlt1Lumi'+i+'Sequence', 
                               Members = HistoMembers,
                               MeasureTime = True
                               ) )
BXTypes = Sequence( 'Hlt1LumiBXTypesSequence'
                  , Members = BXMembers
                  , ModeOR = True
                  , ShortCircuit = False
                  , MeasureTime = True)
LumiSequence( BXTypes )
LumiSequence( Prescale( 'PrescaleLumiDecision', AcceptFraction = 1.0 ) )
LumiSequence( Dummy( 'HltLumiDecision' ) )

# @todo: move writer into dedicated rawbank sequence
if debugging: LumiSequence( RawEventDump('FullRawDataDump' ) )
# @todo Retsore that with LHCb v25r1
#LumiSequence( bankKiller( 'SerialKiller', DefaultIsKill=True, BankTypes=["ODIN","HltLumiSummary"],
#                          OutputLevel=debugOPL ) )
if debugging: LumiSequence( RawEventDump('NanoEventDump' ) )
LumiSequence( HltLumiWriter( OutputLevel=debugOPL )  )

# register with global decision (which is the OR of its input)
HltSelectionFilter('Hlt1Global').InputSelections.append( 'HltLumiDecision' ) 
