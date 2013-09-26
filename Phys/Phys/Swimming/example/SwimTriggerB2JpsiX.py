from Configurables import Swimming
from Gaudi.Configuration import *

Swimming().DataType = '2012'
Swimming().EvtMax = 250
Swimming().Simulation = False
# For v14r2
#Swimming().DDDBtag     = 'head-20120413'
#Swimming().CondDBtag   = 'head-20120420'
# For v14r6
#Swimming().DDDBtag     = 'head-20120413'
#Swimming().CondDBtag   = 'cond-20120730'
# For v14r8,9,11
Swimming().DDDBtag     = 'dddb-20120831'
Swimming().CondDBtag   = 'cond-20120831'
#For v14r2
#Swimming().TCK = '0x0097003d'
#For v14r6 
#Swimming().TCK = '0x00990042'
#For v14r8 
Swimming().TCK = '0x00a10044'
#For v14r9
#Swimming().TCK = '0x00a30044'
#For v14r11
#Swimming().TCK = '0x00ab0046'
Swimming().Persistency = 'ROOT'
Swimming().InputType = 'DST'
#For v14r2
#Swimming().OutputFile = 'SwimTrigDST_v14r2.dst'
#For v14r6
#Swimming().OutputFile = 'SwimTrigDST_v14r6.dst'
#For v14r8
Swimming().OutputFile = 'SwimTrigDST_v14r8.dst'
#For v14r9
#Swimming().OutputFile = 'SwimTrigDST_v14r9.dst'
#For v14r11
#Swimming().OutputFile = 'SwimTrigDST_v14r11.dst'
Swimming().SwimStripping = False
Swimming().Hlt1Triggers = ["Hlt1DiMuonHighMassDecision", "Hlt1TrackAllL0Decision", "Hlt1TrackMuonDecision"]
Swimming().Hlt2Triggers = ["Hlt2DiMuonJPsiDecision", "Hlt2DiMuonDetachedJPsiDecision"]
Swimming().OffCands = "/Event/Dimuon/Phys/BetaSBd2JpsiKstarDetachedLine"
Swimming().TransformName = '2011_WithBeamSpotFilter_NoRecoLines'
Swimming().SelectMethod = 'random'
Swimming().OutputType = 'DST'
Swimming().UseFileStager = True
Swimming().Debug = False
#For v14r2
#Swimming().Input = ['PFN:root://eoslhcb.cern.ch//eos/lhcb/LHCb/Collision12/DIMUON.DST/00018117/0000/00018117_00002737_1.dimuon.dst']
#For v14r6
#Swimming().Input = ['PFN:root://eoslhcb.cern.ch//eos/lhcb/LHCb/Collision12/DIMUON.DST/00024183/0002/00024183_00020006_1.dimuon.dst']
#For v14r8
Swimming().Input = ['PFN:root://eoslhcb.cern.ch//eos/lhcb/LHCb/Collision12/DIMUON.DST/00020567/0000/00020567_00006473_1.dimuon.dst']
#For v14r9
#Swimming().Input = ['PFN:root://eoslhcb.cern.ch//eos/lhcb/LHCb/Collision12/DIMUON.DST/00024183/0000/00024183_00009746_1.dimuon.dst']
#For v14r11
#Swimming().Input = ['PFN:root://eoslhcb.cern.ch//eos/lhcb/LHCb/Collision12/DIMUON.DST/00021211/0000/00021211_00009739_1.dimuon.dst']
# Configure Moore to swim the trigger
from Swimming.Configuration import ConfigureMoore
ConfigureMoore()

# The custom event loop
from Gaudi.Configuration import setCustomMainLoop
from Swimming.EventLoop import SwimmingEventLoop
setCustomMainLoop(SwimmingEventLoop)
