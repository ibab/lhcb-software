from Configurables import Swimming
from Gaudi.Configuration import *

Swimming().DataType = '2012'
Swimming().EvtMax = 1000
Swimming().Simulation = False
Swimming().DDDBtag = 'dddb-20120831'
Swimming().CondDBtag = 'cond-20120831'
Swimming().Persistency = 'ROOT'
Swimming().InputType = 'DST'
Swimming().Input = ["PFN:/afs/cern.ch/user/g/gligorov/cmtuser/DaVinci_v32r2p10/Phys/Swimming/example/SwimTrigDST_v14r8.dst"]
Swimming().SwimStripping = True
Swimming().StrippingStream = 'DiMuon'
Swimming().StrippingVersion = 'Stripping20'
Swimming().StrippingFile = 'B2JpsiXforBeta_s'
Swimming().StrippingLineGroup = 'BetaS'
Swimming().StrippingLine  = 'BetaSBd2JpsiKstarDetachedLine'
Swimming().StripCands = '/Event/Phys/BetaSBd2JpsiKstarDetachedLine'
Swimming().OffCands = '/Event/Dimuon/Phys/BetaSBd2JpsiKstarDetachedLine'
Swimming().SelectMethod = 'random'
Swimming().OutputType = 'MDST'
Swimming().OutputFile = 'SwimStrippingMDST.mdst'
Swimming().UseFileStager = False
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

