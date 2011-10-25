from Configurables import Swimming
from Gaudi.Configuration import *

Swimming().DataType = '2011'
Swimming().EvtMax = -1
Swimming().Simulation = False
Swimming().DDDBtag = 'head-20110722'
Swimming().CondDBtag = 'head-20110901'
Swimming().Persistency = 'ROOT'
Swimming().InputType = 'DST'
Swimming().SwimStripping = True
Swimming().StrippingStream = 'DiMuon'
Swimming().StrippingVersion = 'Stripping16'
Swimming().StrippingFile = 'B2JpsiXforBeta_s'
Swimming().StrippingLine = 'BetaS'
Swimming().StripCands = '/Event/Phys/BetaSBs2JpsiPhiDetachedLine'
Swimming().OffCands = '/Event/SingleCandidate'
Swimming().SelectMethod = 'random'
Swimming().OutputType = 'DST'
Swimming().UseFileStager = True

from Configurables import CondDB
CondDB(UseOracle = True)

# Configure Moore to swim the trigger
from Swimming.Configuration import ConfigureDaVinci
ConfigureDaVinci()

# The custom event loop
from Gaudi.Configuration import setCustomMainLoop
from Swimming.EventLoop import SwimmingEventLoop
setCustomMainLoop(SwimmingEventLoop)
