from Configurables import Swimming
from Gaudi.Configuration import *

Swimming().DataType = '2012'
Swimming().EvtMax = 5000
Swimming().Simulation = False
Swimming().DDDBtag = 'dddb-20120831'
Swimming().CondDBtag = 'cond-20120831'
Swimming().Persistency = 'ROOT'
Swimming().InputType = 'DST'
Swimming().SwimStripping = True
Swimming().StrippingStream = 'CharmToBeSwum'
Swimming().StrippingVersion = 'Stripping20'
Swimming().StrippingFile = 'D2hh'
Swimming().StrippingLineGroup = 'D2hh'
Swimming().StrippingLine  = 'D2hhPromptD2KPiLine'
Swimming().StripCands = '/Event/Phys/D2hhPromptD2KPiLine'
Swimming().OffCands = '/Event/CharmToBeSwum/Phys/D2hhPromptD2KPiLine'
Swimming().SelectMethod = 'random'
Swimming().OutputType = 'MDST'
Swimming().OutputFile = 'SwimStrippingD2hhMDST_v14r8.mdst'
Swimming().UseFileStager = False
Swimming().Input = ["PFN:/afs/cern.ch/user/g/gligorov/cmtuser/DaVinci_v32r2p10/Phys/Swimming/example/SwimTrigD2hhDST_v14r8.dst"]
Swimming().Debug = False

from Configurables import CondDB
CondDB(UseOracle = False, DisableLFC = True)

# Configure Moore to swim the trigger
from Swimming.Configuration import ConfigureDaVinci
ConfigureDaVinci()

# The custom event loop
from Gaudi.Configuration import setCustomMainLoop
from Swimming.EventLoop import SwimmingEventLoop
setCustomMainLoop(SwimmingEventLoop)

