from Configurables import Swimming
from Gaudi.Configuration import *

Swimming().DataType = '2012'
Swimming().EvtMax = 500
Swimming().Simulation = False
# Define the database tags
# For v14r2
#Swimming().DDDBtag     = 'head-20120413'
#Swimming().CondDBtag   = 'head-20120420'
# For v14r6
#Swimming().DDDBtag     = 'head-20120413'
#Swimming().CondDBtag   = 'cond-20120730'
# For v14r8,9,11
Swimming().DDDBtag     = 'dddb-20120831'
Swimming().CondDBtag   = 'cond-20120831'
# Now define the TCK
# For v14r2
#Swimming().TCK = '0x0097003d'
# For v14r6 
#Swimming().TCK = '0x00990042'
# For v14r8 
Swimming().TCK = '0x009f0045'
# For v14r9
#Swimming().TCK = '0x00a30044'
# For v14r11
#Swimming().TCK = '0x00ab0046'
# Now define the output file
# For v14r2
#Swimming().OutputFile = 'SwimTrigDST_v14r2.dst'
# For v14r6
#Swimming().OutputFile = 'SwimTrigDST_v14r6.dst'
# For v14r8
Swimming().OutputFile = 'SwimTrigDST_v14r8.dst'
# For v14r9
#Swimming().OutputFile = 'SwimTrigDST_v14r9.dst'
# For v14r11
#Swimming().OutputFile = 'SwimTrigDST_v14r11.dst'
Swimming().Persistency = 'ROOT'
Swimming().InputType = 'DST'
Swimming().SwimStripping = False
Swimming().Hlt1Triggers = ["Hlt1TrackAllL0Decision"]
Swimming().Hlt2Triggers = ["Hlt2CharmHadD02HH_D02KPiDecision"]
Swimming().OffCands = "/Event/CharmToBeSwum/Phys/D2hhPromptD2KPiLine"
Swimming().TransformName = '2011_WithBeamSpotFilter_NoRecoLines'
Swimming().SelectMethod = 'random'
Swimming().OutputType = 'DST'
Swimming().UseFileStager = False
# Now define the input file
# For v14r2
#Swimming().Input = ["PFN:/afs/cern.ch/lhcb/group/physics/swimming/Moore_v14r2/00020240_00000141_1.CharmToBeSwum.dst"]
# For v14r6
#Swimming().Input = ["PFN:/afs/cern.ch/lhcb/group/physics/swimming/Moore_v14r6/00020392_00002902_1.CharmToBeSwum.dst"]
# For v14r8
Swimming().Input = ["PFN:/afs/cern.ch/lhcb/group/physics/swimming/Moore_v14r8/00020566_00000274_1.CharmToBeSwum.dst"]
# For v14r9
#Swimming().Input = ["PFN:/afs/cern.ch/lhcb/group/physics/swimming/Moore_v14r9/00020847_00019204_1.CharmToBeSwum.dst"]
# For v14r11
#Swimming().Input = ["PFN:/afs/cern.ch/lhcb/group/physics/swimming/Moore_v14r11/00021210_00010066_1.CharmToBeSwum.dst"]

from Configurables import CondDB
CondDB(UseOracle = False, DisableLFC = True )

# Configure Moore to swim the trigger
from Swimming.Configuration import ConfigureMoore
ConfigureMoore()

# The custom event loop
from Gaudi.Configuration import setCustomMainLoop
from Swimming.EventLoop import SwimmingEventLoop
setCustomMainLoop(SwimmingEventLoop)
