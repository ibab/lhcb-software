##############################
#     HLT Lumi Line(s)
##############################

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *

from Configurables import GaudiSequencer as Sequence
from Configurables import HltTrackFilter, HltSelectionFilter
from Configurables import LumiHistoMaker, LumiHisto2dSPD
from Configurables import LoKi__ODINFilter  as ODINFilter
from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import Hlt1Member as Member

from Configurables import LHCbConfigurableUser
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
class HltLumiLinesConf(LHCbConfigurableUser) :
    __used_configurables__ = [ LumiCounterDefinitionConf
                               ]

    __slots__ = { 'TriggerTypes'         : ['RandomTrigger']  # ODIN trigger type accepted
                , 'BXTypes'              : ['NoBeam', 'BeamCrossing','SingleBeamRight','SingleBeamLeft']
                , 'LumiLines'            : ['Count','B1Gas','B2Gas','VDM']
                , 'FullReco'             : False
                , 'OutputLevel'          : WARNING
                , 'Debug'                : False
        }


    def __create_lumi_line__(self, BXType, counters):
        '''
        returns an Hlt1 "Line" including input filter, reconstruction sequence and counting
        adds histogramming
        '''
        # debugging options
        debugOPL = self.getProp('OutputLevel')
        debugging = self.getProp('Debug')
        fullReco = self.getProp('FullReco')

        # define empty reco sequence
        seqRecoName = 'LumiReco'
        lumiRecoSequence = Sequence(seqRecoName+BXType +'Seq'
                                    , ModeOR = True
                                    , ShortCircuit = False
                                    , MeasureTime = True)

        # define empty sequence to collect counters
        seqCountName = 'LumiCount'
        lumiCountSequence = Sequence(seqCountName+BXType +'Seq'
                                    , ModeOR = True
                                    , ShortCircuit = False
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
                

        # populate reco sequence if needed
        if fullReco:
            ## todo: create lumiRecoSequence
            ## HERE RECO sequence
            importOptions('$HLTCONFROOT/options/HltRecoSequence.py') 
           
            # filter to get backward tracks (make sure it always passes by wrapping inside a sequence)
            if veloBW:
                from Configurables import HltTrackFilter, HltSelectionFilter            
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
        ## todo: add also thresholds, bins
        HistoMembers=[]
        HistoMembers.append(LumiHistoMaker('Histo'+BXType, InputVariables = createdCounters, OutputLevel = 3 ))
        HistoMembers.append(LumiHisto2dSPD('Histo2D'+BXType, HistoTitle=str(BXType), OutputLevel = 3 ))
        lumiHistoSequence = Sequence('LumiHisto'+BXType +'Seq'
                                     , Members = HistoMembers
                                     , ModeOR = True
                                     , ShortCircuit = False
                                     , MeasureTime = True)


        return Line (
            'Lumi'+BXType
            , ODIN = ' ( ODIN_TRGTYP == LHCb.ODIN.RandomTrigger ) & ( ODIN_BXTYP == LHCb.ODIN.'+BXType+' ) '
            , algos = [
            lumiRecoSequence,
            lumiCountSequence,
            lumiHistoSequence,
            ] )


    def __apply_configuration__(self) :
        '''
        creates parallel HLT1 Lines for each beam crossing type
        '''
        self.setOtherProps(LumiCounterDefinitionConf(),["FullReco"])

        counters = LumiCounterDefinitionConf().defineCounters()
        BXTypes=self.getProp('BXTypes')
        BXMembers = []
<<<<<<< HltLumiLines.py
        for i in [ 'NoBeam', 'BeamCrossing','SingleBeamRight','SingleBeamLeft'] :
            HistoMembers=[]
            if debugging: HistoMembers.append(HltLumiOdinReader('OdinReaderBefore'+i, OutputLevel = debugOPL ))
            # TODO: move this filter to 'start of line'
            HistoMembers.append( ODINFilter('Filter'+i, Code = 'ODIN_BXTYP == LHCb.ODIN.'+i, OutputLevel = debugOPL ))
            if debugging: HistoMembers.append(HltLumiOdinReader('OdinReaderAfter'+i, OutputLevel = debugOPL ))
            HistoMembers.append(LumiHistoMaker('Histo'+i, InputVariables = createdCounters, OutputLevel = 3 ))
            HistoMembers.append(LumiHisto2dSPD('Histo2D'+i, HistoTitle=str(i),OutputLevel = 3 )) ##Does this work??
            BXMembers.append( Sequence('Hlt1Lumi'+i+'Sequence', 
                                       Members = HistoMembers,
                                       MeasureTime = True
                                       ) )
        BXTypes = Sequence( 'Hlt1LumiBXTypesSequence'
                          , Members = BXMembers
                          , ModeOR = True
                          , ShortCircuit = False
                          , MeasureTime = True)

        importOptions('$HLTCONFROOT/options/HltRecoSequence.py')

        lumiRecoSequence  = Sequence( 'Hlt1LumiRecoSequence'
                                    , Members = [ Sequence('HltRecoSequence') ]
                                    , IgnoreFilterPassed = True
                                    , MeasureTime = True )



        lumiSequence = Line ( 'Lumi'
                            , ODIN = 'ODIN_TRGTYP == LHCb.ODIN.RandomTrigger'# should become 'Random', now 'TimingTrigger' or 'Reserve'
                            , algos =
                            [ # lumiRecoSequence
                            lumiCountSequence
                            , BXTypes
                            ] )





        #if debugging: lumiSequence( RawEventDump('FullRawDataDump' ) )
        # @todo Retsore that with LHCb v25r1
        ####lumiSequence( bankKiller( 'SerialKiller', DefaultIsKill=True, BankTypes=["ODIN","HltLumiSummary"],
        #                          OutputLevel=debugOPL ) )
        #if debugging: lumiSequence( RawEventDump('NanoEventDump' ) )
        ####lumiSequence( HltLumiWriter( OutputLevel=debugOPL )  )
=======
        for bx in BXTypes:
            BXMembers.append(self.__create_lumi_line__(bx, counters))
            
>>>>>>> 1.4
