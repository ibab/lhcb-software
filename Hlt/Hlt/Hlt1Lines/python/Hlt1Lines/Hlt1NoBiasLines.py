##############################
#     HLT NoBias Line(s)
##############################

from Gaudi.Configuration import *
from GaudiConf.Configuration import *

from HltLine.HltLinesConfigurableUser import *
from HltLine.HltLine import Hlt1Line as Line

############# start building the nobias line(s)...
class Hlt1NoBiasLinesConf(HltLinesConfigurableUser) :

    __slots__ = { 'TriggerType'            : 'LumiTrigger'  # ODIN trigger type accepted
                , 'BXTypes'                : ['NoBeam', 'BeamCrossing','Beam1','Beam2']
                }

    def __create_nobias_line__(self, BXType):
        '''
        returns an Hlt1 "Line" including input and output filter
        '''
        return Line ( 'NoBias'+BXType
                    , prescale = self.prescale
                    , ODIN = ' ( ODIN_BXTYP == LHCb.ODIN.%s ) ' % ( BXType )
                    , algos = [ ] 
                    , postscale = self.postscale
                    ) 

    def __apply_configuration__(self) :
        '''
        creates parallel HLT1 Lines for each beam crossing type
        '''
        map( self.__create_nobias_line__, self.getProp('BXTypes') )

        
