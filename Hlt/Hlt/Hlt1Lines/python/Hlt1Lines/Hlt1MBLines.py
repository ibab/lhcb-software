##############################
#     HLT MiniBias Line(s)
##############################

from HltLine.HltLinesConfigurableUser import *

############# start building the MB  line(s)...
class Hlt1MBLinesConf(HltLinesConfigurableUser) :

    __slots__ = { 'MiniBiasL0Channels'     : ['CALO'] #'Hadron'
                , 'BXTypes'                : ['NoBeam', 'BeamCrossing','Beam1','Beam2']
                , 'MicroBiasOdin'          : '(ODIN_TRGTYP == LHCb.ODIN.LumiTrigger)'
                , 'MaxNoBiasRate'          : 1000000.
                , 'Postscale'              : { 'Hlt1MBMicroBias.*RateLimited' : 'RATE(500)', 
                                               'Hlt1CharmCalibrationNoBias' : 'RATE(500)'  }
                }

    def __create_nobias_line__(self ):
        '''
        returns an Hlt1 "Line" including input and output filter
        '''
        from Configurables import LoKi__Hybrid__HltFactory as HltFactory
        for i in [ 'LoKiCore.functions', 'LoKiNumbers.sources' ] :
            if i not in HltFactory('ToolSvc.HltFactory').Modules : HltFactory('ToolSvc.HltFactory').Modules += [ i ]
        from HltLine.HltLine import Hlt1Line as Line
        return Line ( 'MBNoBias'
                    , prescale = self.prescale
                    , ODIN = 'jbit( ODIN_EVTTYP,2 )'
                    , postscale = self.postscale
                    )
    def __create_charm_nobias_line__(self ) :
        ''' 
        returns an Hlt1 "Line" including input and output filter
        '''
        from Configurables import LoKi__Hybrid__HltFactory as HltFactory
        for i in [ 'LoKiCore.functions', 'LoKiNumbers.sources' ] : 
            if i not in HltFactory('ToolSvc.HltFactory').Modules : HltFactory('ToolSvc.HltFactory').Modules += [ i ] 
        from HltLine.HltLine import Hlt1Line as Line
        return Line ( 'CharmCalibrationNoBias'
                    , prescale = self.prescale
                    , ODIN = 'jbit( ODIN_EVTTYP,2 )'
                    , postscale = self.postscale
                    ) 
    def __create_microbias_line__(self, name, tracking) :
        from HltLine.HltLine import Hlt1Member as Member
        from HltLine.HltLine import Hlt1Line as Line
        return Line ( 'MBMicroBias%s' % name 
                    , prescale = self.prescale
                    , ODIN = self.getProp('MicroBiasOdin')
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
        from HltLine.HltLine import Hlt1Line as Line
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
        self.__create_charm_nobias_line__()
    
        from HltTracking.HltReco import MinimalVelo , Hlt1Seeding
        ve = self.__create_microbias_line__('Velo',MinimalVelo )
        ve.clone( ve.name().lstrip('Hlt1') + 'RateLimited',  postscale = self.postscale, prescale = self.prescale )
        ts = self.__create_microbias_line__('TStation',Hlt1Seeding)
        ts.clone( ts.name().lstrip('Hlt1') + 'RateLimited', postscale = self.postscale, prescale = self.prescale )

