##############################
#     HLT NoBias Line(s)
##############################

from HltLine.HltLinesConfigurableUser import *

############# start building the NB line(s)...
class Hlt1NBLinesConf(HltLinesConfigurableUser) :

    __slots__ = { 'NoBiasPrescaledOdin'    : '(ODIN_TRGTYP == LHCb.ODIN.LumiTrigger)'
		, 'Prescale'               : { 'Hlt1NoBiasPrescaled' : 0.5}
                }

    def __create_nobias_line__(self ):
        '''
        returns an Hlt1 "Line" including input and output filter
        '''
        from HltLine.HltLine import Hlt1Line as Line
        return Line ( 'NoBiasPrescaled'
                    , prescale = self.prescale
                    , ODIN = self.getProp('NoBiasPrescaledOdin')
                    , postscale = self.postscale
                    )


    def __apply_configuration__(self) :
        '''
        creates parallel HLT1 Lines for each beam crossing type
        '''
        self.__create_nobias_line__()
   
        
