from Configurables import Swimming
from Gaudi.Configuration import *

Swimming().DataType = '2011'
Swimming().EvtMax = 400
Swimming().Simulation = False
Swimming().RunNumber = 89346
Swimming().Persistency = 'ROOT'
Swimming().InputType = 'DST'
Swimming().OutputFile = 'SwimTrigDST.dst'
Swimming().SwimStripping = False
Swimming().Hlt1Triggers = ["Hlt1DiMuonHighMassDecision", "Hlt1TrackAllL0Decision", "Hlt1TrackMuonDecision"]
Swimming().Hlt2Triggers = ["Hlt2DiMuonJPsiDecision", "Hlt2DiMuonDetachedJPsiDecision"]
Swimming().OffCands = "/Event/Dimuon/Phys/BetaSBs2JpsiPhiDetachedLine/Particles"
Swimming().TransformName = '2011_WithBeamSpotFilter'
Swimming().SelectMethod = 'random'
Swimming().OutputType = 'DST'
Swimming().UseFileStager = True
Swimming().Debug = False

## from Configurables import FileStagerSvc
## FileStagerSvc().OutputLevel = 1

# Configure Moore to swim the trigger
from Swimming.Configuration import ConfigureMoore
ConfigureMoore()

# The custom event loop
from Gaudi.Configuration import setCustomMainLoop
from Swimming.EventLoop import SwimmingEventLoop
setCustomMainLoop(SwimmingEventLoop)
