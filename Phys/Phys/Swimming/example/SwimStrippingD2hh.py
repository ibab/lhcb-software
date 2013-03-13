from Configurables import Swimming
from Gaudi.Configuration import *

Swimming().DataType = '2011'
Swimming().EvtMax = 1000
Swimming().Simulation = False
Swimming().DDDBtag = 'head-20110914'
Swimming().CondDBtag = 'head-20110914'
Swimming().Persistency = 'ROOT'
Swimming().InputType = 'DST'
Swimming().SwimStripping = True
Swimming().StrippingStream = 'CharmCompleteEvent'
Swimming().StrippingVersion = 'Stripping17'
Swimming().StrippingFile = 'D2hh'
Swimming().StrippingLine = 'D2hh'
Swimming().StripCands = '/Event/Phys/D2hhPromptD2KPiLine'
Swimming().OffCands = '/Event/CharmCompleteEvent/Phys/D2hhPromptD2KPiLine'
Swimming().SelectMethod = 'random'
Swimming().OutputType = 'MDST'
Swimming().UseFileStager = False

from Configurables import CondDB
CondDB(UseOracle = False, DisableLFC = True)

# Configure Moore to swim the trigger
from Swimming.Configuration import ConfigureDaVinci
ConfigureDaVinci()

# The custom event loop
from Gaudi.Configuration import setCustomMainLoop
from Swimming.EventLoop import SwimmingEventLoop
setCustomMainLoop(SwimmingEventLoop)

