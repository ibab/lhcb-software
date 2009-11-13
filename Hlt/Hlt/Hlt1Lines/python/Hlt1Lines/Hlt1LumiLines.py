##############################
#     HLT Lumi Line(s)
##############################

from Gaudi.Configuration import *
from GaudiConf.Configuration import *

from HltLine.HltLinesConfigurableUser import *

from Hlt1Lines.LumiCounterDefinition import LumiCounterDefinitionConf
from Configurables import ( LumiCountVertices,
                            LumiCountTracks,
                            LumiFromL0DU,
                            LumiCountHltTracks
                            )
from Configurables import GaudiSequencer as Sequence
from Configurables import DeterministicPrescaler as Scaler


####### create binders...
def _createCounter( counterKind, seqName, seq, enableNonL0Counters ) :
    if counterKind is LumiFromL0DU:
        return  lambda name, value :  seq.Members.append( LumiFromL0DU( seqName+name
                                                        , InputSelection='Trig/L0/L0DUReport'
                                                        , CounterName=name
                                                        , ValueName=value
                                                        , OutputContainer='Hlt/LumiSummary' ) )
    return lambda name, inputSel : seq.Members.append( counterKind( seqName + name
                                                     , InputSelection = inputSel
                                                     , CounterName = name
                                                     , Enable = enableNonL0Counters
                                                     , OutputContainer='Hlt/LumiSummary' ) )

def _createL0Counter( seqName, seq ) :
####### operator, meet arguments...
def _combine( op, arg ) :
    for key,value in arg.iteritems() : op(key,value)
    return arg.keys()

############# start building the lumi line(s)...
class Hlt1LumiLinesConf(HltLinesConfigurableUser) :
    __used_configurables__ = [ LumiCounterDefinitionConf ]

    __slots__ = { 'TriggerType'            : 'LumiTrigger'  # ODIN trigger type accepted
                , 'BXTypes'                : ['NoBeam', 'BeamCrossing','Beam1','Beam2']
                , 'LumiLines'              : ['Count','VDM']
                , 'Tracking'               : False            # use 0/1: switches on/off tracking
                , 'OutputLevel'            : WARNING
                }


    def __create_lumi_line__(self, BXType, counters):
        '''
        returns an Hlt1 "Line" including input filter, reconstruction sequence and counting
        adds histogramming
        '''

        # debugging options
        debugOPL = self.getProp('OutputLevel')
        from HltLine.HltReco import PV2D

        # define empty reco sequence
        seqRecoName = 'LumiReco'
        lumiRecoSequence = Sequence(seqRecoName+BXType +'Seq'
                                    , ModeOR = True
                                    , ShortCircuit = False
                                    , OutputLevel = debugOPL
                                    , IgnoreFilterPassed = True
                                    , MeasureTime = True)

        # define empty sequence to collect counters
        seqCountName = 'LumiCount'
        lumiCountSequence = Sequence(seqCountName+BXType +'Seq'
                                    , ModeOR = True
                                    , ShortCircuit = False
                                    , OutputLevel = debugOPL
                                    , IgnoreFilterPassed = True
                                    , MeasureTime = True)

        # populate count sequence from the definition
        createdCounters = []
        histoThresholds = []
        histoMaxBins = []
        veloBW = False
        for key, definition in counters.iteritems():
            # example 'CaloEt' : [LumiFromL0DU , True , 'Sum(Et)' , 500, 6000],
            (op, flag, inputDef, threshold, bins) = definition
            if flag:
                createdCounters.extend( 
                    _combine( _createCounter( op, seqCountName+BXType, lumiCountSequence, self.getProp('Tracking') ), { key : inputDef } ) )
                histoThresholds.extend( [threshold] )
                histoMaxBins.extend( [bins] )
                if key == 'RZVeloBW': veloBW=True
                if debugOPL <= DEBUG:
                    print '# DEBUG   : Hlt1LumiLines::HistoMaker:', BXType, key, threshold, bins
                
        # create filter sequence for the tracking
        if self.getProp('Tracking'): trackingFraction = 1
        else: trackingFraction = 0
        lumiRecoSequence.Members.append(
            Sequence('LumiRecoFilterSequence'+BXType ,
                     Members = [Scaler( 'Lumi'+BXType+'TrackingScaler' ,
                                        AcceptFraction = trackingFraction
                                        ),
                                Sequence('LumiTrackRecoSequence' ,
                                         IgnoreFilterPassed = True,
                                         Members = PV2D.members(),
                                         MeasureTime = True ) 
                                ],
                     IgnoreFilterPassed = False,
                     MeasureTime = True ) )

        # filter to get backward tracks (make sure it always passes by wrapping inside a sequence)
        from Configurables import HltTrackFilter
        lumiRecoFilterSequence = Sequence( 'LumiRecoFilterSequence'+BXType )
        lumiRecoFilterSequence.Members.append(
            Sequence('HltRZVeloBWSequence'
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

        # define histogrammers
        from Configurables import LumiHistoMaker, LumiHisto2dSPD
        HistoMembers=[]
        HistoMembers.append(LumiHistoMaker('Histo'+BXType,
                                           InputVariables = createdCounters,
                                           Thresholds = histoThresholds,
                                           MaxBins = histoMaxBins,
                                           OutputLevel = debugOPL
                                           ))
        HistoMembers.append(LumiHisto2dSPD('Histo2D'+BXType,
                                           HistoTitle=str(BXType),
                                           OutputLevel = debugOPL ))

        lumiHistoSequence = Sequence('LumiHisto'+BXType +'Seq'
                                     , Members = HistoMembers
                                     , ModeOR = True
                                     , ShortCircuit = False
                                     , MeasureTime = True
                                     , OutputLevel = debugOPL
                                     )


        from HltLine.HltLine import Hlt1Line   as Line
        return Line ( 'Lumi'+BXType
                    , prescale = self.prescale
                    , ODIN = ' ( ODIN_TRGTYP == LHCb.ODIN.%s ) & ( ODIN_BXTYP == LHCb.ODIN.%s)' % ( self.getProp('TriggerType'),BXType )
                    , algos = [ lumiRecoSequence, lumiCountSequence, lumiHistoSequence ] 
                    , postscale = self.postscale
                    ) 

    def __apply_configuration__(self) :
        '''
        creates parallel HLT1 Lines for each beam crossing type
        '''
        from HltLine.HltLine import Hlt1Line   as Line
        from HltLine.HltLine import Hlt1Member as Member

        counters = LumiCounterDefinitionConf().defineCounters()
        BXTypes=self.getProp('BXTypes')
        BXMembers = []
        for bx in BXTypes: 
            BXMembers.append(self.__create_lumi_line__(bx, counters))

        
