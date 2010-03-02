##############################
#     HLT MiniBias Line(s)
##############################

from HltLine.HltLinesConfigurableUser import *
from HltLine.HltLine import Hlt1Line as Line

############# start building the MB  line(s)...
class Hlt1MBLinesConf(HltLinesConfigurableUser) :

    __slots__ = { 'MiniBiasL0Channels'     : ['CALO'] #'Hadron'
                , 'NoBiasTriggerType'      : 'LumiTrigger'  # ODIN trigger type accepted
                , 'BXTypes'                : ['NoBeam', 'BeamCrossing','Beam1','Beam2']
                }

    def __create_nobias_line__(self, BXType):
        '''
        returns an Hlt1 "Line" including input and output filter
        '''
        return Line ( 'MBNoBias'+BXType
                    , prescale = self.prescale
                    , ODIN = '( ODIN_BXTYP == LHCb.ODIN.%s ) & (ODIN_TRGTYP == LHCb.ODIN.%s)' % ( BXType, self.getProp('NoBiasTriggerType') )
                    , postscale = self.postscale
                    ) 
    def __create_microbias_line__(self) :
        from HltLine.HltReco import MinimalRZVelo
        from HltLine.HltLine import Hlt1Member as Member
        return Line ( 'MBMicroBiasRZVelo'
                    , prescale = self.prescale
                    , ODIN = '( ODIN_BXTYP == LHCb.ODIN.BeamCrossing ) & (ODIN_TRGTYP <= LHCb.ODIN.LumiTriggers)'
                    , algos = [ MinimalRZVelo
                              , Member( 'Hlt::TrackFilter','All'
                                      , Code = [ 'TrALL' ]
                                      , InputSelection = 'TES:%s' % MinimalRZVelo.outputSelection()
                                      , OutputSelection = '%Decision'
                                      ) 
                              ]
                    , postscale = self.postscale
                    ) 

    def __create_minibias_line__(self, BXType):
        '''
        returns an Hlt1 "Line" including input and output filter
        '''
        return Line ( 'MBMiniBias'+BXType
                    , prescale = self.prescale
                    , ODIN = '( ODIN_BXTYP == LHCb.ODIN.%s )' % ( BXType )
                    , L0DU  = ' | '.join( [ "L0_CHANNEL('%s') "%(x) for x in  self.getProp('MiniBiasL0Channels') ] )
                    , postscale = self.postscale
                    ) 

    def __apply_configuration__(self) :
        '''
        creates parallel HLT1 Lines for each beam crossing type
        '''
        for i in self.getProp('BXTypes') :
            self.__create_minibias_line__(i)
            self.__create_nobias_line__(i)

        self.__create_microbias_line__()
