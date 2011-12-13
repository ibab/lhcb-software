from Configurables import Swimming
from Gaudi.Configuration import *

Swimming().DataType = '2011'
Swimming().EvtMax = 1
Swimming().SkipEvents = 655
Swimming().Simulation = False
Swimming().Persistency = 'ROOT'
Swimming().InputType = 'DST'
Swimming().SwimStripping = False
Swimming().Hlt1Triggers = ["Hlt1TrackAllL0Decision"]
Swimming().Hlt2Triggers = ["Hlt2Topo2BodyBBDTDecision","Hlt2InclusivePhiDecision"]#,
                           #"Hlt2Topo3BodyBBDTDecision",
                           #"Hlt2Topo4BodyBBDTDecision"]
Swimming().OutputFile = 'Bs2DsPi.Bs2DsPiSwimTrigger.TOS.dst'
Swimming().OffCands = "/Event/Phys/Selector"
Swimming().TransformName = '2011_WithBeamSpotFilter'
Swimming().SelectMethod = 'random'
Swimming().OutputType = 'DST'
Swimming().UseFileStager = False
Swimming().Debug = True

Swimming().DDDBtag = "head-20110302"
Swimming().CondDBtag = "head-20110622"
Swimming().TCK = '0x00760037'
Swimming().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/a/aphan/GangaSyracuse/584/27513859_Bs2DsPi.Bs2DsPiSequence.dst'"]

from Configurables import CondDB
CondDB(UseOracle = False, DisableLFC = True)

from Swimming.Configuration import ConfigureMoore
ConfigureMoore()

from Gaudi.Configuration import setCustomMainLoop
from Swimming.EventLoop import SwimmingEventLoop
setCustomMainLoop(SwimmingEventLoop)
