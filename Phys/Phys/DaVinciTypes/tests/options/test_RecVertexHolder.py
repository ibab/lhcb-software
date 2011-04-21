from Configurables import LHCbApp, ApplicationMgr, TestRecVertexHolder

LHCbApp(EvtMax=10)

ApplicationMgr(TopAlg = [TestRecVertexHolder(OutputLevel=3)],
               EvtSel = 'NONE')
