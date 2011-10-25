from Configurables import Swimming
from Gaudi.Configuration import *

Swimming().DataType = '2011'
Swimming().EvtMax = 25
Swimming().Simulation = False
Swimming().DDDBtag = "head-20110722"
Swimming().CondDBtag = "head-20110901"
Swimming().Persistency = 'ROOT'
Swimming().InputType = 'DST'
Swimming().OutputFile = 'SwimTrigDST.dst'
Swimming().SwimStripping = False
Swimming().TCK = '0x00790038'
Swimming().Hlt1Triggers = ["Hlt1TrackAllL0Decision", "Hlt1TrackMuonDecision"]
Swimming().Hlt2Triggers = ["Hlt2DiMuonJPsiDecision", "Hlt2DiMuonDetachedJPsiDecision"]
Swimming().OffCands = "/Event/SingleCandidate"
Swimming().TransformName = '2011_WithBeamSpotFilter'
Swimming().SelectMethod = 'random'
Swimming().UseFileStager = True

from Configurables import CondDB
CondDB(UseOracle = True)

# Configure Moore to swim the trigger
from Swimming.Configuration import ConfigureMoore
ConfigureMoore()

# The custom event loop
from Gaudi.Configuration import setCustomMainLoop
from Swimming.EventLoop import SwimmingEventLoop
setCustomMainLoop(SwimmingEventLoop)
