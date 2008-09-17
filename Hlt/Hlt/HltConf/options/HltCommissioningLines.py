from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import Hlt1Member as Member

OdinTypes = set([  'Reserve'
                ,  'PhysicsTrigger'
                ,  'RandomTrigger'
                ,  'AuxilliaryTrigger'
                ,  'PeriodicTrigger'
                ,  'NonZSupTrigger'
                ,  'TimingTrigger'
                ,  'CalibrationTrigger' ])

physics = Line( 'Physics'
              ,  ODIN = { 'TriggerTypes' : OdinTypes - set([ 'RandomTrigger' ]) }
              ,  algos = [ Member('Dummy','Selection',OutputSelection = '%Decision') ]
              )

random  = Line('Random'
              ,  prescale = 1 # @OnlineEnv.AcceptRate
              ,  ODIN = { 'TriggerTypes' : [ 'RandomTrigger' ] }
              ,  algos = [ Member('Dummy','Selection',OutputSelection = '%Decision') ]
              )
