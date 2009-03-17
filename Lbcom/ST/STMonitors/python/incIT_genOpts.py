from Gaudi.Configuration import *
from Configurables import CreateHistDBPages
createHistAlg = CreateHistDBPages("createHistAlg")

createHistAlg.PageBase = "/IT"
createHistAlg.HistoBase = "ITDAQMon/STZSMonitor"

createHistAlg.OutputLevel = INFO

app = ApplicationMgr()
app.TopAlg.append(createHistAlg)
