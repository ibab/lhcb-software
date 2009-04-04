##############################
#     HLT Lumi Line(s)
##############################

from Gaudi.Configuration import *
from GaudiConf.Configuration import *

from HltConf.HltLinesConfigurableUser import *
from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import Hlt1Member as Member

from HltConf.LumiCounterDefinition import LumiCounterDefinitionConf
from Configurables import ( LumiCountVertices,
                            LumiCountTracks,
                            LumiFromL0DU,
                            LumiCountHltTracks
                            )


####### create binders...
def _createCounter( counterKind, seqName, seq ) :
    if counterKind is LumiFromL0DU:
        return _createL0Counter( seqName, seq ) 
    return lambda name, inputSel  :  seq.Members.append( counterKind( seqName + name
                                                                 , InputSelection = inputSel
                                                                 , CounterName = name
                                                                 , OutputContainer='Hlt/LumiSummary' ) )
def _createL0Counter( seqName, seq ) :
    return lambda name, value :  seq.Members.append(  LumiFromL0DU(seqName+name
                                                                 , InputSelection='Trig/L0/L0DUReport'
                                                                 , CounterName=name
                                                                 , ValueName=value
                                                                 , OutputContainer='Hlt/LumiSummary' ) )
####### operator, meet arguments...
def _combine( op, arg ) :
    for key,value in arg.iteritems() : op(key,value)
    return arg.keys()

############# start building the lumi line(s)...
class HltLumiLinesConf(HltLinesConfigurableUser) :
    __used_configurables__ = [ LumiCounterDefinitionConf ]

    __slots__ = { 'TriggerTypes'         : ['RandomTrigger']  # ODIN trigger type accepted
                , 'BXTypes'              : ['NoBeam', 'BeamCrossing','SingleBeamRight','SingleBeamLeft']
                , 'LumiLines'            : ['Count','VDM']
                , 'FullReco'             : True
                , 'OutputLevel'          : INFO
                , 'Debug'                : False
        }


    def __create_lumi_line__(self, BXType, counters):
        '''
        returns an Hlt1 "Line" including input filter, reconstruction sequence and counting
        adds histogramming
        '''
        from Configurables import GaudiSequencer as Sequence

        # debugging options
        debugOPL = self.getProp('OutputLevel')
        debugging = self.getProp('Debug')
        fullReco = self.getProp('FullReco')
        from HltConf.HltReco import PV2D

        # define empty reco sequence
        seqRecoName = 'LumiReco'
        lumiRecoSequence = Sequence(seqRecoName+BXType +'Seq'
                                    , ModeOR = True
                                    , ShortCircuit = False
                                    , OutputLevel = debugOPL
                                    , MeasureTime = True)

        # define empty sequence to collect counters
        seqCountName = 'LumiCount'
        lumiCountSequence = Sequence(seqCountName+BXType +'Seq'
                                    , ModeOR = True
                                    , ShortCircuit = False
                                    , OutputLevel = debugOPL
                                    , MeasureTime = True)

        # populate count sequence from the definition
        createdCounters = []
        histoThresholds = []
        histoMaxBins = []
        veloBW = False
        for key, definition in counters.iteritems():
            # example 'CaloEt'   : [LumiFromL0DU      , True    , 'Sum(Et)'         , 500, 6000],
            (op, flag, inputDef, threshold, bins) = definition
            if flag:
                createdCounters.extend( 
                    _combine( _createCounter( op, seqCountName+BXType , lumiCountSequence), { key : inputDef } ) )
                histoThresholds.extend( [threshold] )
                histoMaxBins.extend( [bins] )
                if key == 'RZVeloBW': veloBW=True
                if debugOPL <= INFO:
                    print '# INFO   : HltLumiLines::HistoMaker:', BXType, key, threshold, bins
                

        # populate reco sequence if needed
        if fullReco:
            # create lumiRecoSequence
            lumiTrackRecoSequence = GaudiSequencer( 'LumiTrackRecoSequence'
                                                    ,  IgnoreFilterPassed = True
                                                    ,  Members = PV2D.members()
                                                    #[  Sequence('HltRecoRZVeloSequence')
                                                    #,  recoVelo
                                                    #,  prepareVelo
                                                    #,  recoPV3D ]
                                                    )
            lumiRecoSequence.Members.append( Sequence('LumiTrackRecoSequence' , MeasureTime = True ) )
            # filter to get backward tracks (make sure it always passes by wrapping inside a sequence)
            if veloBW:
                from Configurables import HltTrackFilter
                lumiRecoSequence.Members.append(
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


        return Line ( 'Lumi'+BXType
                    , prescale = self.prescale
                    , ODIN = ' ( ODIN_TRGTYP == LHCb.ODIN.RandomTrigger ) & ( ODIN_BXTYP == LHCb.ODIN.'+BXType+' ) '
                    , algos = [ lumiRecoSequence, lumiCountSequence, lumiHistoSequence ] 
                    , postscale = self.postscale
                    ) 

    def __apply_configuration__(self) :
        '''
        creates parallel HLT1 Lines for each beam crossing type
        '''
        self.setOtherProps(LumiCounterDefinitionConf(),["FullReco"])

        counters = LumiCounterDefinitionConf().defineCounters()
        BXTypes=self.getProp('BXTypes')
        BXMembers = []
        for bx in BXTypes:
            BXMembers.append(self.__create_lumi_line__(bx, counters))
