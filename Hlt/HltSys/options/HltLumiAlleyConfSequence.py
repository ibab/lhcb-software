##############################
#     HLT Lumi Line(s)
##############################
import GaudiPython
from Gaudi.Configuration import *
from Configurables import GaudiSequencer as Sequence
from Configurables import DeterministicPrescaler as Prescale
from Configurables import HltDummySelection as Dummy
from Configurables import OdinTypesFilter 
from Configurables import HltTrackFilter, HltSelectionFilter
from Configurables import LumiCountVertices, LumiCountTracks, LumiFromL0DU, LumiCountHltTracks, HltLumiOdinReader, HltLumiWriter

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

############# start building the lumi line(s)...

# HLT Reconstruction
importOptions('$HLTSYSROOT/options/HltRecoSequence.opts')
# init is needed for ANNSvc
importOptions('$HLTSYSROOT/options/HltLumiInit.opts')

LumiSequence = sequenceAppender( Sequence('HltLumiSequence', MeasureTime = True ) )

LumiSequence( OdinTypesFilter( TriggerTypes=['Reserve'],  # should become 'Random'
                               BXTypes= ['NoBeam','BeamCrossing','SingleBeamRight','SingleBeamLeft'] ) )
LumiSequence( Prescale( 'HltLumiPrescaler', AcceptFraction = 1.0 ) )
LumiSequence( Sequence( 'HltLumiRecoSequence'
                      , Members = [ Sequence('HltRecoSequence') ]
                      , ModeOR = True
                      , ShortCircuit = False
                      , IgnoreFilterPassed = True
                      , MeasureTime = True ) )

seqName = 'LumiCount'
LumiCountSequence = Sequence('Hlt'+seqName +'Sequence'
                            , ModeOR = True
                            , ShortCircuit = False
                            , IgnoreFilterPassed = True
                            , MeasureTime = True)

combine( createCounter( LumiCountTracks,   seqName , LumiCountSequence),
         {   'Velo'   : 'Hlt/Track/Velo'
         , 'RZVelo'   : 'Hlt/Track/RZVelo'
         #, 'Muon'    : 'Rec/Tracks/Muons' 
         } )
combine( createCounter( LumiCountVertices, seqName, LumiCountSequence ),
         {  'PV2D'  : 'Hlt/Vertex/PV2D'
         #,  'PV3D'  : 'Hlt/Vertex/PV3D' 
         } )

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
                                          , IgnoreFilterPassed = True ) )

LumiCountSequence.Members.append( LumiCountHltTracks( 'LumiCountHltRZVeloBW',
                                                      InputSelection='RZVeloBW',
                                                      CounterName='RZVeloBW',
                                                      OutputContainer='Hlt/LumiSummary' ) )

combine( createL0Counter( seqName, LumiCountSequence ),
         { 'SPDMult'  : 'Spd(Mult)'
         , 'PUMult'   : 'PUHits(Mult)'
         , 'CaloEt'   : 'Sum(Et)' 
         } )

LumiSequence( LumiCountSequence )
LumiSequence( HltLumiOdinReader( ) )

BXMembers = []
for i in [ 'NoBeam', 'BeamCrossing'] :
    BXMembers.append( Sequence('HltLumi'+i+'Sequence', 
                               Members = [ OdinTypesFilter('Filter'+i, BXTypes = [i]),
                                           HltLumiOdinReader('OdinReader'+i) ] ,
                               MeasureTime = True
                               ) )
LumiSequence( Sequence( 'HltLumiBXTypesSequence'
                      , Members = BXMembers
                      , ModeOR = True
                      , ShortCircuit = False
                      , IgnoreFilterPassed = True
                      , MeasureTime = True) )
LumiSequence( Prescale( 'HltLumiPrescaleDecision', AcceptFraction = 0.5 ) )
LumiSequence( Dummy( 'HltLumiDecision' ) )

# TODO: move writer into dedicated rawbank sequence
LumiSequence( HltLumiWriter( ) )

# register with global decision (which is the OR of its input)
HltSelectionFilter('Hlt1Global').InputSelections.append( 'HltLumiDecision' ) 
