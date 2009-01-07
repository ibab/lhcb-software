from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import Hlt1Member as Member

Line('Physics' ,  ODIN = 'ODIN_TRGTYP != LHCb.ODIN.RandomTrigger'
    , prescale = 1 
    )

Line('Random' ,  ODIN = 'ODIN_TRGTYP == LHCb.ODIN.RandomTrigger'
    , prescale = 1 # @OnlineEnv.AcceptRate
    )

Line('Force', L0DU = 'L0_FORCEBIT'
    , prescale = 1
    )
