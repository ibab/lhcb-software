from Configurables import Swimming
from Gaudi.Configuration import *

Swimming().DataType = '2011'
Swimming().EvtMax = 500
Swimming().Simulation = False
Swimming().DDDBtag = "head-20110302"
Swimming().CondDBtag = "head-20110622-Reco10"
Swimming().Persistency = 'ROOT'
Swimming().InputType = 'DST'
Swimming().OutputFile = 'SwimTrigDST.dst'
Swimming().SwimStripping = False
Swimming().TCK = '0x00730035'
Swimming().Hlt1Triggers = ["Hlt1TrackAllL0Decision"]
Swimming().Hlt2Triggers = ["Hlt2CharmHadD02HH_D02KPiDecision"]
Swimming().OffCands = "/Event/CharmCompleteEvent/Phys/D2hhPromptD2KPiLine"
Swimming().TransformName = '2011_WithBeamSpotFilter'
Swimming().SelectMethod = 'random'
Swimming().OutputType = 'DST'
Swimming().UseFileStager = False
Swimming().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/LHCb/Collision11/CHARMCOMPLETEEVENT.DST/00012606/0000/00012606_00000104_1.charmcompleteevent.dst'"]

from Configurables import FileStagerSvc
FileStagerSvc().OutputLevel = 1

from Configurables import CondDB
CondDB(UseOracle = False, DisableLFC = True )

# Configure Moore to swim the trigger
from Swimming.Configuration import ConfigureMoore
ConfigureMoore()

# The custom event loop
from Gaudi.Configuration import setCustomMainLoop
from Swimming.EventLoop import SwimmingEventLoop
setCustomMainLoop(SwimmingEventLoop)
