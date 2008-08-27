from HltConf.HltLine import Hlt1Line   as Line

OdinTypes = set([  'Reserve'
                ,  'PhysicsTrigger'
                ,  'AuxilliaryTrigger'
                ,  'PeriodicTrigger'
                ,  'NonZSupTrigger'
                ,  'TimingTrigger'
                ,  'CalibrationTrigger' ])

physics = Line( 'Physics'
              ,  ODIN = { 'TriggerTypes' : OdinTypes - set([ 'RandomTrigger' ]) }
              )

random  = Line('Random'
              ,  prescale = 1 # @OnlineEnv.AcceptRate
              ,  ODIN = { 'TriggerTypes' : [ 'RandomTrigger' ] }
              )
