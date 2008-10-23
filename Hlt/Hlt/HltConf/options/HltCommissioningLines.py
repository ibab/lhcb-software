from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import Hlt1Member as Member

physics = Line( 'Physics'
              ,  ODIN = 'ODIN_TRGTYP != LHCb.ODIN.RandomTrigger'
              )

random  = Line('Random' 
              ,  prescale = 1 # @OnlineEnv.AcceptRate
              ,  ODIN = 'ODIN_TRGTYP == LHCb.ODIN.RandomTrigger'
              )
