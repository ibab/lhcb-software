##############################
#     HLT MiniBias Line(s)
##############################

from Gaudi.Configuration import *
from GaudiConf.Configuration import *

from HltLine.HltLinesConfigurableUser import *
from HltLine.HltLine import Hlt1Line as Line

############# start building the minibias line(s)...
class Hlt1MiniBiasLinesConf(HltLinesConfigurableUser) :

    __slots__ = { 'L0Channel'              : 'Hadron+Muon'
                , 'BXTypes'                : ['NoBeam', 'BeamCrossing','Beam1','Beam2']
                }

    def __create_minibias_line__(self, BXType):
        '''
        returns an Hlt1 "Line" including input and output filter
        '''
        lzero_list = self.getProp('L0Channel').split('+')
        l0du = " ( L0_CHANNEL('%s') ) "%(lzero_list.pop())
        for l0 in lzero_list:
            l0du += " | ( L0_CHANNEL('%s') ) "%(l0)
        print l0du
        return Line ( 'MiniBias'+BXType
                    , prescale = self.prescale
                    , ODIN = ' ( ODIN_BXTYP == LHCb.ODIN.%s ) ' % ( BXType )
                    , L0DU  = l0du
                    , algos = [ ] 
                    , postscale = self.postscale
                    ) 

    def __apply_configuration__(self) :
        '''
        creates parallel HLT1 Lines for each beam crossing type
        '''
        map( self.__create_minibias_line__, self.getProp('BXTypes') )
        
        
