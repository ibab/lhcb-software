##############################
#     HLT Lumi Lines
##############################
import GaudiPython
from Gaudi.Configuration import *
from Configurables import GaudiSequencer as Sequence
from Configurables import DeterministicPrescaler as Prescale
from Configurables import HltDummySelection as Dummy
from Configurables import OdinTypesFilter 
from Configurables import HltTrackFilter, HltSelectionFilter
from Configurables import LumiCountVertices, LumiCountTracks, LumiFromL0DU, LumiCountHltTracks, HltLumiOdinReader, HltLumiWriter

# HLT Reconstruction
importOptions('$HLTSYSROOT/options/HltRecoSequence.opts')
# init is needed for ANNSvc
importOptions('$HLTSYSROOT/options/HltLumiInit.opts')

LumiSequence = Sequence('HltLumiSequence', MeasureTime = True )
LumiSequence.Members.append( OdinTypesFilter( TriggerTypes=['Reserve'],  # should become 'Random'
                                              BXTypes= ['NoBeam','BeamCrossing','SingleBeamRight','SingleBeamLeft'] ) )

LumiSequence.Members.append( Sequence( 'Hlt1LumiRecoSequence',
                                         Members = [ Sequence('HltRecoSequence') ]
                                         ModeOR = True, ShortCircuit = False, IgnoreFilterPassed = True, MeasureTime = True
                                         ) )

trackCounters = {   'Velo'   : 'Hlt/Track/Velo'
                , 'RZVelo'   : 'Hlt/Track/RZVelo'
                #, 'Muon'    : 'Rec/Tracks/Muons' 
                }
vertexCounters = {  'PV2D'  : 'Hlt/Vertex/PV2D'
                 #,  'PV3D'  : 'Hlt/Vertex/PV3D' 
                 }
L0Counters = { 'SPDMult'  : 'Spd(Mult)'
             , 'PUMult'   : 'PUHits(Mult)'
             , 'CaloEt'   : 'Sum(Et)' 
             }

seqName = 'LumiCount'
LumiCountSequence = Sequence('Hlt'+seqName +'Sequence'
                            , ModeOR = True
                            , ShortCircuit = False
                            , IgnoreFilterPassed = True
                            , MeasureTime = True)

for name,location in vertexCounters.iteritems() :
    LumiCountSequence.Members.append( LumiCountVertices(seqName+name,
                                                        InputSelection = location,
                                                        CounterName = name,
                                                        OutputContainer = 'Hlt/LumiSummary' ) )
for name,location in trackCounters.iteritems() :
    LumiCountSequence.Members.append( LumiCountTracks(seqName+name,
                                                      InputSelection = location,
                                                      CounterName = name,
                                                      OutputContainer = 'Hlt/LumiSummary' ) )
# filter to get backward tracks (always pass)
LumiCountSequence.Members.append( Sequence('HltBWRZVeloSequence'
                                          , Members  = [ HltTrackFilter('HltPrepareBWRZVelo',
                                                                        InputSelection   = 'TES:Hlt/Track/RZVelo',
                                                                        AddInfo = False,
                                                                        FilterDescriptor = ['IsBackward,>,0.5'],
                                                                        OutputSelection     = 'BWRZVelo',
                                                                        RequirePositiveInputs = False
                                                                        ) ]
                                          , MeasureTime = True
                                          , ModeOR = True
                                          , ShortCircuit = False
                                          , IgnoreFilterPassed = True ) )

LumiCountSequence.Members.append( LumiCountHltTracks( 'LumiCountHltBWRZVelo',
                                                      InputSelection='BWRZVelo',
                                                      CounterName='RZVeloBW',
                                                      OutputContainer='Hlt/LumiSummary' ) )

for name,value in L0Counters.iteritems() :
    LumiCountSequence.Members.append( LumiFromL0DU(seqName+name,
                                                   InputSelection='Trig/L0/L0DUReport',
                                                   CounterName=name,
                                                   ValueName=value,
                                                   OutputContainer='Hlt/LumiSummary' ) )



# try to write other/new TES container   (TEMP)
LumiCountSequence.Members.append( LumiCountTracks(seqName+'VeloTwo',
                                                  InputSelection='Hlt/Track/Velo',
                                                  CounterName='Velo',
                                                  OutputContainer='Hlt/LumiSummaryTwo') )

LumiSequence.Members.append( LumiCountSequence )
LumiSequence.Members.append( HltLumiOdinReader( ) )

BXMembers = []
for i in [ 'NoBeam', 'BeamCrossing'] :
    BXMembers.append( Sequence('Hlt1Lumi'+i+'Sequence', 
                               Members = [ OdinTypesFilter('Filter'+i, BXTypes = [i]),
                                           HltLumiOdinReader('OdinReader'+i) ] ,
                               MeasureTime = True
                               ) )
BXTypes = Sequence( 'Hlt1LumiBXTypesSequence'
                  , Members = BXMembers
                  , ModeOR = True
                  , ShortCircuit = False
                  , IgnoreFilterPassed = True
                  , MeasureTime = True)
LumiSequence.Members.append( BXTypes )
LumiSequence.Members.append( Prescale( 'PrescaleLumiDecision', AcceptFraction = 0.5 ) )
LumiSequence.Members.append( Dummy( 'Hlt1LumiDecision' ) )

# TODO: move writer into dedicated rawbank sequence
LumiSequence.Members.append( HltLumiWriter( ) )

# register with global decision (which is the OR of its input)
for i in   [ 'Hlt1LumiDecision','Hlt1LumiDecision2'] :
     HltSelectionFilter('Hlt1Global').InputSelections.append( i ) 

# print LumiSequence
