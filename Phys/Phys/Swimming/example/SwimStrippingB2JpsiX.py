from Configurables import Swimming
from Gaudi.Configuration import *

Swimming().DataType = '2011'
Swimming().EvtMax = 1000
Swimming().Simulation = False
Swimming().DDDBtag = 'head-20110914'
Swimming().CondDBtag = 'head-20110914'
Swimming().Persistency = 'ROOT'
Swimming().InputType = 'DST'
Swimming().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/r/raaij/Swimming/SwimTrigDST_0x00730035.dst'"]
Swimming().SwimStripping = True
Swimming().StrippingStream = 'DiMuon'
Swimming().StrippingVersion = 'Stripping17'
Swimming().StrippingFile = 'B2JpsiXforBeta_s'
Swimming().StrippingLine = 'BetaS'
Swimming().StripCands = '/Event/Phys/BetaSBs2JpsiPhiDetachedLine'
Swimming().OffCands = '/Event/SingleCandidate'
Swimming().SelectMethod = 'random'
Swimming().OutputType = 'MDST'
Swimming().UseFileStager = True
Swimming().Debug = True

from Configurables import CondDB
CondDB(UseOracle = False, DisableLFC = True)

# Configure Moore to swim the trigger
from Swimming.Configuration import ConfigureDaVinci
ConfigureDaVinci()

# The custom event loop
from Gaudi.Configuration import setCustomMainLoop
from Swimming.EventLoop import SwimmingEventLoop
setCustomMainLoop(SwimmingEventLoop)

