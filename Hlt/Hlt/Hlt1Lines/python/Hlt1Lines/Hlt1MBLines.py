##############################
#     HLT MiniBias Line(s)
##############################

from HltLine.HltLinesConfigurableUser import *
from HltLine.HltLine import Hlt1Line as Line

############# start building the MB  line(s)...
class Hlt1MBLinesConf(HltLinesConfigurableUser) :

    __slots__ = { 'MiniBiasL0Channels'     : ['CALO'] #'Hadron'
                , 'BXTypes'                : ['NoBeam', 'BeamCrossing','Beam1','Beam2']
                , 'MaxNoBiasRate'          : 100.
                #, 'Postscale'              : { 'Hlt1MBMicro.*R500' : 'RATE(500)' 
                #                             , 'Hlt1MBMicro.*P1000' : 0.001 
                #                             }
                }

    def __create_nobias_line__(self ):
        '''
        returns an Hlt1 "Line" including input and output filter
        '''
        from Configurables import LoKi__Hybrid__HltFactory as HltFactory
        for i in [ 'LoKiCore.functions', 'LoKiNumbers.sources' ] :
            if i not in HltFactory('ToolSvc.HltFactory').Modules : HltFactory('ToolSvc.HltFactory').Modules += [ i ]
        return Line ( 'MBNoBias'
                    , prescale = self.prescale
                    , ODIN = 'scale( ODIN_TRGTYP == LHCb.ODIN.LumiTrigger , RATE(%s,False)) ' % ( self.getProp('MaxNoBiasRate') )
                    , postscale = self.postscale
                    ) 
    def __create_microbias_line__(self, name, tracking) :
        from HltLine.HltLine import Hlt1Member as Member
        Line ( 'MBMicroBias%s' % name 
               , prescale = self.prescale
               , ODIN = '(ODIN_TRGTYP == LHCb.ODIN.LumiTrigger)'
               , algos = [ tracking
                           , Member( 'Hlt::TrackFilter','All'
                                     , Code = [ 'TrALL' ]
                                     , InputSelection = 'TES:%s' % tracking.outputSelection()
                                     , OutputSelection = '%Decision'
                                     ) 
                           ]
               , postscale = self.postscale
               ) 

    def __create_minibias_line__(self ):
        '''
        returns an Hlt1 "Line" including input and output filter
        '''
        return Line ( 'MBMiniBias'
                    , prescale = self.prescale
                    , L0DU  = ' | '.join( [ "L0_CHANNEL('%s') "%(x) for x in  self.getProp('MiniBiasL0Channels') ] )
                    , postscale = self.postscale
                    ) 

    def __apply_configuration__(self) :
        '''
        creates parallel HLT1 Lines for each beam crossing type
        '''
        self.__create_minibias_line__()
        self.__create_nobias_line__()

        from HltTracking.HltReco import MinimalRZVelo, Hlt1Seeding
        rz = self.__create_microbias_line__('RZVelo',MinimalRZVelo)
        ts = self.__create_microbias_line__('TStation',Hlt1Seeding)
        #for i in [ rz, ts ] :
        #    i.clone( i.name() + 'R500',  postscale = self.postscale )
        #    i.clone( i.name() + 'P1000',  postscale = self.postscale )

